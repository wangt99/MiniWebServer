#include "core/connection.h"
#include "common/logger.h"
#include <asm-generic/errno-base.h>
#include <asm-generic/errno.h>
#include <cstring>
#include <sys/socket.h>
#include <sys/types.h>
#include <errno.h>


namespace pine {

constexpr int TEMP_BUF_SIZE = 2048;

void Connection::Send() {
  ssize_t curr_write = 0;
  ssize_t write;
  auto total_write = write_buffer_->Size();
  auto buffer = write_buffer_->Data();
  while (curr_write < total_write) {
    write = send(socket_->GetFd(), buffer + curr_write, total_write - curr_write, 0);
    if (write <= 0) {
      if (errno != EINTR && errno != EAGAIN && errno != EWOULDBLOCK) {
        LOG_ERROR("Error in Connection::Send");
        ClearWriteBuffer();
        return;
      }
      write = 0;
    }
    curr_write += write;
  }
  ClearWriteBuffer();
}

/**
 * @brief recv from client, and return the pair, which represent actual read and exit flag.
 * 
 * @return std::pair<ssize_t, bool>
 */
auto Connection::Recv() -> std::pair<ssize_t, bool> {
  int read = 0;
  char buf[TEMP_BUF_SIZE + 1];
  memset(buf, 0, sizeof(buf));
  while (true) {
    auto curr_read = recv(socket_->GetFd(), buf, TEMP_BUF_SIZE, 0);
    if (curr_read > 0) {
      read += curr_read;
      WriteToReadBuffer(buf, curr_read);
      memset(buf, 0, TEMP_BUF_SIZE);
    } else if (curr_read == 0) {
      // the client exit.
      // std::cout << "client exit " << read << '\n';
      return {read, true};
    }
    else if (curr_read == -1 && errno == EINTR) {
      // normal interrupt
      // std::cout << "normal interrupt " << read << '\n';
      continue;
    } else if (curr_read == -1 && (errno == EAGAIN && errno == EWOULDBLOCK)) {
      //all data read
      // std::cout << "all data read " << read << '\n';
      break;
    } else {
      LOG_ERROR("Connection:Recv error");
      return {read, true};
    }
  }
  return {read, false};
}
 
}