#include <glad/glad.h>

/** SEPARATOR */
#include <GLFW/glfw3.h>
/** SEPARATOR */
#include <pd/core/core.hpp>
#include <pd/lithium/backend.hpp>
#include <pd_p_bknd_api.hpp>

namespace PD {
namespace LI {
class PD_BKND_DESKTOP_API Backend_GL2 : public PD::LI::Backend {
 public:
  Backend_GL2() : LI::Backend("OpenGL2") {}
  ~Backend_GL2() {}

  PD_SMART_CTOR(Backend_GL2)

  void Init() override;
  void Deinit() override;
  void NewFrame() override;
  void BindTexture(PD::LI::TexAddress addr) override;
  void RenderDrawData(const PD::Vec<PD::LI::Command::Ref>& Commands) override;
  PD::LI::Texture::Ref LoadTexture(
      const std::vector<PD::u8>& pixels, int w, int h,
      PD::LI::Texture::Type type = PD::LI::Texture::Type::RGBA32,
      PD::LI::Texture::Filter filter =
          PD::LI::Texture::Filter::LINEAR) override;

 private:
  PD::Vec<PD::LI::Vertex> VertexBuffer;
  PD::Vec<PD::u16> IndexBuffer;
  size_t CurrentVertex = 0;
  size_t CurrentIndex = 0;
  GLuint Shader;
  GLuint pLocProjection;
  GLuint pLocTex;
  Mat4 Projection;
  GLuint VBO, IBO;
  // For Stats
  PD::u32 num_vtx = 0;
  PD::u32 num_idx = 0;
};

}  // namespace LI
}  // namespace PD