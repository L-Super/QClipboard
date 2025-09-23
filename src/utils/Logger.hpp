//
// Created by LMR on 2025/9/14.
//

#pragma once

#include <chrono>
#include <memory>
// clang-format off
// spdlog.h must before fmt header
#include "spdlog/spdlog.h"
#include "spdlog/fmt/ostr.h"
// clang-format on
#include "spdlog/sinks/rotating_file_sink.h"
#include "spdlog/sinks/stdout_color_sinks.h"

#include <QByteArray>
#include <QDebug>
#include <QString>

template<>
struct fmt::formatter<QString> : formatter<string_view> {
  auto format(const QString& str, format_context& ctx) const -> format_context::iterator {
    return formatter<string_view>::format(str.toStdString(), ctx);
  }
};

template<>
struct fmt::formatter<QByteArray> : formatter<string_view> {
  auto format(const QByteArray& str, format_context& ctx) const -> format_context::iterator {
    return formatter<string_view>::format(str.toStdString(), ctx);
  }
};

static void initLogging(const std::string&& filePath) {
  try {
    auto maxSize = 1024 * 1024 * 5; // 5MB
    auto maxFiles = 3;

    auto stdout_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    auto rotating_sink = std::make_shared<spdlog::sinks::rotating_file_sink_mt>(filePath, maxSize, maxFiles);
    std::vector<spdlog::sink_ptr> sinks{stdout_sink, rotating_sink};
    auto logger = std::make_shared<spdlog::logger>("qclipboard", std::begin(sinks), std::end(sinks));

    spdlog::set_default_logger(logger);

    std::string format{R"([%Y-%m-%d %T.%e] [%^%l%$] [%n] %v)"};
    spdlog::set_pattern(format);
    using namespace std::chrono_literals;
    spdlog::flush_every(5s);
  }
  catch (const spdlog::spdlog_ex& e) {
    qWarning() << "Log init failed, " << e.what();
  }
}
