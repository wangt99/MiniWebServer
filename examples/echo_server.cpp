#include "core/connection.h"
#include "core/net_address.h"
#include "core/pine_server.h"
#include <ctime>

int main()
{
  pine::NetAddress local_host("127.0.0.1", 10001);
  pine::PineServer echo_server(local_host);

  echo_server.OnHnadle([](pine::Connection *client_conn){
    int from_fd = client_conn->GetFd();
    std::cout << "conn fd: " << from_fd << '\n';
    auto [read, exit] = client_conn->Recv();
    if (exit) {
      client_conn->GetLooper()->RemoveConnection(from_fd);
      return;
    }
    if (read) {
      std::cout << "from clinet: " << client_conn->ReadAsString() << '\n';
      client_conn->WriteToWriteBuffer(client_conn->ReadAsString());
      client_conn->Send();
      client_conn->ClearReadBuffer();
    }
  }).Run();
  return 0;
}