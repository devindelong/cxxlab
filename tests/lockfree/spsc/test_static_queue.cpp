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
   REQUIRE(queue.capacity() == 8);

   auto queue2 = cxxlab::spsc::static_queue<int, 8>{};
   REQUIRE(queue2.capacity() == 8);
}

TEST_CASE("spsc::static_queue: correct size tracking", "[spsc][queue][size]")
{
   auto queue = cxxlab::spsc::static_queue<int, 4>{};
   auto result = 0;

   // Enqueue

   // No elements to dequeue.
   REQUIRE(queue.empty());
   REQUIRE_FALSE(queue.full());
   REQUIRE(queue.size() == 0);
   REQUIRE(queue.available() == 4);
   REQUIRE_FALSE(queue.try_dequeue());

   // Acxxlab element 1
   REQUIRE(queue.try_enqueue(0));
   REQUIRE_FALSE(queue.empty());
   REQUIRE_FALSE(queue.full());
   REQUIRE(queue.size() == 1);
   REQUIRE(queue.available() == 3);

   // Acxxlab element 2
   REQUIRE(queue.try_enqueue(1));
   REQUIRE_FALSE(queue.empty());
   REQUIRE_FALSE(queue.full());
   REQUIRE(queue.size() == 2);
   REQUIRE(queue.available() == 2);

   // Acxxlab element 3
   REQUIRE(queue.try_enqueue(2));
   REQUIRE_FALSE(queue.empty());
   REQUIRE_FALSE(queue.full());
   REQUIRE(queue.size() == 3);
   REQUIRE(queue.available() == 1);

   // Acxxlab element 4
   REQUIRE(queue.try_enqueue(3));
   REQUIRE_FALSE(queue.empty());
   REQUIRE(queue.full());
   REQUIRE(queue.size() == 4);
   REQUIRE(queue.available() == 0);

   // Queue is full.
   REQUIRE_FALSE(queue.try_enqueue(4));

   // Sizes did not change.
   REQUIRE_FALSE(queue.empty());
   REQUIRE(queue.full());
   REQUIRE(queue.size() == 4);
   REQUIRE(queue.available() == 0);

   // Dequeue

   // Dequeue element 1
   REQUIRE(queue.try_dequeue(result));
   REQUIRE(result == 0);
   REQUIRE_FALSE(queue.empty());
   REQUIRE_FALSE(queue.full());
   REQUIRE(queue.size() == 3);
   REQUIRE(queue.available() == 1);

   // Dequeue element 2
   REQUIRE(queue.try_dequeue(result));
   REQUIRE(result == 1);
   REQUIRE_FALSE(queue.empty());
   REQUIRE_FALSE(queue.full());
   REQUIRE(queue.size() == 2);
   REQUIRE(queue.available() == 2);

   // Dequeue element 3
   REQUIRE(queue.try_dequeue(result));
   REQUIRE(result == 2);
   REQUIRE_FALSE(queue.empty());
   REQUIRE_FALSE(queue.full());
   REQUIRE(queue.size() == 1);
   REQUIRE(queue.available() == 3);

   // Dequeue element 4
   REQUIRE(queue.try_dequeue(result));
   REQUIRE(result == 3);
   REQUIRE(queue.empty());
   REQUIRE_FALSE(queue.full());
   REQUIRE(queue.size() == 0);
   REQUIRE(queue.available() == 4);

   // Empty, cannot dequeue anymore.
   REQUIRE_FALSE(queue.try_dequeue());
}

TEST_CASE("spsc::static_queue: single thread try_enqueue and try_dequeue", "[spsc][queue]")
{
   auto queue = cxxlab::spsc::static_queue<int, 4>{};

   REQUIRE_FALSE(queue.try_dequeue());
   REQUIRE(queue.try_enqueue(0));
   REQUIRE(queue.try_enqueue(1));
   REQUIRE(queue.try_enqueue(2));
   REQUIRE(queue.try_enqueue(3));
   REQUIRE_FALSE(queue.try_enqueue(4));
   REQUIRE(queue.size() == 4);
   REQUIRE_FALSE(queue.empty());
   REQUIRE(queue.full());

   auto val = queue.try_dequeue();
   REQUIRE(val);
   REQUIRE(val == 0);
   REQUIRE(queue.size() == 3);
   REQUIRE_FALSE(queue.empty());
   REQUIRE_FALSE(queue.full());

   val = queue.try_dequeue();
   REQUIRE(val);
   REQUIRE(val == 1);
   REQUIRE(queue.size() == 2);
   REQUIRE_FALSE(queue.empty());
   REQUIRE_FALSE(queue.full());

   val = queue.try_dequeue();
   REQUIRE(val);
   REQUIRE(val == 2);
   REQUIRE(queue.size() == 1);
   REQUIRE_FALSE(queue.empty());
   REQUIRE_FALSE(queue.full());

   val = queue.try_dequeue();
   REQUIRE(val);
   REQUIRE(val == 3);
   REQUIRE(queue.size() == 0);
   REQUIRE(queue.empty());
   REQUIRE_FALSE(queue.full());

   val = queue.try_dequeue();
   REQUIRE_FALSE(val);
}

TEST_CASE("spsc::static_queue: single-threaded try_enqueue_bulk", "[spsc][queue]")
{
   auto queue = cxxlab::spsc::static_queue<int, 4>{};

   REQUIRE(queue.try_enqueue_bulk(std::views::iota(10, 14)));
   REQUIRE(queue.try_dequeue().value_or(0) == 10);
   REQUIRE(queue.try_dequeue().value_or(0) == 11);
   REQUIRE(queue.try_dequeue().value_or(0) == 12);
   REQUIRE(queue.try_dequeue().value_or(0) == 13);

   // Too many elements for the queue.
   REQUIRE_FALSE(queue.try_enqueue_bulk(std::views::iota(10, 15)));
}

TEST_CASE("spsc::static_queue: single-threaded try_dequeue_bulk", "[spsc][queue]")
{
   auto queue = cxxlab::spsc::static_queue<int, 4>{};
   auto elements = std::views::iota(10, 14);
   auto result = std::array<int, 4>{0, 0, 0, 0};

   REQUIRE(queue.try_dequeue_bulk(result.begin(), 10) == 0);
   REQUIRE(queue.try_enqueue_bulk(elements));

   REQUIRE(queue.try_dequeue_bulk(result.begin(), 6) == 4);
   REQUIRE(result[0] == 10);
   REQUIRE(result[1] == 11);
   REQUIRE(result[2] == 12);
   REQUIRE(result[3] == 13);
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

   REQUIRE(std::ranges::equal(result, expected));
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

   REQUIRE(std::ranges::equal(result, expected));
}
