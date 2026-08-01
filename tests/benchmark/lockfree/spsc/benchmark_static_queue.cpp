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

#include <cstdint>
#include <ranges>
#include <thread>

// -------------------------------------------------------------------------------------------------
// Benchmarks
// -------------------------------------------------------------------------------------------------

TEST_CASE("spsc::static_queue - benchmark latency", "[spsc][static_queue]")
{
   static constexpr auto queue_size = 1024;
   auto ping = cxxlab::spsc::static_queue<std::int64_t, queue_size>{};
   auto pong = cxxlab::spsc::static_queue<std::int64_t, queue_size>{};

   std::jthread responder(
      [&](std::stop_token st)
      {
         auto value = std::int64_t{};
         while (not st.stop_requested())
         {
            if (ping.try_dequeue(value))
            {
               while (not pong.try_enqueue(value))
               {
               }
            }
         }
      });

   auto value = std::int64_t{0};

   BENCHMARK_ADVANCED("Queue Round-Trip Time")(Catch::Benchmark::Chronometer meter)
   {
      meter.measure(
         [&]
         {
            while (not ping.try_enqueue(42))
            {
            }
            while (not pong.try_dequeue(value))
            {
            }
            return value;
         });
   };

   responder.request_stop();
   while (pong.try_dequeue(value))
      ;
}

TEST_CASE("spsc::static_queue - benchmark throughput", "[spsc][static_queue]")
{
   static constexpr auto queue_size = 1024uz;
   static constexpr auto num_elements = 1'048'576uz;

   auto queue = cxxlab::spsc::static_queue<std::size_t, queue_size>{};

   // Preill to half capacity.
   for (auto n : std::views::iota(0uz, queue_size / 2))
   {
      queue.try_enqueue(n);
   }

   std::jthread producer(
      [&](std::stop_token st)
      {
         auto num = std::size_t{1123};
         while (not st.stop_requested())
         {
            while (not queue.try_enqueue(num))
            {
            }
         }
      });

   BENCHMARK_ADVANCED("Queue Throughput")(Catch::Benchmark::Chronometer meter)
   {
      meter.measure(
         [&]
         {
            auto received = std::size_t{0};
            while (received < num_elements)
            {
               if (auto num = queue.try_dequeue())
               {
                  ++received;
               }
            }
            return received;
         });
   };

   producer.request_stop();

   // empty the queue.
   while (queue.try_dequeue())
   {
   }
}

TEST_CASE("spsc::static_queue - benchmark bulk throughput", "[spsc][static_queue]")
{
   static constexpr auto queue_size = 1024uz * 4;
   static constexpr auto bulk_size = 16uz;
   static constexpr auto num_elements = 1'048'576uz; // 2^20
   static constexpr auto num_dequeue = num_elements / bulk_size;

   auto queue = cxxlab::spsc::static_queue<std::size_t, queue_size>{};

   // Preill to half capacity.
   for (auto n : std::views::iota(0uz, queue_size / 2))
   {
      queue.try_enqueue(n);
   }

   std::jthread producer(
      [&](std::stop_token st)
      {
         auto input = std::views::iota(0uz, bulk_size);
         while (not st.stop_requested())
         {
            while (not queue.try_enqueue_bulk(input))
            {
            }
         }
      });

   BENCHMARK_ADVANCED("Queue Bulk Throughput")(Catch::Benchmark::Chronometer meter)
   {
      meter.measure(
         [&]
         {
            auto buffer = std::array<std::size_t, bulk_size>{};
            auto received = std::size_t{0};
            while (received < num_dequeue)
            {
               if (queue.try_dequeue_bulk(buffer.begin(), bulk_size) > 0)
               {
                  ++received;
               }
            }
            return received;
         });
   };

   producer.request_stop();

   // empty the queue.
   while (queue.try_dequeue())
   {
   }
}
