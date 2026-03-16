#pragma once

/*
MIT License
Copyright (c) 2024 - 2026 René Amthor (tobid7)

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

#include <pd/common.hpp>

namespace PD {
/**
 * Timer class
 */
class PD_API Timer {
 public:
  /**
   * Constructor
   * @param auto_start [default true] sets if timer should start after creation
   */
  Timer(bool auto_start = true);
  /**
   * Unused Deconstructor
   */
  ~Timer() {}

  /**
   * Resume Timer if Paused
   */
  void Rseume();
  /**
   * Pause Timer if not Paused
   */
  void Pause();
  /**
   * Update Timer
   */
  void Update();
  /**
   * Reset Timer
   */
  void Reset();
  /**
   * Check if the Timer is Running
   * @return true if its running
   */
  bool IsRunning() const;
  /**
   *  Get 64 Bit milliseconds value
   *  @return 64Bit millis
   */
  u64 Get();
  /**
   * Get as Seconds
   * @return seconds as floating number
   */
  double GetSeconds();

  /** Start of the Timer */
  u64 pStart;
  /** Current Time */
  u64 pNow;
  /** Is Running */
  bool pIsRunning = false;
};
}  // namespace PD