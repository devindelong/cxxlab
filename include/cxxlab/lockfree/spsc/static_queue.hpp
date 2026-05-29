/**
 * @copyright Copyright (c) 2026, Devin DeLong. All rights reserved.
 *
 * @license This code is distributed under the BSD 3-Clause License.
 *          See the LICENSE file for the full text of the license.
 *
 * @author Devin DeLong
 */

#pragma once

#include "cxxlab/functional/repeat.hpp"
#include "cxxlab/lockfree/spsc/detail/slot.hpp"
#include "cxxlab/memory/cache_line_size.hpp"

#include <algorithm>
#include <atomic>
#include <bit>
#include <cstddef>
#include <iterator>
#include <optional>
#include <ranges>
#include <type_traits>
#include <utility>

namespace cxxlab::spsc
{

/**
 * @brief Statically-sized Single-Consumer-Single-Producer queue.
 * @tparam T Element type.
 * @tparam Capacity Maximum number of elements can store.
 * @details The actual capacity used will be a power of 2 large enough to hold the specified
 * capacity.
 */
template <typename T, std::size_t Capacity>
class static_queue
{
   // Power of 2 capacity.
   static constexpr auto BIT_CEIL_CAPACITY_ = std::bit_ceil(Capacity);
   // Internal storage type.
   using storage_type = std::array<detail::slot<T>, BIT_CEIL_CAPACITY_>;

 public:
   /**
    * @brief Gets the capacity.
    * @return The capacity.
    */
   [[nodiscard]] static consteval auto capacity() noexcept -> std::size_t
   {
      return BIT_CEIL_CAPACITY_;
   }

   /**
    * @brief Checks if the queue is empty.
    * @return True if empty, false otherwise.
    * @note Due to the nature of concurrency, this is an approximate value.
    */
   [[nodiscard]] auto empty() const noexcept -> bool
   {
      return empty(head_.load(std::memory_order_acquire), tail_.load(std::memory_order_acquire));
   }

   /**
    * @brief Gets the approximate size of the queue.
    * @return The capacity.
    * @note Due to the nature of concurrency, this is an approximate value.
    */
   [[nodiscard]] auto size() const noexcept -> std::size_t
   {
      return size(head_.load(std::memory_order_acquire), tail_.load(std::memory_order_acquire));
   }

   /**
    * @brief Checks if the queue is full.
    * @return True if full, false otherwise.
    * @note Due to the nature of concurrency, this is an approximate value.
    */
   [[nodiscard]] auto full() const noexcept -> bool
   {
      return full(head_.load(std::memory_order_acquire), tail_.load(std::memory_order_acquire));
   }

   /**
    * @brief Gets the approximate number of available slots in the queue.
    * @return The number of available slots is given by capacity() - size().
    * @note Due to the nature of concurrency, this is an approximate value.
    */
   [[nodiscard]] auto available() const noexcept -> std::size_t
   {
      return available(
         head_.load(std::memory_order_acquire), tail_.load(std::memory_order_acquire));
   }

   /**
    * @brief Tries to emplace an element into the queue.
    * @param args Arguments for the constructor of the element.
    * @return True if emplace was successful, and the queue was not full, false otherwise.
    */
   template <typename... Args>
   auto try_emplace(Args&&... args) noexcept(std::is_nothrow_constructible_v<T, Args...>) -> bool
      requires std::constructible_from<T, Args...>
   {
      auto tail = tail_.load(std::memory_order_relaxed);
      if (full(cached_head_, tail))
      {
         cached_head_ = head_.load(std::memory_order_acquire);
         if (full(cached_head_, tail))
         {
            return false;
         }
      }

      emplace_at(tail, std::forward<Args>(args)...);
      tail_.store(tail + 1, std::memory_order_release);
      return true;
   }

   /**
    * @brief Tries to enqueue an exisitng element into the queue.
    * @param elem The element to enqueue.
    * @return True if enqueue was successful, and the queue was not full, false otherwise.
    */
   template <typename E>
   auto try_enqueue(E&& elem) noexcept(std::is_nothrow_constructible_v<T>) -> bool
      requires std::convertible_to<E, T>
   {
      return try_emplace(std::forward<E>(elem));
   }

   /**
    * @brief Tries to dequeue an element off the queue.
    * @return If the queue is not empty, an optional containing the deququed element,
    * otherwise std::nullupt.
    */
   [[nodiscard]]
   auto try_dequeue() noexcept(std::is_nothrow_move_constructible_v<T>) -> std::optional<T>
      requires std::is_move_constructible_v<T>
   {
      auto head = head_.load(std::memory_order_relaxed);
      if (empty(head, cached_tail_))
      {
         cached_tail_ = tail_.load(std::memory_order_acquire);
         if (empty(head, cached_tail_))
         {
            return {};
         }
      }

      auto result = std::make_optional(dequeue_at(head));
      head_.store(head + 1, std::memory_order_release);
      return result;
   }

   /**
    * @brief Tries to dequeue an element off the queue.
    * @param result The object to write the dequeued element to.
    * @return True if there was an element to dequeue, false otherwise.
    */
   [[nodiscard]]
   auto try_dequeue(T& result) noexcept(std::is_nothrow_move_constructible_v<T>) -> bool
      requires std::is_move_constructible_v<T>
   {
      // Duplicates the logic in the optional form of try_dequeue. We could have that function call
      // this function, but then that would require T to be default constructible, and would add an
      // extra move.
      auto head = head_.load(std::memory_order_relaxed);
      if (empty(head, cached_tail_))
      {
         cached_tail_ = tail_.load(std::memory_order_acquire);
         if (empty(head, cached_tail_))
         {
            return false;
         }
      }

      result = std::move(dequeue_at(head));
      head_.store(head + 1, std::memory_order_release);
      return true;
   }

   /**
    * @brief Tries to enqueue a range of elements into the queue.
    *
    * This only enqueues the elements of the range if there is enough space in the queue for all of
    * them. If there isn't enough space, this resturns false and leaves the range unchanged.
    *
    * @param range A range of elements to enqueue.
    * @return True if the entire rnage was enqueued, false otherwise.
    */
   template <std::ranges::input_range R>
   auto try_enqueue_bulk(R&& range) -> bool
      requires std::convertible_to<std::ranges::range_reference_t<R>, T> and
               std::ranges::sized_range<R>
   {
      auto tail = tail_.load(std::memory_order_relaxed);
      auto range_size = std::ranges::size(range);

      if (available(cached_head_, tail) < range_size)
      {
         cached_head_ = head_.load(std::memory_order_acquire);
         if (available(cached_head_, tail) < range_size)
         {
            return false;
         }
      }

      std::ranges::for_each(
         std::forward<R>(range),
         [&](auto&& elem) { emplace_at(tail++, std::forward<decltype(elem)>(elem)); });

      tail_.store(tail, std::memory_order_release);
      return true;
   }

   /**
    * @brief Tries to dequeue a number of elements from the queue.
    *
    * Tries to dequeue a number of elements from the queue. If there are fewer in the queue than
    * requested, this will dequeue however many elements are in the queue up to the number
    * requested. This returns the number of elements dequeued.
    *
    * @param out Output iterator to write to.
    * @param num_elements Number of elements to dequeue.
    * @return The number of elements that were dequeued.
    */
   template <typename O>
   [[nodiscard]]
   auto try_dequeue_bulk(O out, std::size_t num_elements) -> std::size_t
      requires std::weakly_incrementable<O> and std::indirectly_writable<O, T>
   {
      auto head = head_.load(std::memory_order_relaxed);
      if (size(head, cached_tail_) < num_elements)
      {
         cached_tail_ = tail_.load(std::memory_order_acquire);
         if (empty(head, cached_tail_))
         {
            return 0;
         }
      }

      auto dequeue_count = std::min(size(head, cached_tail_), num_elements);

      repeat([&]() { *out++ = dequeue_at(head++); }, dequeue_count);
      head_.store(head, std::memory_order_release);
      return dequeue_count;
   }

   /**
    * @brief Destructs all the elements in the queue.
    */
   ~static_queue()
   {
      while (try_dequeue())
      {
      };
   }

 private:
   /**
    * @brief Emplaces an element at a specified tail index.
    * @param tail The tail index to emplace a new object.
    * @param args The arguents to pass to the element's constructor.
    */
   template <typename... Args>
   auto emplace_at(std::size_t tail, Args&&... args) noexcept(
      std::is_nothrow_constructible_v<T, Args...>) -> void
      requires std::constructible_from<T, Args...>
   {
      data_[wrap(tail)].construct(std::forward<Args>(args)...);
   }

   /**
    * @brief Dequques an element at a specified head index.
    * @param head The index of the element to dequeue.
    * @return The dequeued element.
    * @note This does not verify that an element actually exists at the head index.
    */
   auto dequeue_at(std::size_t head) noexcept(std::is_nothrow_move_constructible_v<T>) -> T
      requires std::is_move_constructible_v<T>
   {
      auto& slot = data_[wrap(head)];
      auto elem{std::move(slot.get())};
      slot.destroy();
      return elem;
   }

   /**
    * @brief Wraps an integer index mod N (where N is the capacity).
    * @detail This only works for integers that are powers of 2.
    * @param index The linear index to wrap around.
    * #return The wrapped index.
    */
   [[nodiscard]] static auto wrap(std::size_t index) noexcept -> std::size_t
   {
      return index & (capacity() - 1);
   }

   /**
    * @brief Checks if the queue is empty.
    * @param head The current head index.
    * @param tail The current tail index.
    * @return True if the queue is empty, false otherwise.
    */
   [[nodiscard]] static auto empty(std::size_t head, std::size_t tail) noexcept -> bool
   {
      return tail == head;
   }

   /**
    * @brief Checks if the queue is full.
    * @param head The current head index.
    * @param tail The current tail index.
    * @return True if the queue is full, false otherwise.
    */
   [[nodiscard]] static auto full(std::size_t head, std::size_t tail) noexcept -> bool
   {
      return size(head, tail) == capacity();
   }

   /**
    * @brief Gets the number of elements in the container.
    * @param head The current head index.
    * @param tail The current tail index.
    * @return The number of elements in the queue..
    */
   [[nodiscard]] static auto size(std::size_t head, std::size_t tail) noexcept -> std::size_t
   {
      return tail - head;
   }

   /**
    * @brief Gets the number of spaces available in the queue.
    * @param head The current head index.
    * @param tail The current tail index.
    * @return The number of available slots: capacity() - size().
    */
   [[nodiscard]] static auto available(std::size_t head, std::size_t tail) noexcept -> std::size_t
   {
      return capacity() - size(head, tail);
   }

   // Queue elements.
   alignas(constants::cache_line_size) storage_type data_;

   // Front index of the queue.
   alignas(constants::cache_line_size) std::atomic_size_t head_{0};
   // Back index of the queue.
   alignas(constants::cache_line_size) std::atomic_size_t tail_{0};
   // Cached front index of the queue to reduce the number of acquiresd
   alignas(constants::cache_line_size) std::size_t cached_head_{0};
   // Cached back index of the queue to reduce the number of acquires.
   alignas(constants::cache_line_size) std::size_t cached_tail_{0};
};

} // namespace cxxlab::spsc
