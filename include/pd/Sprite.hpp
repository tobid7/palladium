#pragma once

#include <citro3d.h>

#include <pd/Image.hpp>
#include <pd/smart_ctor.hpp>

namespace Palladium {
/// @brief Sprite Class
class Sprite {
 public:
  /// \brief Construct Sprite
  Sprite() = default;
  /// \brief Deconstruct Sprite
  ~Sprite() = default;
  PD_SMART_CTOR(Sprite)
  /// \brief Load a Sprite From SpriteSheet
  /// \param img the Image to load from.(Palladium::Image)
  void FromImage(Palladium::Image::Ref img);
  /// @brief Draw the Sprite
  /// @return success ?
  bool Draw();
  /// @brief Set the Center Position
  /// @param x X Pos
  /// @param y Y Pos
  void SetCenter(float x, float y);
  /// @brief Set the Sprite's Position
  /// @param x X Pos
  /// @param y Y Pos
  void SetPos(float x, float y);
  /// @brief Set The Sprite's Scale
  /// @param x Scale on X-Axis
  /// @param y Scale on Y-Axis
  void SetScale(float x, float y);
  /// @brief Set the Sprite's Rotation
  /// @param rotation ratation
  void SetRotation(float rotation);
  /// @brief Rotate the Sprite
  /// @param speed Speed to Rotate
  void Rotate(float speed);
  /// @brief Get Tje Sprite's Width
  /// @return Width
  float GetWidth();
  /// @brief Get the Sprite's Height
  /// @return Height
  float GetHeight();
  /// @brief Get The Sprite's X Position
  /// @return X Position
  float GetPosX();
  /// @brief Get the Sprite's Y Position
  /// @return Y Position
  float GetPosY();
  NVec2 GetSize();
  NVec2 GetPos();
  void SetPos(NVec2 pos);
  void SetScale(NVec2 scale);
  void SetRotCenter(NVec2 percentage);

 private:
  ///// @param sprite The Sprite
  //C2D_Sprite sprite;
};
}  // namespace Palladium