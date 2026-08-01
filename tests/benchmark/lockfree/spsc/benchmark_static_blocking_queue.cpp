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
#include <string>
#include <thread>

TEST_CASE(
   "spsc::static_blocking_queue - benchmark blocking latency", "[spsc][static_blocking_queue]")
{
   static constexpr auto queue_size = 1024uz;
   auto ping = cxxlab::spsc::static_blocking_queue<std::int32_t, queue_size>{};
   auto pong = cxxlab::spsc::static_blocking_queue<std::int32_t, queue_size>{};
   auto timeout = std::chrono::milliseconds{1};

   std::jthread responder(
      [&](std::stop_token st)
      {
         auto value = std::int32_t{0};
         while (not st.stop_requested())
         {
            if (ping.try_dequeue_for(timeout, value))
            {
               pong.enqueue(value);
            }
         }
      });

   BENCHMARK_ADVANCED("Queue Round-Trip Time")(Catch::Benchmark::Chronometer meter)
   {
      meter.measure(
         [&]
         {
            ping.enqueue(42);
            return pong.dequeue();
         });
   };

   responder.request_stop();
}

TEST_CASE(
   "spsc::static_blocking_queue - benchmark non-blocking latency", "[spsc][static_blocking_queue]")
{
   // Benchmarks the overhead of the counting_semaphores when only non-blocking
   // try_enqueue/try_dequeue member functions are called. This is the same test used in the
   // non-blocing queue.
   static constexpr auto queue_size = 1024uz;
   auto ping = cxxlab::spsc::static_blocking_queue<std::int32_t, queue_size>{};
   auto pong = cxxlab::spsc::static_blocking_queue<std::int32_t, queue_size>{};

   std::jthread responder(
      [&](std::stop_token st)
      {
         auto value = std::int32_t{};
         while (not st.stop_requested())
         {
            if (ping.try_dequeue(value))
            {
               while (not pong.try_enqueue(value))
                  ;
            }
         }
      });

   auto value = std::int32_t{0};

   BENCHMARK_ADVANCED("Queue Round-Trip Time")(Catch::Benchmark::Chronometer meter)
   {
      meter.measure(
         [&]
         {
            while (not ping.try_enqueue(42))
               ;
            while (not pong.try_dequeue(value))
               ;
            return value;
         });
   };

   responder.request_stop();
   while (pong.try_dequeue(value))
      ;
}
