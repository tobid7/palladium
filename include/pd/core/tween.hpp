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

#include <pd/core/vec.hpp>

namespace PD {
/**
 * D7 Tween Engine (or something like that)
 * @tparam T Any Numeric value
 */
template <typename T>
class Tween {
 public:
  /**
   * Effects Table
   */
  enum Effect {
    Linear,          ///< Linear Movement [works]
    EaseInQuad,      ///< EaseInQuad Movement [works]
    EaseOutQuad,     ///< EaseOutQuad Movement [works]
    EaseInOutQuad,   ///< EaseInOutQuad Movement [works]
    EaseInCubic,     ///< EaseInCubic Movement [not tested]
    EaseOutCubic,    ///< EaseOutCubic Movement [not tested]
    EaseInOutCubic,  ///< EaseInOutCubic Movement [disabled]
    EaseInSine,      ///< EaseInSine Movement [works]
    EaseOutSine,     ///< EaseOutSine Movement [works]
    EaseInOutSine,   ///< EaseInOutSine Movement [not tested]
  };
  Tween() = default;
  ~Tween() = default;

  /**
   * Update Tween
   * @param delta deltatime
   */
  void Update(float delta) {
    time += delta / 1000.f;
    if (time > tend) {
      finished = true;
      time = tend;
    }
  }

  /**
   * Check if Tween is finished
   * @return true if finished
   */
  bool IsFinished() const { return finished; }

  /**
   * Force finish the animation
   * @return Class reference
   */
  Tween& Finish() {
    time = tend;
    finished = true;
    return *this;
  }

  /**
   * Set Start Value
   * @tparam T datatype of the Tween
   * @param start Start Value
   * @return class Reference
   */
  Tween& From(const T& start) {
    Reset();
    this->start = start;
    return *this;
  }
  /**
   * Set End Value
   * @tparam T datatype of the Tween
   * @param end End Value
   * @return class Reference
   */
  Tween& To(const T& end) {
    Reset();
    this->end = end;
    return *this;
  }
  /**
   * Set the Duration (in seconds)
   * @param seconds Duration
   * @return class Reference
   */
  Tween& In(float seconds) {
    Reset();
    tend = seconds;
    return *this;
  }
  /**
   * Set Effect of the Tween
   * @param e Effect
   * @return class Reference
   */
  Tween& As(const Effect& e) {
    effect = e;
    return *this;
  }
  /**
   * Reset to time 0
   * @return class Reference
   */
  Tween& Reset() {
    finished = false;
    time = 0.f;
    return *this;
  }
  /**
   * Get the Prograss in percent (0.0 to 1.0) of the tween
   * @return progress value
   */
  float Progress() const { return time / tend; }

  /**
   * Swap Start and end Position of the Tween
   * @return class reference
   */
  Tween& Swap() {
    T temp = start;
    start = end;
    end = temp;
    return *this;
  }

  /**
   * Operator that returns the current value calculated
   * by time and effect
   */
  operator T() {
    float t = 0.f;
    switch (effect) {
      case EaseInQuad:
        t = time / tend;
        return (end - start) * t * t + start;
        break;
      case EaseOutQuad:
        t = time / tend;
        return -(end - start) * t * (t - 2) + start;
        break;
      case EaseInOutQuad:
        t = time / (tend / 2);
        if (t < 1) return (end - start) / 2 * t * t + start;
        t--;
        return -(end - start) / 2 * (t * (t - 2) - 1) + start;
        break;
      case EaseInCubic:
        t = time / tend;
        return (end - start) * t * t * t + start;
        break;
      case EaseOutCubic:
        t = time / tend;
        t--;
        return (end - start) * (t * t * t + 1) + start;
        break;
      // case EaseInOutCubic:
      //   t = time / (tend / 2);
      //   if (t < 1) return (end - start) / 2 * t * t * t + start;
      //   t--;
      //   return (end - start) / 2 * (t * t * t * 2) + start;
      //   break;
      case EaseInSine:
        return -(end - start) * cos(time / tend * (M_PI / 2)) + (end - start) +
               start;
        break;
      case EaseOutSine:
        return (end - start) * sin(time / tend * (M_PI / 2)) + start;
        break;
      case EaseInOutSine:
        return -(end - start) / 2 * (cos(M_PI * time / tend) - 1) + start;
        break;

      default:  // Linear
        return (end - start) * (time / tend) + start;
        break;
    }
  }

 private:
  /** Animation Effect */
  Effect effect;
  /** Time */
  float time = 0.f;
  /**
   * End time
   * Defaulting to one to prevent div zero
   * without a safetey check
   * not implementing one cause if the user is
   * Writing a In(0.f) its their fault
   */
  float tend = 1.f;
  /** Start value */
  T start;
  /** end value */
  T end;
  /** is finished value */
  bool finished = false;
};
}  // namespace PD