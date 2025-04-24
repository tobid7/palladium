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

/** Need to outsource this into the backend */

#include <pd/net/backend.hpp>
#include <pd/net/socket.hpp>

namespace PD {
namespace Net {
PD_NET_API bool Socket::Create() {
  pSocket = backend->NewSocket();
  return pSocket != backend->GetInvalidRef();
}
PD_NET_API bool Socket::Bind(u16 port) { return backend->Bind(pSocket, port); }

PD_NET_API bool Socket::Listen(int backlog) {
  return backend->Listen(pSocket, backlog);
}

PD_NET_API bool Socket::Accept(Socket::Ref client) {
  return backend->Accept(pSocket, client);
}

PD_NET_API bool Socket::Connect(const std::string& ip, u16 port) {
  return backend->Connect(pSocket, ip, port);
}

PD_NET_API int Socket::Send(const std::string& data) {
  return backend->Send(pSocket, data);
}

PD_NET_API int Socket::Receive(std::string& data, int size) {
  return backend->Receive(pSocket, data, size);
}

PD_NET_API void Socket::Close() {
  if (IsValid()) {
    backend->Close(pSocket);
    pSocket = backend->GetInvalidRef();
  }
}

PD_NET_API bool Socket::IsValid() const {
  return pSocket != backend->GetInvalidRef();
}
}  // namespace Net
}  // namespace PD