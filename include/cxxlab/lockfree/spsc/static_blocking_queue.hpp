/**
 * @copyright Copyright (c) 2026, Devin DeLong. All rights reserved.
 *
 * @license This code is distributed under the BSD 3-Clause License.
 *          See the LICENSE file for the full text of the license.
 *
 * @author Devin DeLong
 */

#pragma once

#include "cxxlab/concepts/chrono.hpp"
#include "cxxlab/lockfree/spsc/static_queue.hpp"
#include "cxxlab/memory/cache_line_size.hpp"

#include <algorithm>
#include <chrono>
#include <concepts>
#include <cstddef>
#include <optional>
#include <semaphore>
#include <utility>

namespace cxxlab::spsc
{

/**
 * @brief Statically-sized, lock-free, Single-Consumer-Single-Producer queue with blocking
 * functionality.
 *
 * @tparam T Element type.
 * @tparam Capacity Maximum number of elements can store.
 *
 * @details The actual capacity used will be a power of 2 large enough to hold the specified
 * capacity.
 *
 * @note There is a deadlock bug in std::counting_semaphore that might not be fixed until gcc-16:
 * https://gcc.gnu.org/bugzilla/show_bug.cgi?id=104928. It's possible that it could be fixed in
 * gcc-15.2, but unsure of exactly which versions have the patch.
 */
template <typename T, std::size_t Capacity>
class static_blocking_queue
{
   // Lock-free, statically-sized queue.
   using queue_type = static_queue<T, Capacity>;
   // The semaphore used.
   using semaphore_type = std::counting_semaphore<queue_type::capacity()>;
   // Type to indicate there is no wait argument to try_acquire.
   struct no_wait_time
   {
   };

 public:
   /**
    * @brief Gets the capacity.
    * @return The capacity.
    */
   [[nodiscard]] static consteval auto capacity() noexcept -> std::size_t
   {
      return queue_type::capacity();
   }

   /**
    * @brief Checks if the queue is empty.
    * @return True if empty, false otherwise.
    * @details Due to the nature of concurrency, this is an approximate value.
    */
   [[nodiscard]] auto empty() const noexcept -> bool { return queue_.empty(); }

   /**
    * @brief Gets number of elements in the queue.
    * @return Number of elements in the queue.
    * @details Due to the nature of concurrency, this is an approximate value.
    */
   [[nodiscard]] auto size() const noexcept -> std::size_t { return queue_.size(); }

   /**
    * @brief Checks if the queue is full.
    * @return True if full, false otherwise.
    * @details Due to the nature of concurrency, this is an approximate value.
    */
   [[nodiscard]] auto full() const noexcept -> bool { return queue_.full(); }

   /**
    * @brief Gets the number of available slots in the queue.
    * @return The Number of available slots.
    * @details Due to the nature of concurrency, this is an approximate value.
    */
   [[nodiscard]] auto available() const noexcept -> std::size_t { return queue_.available(); }

   /**
    * @brief Tries to emplace an element into the queue.
    * @param args Arguments for the constructor of T.
    * @return True on success, false otherwise (the queue was full).
    */
   template <typename... Args>
   auto try_emplace(Args&&... args) -> bool
      requires std::constructible_from<T, Args...>
   {
      return try_emplace_impl_(no_wait_time{}, std::forward<Args>(args)...);
   }

   /**
    * @brief Tries to emplace an element into the queue for a given time duration.
    *
    * If the queue is full, then this will wait until the specified time duration has passed or a
    * slot in the queue has become available, whichever comes first.
    *
    * @param timeout The time to wait for the enqueue to succeed.
    * @param args Arguments for the constructor of T.
    *
    * @return True if the enqueue was successful, false otherwise.
    */
   template <typename... Args>
   [[nodiscard]] auto try_emplace_for(chrono_duration auto timeout, Args&&... args) -> bool
      requires std::constructible_from<T, Args...>
   {
      return try_emplace_impl_(timeout, std::forward<Args>(args)...);
   }

   /**
    * @brief Tries to emplace an element into the queue until a given absolute time.
    *
    * If the queue is full, then this will wait until the specified time point or a slot in the
    * queue has become available, whichever comes first.
    *
    * @param time_point The absolute time to wait until for the enqueue to succeed.
    * @param args Arguments for the constructor of T.
    *
    * @return True if the enqueue was successful, false otherwise.
    */
   template <typename... Args>
   [[nodiscard]] auto try_emplace_until(chrono_time_point auto time_point, Args&&... args) -> bool
      requires std::constructible_from<T, Args...>
   {
      return try_emplace_impl_(time_point, std::forward<Args>(args)...);
   }

   /**
    * @brief Tries to enqueue an element into the queue.
    * @param elem The element to enqueue.
    * @return True on success, false otherwise (the queue was full).
    */
   template <typename Elem>
   auto try_enqueue(Elem&& elem) -> bool
      requires std::convertible_to<Elem, T>
   {
      return try_enqueue_impl_(no_wait_time{}, std::forward<Elem>(elem));
   }

   /**
    * @brief Tries to enqueue an element into the queue for a given time duration.
    *
    * If the queue is full, then this will wait until the specified time duration has passed or a
    * slot in the queue has become available, whichever comes first.
    *
    * @param timeout The time to wait for the enqueue to succeed.
    * @param elem The element to enqueue.
    *
    * @return True if the enqueue was successful, false otherwise.
    */
   template <typename Elem>
   [[nodiscard]] auto try_enqueue_for(chrono_duration auto timeout, Elem&& elem) -> bool
      requires std::convertible_to<Elem, T>
   {
      return try_enqueue_impl_(timeout, std::forward<Elem>(elem));
   }

   /**
    * @brief Tries to enqueue an element into the queue until a given absolute time.
    *
    * If the queue is full, then this will wait until the specified time point or a slot in the
    * queue has become available, whichever comes first.
    *
    * @param time_point The absolute time to wait until for the enqueue to succeed.
    * @param elem The element to enqueue.
    *
    * @return True if the enqueue was successful, false otherwise.
    */
   template <typename Elem>
   [[nodiscard]] auto try_enqueue_until(chrono_time_point auto time_point, Elem&& elem) -> bool
      requires std::convertible_to<Elem, T>
   {
      return try_enqueue_impl_(time_point, std::forward<Elem>(elem));
   }

   /**
    * @brief Tries to dequeue an element off the queue.
    * @param result The object to write the dequeued element to.
    * @return True if there was an element to dequeue, false otherwise.
    */
   [[nodiscard]] auto try_dequeue(T& result) -> bool
   {
      return reference_try_dequeue_impl_(no_wait_time{}, result);
   }

   /**
    * @brief Tries to dequeue an element off the queue for a given relative duration.
    *
    * If there are no elements to dequeue, then this will wait until the specified duration
    * has passed or a new element was enqueued into the queue, whichever comes first.
    *
    * @param timeout The time to wait for the dequeue to succeed.
    * @param result The object to write the dequeued element to.
    *
    * @return True if successful, false otherwise.
    */
   [[nodiscard]] auto try_dequeue_for(chrono_duration auto timeout, T& result) -> bool
   {
      return reference_try_dequeue_impl_(timeout, result);
   }

   /**
    * @brief Tries to dequeue an element off the queue before a given absolute time point.
    *
    * If there are no elements to dequeue, then this will wait until the specified absolute
    * time point has passed or a new element was enqueued into the queue, whichever comes
    * first.
    *
    * @param time_point The absolute time to wait until for the dequeue to succeed.
    * @param result The object to write the dequeued element to.
    *
    * @return True if successful, false otherwise..
    */
   [[nodiscard]] auto try_dequeue_until(chrono_time_point auto time_point, T& result) -> bool
   {
      return reference_try_dequeue_impl_(time_point, result);
   }

   /**
    * @brief Tries to dequeue an element off the queue.
    * @return If the queue is not empty, an optional containing the dequeued element,
    * otherwise std::nullupt.
    */
   [[nodiscard]] auto try_dequeue() -> std::optional<T>
   {
      return optional_try_dequeue_impl_(no_wait_time{});
   }

   /**
    * @brief Tries to dequeue an element off the queue for a given relative duration.
    *
    * If there are no elements to dequeue, then this will wait until the specified duration
    * has passed or a new element was enqueued into the queue, whichever comes first.
    *
    * @param timeout The time to wait for the dequeue to succeed.
    *
    * @return If the queue is not empty, an optional containing the dequeued element,
    * otherwise std::nullopt.
    */
   [[nodiscard]] auto try_dequeue_for(chrono_duration auto timeout) -> std::optional<T>
   {
      return optional_try_dequeue_impl_(timeout);
   }

   /**
    * @brief Tries to dequeue an element off the queue before a given absolute time point.
    *
    * If there are no elements to dequeue, then this will wait until the specified absolute
    * time point has passed or a new element was enqueued into the queue, whichever comes
    * first.
    *
    * @param time_point The absolute time to wait until for the dequeue to succeed.
    *
    * @return If the queue is not empty, an optional containing the dequeued element,
    * otherwise std::nullupt.
    */
   [[nodiscard]] auto try_dequeue_until(chrono_time_point auto time_point) -> std::optional<T>
   {
      return optional_try_dequeue_impl_(time_point);
   }

   /**
    * @brief Tries to enqueue an element into the queue and blocks while it is full.
    * @param elem The element to enqueue.
    */
   template <typename Elem>
   auto enqueue(Elem&& elem) -> void
      requires std::convertible_to<Elem, T>
   {
      spaces_sem_.acquire();
      queue_.try_enqueue(std::forward<Elem>(elem));
      items_sem_.release();
   }

   /**
    * @brief Dequeues an element off the queue and blocks while the queue is empty.
    * @return If the queue is not empty, an optional containing the dequeued element,
    * otherwise std::nullopt.
    */
   [[nodiscard]] auto dequeue() -> T
   {
      items_sem_.acquire();
      auto elem = queue_.try_dequeue();
      spaces_sem_.release();
      return std::move(elem).value();
   }

 private:
   /**
    * @brief Calls try_acquire_for and protects against spurious failures.
    *
    * @param sem The semaphore to call acquire on.
    * @param timeout A chrono time duration.
    *
    * @return True if the acquire was successful.
    */
   [[nodiscard]] auto try_acquire_for_(semaphore_type& sem, chrono_duration auto timeout)
   {
      // Protect against spurious failures in counting_semaphore::try_acquire_for.
      return try_acquire_until_(sem, std::chrono::steady_clock::now() + timeout);
   }

   /**
    * @brief Calls try_acquire_until and protects against spurious failures.
    *
    * @param sem The semaphore to call acquire on.
    * @param time_point A chrono time_point.
    *
    * @return True if the acquire was successful.
    */
   template <typename Clock, typename Duration>
   [[nodiscard]] auto
   try_acquire_until_(semaphore_type& sem, std::chrono::time_point<Clock, Duration> time_point)
      -> bool
   {
      // Protect against spurious failures in counting_semaphore.
      do
      {
         if (sem.try_acquire_until(time_point))
         {
            return true;
         }
      } while (Clock::now() < time_point);
      return false;
   }

   /**
    * @brief Calls a variation of semaphore.try_acquire().
    *
    * This will call the no-parameter version of sem.try_acquire() if no_wait_time is passed to
    * time_spec. Otherwise this calls either sem.try_acquire_for or sem.try_acquire_until if a
    * chrono duration or chrono time_point is passed, respectively.
    *
    * @param sem The semaphore to call acquire on.
    * @param time_spec Either duration, time_point, or no_wait_time.
    *
    * @return True if the acquire was successful.
    */
   template <typename Time>
   [[nodiscard]] auto try_acquire_(semaphore_type& sem, Time time_spec) -> bool
   {
      if constexpr (std::same_as<Time, no_wait_time>)
      {
         return sem.try_acquire();
      }
      else if constexpr (chrono_duration<Time>)
      {
         return try_acquire_for_(sem, time_spec);
      }
      else if constexpr (chrono_time_point<Time>)
      {
         return try_acquire_until_(sem, time_spec);
      }
   }

   /**
    * @brief Tries to emplace an element into the queue.
    *
    * @param time_spec Either duration, time_point, or no_wait_time.
    * @param args Arguments for the constructor of T.
    *
    * @return True if the enqueue was successful, false otherwise.
    */
   template <typename Time, typename... Args>
   [[nodiscard]] auto try_emplace_impl_(Time time_spec, Args&&... args) -> bool
      requires std::constructible_from<T, Args...>
   {
      if (not try_acquire_(spaces_sem_, time_spec))
      {
         return false;
      }

      auto success = queue_.try_emplace(std::forward<Args>(args)...);
      items_sem_.release();
      return success;
   }

   /**
    * @brief Tries to emplace an element into the queue.
    *
    * @param time_spec Either duration, time_point, or no_wait_time.
    * @param elem The element to enqueue.
    *
    * @return True if the enqueue was successful, false otherwise.
    */
   template <typename Time, typename Elem>
   [[nodiscard]] auto try_enqueue_impl_(Time time_spec, Elem&& elem) -> bool
      requires std::convertible_to<Elem, T>
   {
      if (not try_acquire_(spaces_sem_, time_spec))
      {
         return false;
      }

      auto success = queue_.try_enqueue(std::forward<Elem>(elem));
      items_sem_.release();
      return success;
   }

   /**
    * @brief Tries to dequeue an element off the queue.
    *
    * @param time_spec Either duration, time_point, or no_wait_time.
    * @param result The object to write the dequeued element to.
    *
    * @return True if there was an element to dequeue, false otherwise.
    */
   template <typename Time>
   [[nodiscard]] auto reference_try_dequeue_impl_(Time time_spec, T& result) -> bool
   {
      if (not try_acquire_(items_sem_, time_spec))
      {
         return false;
      }

      auto success = queue_.try_dequeue(result);
      spaces_sem_.release();
      return success;
   }

   /**
    * @brief Tries to dequeue an element off the queue.
    *
    * @param time_spec Either duration, time_point, or no_wait_time.
    * @param time_point The absolute time to wait until for the dequeue to succeed.
    *
    * @return If the queue is not empty, an optional containing the dequeued element,
    * otherwise std::nullopt.
    */
   template <typename Time>
   [[nodiscard]] auto optional_try_dequeue_impl_(Time time_spec) -> std::optional<T>
   {
      if (not try_acquire_(items_sem_, time_spec))
      {
         return {};
      }

      auto elem = queue_.try_dequeue();
      spaces_sem_.release();
      return elem;
   }

   // Queue.
   alignas(constants::cache_line_size) queue_type queue_;
   // Items available for dequeue.
   alignas(constants::cache_line_size) semaphore_type items_sem_{0};
   // Spaces available for enqueue.
   alignas(constants::cache_line_size) semaphore_type spaces_sem_{capacity()};
};
} // namespace cxxlab::spsc
