//
// impl/as_tuple.hpp
// ~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2024 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef ASIO_IMPL_AS_TUPLE_HPP
#define ASIO_IMPL_AS_TUPLE_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include "deps/asio/detail/config.hpp"
#include <tuple>
#include "deps/asio/associator.hpp"
#include "deps/asio/async_result.hpp"
#include "deps/asio/detail/handler_cont_helpers.hpp"
#include "deps/asio/detail/type_traits.hpp"

#include "deps/asio/detail/push_options.hpp"

namespace asio {
namespace detail {

// Class to adapt a as_tuple_t as a completion handler.
template <typename Handler>
class as_tuple_handler
{
public:
  typedef void result_type;

  template <typename CompletionToken>
  as_tuple_handler(as_tuple_t<CompletionToken> e)
    : handler_(static_cast<CompletionToken&&>(e.token_))
  {
  }

  template <typename RedirectedHandler>
  as_tuple_handler(RedirectedHandler&& h)
    : handler_(static_cast<RedirectedHandler&&>(h))
  {
  }

  template <typename... Args>
  void operator()(Args&&... args)
  {
    static_cast<Handler&&>(handler_)(
        std::make_tuple(static_cast<Args&&>(args)...));
  }

//private:
  Handler handler_;
};

template <typename Handler>
inline bool asio_handler_is_continuation(
    as_tuple_handler<Handler>* this_handler)
{
  return asio_handler_cont_helpers::is_continuation(
        this_handler->handler_);
}

template <typename Signature>
struct as_tuple_signature;

template <typename R, typename... Args>
struct as_tuple_signature<R(Args...)>
{
  typedef R type(std::tuple<decay_t<Args>...>);
};

template <typename R, typename... Args>
struct as_tuple_signature<R(Args...) &>
{
  typedef R type(std::tuple<decay_t<Args>...>) &;
};

template <typename R, typename... Args>
struct as_tuple_signature<R(Args...) &&>
{
  typedef R type(std::tuple<decay_t<Args>...>) &&;
};

#if defined(ASIO_HAS_NOEXCEPT_FUNCTION_TYPE)

template <typename R, typename... Args>
struct as_tuple_signature<R(Args...) noexcept>
{
  typedef R type(std::tuple<decay_t<Args>...>) noexcept;
};

template <typename R, typename... Args>
struct as_tuple_signature<R(Args...) & noexcept>
{
  typedef R type(std::tuple<decay_t<Args>...>) & noexcept;
};

template <typename R, typename... Args>
struct as_tuple_signature<R(Args...) && noexcept>
{
  typedef R type(std::tuple<decay_t<Args>...>) && noexcept;
};

#endif // defined(ASIO_HAS_NOEXCEPT_FUNCTION_TYPE)

} // namespace detail

#if !defined(GENERATING_DOCUMENTATION)

template <typename CompletionToken, typename... Signatures>
struct async_result<as_tuple_t<CompletionToken>, Signatures...>
  : async_result<CompletionToken,
      typename detail::as_tuple_signature<Signatures>::type...>
{
  template <typename Initiation>
  struct init_wrapper
  {
    init_wrapper(Initiation init)
      : initiation_(static_cast<Initiation&&>(init))
    {
    }

    template <typename Handler, typename... Args>
    void operator()(Handler&& handler, Args&&... args)
    {
      static_cast<Initiation&&>(initiation_)(
          detail::as_tuple_handler<decay_t<Handler>>(
            static_cast<Handler&&>(handler)),
          static_cast<Args&&>(args)...);
    }

    Initiation initiation_;
  };

  template <typename Initiation, typename RawCompletionToken, typename... Args>
  static auto initiate(Initiation&& initiation,
      RawCompletionToken&& token, Args&&... args)
    -> decltype(
      async_initiate<
        conditional_t<
          is_const<remove_reference_t<RawCompletionToken>>::value,
            const CompletionToken, CompletionToken>,
        typename detail::as_tuple_signature<Signatures>::type...>(
          init_wrapper<decay_t<Initiation>>(
            static_cast<Initiation&&>(initiation)),
          token.token_, static_cast<Args&&>(args)...))
  {
    return async_initiate<
      conditional_t<
        is_const<remove_reference_t<RawCompletionToken>>::value,
          const CompletionToken, CompletionToken>,
      typename detail::as_tuple_signature<Signatures>::type...>(
        init_wrapper<decay_t<Initiation>>(
          static_cast<Initiation&&>(initiation)),
        token.token_, static_cast<Args&&>(args)...);
  }
};

#if defined(ASIO_MSVC)

// Workaround for MSVC internal compiler error.

template <typename CompletionToken, typename Signature>
struct async_result<as_tuple_t<CompletionToken>, Signature>
  : async_result<CompletionToken,
      typename detail::as_tuple_signature<Signature>::type>
{
  template <typename Initiation>
  struct init_wrapper
  {
    init_wrapper(Initiation init)
      : initiation_(static_cast<Initiation&&>(init))
    {
    }

    template <typename Handler, typename... Args>
    void operator()(Handler&& handler, Args&&... args)
    {
      static_cast<Initiation&&>(initiation_)(
          detail::as_tuple_handler<decay_t<Handler>>(
            static_cast<Handler&&>(handler)),
          static_cast<Args&&>(args)...);
    }

    Initiation initiation_;
  };

  template <typename Initiation, typename RawCompletionToken, typename... Args>
  static auto initiate(Initiation&& initiation,
      RawCompletionToken&& token, Args&&... args)
    -> decltype(
      async_initiate<
        conditional_t<
          is_const<remove_reference_t<RawCompletionToken>>::value,
            const CompletionToken, CompletionToken>,
        typename detail::as_tuple_signature<Signature>::type>(
          init_wrapper<decay_t<Initiation>>(
            static_cast<Initiation&&>(initiation)),
          token.token_, static_cast<Args&&>(args)...))
  {
    return async_initiate<
      conditional_t<
        is_const<remove_reference_t<RawCompletionToken>>::value,
          const CompletionToken, CompletionToken>,
      typename detail::as_tuple_signature<Signature>::type>(
        init_wrapper<decay_t<Initiation>>(
          static_cast<Initiation&&>(initiation)),
        token.token_, static_cast<Args&&>(args)...);
  }
};

#endif // defined(ASIO_MSVC)

template <template <typename, typename> class Associator,
    typename Handler, typename DefaultCandidate>
struct associator<Associator,
    detail::as_tuple_handler<Handler>, DefaultCandidate>
  : Associator<Handler, DefaultCandidate>
{
  static typename Associator<Handler, DefaultCandidate>::type get(
      const detail::as_tuple_handler<Handler>& h) noexcept
  {
    return Associator<Handler, DefaultCandidate>::get(h.handler_);
  }

  static auto get(const detail::as_tuple_handler<Handler>& h,
      const DefaultCandidate& c) noexcept
    -> decltype(Associator<Handler, DefaultCandidate>::get(h.handler_, c))
  {
    return Associator<Handler, DefaultCandidate>::get(h.handler_, c);
  }
};

#endif // !defined(GENERATING_DOCUMENTATION)

} // namespace asio

#include "deps/asio/detail/pop_options.hpp"

#endif // ASIO_IMPL_AS_TUPLE_HPP
