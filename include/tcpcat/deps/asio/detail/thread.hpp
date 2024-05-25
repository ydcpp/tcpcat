//
// detail/thread.hpp
// ~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2024 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef ASIO_DETAIL_THREAD_HPP
#define ASIO_DETAIL_THREAD_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include "tcpcat/deps/asio/detail/config.hpp"

#if !defined(ASIO_HAS_THREADS)
# include "tcpcat/deps/asio/detail/null_thread.hpp"
#elif defined(ASIO_HAS_PTHREADS)
# include "tcpcat/deps/asio/detail/posix_thread.hpp"
#elif defined(ASIO_WINDOWS)
# if defined(UNDER_CE)
#  include "tcpcat/deps/asio/detail/wince_thread.hpp"
# elif defined(ASIO_WINDOWS_APP)
#  include "tcpcat/deps/asio/detail/winapp_thread.hpp"
# else
#  include "tcpcat/deps/asio/detail/win_thread.hpp"
# endif
#else
# include "tcpcat/deps/asio/detail/std_thread.hpp"
#endif

namespace asio {
namespace detail {

#if !defined(ASIO_HAS_THREADS)
typedef null_thread thread;
#elif defined(ASIO_HAS_PTHREADS)
typedef posix_thread thread;
#elif defined(ASIO_WINDOWS)
# if defined(UNDER_CE)
typedef wince_thread thread;
# elif defined(ASIO_WINDOWS_APP)
typedef winapp_thread thread;
# else
typedef win_thread thread;
# endif
#else
typedef std_thread thread;
#endif

} // namespace detail
} // namespace asio

#endif // ASIO_DETAIL_THREAD_HPP