/**
 * @copyright Copyright (c) 2026, Devin DeLong. All rights reserved.
 *
 * @license This code is distributed under the BSD 3-Clause License.
 *          See the LICENSE file for the full text of the license.
 *
 * @author Devin DeLong
 */

#include "cxxlab/lockfree/spsc/static_blocking_queue.hpp"

#include <catch2/benchmark/catch_benchmark.hpp>
#include <catch2/catch_test_macros.hpp>

#include <chrono>
#include <ranges>
#include <string>
#include <thread>
#include <vector>

using namespace std::chrono_literals;

TEST_CASE("spsc::static_blocking_queue: capacity is power of 2", "[spsc][blocking_queue][capacity]")
{
   auto queue = cxxlab::spsc::static_blocking_queue<int, 7>{};
   STATIC_CHECK(queue.capacity() == 8);

   auto queue2 = cxxlab::spsc::static_blocking_queue<int, 8>{};
   STATIC_CHECK(queue2.capacity() == 8);
}

TEST_CASE("spsc::static_blocking_queue: correct size tracking", "[spsc][blocking_queue][size]")
{
   auto queue = cxxlab::spsc::static_blocking_queue<int, 4>{};
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

TEST_CASE(
   "spsc::static_blocking_queue: single-threaded try_enqueue and try_dequeue",
   "[spsc][blocking_queue]")
{
   auto queue = cxxlab::spsc::static_blocking_queue<int, 4>{};

   CHECK(queue.capacity() == 4);

   CHECK_FALSE(queue.try_dequeue());
   CHECK(queue.try_enqueue(10));
   CHECK(queue.try_enqueue(11));
   CHECK(queue.try_enqueue(12));
   CHECK(queue.try_enqueue(13));
   CHECK_FALSE(queue.try_enqueue(14));
   CHECK(queue.size() == 4);
   CHECK_FALSE(queue.empty());
   CHECK(queue.full());

   auto val = queue.try_dequeue();
   REQUIRE(val);
   CHECK(val == 10);
   CHECK(queue.size() == 3);
   CHECK_FALSE(queue.empty());
   CHECK_FALSE(queue.full());

   val = queue.try_dequeue();
   REQUIRE(val);
   CHECK(val == 11);
   CHECK(queue.size() == 2);
   CHECK_FALSE(queue.empty());
   CHECK_FALSE(queue.full());

   val = queue.try_dequeue();
   REQUIRE(val);
   CHECK(val == 12);
   CHECK(queue.size() == 1);
   CHECK_FALSE(queue.empty());
   CHECK_FALSE(queue.full());

   val = queue.try_dequeue();
   REQUIRE(val);
   CHECK(val == 13);
   CHECK(queue.size() == 0);
   CHECK(queue.empty());
   CHECK_FALSE(queue.full());

   val = queue.try_dequeue();
   CHECK_FALSE(val);
}

TEST_CASE(
   "spsc::static_blocking_queue: concurrent try_enqueue and try_dequeue",
   "[spsc][blocking_queue][concurrent]")
{
   constexpr auto N = 1024;
   auto queue = cxxlab::spsc::static_blocking_queue<int, 4>{};

   auto expected = std::views::iota(0, N);
   auto result = std::vector<int>{};
   result.reserve(N);

   auto producer = std::jthread(
      [&]()
      {
         for (auto i : std::views::iota(0, N))
         {
            while (not queue.try_enqueue(i))
            {
            };
         }
      });

   auto consumer = std::jthread(
      [&]()
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
   "spsc::static_blocking_queue: concurrent, blocking enqueue and dequeue",
   "[spsc][blocking_queue][concurrent]")
{
   constexpr auto N = 1024;
   auto queue = cxxlab::spsc::static_blocking_queue<int, 16>{};

   auto expected = std::views::iota(0, N);
   auto result = std::vector<int>{};
   result.reserve(N);

   auto producer = std::jthread(
      [&]()
      {
         for (auto i : std::views::iota(0, N))
         {
            queue.enqueue(i);
         }
      });

   auto consumer = std::jthread(
      [&]()
      {
         for ([[maybe_unused]] auto _ : std::views::iota(0, N))
         {
            auto elem = queue.dequeue();
            result.push_back(elem);
         }
      });

   producer.join();
   consumer.join();

   CHECK(std::ranges::equal(result, expected));
}

TEST_CASE(
   "spsc::static_blocking_queue: single-threaded try_emplace_for/until times out",
   "[spsc][blocking_queue]")
{
   static constexpr auto timeout = 10ms;
   auto queue = cxxlab::spsc::static_blocking_queue<std::string, 2>{};

   queue.try_enqueue("ten");
   queue.try_enqueue("twenty");
   CHECK(queue.full());

   SECTION("try_emplace_for times out")
   {
      auto start = std::chrono::steady_clock::now();
      auto success = queue.try_emplace_for(timeout, "thirty");

      CHECK(not success);
      CHECK(std::chrono::steady_clock::now() >= (timeout + start));
   }

   SECTION("try_emplace_until times out")
   {
      auto deadline = std::chrono::steady_clock::now() + timeout;
      auto success = queue.try_emplace_until(deadline, "thirty");

      CHECK(not success);
      CHECK(std::chrono::steady_clock::now() >= deadline);
   }
}

TEST_CASE(
   "spsc::static_blocking_queue: concurrent try_emplace_for unblocks and emplaces element",
   "[spsc][blocking_queue][concurrent]")
{
   auto queue = cxxlab::spsc::static_blocking_queue<std::string, 2>{};
   queue.try_emplace("ten");
   queue.try_emplace("twenty");

   auto producer = std::jthread([&queue]() { CHECK(queue.try_emplace_for(1s, "thirty")); });

   std::this_thread::sleep_for(10ms);
   auto elem = queue.try_dequeue_for(1s);
   REQUIRE(elem);
   CHECK(elem == "ten");

   elem = queue.try_dequeue_for(1s);
   REQUIRE(elem);
   CHECK(elem == "twenty");

   elem = queue.try_dequeue_for(1s);
   REQUIRE(elem);
   CHECK(elem == "thirty");
}

TEST_CASE(
   "spsc::static_blocking_queue: concurrent try_emplace_until unblocks and emplaces element",
   "[spsc][blocking_queue][concurrent]")
{
   auto queue = cxxlab::spsc::static_blocking_queue<std::string, 2>{};
   queue.try_emplace("ten");
   queue.try_emplace("twenty");

   auto producer = std::jthread(
      [&queue]()
      { CHECK(queue.try_emplace_until(std::chrono::steady_clock::now() + 1s, "thirty")); });

   std::this_thread::sleep_for(10ms);
   auto elem = queue.try_dequeue_for(1s);
   REQUIRE(elem);
   CHECK(elem == "ten");

   elem = queue.try_dequeue_for(1s);
   REQUIRE(elem);
   CHECK(elem == "twenty");

   elem = queue.try_dequeue_for(1s);
   REQUIRE(elem);
   CHECK(elem == "thirty");
}

TEST_CASE(
   "spsc::static_blocking_queue: single-threaded try_enqueue_for/until times out",
   "[spsc][blocking_queue]")
{
   static constexpr auto timeout = 10ms;
   auto queue = cxxlab::spsc::static_blocking_queue<int, 2>{};

   queue.try_enqueue(10);
   queue.try_enqueue(20);
   CHECK(queue.full());

   SECTION("try_enqueue_for times out")
   {
      auto start = std::chrono::steady_clock::now();
      auto success = queue.try_enqueue_for(timeout, 30);

      CHECK(not success);
      CHECK(std::chrono::steady_clock::now() >= (timeout + start));
   }

   SECTION("try_enqueue_until times out")
   {
      auto deadline = std::chrono::steady_clock::now() + timeout;
      auto success = queue.try_enqueue_until(deadline, 30);

      CHECK(not success);
      CHECK(std::chrono::steady_clock::now() >= deadline);
   }
}

TEST_CASE(
   "spsc::static_blocking_queue: concurrent try_enqueue_for unblocks and enqueues element",
   "[spsc][blocking_queue][concurrent]")
{
   auto queue = cxxlab::spsc::static_blocking_queue<int, 2>{};
   queue.try_enqueue(10);
   queue.try_enqueue(20);

   auto producer = std::jthread([&queue]() { CHECK(queue.try_enqueue_for(1s, 30)); });

   std::this_thread::sleep_for(10ms);
   auto elem = queue.try_dequeue_for(1s);
   REQUIRE(elem);
   CHECK(elem == 10);

   elem = queue.try_dequeue_for(1s);
   REQUIRE(elem);
   CHECK(elem == 20);

   elem = queue.try_dequeue_for(1s);
   REQUIRE(elem);
   CHECK(elem == 30);
}

TEST_CASE(
   "spsc::static_blocking_queue: concurrent try_enqueue_until unblocks and enqueues element",
   "[spsc][blocking_queue][concurrent]")
{
   auto queue = cxxlab::spsc::static_blocking_queue<int, 2>{};
   queue.try_enqueue(10);
   queue.try_enqueue(20);

   auto producer = std::jthread(
      [&queue]() { CHECK(queue.try_enqueue_until(std::chrono::steady_clock::now() + 1s, 30)); });

   std::this_thread::sleep_for(10ms);
   auto elem = queue.try_dequeue_for(1s);
   REQUIRE(elem);
   CHECK(elem == 10);

   elem = queue.try_dequeue_for(1s);
   REQUIRE(elem);
   CHECK(elem == 20);

   elem = queue.try_dequeue_for(1s);
   REQUIRE(elem);
   CHECK(elem == 30);
}

TEST_CASE(
   "spsc::static_blocking_queue: single-threaded try_dequeue_for times out",
   "[spsc][blocking_queue]")
{
   static constexpr auto timeout = 10ms;

   SECTION("optional return value")
   {
      auto queue = cxxlab::spsc::static_blocking_queue<int, 2>{};
      auto start = std::chrono::steady_clock::now();
      auto elem = queue.try_dequeue_for(timeout);

      CHECK_FALSE(elem.has_value());
      CHECK(std::chrono::steady_clock::now() >= (timeout + start));
   }

   SECTION("out param return value")
   {
      auto queue = cxxlab::spsc::static_blocking_queue<int, 2>{};
      auto start = std::chrono::steady_clock::now();
      auto result = int{0};

      CHECK_FALSE(queue.try_dequeue_for(timeout, result));
      CHECK(std::chrono::steady_clock::now() >= (timeout + start));
   }
}

TEST_CASE(
   "spsc::static_blocking_queue: single-threaded try_dequeue_until times out",
   "[spsc][blocking_queue]")
{
   static constexpr auto timeout = 10ms;

   SECTION("optional return value")
   {
      auto queue = cxxlab::spsc::static_blocking_queue<int, 2>{};
      auto deadline = std::chrono::steady_clock::now() + timeout;
      auto elem = queue.try_dequeue_until(deadline);

      CHECK_FALSE(elem.has_value());
      CHECK(std::chrono::steady_clock::now() >= deadline);
   }

   SECTION("out param return value")
   {
      auto queue = cxxlab::spsc::static_blocking_queue<int, 2>{};
      auto deadline = std::chrono::steady_clock::now() + timeout;
      auto result = int{0};

      CHECK_FALSE(queue.try_dequeue_until(deadline, result));
      CHECK(std::chrono::steady_clock::now() >= deadline);
   }
}

TEST_CASE(
   "spsc::static_blocking_queue: concurrent try_dequeue_for unblocks and dequeues element",
   "[spsc][blocking_queue][concurrent]")
{
   SECTION("optional return value")
   {
      auto queue = cxxlab::spsc::static_blocking_queue<int, 2>{};
      auto producer = std::jthread(
         [&queue]()
         {
            std::this_thread::sleep_for(10ms);
            CHECK(queue.try_enqueue(11235));
         });

      auto elem = queue.try_dequeue_for(1s);
      REQUIRE(elem);
      CHECK(elem == 11235);
   }

   SECTION("out param return value")
   {
      auto queue = cxxlab::spsc::static_blocking_queue<int, 2>{};
      auto result = int{0};
      auto producer = std::jthread(
         [&queue]()
         {
            std::this_thread::sleep_for(10ms);
            CHECK(queue.try_enqueue(11235));
         });

      CHECK(queue.try_dequeue_for(1s, result));
      CHECK(result == 11235);
   }
}

TEST_CASE(
   "spsc::static_blocking_queue: concurrent try_dequeue_until unblocks and dequeues element",
   "[spsc][blocking_queue][concurrent]")
{
   SECTION("optional return value")
   {
      auto queue = cxxlab::spsc::static_blocking_queue<int, 2>{};
      auto producer = std::jthread(
         [&queue]()
         {
            std::this_thread::sleep_for(10ms);
            CHECK(queue.try_enqueue(11235));
         });

      auto elem = queue.try_dequeue_until(std::chrono::steady_clock::now() + 1s);
      REQUIRE(elem);
      CHECK(elem == 11235);
   }

   SECTION("out param return value")
   {
      auto queue = cxxlab::spsc::static_blocking_queue<int, 2>{};
      auto result = int{0};
      auto producer = std::jthread(
         [&queue]()
         {
            std::this_thread::sleep_for(10ms);
            CHECK(queue.try_enqueue(11235));
         });

      CHECK(queue.try_dequeue_until(std::chrono::steady_clock::now() + 1s, result));
      CHECK(result == 11235);
   }
}
