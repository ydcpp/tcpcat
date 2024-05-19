//
// ts/executor.hpp
// ~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2024 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef ASIO_TS_EXECUTOR_HPP
#define ASIO_TS_EXECUTOR_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include "tcpcat/deps/asio/async_result.hpp"
#include "tcpcat/deps/asio/associated_allocator.hpp"
#include "tcpcat/deps/asio/execution_context.hpp"
#include "tcpcat/deps/asio/is_executor.hpp"
#include "tcpcat/deps/asio/associated_executor.hpp"
#include "tcpcat/deps/asio/bind_executor.hpp"
#include "tcpcat/deps/asio/executor_work_guard.hpp"
#include "tcpcat/deps/asio/system_executor.hpp"
#include "tcpcat/deps/asio/executor.hpp"
#include "tcpcat/deps/asio/any_io_executor.hpp"
#include "tcpcat/deps/asio/dispatch.hpp"
#include "tcpcat/deps/asio/post.hpp"
#include "tcpcat/deps/asio/defer.hpp"
#include "tcpcat/deps/asio/strand.hpp"
#include "tcpcat/deps/asio/packaged_task.hpp"
#include "tcpcat/deps/asio/use_future.hpp"

#endif // ASIO_TS_EXECUTOR_HPP
