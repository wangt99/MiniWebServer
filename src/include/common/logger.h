/**
 * @file logger.h
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2023-04-11
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#pragma once

#include <iostream>
#include <ostream>
#include <sstream>
#include <string>
#include <deque>
#include <functional>
#include <mutex>
#include <condition_variable>
#include <string_view>
#include <thread>
#include <ctime>
#include <chrono>
#include <iomanip>
#include <atomic>


namespace pine {

using namespace std::chrono_literals;

#define LOG_LOG_TIME_FORMAT "[%d %b %Y %H:%M:%S]"
#define LOG_FILE "Log"

constexpr int COUNT_THRESHOLD = 1000;
constexpr std::chrono::duration REFRESH_THRESHOLD = 1000ms;


enum class LogLevel {
  INFO = 0,
  WARNING = 1,
  DEBUG = 2,
  ERROR = 3,
  FATAL = 4
};

enum class Color {
  FG_RED = 31,
  FG_GREEN = 32,
  FG_YELLOW = 33,
  FG_BLUE = 34,
  FG_DEFAULT = 39,
  BG_RED = 41,
  BG_GREEN = 42,
  BG_YELLOW = 43,
  BG_BLUE = 44,
  BG_DEFAULT = 49,
};

inline auto operator << (std::ostream &os, Color code) -> std::ostream & {
  return os << "\033[" << static_cast<int>(code) << "m";
}

static const char *log_level_name[] = {" INFO: ", " WARNING: ", " DEBUG: ", " ERROR: ",  " FATAL: "};

/**
 * @brief a simple async logger
 * 
 */
class Logger {
public:
  static auto GetLogger() noexcept -> Logger &;

  static void LogMessage(LogLevel level, const std::string &msg) noexcept;

  // static void LogMessage(LogLevel level, std::string_view msg) noexcept;

  struct Log {
    std::string stamped_msg_;

    Log(LogLevel level, const std::string & msg) noexcept {
      auto utc_time = std::time(nullptr);
      auto local_time = *std::localtime(&utc_time);
      std::ostringstream stream;
      switch (level) {
        case LogLevel::INFO:
          stream << Color::FG_GREEN <<  std::put_time(&local_time, LOG_LOG_TIME_FORMAT) << log_level_name[static_cast<int>(level)] << msg << '\n';
          break;
        case LogLevel::ERROR:
          stream << Color::FG_RED << std::put_time(&local_time, LOG_LOG_TIME_FORMAT) << log_level_name[static_cast<int>(level)] << msg << '\n';
          break;
        case LogLevel::WARNING:
          stream << Color::FG_YELLOW << std::put_time(&local_time, LOG_LOG_TIME_FORMAT) << log_level_name[static_cast<int>(level)] << msg << '\n';
          break;
        default:
          stream << std::put_time(&local_time, LOG_LOG_TIME_FORMAT) << log_level_name[static_cast<int>(level)] << msg << '\n';
      }
      
      stamped_msg_ = stream.str();
    }

    friend auto operator << (std::ostream &os, const Log &log) -> std::ostream & {
      os << log.stamped_msg_;
      return os;
    }
  };

private:
  std::deque<Log> log_queue_;
  std::atomic<bool> done_;
  std::function<void(const std::deque<Log> &)> log_strategy_;
  std::thread log_writer_;
  std::chrono::milliseconds last_flush_;
  std::mutex mutex_;
  std::condition_variable cv_;

  explicit Logger(const std::function<void(const std::deque<Log> &)> &log_strategy);

  ~Logger();

  void LogWriter();

  void PushLog(Log &&log) noexcept;
};

}

#ifdef NOLOG
#define LOG_INFO(x) {};
#define LOG_WRANING(x) {};
#define LOG_DEBUG(x) {};
#define LOG_ERROR(x) {};
#define LOG_FATAL(x) {};
#else
#define LOG_INFO(x) pine::Logger::LogMessage(pine::LogLevel::INFO, x);
#define LOG_WRANING(x) pine::Logger::LogMessage(pine::LogLevel::WARNING, x);
#define LOG_DEBUG(x) pine::Logger::LogMessage(pine::LogLevel::DEBUG, x);
#define LOG_ERROR(x) pine::Logger::LogMessage(pine::LogLevel::ERROR, x);
#define LOG_FATAL(x) pine::Logger::LogMessage(pine::LogLevel::FATAL, x);
#endif