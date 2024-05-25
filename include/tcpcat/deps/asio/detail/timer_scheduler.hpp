//
// detail/timer_scheduler.hpp
// ~~~~~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2024 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef ASIO_DETAIL_TIMER_SCHEDULER_HPP
#define ASIO_DETAIL_TIMER_SCHEDULER_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include "tcpcat/deps/asio/detail/config.hpp"
#include "tcpcat/deps/asio/detail/timer_scheduler_fwd.hpp"

#if defined(ASIO_WINDOWS_RUNTIME)
# include "tcpcat/deps/asio/detail/winrt_timer_scheduler.hpp"
#elif defined(ASIO_HAS_IOCP)
# include "tcpcat/deps/asio/detail/win_iocp_io_context.hpp"
#elif defined(ASIO_HAS_IO_URING_AS_DEFAULT)
# include "tcpcat/deps/asio/detail/io_uring_service.hpp"
#elif defined(ASIO_HAS_EPOLL)
# include "tcpcat/deps/asio/detail/epoll_reactor.hpp"
#elif defined(ASIO_HAS_KQUEUE)
# include "tcpcat/deps/asio/detail/kqueue_reactor.hpp"
#elif defined(ASIO_HAS_DEV_POLL)
# include "tcpcat/deps/asio/detail/dev_poll_reactor.hpp"
#else
# include "tcpcat/deps/asio/detail/select_reactor.hpp"
#endif

#endif // ASIO_DETAIL_TIMER_SCHEDULER_HPP