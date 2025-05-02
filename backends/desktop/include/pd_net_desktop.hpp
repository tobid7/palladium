#pragma once

/*
MIT License
Copyright (c) 2024 - 2025 René Amthor (tobid7)

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
 */

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#ifdef _MSVC
#pragma comment(lib, "ws2_32.lib")
#endif
#else
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#endif

#include <pd/net/backend.hpp>
namespace PD {
class NetBackendDesktop : public PD::Net::Backend {
 public:
#ifdef _WIN32
  NetBackendDesktop() : Net::Backend("Desktop (Windows)") {
#else
  NetBackendDesktop() : Net::Backend("Desktop (Unix)") {
#endif
  }
  ~NetBackendDesktop() = default;
  PD_SMART_CTOR(NetBackendDesktop)
  bool Init() override {
#ifdef _WIN32
    WSADATA wsa_data;
    return (WSAStartup(MAKEWORD(2, 2), &wsa_data) == 0);
#else
    return true;
#endif
  }
  void Deinit() override {
#ifdef _WIN32
    WSACleanup();
#endif
  }
  int NewSocket() override { return socket(AF_INET, SOCK_STREAM, 0); }
  void Close(int sock_id) override {
#ifdef _WIN32
    closesocket(sock_id);
#else
    close(sock_id);
#endif
  }
  int GetInvalidRef() const override {
#ifdef _WIN32
    return INVALID_SOCKET;
#else
    return -1;
#endif
  }

  bool Bind(int sock_id, u16 port) override {
    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = INADDR_ANY;
    return bind(sock_id, (sockaddr*)&addr, sizeof(addr)) != -1;
  }
  bool Listen(int sock_id, int backlog = 5) override {
    return listen(sock_id, backlog) != -1;
  }

  bool WaitForRead(int sock_id, int timeout_ms) override {
    fd_set set;
    FD_ZERO(&set);
    FD_SET(sock_id, &set);

    timeval timeout{};
    timeout.tv_sec = timeout_ms / 1000;
    timeout.tv_usec = (timeout_ms % 1000) * 1000;

    int result = select(sock_id + 1, &set, nullptr, nullptr, &timeout);
    return (result > 0 && FD_ISSET(sock_id, &set));
  }

  bool Accept(int sock_id, Net::Socket::Ref client) override {
    int client_soc = accept(sock_id, nullptr, nullptr);
    if (client_soc == GetInvalidRef()) {
      return false;
    }
    client->pSocket = client_soc;
    return true;
  }
  bool Connect(int sock_id, const std::string& ip, u16 port) override {
    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    inet_pton(AF_INET, ip.c_str(), &addr.sin_addr);
    return connect(sock_id, (sockaddr*)&addr, sizeof(addr)) != -1;
  }
  int Send(int sock_id, const std::string& data) override {
    return send(sock_id, data.c_str(), static_cast<int>(data.size()), 0);
  }
  int Receive(int sock_id, std::string& data, int size = 1024) override {
    char* tmp = new char[size];
    int res = recv(sock_id, tmp, size, 0);
    if (res > 0) {
      data.assign(tmp, res);
    }
    delete[] tmp;
    return res;
  }
};
}  // namespace PD
