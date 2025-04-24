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
#include <pd/lithium/renderer.hpp>

namespace PD {
/**
 * Overlay Template class
 */
class Overlay : public SmartCtor<Overlay> {
 public:
  Overlay() = default;
  virtual ~Overlay() = default;

  /**
   * Update Function for Overlay input and rendering
   * @param delta Deltatime
   * @param ren Renderer reference
   * @param inp Input Driver
   */
  virtual void Update(float delta, LI::Renderer::Ref ren, Hid::Ref inp) = 0;

  /** Check if killed to remove from Overlay Manager */
  bool IsKilled() const { return kill; }

 protected:
  /** Internall Kill function to call from your Overlay */
  void Kill() { kill = true; }

 private:
  bool kill = false;  ///< Should be killed or not
};
}  // namespace PD