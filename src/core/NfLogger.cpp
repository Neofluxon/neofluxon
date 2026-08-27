/**
 * File name: NfLogger.cpp
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

#include "NfLogger.h"

#include <cstdio>

namespace NfCore {

void NfLogger::log(NfLogger::LogLevel level, std::string msg)
{
        auto& inst = instance();
        {
                std::lock_guard lock(inst.m_mutex);
                inst.m_queue.emplace_back(level, std::move(msg));
        }
        inst.m_cv.notify_one();
}

void NfLogger::log(NfLogger::LogLevel level, std::string_view msg)
{
        log(level, msg);
}

NfLogger& NfLogger::instance()
{
        static NfLogger inst;
        return inst;
}

NfLogger::NfLogger()
{
        m_thread = std::jthread([this](std::stop_token stoken) {
                process(stoken);
        });
}

NfLogger::~NfLogger()
{
        m_thread.request_stop();
        m_cv.notify_one();
}

void NfLogger::process(std::stop_token stoken)
{
        while (!stoken.stop_requested()) {
                std::vector<LogMessage> localQueue;
                {
                        std::unique_lock lock(m_mutex);
                        m_cv.wait(lock, stoken, [this, &stoken] {
                                return !m_queue.empty() || stoken.stop_requested();
                        });

                        if (stoken.stop_requested())
                                break;

                        if (m_queue.empty())
                                continue;

                        localQueue = std::move(m_queue);
                        m_queue.clear();
                }

                for (const auto& [level, message] : localQueue) {
                        std::string_view dbLabel = "INFO ";
                        switch (level) {
                        case LogLevel::Debug:   dbLabel = "DEBUG"; break;
                        case LogLevel::Warning: dbLabel = "WARN "; break;
                        case LogLevel::Error:   dbLabel = "ERROR"; break;
                        default: break;
                        }

                        std::println(stderr, "[{}] {}", dbLabel, message);
                }
        }

        std::fprintf(stderr, "Neofluxon: Logger Exit\n");
}

} // namespace NfCore
