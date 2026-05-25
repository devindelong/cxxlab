/**
 * @copyright Copyright (c) 2026, Devin DeLong. All rights reserved.
 *
 * @license This code is distributed under the BSD 3-Clause License.
 *          See the LICENSE file for the full text of the license.
 *
 * @author Devin DeLong
 */

#pragma once

#include <atomic>

namespace cxxlab
{

class ttas_mutex
{

 public:
   auto lock() noexcept -> void
   {
      while (true)
      {
         if (!lock_.test_and_set(std::memory_order_acquire))
         {
            return;
         }

         while (lock_.test(std::memory_order_relaxed))
         {
            lock_.wait(true, std::memory_order_relaxed);
         }
      }
   }

   auto try_lock() noexcept -> bool
   {
      return !lock_.test(std::memory_order_relaxed) &&
             !lock_.test_and_set(std::memory_order_acquire);
   }

   auto unlock() noexcept -> void
   {
      lock_.clear(std::memory_order_release);
      lock_.notify_one();
   }

 private:
   std::atomic_flag lock_{};
};
} // namespace cxxlab
