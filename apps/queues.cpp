/**
 * @copyright Copyright (c) 2026, Devin DeLong. All rights reserved.
 *
 * @license This code is distributed under the BSD 3-Clause License.
 *          See the LICENSE file for the full text of the license.
 *
 * @author Devin DeLong
 */

#include "cxxlab/lockfree/spsc/static_blocking_queue.hpp"
#include "cxxlab/lockfree/spsc/static_queue.hpp"

#include <algorithm>
#include <chrono>
#include <latch>
#include <new>
#include <print>
#include <ranges>
#include <thread>
#include <vector>

auto benchmark_latency(std::size_t num_iterations) -> std::chrono::steady_clock::duration
{
   auto ping = cxxlab::spsc::static_queue<std::int32_t, 1024>{};
   auto pong = cxxlab::spsc::static_queue<std::int32_t, 1024>{};
   auto start = std::latch{2};
   auto samples = std::vector<std::chrono::steady_clock::duration>(num_iterations);

   std::jthread responder(
      [&]
      {
         start.arrive_and_wait();
         for (auto i : std::views::iota(0uz, num_iterations))
         {
            while (not ping.try_dequeue())
               ;
            while (not pong.try_enqueue(static_cast<std::int32_t>(i)))
               ;
         }
      });

   std::jthread requester(
      [&]
      {
         start.arrive_and_wait();
         for (auto i : std::views::iota(0uz, num_iterations))
         {
            auto t0 = std::chrono::steady_clock::now();
            while (not ping.try_enqueue(static_cast<std::int32_t>(i)))
               ;
            while (not pong.try_dequeue())
               ;
            samples[i] = std::chrono::steady_clock::now() - t0;
         }
      });

   responder.join();
   requester.join();

   std::ranges::sort(samples);

   // ... percentile calculations
   auto count = [&](double p)
   {
      return std::chrono::duration_cast<std::chrono::nanoseconds>(
                samples[static_cast<std::size_t>(static_cast<double>(num_iterations) * p)])
         .count();
   };

   std::println("Latency RTT:");
   std::println("  p05:  {} ns", count(0.05));
   std::println("  p50:  {} ns", count(0.50));
   std::println("  p99:  {} ns", count(0.99));
   std::println("  p999: {} ns", count(0.999));
   std::println("  max:  {} ns", samples.back().count());

   return samples[static_cast<std::size_t>(static_cast<double>(num_iterations) * 0.50)];
}

//
//
//
//
//
//
//

auto benchmark_throughput(std::size_t num_elements) -> std::chrono::steady_clock::duration
{

   auto q = cxxlab::spsc::static_queue<std::size_t, 1024>{};
   // moodycamel::ReaderWriterQueue<int> q(1024);  // fixed capacity

   auto start = std::latch{2};

   auto tstart = std::chrono::steady_clock::time_point{};
   auto tend = std::chrono::steady_clock::time_point{};

   std::jthread producer(
      [&]
      {
         // Fill up half of the queue.
         for (auto n : std::views::iota(0uz, 512uz))
         {
            q.try_enqueue(n);
         }

         start.arrive_and_wait();
         for (auto n : std::views::iota(512uz, num_elements))
         {
            while (not q.try_enqueue(n))
               ;
         }
      });

   std::jthread consumer(
      [&]
      {
         start.arrive_and_wait();
         tstart = std::chrono::steady_clock::now();
         auto received = std::size_t{0};
         while (received < num_elements)
         {
            if (auto num = q.try_dequeue())
            {
               ++received;
            }
         }
         tend = std::chrono::steady_clock::now();
         std::println("Num  dequeued: {}", received);
      });

   producer.join();
   consumer.join();

   return tend - tstart;
}

//
//
//
//
//
//
//
static constexpr auto N = std::size_t{10'000'000};

int main()
{
   std::println("Align: {}", std::hardware_destructive_interference_size);

   for ([[maybe_unused]] auto _ : std::views::iota(0uz, 10uz))
   {
      auto dt = benchmark_throughput(N);
      auto dt_ms = std::chrono::duration_cast<std::chrono::milliseconds>(dt).count();
      std::println("Ops/ms: {}", static_cast<std::ptrdiff_t>(N) / dt_ms);
   }

   for (auto _ : std::views::iota(0uz, 10uz))
   {
      benchmark_latency(N);
   }

   for ([[maybe_unused]] auto _ : std::views::iota(0uz, 10uz))
   {
      auto dt = benchmark_throughput(N);
      auto dt_ms = std::chrono::duration_cast<std::chrono::milliseconds>(dt).count();
      std::println("Ops/ms: {}", static_cast<std::ptrdiff_t>(N) / dt_ms);
   }

   for ([[maybe_unused]] auto _ : std::views::iota(0uz, 10uz))
   {
      benchmark_latency(N);
   }

   return 0;
}
