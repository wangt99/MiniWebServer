#include "common/logger.h"
#include <algorithm>
#include <filesystem>
#include <fstream>


namespace pine {

/* Helper functions */

auto GetCurrentTime() -> std::chrono::milliseconds {
  using namespace std::chrono;
  return duration_cast<milliseconds>(system_clock::now().time_since_epoch());
}

auto GetCurrentDate() -> std::string {
      auto utc_time = std::time(nullptr);
      auto local_time = *std::localtime(&utc_time);
      std::ostringstream stream;
      stream << std::put_time(&local_time, "%d-%b-%Y");
      return stream.str();
}

struct StreamWriter {
  StreamWriter(const std::filesystem::path &path) noexcept {
    auto file = path.string() + "_" + GetCurrentDate();
    file_stream_.open(file, std::fstream::out | std::fstream::trunc);
  }

  ~StreamWriter() {
    if (file_stream_.is_open()) {
      file_stream_.flush();
      file_stream_.close();
    }
  }

  void Flush() noexcept {
    if (file_stream_.is_open()) {
      file_stream_.flush();
    }
  }

  void Write(const std::deque<Logger::Log> &logs) noexcept {
    std::for_each(logs.begin(), logs.end(), [&](auto &log){ file_stream_ << log;});
    Flush();
  }

  std::fstream file_stream_;
};

void PrintToScreen(const std::deque<Logger::Log> &logs) {
  std::for_each(logs.begin(), logs.end(), [](auto &log){ std::cout << log; });
}

void PrintToFile(const std::deque<Logger::Log> &logs) {
  static StreamWriter writer{LOG_FILE};
  writer.Write(logs);
}



/* Logger member functions */

auto Logger::GetLogger() noexcept -> Logger & {
   static Logger logger {PrintToFile};
   return logger;
}

void Logger::LogMessage(LogLevel level, const std::string &msg) noexcept {
  GetLogger().PushLog(Logger::Log(level, msg));
}

Logger::Logger(const std::function<void(const std::deque<Log> &)> &log_strategy)
  : log_strategy_(log_strategy) {
    last_flush_ = GetCurrentTime();
    log_writer_= std::thread(&Logger::LogWriter, this);
  }

Logger::~Logger() {
  done_ = true;
  cv_.notify_all();
  if (log_writer_.joinable()) {
    log_writer_.join();
  }
}

/* Internel helper to push the log to log queue. */
void Logger::PushLog(Log &&log) noexcept {
  bool is_notify {false};
  {
    std::unique_lock<std::mutex> lk{mutex_};
    log_queue_.push_back(std::move(log));
    auto now = GetCurrentTime();
    if ((now - last_flush_) > REFRESH_THRESHOLD || log_queue_.size() > COUNT_THRESHOLD) {
      is_notify = true;
    }
  }
  if (is_notify) {
    cv_.notify_one();
  }
}

/* The thread routine for the backend log writer */
void Logger::LogWriter() {
  std::deque<Log> writer_queue;
  while(true) {
    std::unique_lock<std::mutex> lk{mutex_};
    cv_.wait(lk, [this]() {
      return done_ || log_queue_.size() > COUNT_THRESHOLD || GetCurrentTime() - last_flush_ > REFRESH_THRESHOLD;
    });

    if (!log_queue_.empty()) {
      writer_queue.swap(log_queue_);
      lk.unlock();
      log_strategy_(writer_queue);
      last_flush_ = GetCurrentTime();
      writer_queue.clear();
    }
    if (done_) {
      return;
    }
  }
}

}