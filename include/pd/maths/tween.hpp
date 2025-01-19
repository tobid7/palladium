#pragma once

#include <pd/maths/vec.hpp>

namespace PD {
template <typename T>
class Tween {
 public:
  enum Effect {
    Linear,
    EaseInQuad,
    EaseOutQuad,
    EaseInOutQuad,
    EaseInCubic,
    EaseOutCubic,
    EaseInOutCubic,
    EaseInSine,
    EaseOutSine,
    EaseInOutSine,
  };
  Tween() {}
  ~Tween() {}

  void Update(float delta) {
    time += delta / 1000.f;
    if (time > tend) {
      finished = true;
      time = tend;
    }
  }

  bool IsFinished() const { return finished; }

  Tween& Finish() {
    time = tend;
    finished = true;
    return *this;
  }

  Tween& From(const T& start) {
    Reset();
    this->start = start;
    return *this;
  }
  Tween& To(const T& end) {
    Reset();
    this->end = end;
    return *this;
  }
  Tween& In(float seconds) {
    Reset();
    tend = seconds;
    return *this;
  }
  Tween& As(const Effect& e) {
    effect = e;
    return *this;
  }
  Tween& Reset() {
    finished = false;
    time = 0.f;
    return *this;
  }
  /// @brief Probably usefull for fading colors by animation
  /// Used to create this caus dont wanted to create a
  /// fade effect fpr keyboard without having to Tween functions
  /// @return
  float Progress() const { return time / tend; }

  Tween& Swap() {
    T temp = start;
    start = end;
    end = temp;
    return *this;
  }

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
  Effect effect;
  float time = 0.f;
  // Defaulting to one to prevent div zero
  // without a safetey check
  // not implementing one cause if the user is
  // Writing a In(0.f) its their fault
  float tend = 1.f;
  T start;
  T end;
  bool finished = false;
};
}  // namespace PD