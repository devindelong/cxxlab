/**
 * @copyright Copyright (c) 2026, Devin DeLong. All rights reserved.
 *
 * @license This code is distributed under the BSD 3-Clause License.
 *          See the LICENSE file for the full text of the license.
 *
 * @author Devin DeLong
 */

#pragma once

#include <concepts>
#include <cstddef>
#include <functional>
#include <iterator>
#include <utility>

namespace cxxlab::pipes
{

// -----------------------------------------------------------------------------
// adaptor_closure
// -----------------------------------------------------------------------------

class adaptor_closure
{
};

template <typename T>
concept adaptable = std::derived_from<T, adaptor_closure>;

// -----------------------------------------------------------------------------
// composed_adaptor
// -----------------------------------------------------------------------------

namespace detail
{

template <typename Left, typename Right>
class composed_adaptor : public adaptor_closure
{
 public:
   constexpr composed_adaptor(Left&& left, Right&& right)
       : left_{std::forward<Left>(left)}, right_{std::forward<Right>(right)}
   {
   }

   // This will be an iterator or an adapter.
   template <typename Sink>
   constexpr auto operator()(Sink&& sink) const
   {
      return left_(right_(std::forward<Sink>(sink)));
   }

 private:
   [[no_unique_address]] Left left_;
   [[no_unique_address]] Right right_;
};
} // namespace detail

// -----------------------------------------------------------------------------
// generic_adaptor
// -----------------------------------------------------------------------------

template <template <typename...> typename T, typename... Args>
class generic_adaptor : public adaptor_closure
{
 public:
   template <typename... FwdArgs>
      requires(std::convertible_to<FwdArgs, Args> && ...)
   explicit constexpr generic_adaptor(FwdArgs&&... args) : args_{std::forward<FwdArgs>(args)...}
   {
   }

   template <typename Self, typename Iterator>
   constexpr auto operator()(this Self&& self, Iterator&& iter)
   {
      return std::apply(
         [&iter]<typename... FwdArgs>(FwdArgs&&... args)
         { return T{std::forward<Iterator>(iter), std::forward<FwdArgs>(args)...}; },
         std::forward_like<Self>(self.args_));
   }

 private:
   [[no_unique_address]] std::tuple<Args...> args_;
};

// -----------------------------------------------------------------------------
// Pipe operators
// -----------------------------------------------------------------------------

template <adaptable Adapter1, adaptable Adapter2>
constexpr auto operator|(Adapter1&& left, Adapter2&& right)
{
   return detail::composed_adaptor{std::forward<Adapter1>(left), std::forward<Adapter2>(right)};
}

template <std::weakly_incrementable Iterator, adaptable Adapter>
constexpr auto operator|(Adapter&& adapter, Iterator&& iter)
   requires std::invocable<Adapter, Iterator>
{
   return std::invoke(std::forward<Adapter>(adapter), std::forward<Iterator>(iter));
}

} // namespace cxxlab::pipes
