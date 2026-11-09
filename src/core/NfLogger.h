/**
 * File name: NfLogger.h
 * Project: Neofluxon (a photography workflow software)
 *
 * Copyright (C) 2026 Iurie Nistor
 *
 * This file is part of Neofluxon.
 *
 * Neofluxon is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */

#ifndef NFLOGGER_H
#define NFLOGGER_H

#include "Neofluxon.h"

#include <atomic>
#include <thread>
#include <vector>
#include <string>
#include <mutex>
#include <condition_variable>
#include <string_view>
#include <sstream>

namespace NfCore {

class NfLogger {
 public:

        enum class LogLevel {
                Debug,
                Info,
                Warning,
                Error
        };

        NfLogger(const NfLogger&) = delete;
        NfLogger& operator=(const NfLogger&) = delete;
        static void log(LogLevel level, std::string msg);
        static void log(LogLevel level, std::string_view msg);

 private:
        NfLogger();
        ~NfLogger();

 private:
        struct LogMessage {
                LogLevel level;
                std::string message;
        };

        static NfLogger& instance();
        void process(std::stop_token stoken);

        std::jthread m_thread;
        std::mutex m_mutex;
        std::condition_variable_any m_cv;
        std::vector<LogMessage> m_queue;
};

} // namespace NfCore


#ifdef NDEBUG
#define NF_LOG_INFO(msg) ((void)0)
#define NF_LOG_ERROR(msg) ((void)0)
#define NF_LOG_DEBUG(msg) ((void)0)
#else
#define NF_LOG_INFO(msg) \
    NfCore::NfLogger::log(NfCore::NfLogger::LogLevel::Info, \
       ([&, context = __PRETTY_FUNCTION__]() {        \
            std::ostringstream oss; \
            oss << "[" << NEOFLUXON_NAME << "] " \
                << "[" << std::this_thread::get_id() << "] " \
                << context << " " \
                << msg; \
            return oss.str(); \
        })() \
    )
#define NF_LOG_ERROR(msg) \
    NfCore::NfLogger::log(NfCore::NfLogger::LogLevel::Error, \
      ([&, context = __PRETTY_FUNCTION__]() {        \
            std::ostringstream oss; \
            oss << "[" << NEOFLUXON_NAME << "] " \
                << "[" << std::this_thread::get_id() << "] " \
                << context << " " \
                << msg; \
            return oss.str(); \
        })() \
    )

#define NF_LOG_DEBUG(msg) \
    NfCore::NfLogger::log(NfCore::NfLogger::LogLevel::Debug, \
           ([&, context = __PRETTY_FUNCTION__]() {        \
            std::ostringstream oss; \
            oss << "[" << NEOFLUXON_NAME << "] " \
                << "[" << std::this_thread::get_id() << "] " \
                << context << " " \
                << msg; \
            return oss.str(); \
        })() \
    )
#endif

#endif // NFLOGGER_H
