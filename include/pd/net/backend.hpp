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
#include <pd/net/socket.hpp>

namespace PD {
namespace Net {
class Backend {
 public:
  Backend(const std::string& name = "NullBackend") : pName(name) {}
  ~Backend() = default;
  PD_SMART_CTOR(Backend)
  virtual bool Init() = 0;
  virtual void Deinit() = 0;
  virtual int NewSocket() = 0;
  virtual void Close(int sock_id) = 0;
  virtual int GetInvalidRef() const = 0;
  virtual bool Bind(int sock_id, u16 port) = 0;
  virtual bool Listen(int sock_id, int backlog = 5) = 0;
  virtual bool WaitForRead(int sock_id, int timeout_ms) = 0;
  virtual bool Accept(int sock_id, Socket::Ref client) = 0;
  virtual bool Connect(int sock_id, const std::string& ip, u16 port) = 0;
  virtual int Send(int sock_id, const std::string& data) = 0;
  virtual int Receive(int sock_id, std::string& data, int size = 1024) = 0;

  /** Backend identification name */
  const std::string pName;
};
}  // namespace Net
}  // namespace PD