/**
 * @copyright Copyright (c) 2026, Devin DeLong. All rights reserved.
 *
 * @license This code is distributed under the BSD 3-Clause License.
 *          See the LICENSE file for the full text of the license.
 *
 * @author Devin DeLong
 */

#include "cxxlab/lockfree/spsc/static_queue.hpp"

#include <catch2/benchmark/catch_benchmark.hpp>
#include <catch2/catch_test_macros.hpp>

#include <iterator>
#include <ranges>
#include <thread>
#include <vector>

TEST_CASE("spsc::static_queue: capacity is power of 2", "[spsc][queue][capacity]")
{
   auto queue = cxxlab::spsc::static_queue<int, 7>{};
   STATIC_CHECK(queue.capacity() == 8);

   auto queue2 = cxxlab::spsc::static_queue<int, 8>{};
   STATIC_CHECK(queue2.capacity() == 8);
}

TEST_CASE("spsc::static_queue: correct size tracking", "[spsc][queue][size]")
{
   auto queue = cxxlab::spsc::static_queue<int, 4>{};
   auto result = 0;

   // Enqueue

   // No elements to dequeue.
   CHECK(queue.empty());
   CHECK_FALSE(queue.full());
   CHECK(queue.size() == 0);
   CHECK(queue.available() == 4);
   CHECK_FALSE(queue.try_dequeue());

   // Acxxlab element 1
   CHECK(queue.try_enqueue(0));
   CHECK_FALSE(queue.empty());
   CHECK_FALSE(queue.full());
   CHECK(queue.size() == 1);
   CHECK(queue.available() == 3);

   // Acxxlab element 2
   CHECK(queue.try_enqueue(1));
   CHECK_FALSE(queue.empty());
   CHECK_FALSE(queue.full());
   CHECK(queue.size() == 2);
   CHECK(queue.available() == 2);

   // Acxxlab element 3
   CHECK(queue.try_enqueue(2));
   CHECK_FALSE(queue.empty());
   CHECK_FALSE(queue.full());
   CHECK(queue.size() == 3);
   CHECK(queue.available() == 1);

   // Acxxlab element 4
   CHECK(queue.try_enqueue(3));
   CHECK_FALSE(queue.empty());
   CHECK(queue.full());
   CHECK(queue.size() == 4);
   CHECK(queue.available() == 0);

   // Queue is full.
   CHECK_FALSE(queue.try_enqueue(4));

   // Sizes did not change.
   CHECK_FALSE(queue.empty());
   CHECK(queue.full());
   CHECK(queue.size() == 4);
   CHECK(queue.available() == 0);

   // Dequeue

   // Dequeue element 1
   CHECK(queue.try_dequeue(result));
   CHECK(result == 0);
   CHECK_FALSE(queue.empty());
   CHECK_FALSE(queue.full());
   CHECK(queue.size() == 3);
   CHECK(queue.available() == 1);

   // Dequeue element 2
   CHECK(queue.try_dequeue(result));
   CHECK(result == 1);
   CHECK_FALSE(queue.empty());
   CHECK_FALSE(queue.full());
   CHECK(queue.size() == 2);
   CHECK(queue.available() == 2);

   // Dequeue element 3
   CHECK(queue.try_dequeue(result));
   CHECK(result == 2);
   CHECK_FALSE(queue.empty());
   CHECK_FALSE(queue.full());
   CHECK(queue.size() == 1);
   CHECK(queue.available() == 3);

   // Dequeue element 4
   CHECK(queue.try_dequeue(result));
   CHECK(result == 3);
   CHECK(queue.empty());
   CHECK_FALSE(queue.full());
   CHECK(queue.size() == 0);
   CHECK(queue.available() == 4);

   // Empty, cannot dequeue anymore.
   CHECK_FALSE(queue.try_dequeue());
}

TEST_CASE("spsc::static_queue: single thread try_enqueue and try_dequeue", "[spsc][queue]")
{
   auto queue = cxxlab::spsc::static_queue<int, 4>{};

   CHECK_FALSE(queue.try_dequeue());
   CHECK(queue.try_enqueue(0));
   CHECK(queue.try_enqueue(1));
   CHECK(queue.try_enqueue(2));
   CHECK(queue.try_enqueue(3));
   CHECK_FALSE(queue.try_enqueue(4));
   CHECK(queue.size() == 4);
   CHECK_FALSE(queue.empty());
   CHECK(queue.full());

   auto val = queue.try_dequeue();
   REQUIRE(val);
   CHECK(val == 0);
   CHECK(queue.size() == 3);
   CHECK_FALSE(queue.empty());
   CHECK_FALSE(queue.full());

   val = queue.try_dequeue();
   REQUIRE(val);
   CHECK(val == 1);
   CHECK(queue.size() == 2);
   CHECK_FALSE(queue.empty());
   CHECK_FALSE(queue.full());

   val = queue.try_dequeue();
   REQUIRE(val);
   CHECK(val == 2);
   CHECK(queue.size() == 1);
   CHECK_FALSE(queue.empty());
   CHECK_FALSE(queue.full());

   val = queue.try_dequeue();
   REQUIRE(val);
   CHECK(val == 3);
   CHECK(queue.size() == 0);
   CHECK(queue.empty());
   CHECK_FALSE(queue.full());

   val = queue.try_dequeue();
   CHECK_FALSE(val);
}

TEST_CASE("spsc::static_queue: single-threaded try_enqueue_bulk", "[spsc][queue]")
{
   auto queue = cxxlab::spsc::static_queue<int, 4>{};

   CHECK(queue.try_enqueue_bulk(std::views::iota(10, 14)));
   CHECK(queue.try_dequeue().value_or(0) == 10);
   CHECK(queue.try_dequeue().value_or(0) == 11);
   CHECK(queue.try_dequeue().value_or(0) == 12);
   CHECK(queue.try_dequeue().value_or(0) == 13);

   // Too many elements for the queue.
   CHECK_FALSE(queue.try_enqueue_bulk(std::views::iota(10, 15)));
}

TEST_CASE("spsc::static_queue: single-threaded try_dequeue_bulk", "[spsc][queue]")
{
   auto queue = cxxlab::spsc::static_queue<int, 4>{};
   auto elements = std::views::iota(10, 14);
   auto result = std::array<int, 4>{0, 0, 0, 0};

   CHECK(queue.try_dequeue_bulk(result.begin(), 10) == 0);
   CHECK(queue.try_enqueue_bulk(elements));

   CHECK(queue.try_dequeue_bulk(result.begin(), 6) == 4);
   CHECK(result[0] == 10);
   CHECK(result[1] == 11);
   CHECK(result[2] == 12);
   CHECK(result[3] == 13);
}

TEST_CASE("spsc::static_queue: concurrent try_enqueue and try_dequeue", "[spsc][queue][concurrent]")
{
   constexpr auto N = 1024;
   auto queue = cxxlab::spsc::static_queue<int, 4>{};

   auto expected = std::views::iota(0, N);
   auto result = std::vector<int>{};
   result.reserve(N);

   std::jthread producer(
      [&]
      {
         for (auto i : std::views::iota(0, N))
         {
            while (not queue.try_enqueue(i))
            {
            };
         }
      });

   std::jthread consumer(
      [&]
      {
         for ([[maybe_unused]] auto _ : std::views::iota(0, N))
         {
            auto elem = std::optional<int>{};
            while (not(elem = queue.try_dequeue()))
            {
            }
            result.push_back(elem.value());
         }
      });

   producer.join();
   consumer.join();

   CHECK(std::ranges::equal(result, expected));
}

TEST_CASE(
   "spsc::static_queue: concurrent try_enqueue_bulk and try_dequeue_bulk",
   "[spsc][queue][concurrent]")
{
   constexpr auto BULK_SIZE = 8;
   constexpr auto N = 1024;
   constexpr auto QUEUE_SIZE = 2 * BULK_SIZE;
   constexpr auto N_ITERATIONS = N / BULK_SIZE;

   auto queue = cxxlab::spsc::static_queue<int, QUEUE_SIZE>{};

   auto expected = std::views::iota(0, N);
   auto result = std::vector<int>{};
   result.reserve(N);

   std::jthread producer(
      [&]
      {
         for (auto i : std::views::iota(0, N_ITERATIONS))
         {
            auto begin = BULK_SIZE * i;
            auto end = begin + BULK_SIZE;
            while (not queue.try_enqueue_bulk(std::views::iota(begin, end)))
            {
            };
         }
      });

   std::jthread consumer(
      [&]
      {
         for ([[maybe_unused]] auto _ : std::views::iota(0, N_ITERATIONS))
         {
            while (queue.try_dequeue_bulk(std::back_inserter(result), BULK_SIZE) == 0)
            {
            }
         }
      });

   producer.join();
   consumer.join();

   CHECK(std::ranges::equal(result, expected));
}
