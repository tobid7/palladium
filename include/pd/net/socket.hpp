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

#include <pd/core/core.hpp>
#include <pd/net/pd_p_api.hpp>

namespace PD {
namespace Net {
class Backend;
/**
 * Wrapper class around the functionality of Net::Backend
 */
class PD_NET_API Socket {
 public:
  Socket(PD::SmartCtor<Backend>::Ref bknd) { backend = bknd; };
  Socket(PD::SmartCtor<Backend>::Ref bknd, u16 port) {
    backend = bknd;
    this->Create();
    this->Bind(port);
  }
  ~Socket() { Close(); };
  PD_SMART_CTOR(Socket);

  bool Create();
  bool Bind(u16 port);
  bool Listen(int backlog = 5);
  bool WaitForRead(int timeout_ms);
  bool Accept(Socket::Ref client);
  bool Connect(const std::string& ip, u16 port);
  int Send(const std::string& data);
  int Receive(std::string& data, int size = 1024);
  void Close();
  bool IsValid() const;

  int pSocket;
  PD::SmartCtor<Net::Backend>::Ref backend;
};
}  // namespace Net
}  // namespace PD