#pragma once

/*
MIT License

Copyright (c) 2024 - 2025 tobid7

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

#include <pd/core/common.hpp>

namespace PD {
template <typename T>
class List {
 public:
  List() {}
  ~List() {}

  struct Node {
    Node(const T& v) : Data(v) {}
    T Data;
    Node* Prev = nullptr;
    Node* Next = nullptr;
  };

  class Iterator {
   public:
    Iterator(Node* n) : pNode(n) {}
    T& operator*() { return pNode->Data; }
    Iterator& operator++() {
      pNode = pNode->Next;
      return *this;
    }
    bool operator!=(const Iterator& o) const { return pNode != o.pNode; }

    Node* pNode = nullptr;
  };

  void PushFront(const T& val) {
    Node* node = new Node(val);
    // node->Data = val;
    node->Prev = nullptr;
    node->Next = pHead;
    if (pHead) {
      pHead->Prev = node;
    }
    pHead = node;
    if (!pTail) {
      pTail = node;
    }
    pSize++;
  }

  void PushBack(const T& val) {
    Node* node = new Node(val);
    // node->Data = val;
    node->Prev = pTail;
    node->Next = nullptr;
    if (pTail) {
      pTail->Next = node;
    }
    pTail = node;
    if (!pHead) {
      pHead = node;
    }
    pSize++;
  }

  void PopFront() {
    if (!pHead) {
      return;
    }
    Node* t = pHead;
    pHead = pHead->Next;
    if (pHead) {
      pHead->Prev = nullptr;
    } else {
      pTail = nullptr;
    }
    delete t;
    pSize--;
  }

  void PopBack() {
    if (!pTail) {
      return;
    }
    Node* t = pTail;
    pTail = pTail->Prev;
    if (pTail) {
      pTail->Next = nullptr;
    } else {
      pHead = nullptr;
    }
    delete t;
    pSize--;
  }

  void Clear() {
    while (pHead) {
      PopFront();
    }
  }

  void Remove(const T& v) {
    Node* s = pHead;
    while (s) {
      if (s->Data == v) {
        if (s->Prev) {
          s->Prev->Next = s->Next;
        } else {
          pHead = s->Next;
        }
        if (s->Next) {
          s->Next->Prev = s->Prev;
        } else {
          pTail = s->Prev;
        }
        delete s;
        pSize--;
        return;
      }
      s = s->Next;
    }
  }

  void Reverse() {
    Node* cur = pHead;
    while (cur) {
      Node* temp = cur->Prev;
      cur->Prev = cur->Next;
      cur->Next = temp;
      cur = cur->Prev;
    }
    Node* temp = pHead;
    pHead = pTail;
    pTail = temp;
  }

  T& Front() {
    if (pHead) {
      return pHead->Data;
    }
    // Need a List Empty Error Here (exceptions are disabled on 3ds)
    exit(1);
  }

  const T& Front() const {
    if (pHead) {
      return pHead->Data;
    }
    // Need a List Empty Error Here (exceptions are disabled on 3ds)
    exit(1);
  }

  T& Back() {
    if (pTail) {
      return pTail->Data;
    }
    // Need a List Empty Error Here (exceptions are disabled on 3ds)
    exit(1);
  }

  const T& Back() const {
    if (pTail) {
      return pTail->Data;
    }
    // Need a List Empty Error Here (exceptions are disabled on 3ds)
    exit(1);
  }

  size_t Size() const { return pSize; }

  Iterator begin() { return Iterator(pHead); }
  Iterator end() { return Iterator(nullptr); }

 private:
  Node* Find(const T& v) const {
    Node* t = pHead;
    while (t) {
      if (t->Data == v) {
        return t;
      }
      t = t->Next;
    }
    return nullptr;
  }

  Node* pHead = nullptr;
  Node* pTail = nullptr;
  size_t pSize = 0;
};
}  // namespace PD