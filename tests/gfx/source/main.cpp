#include <glad/glad.h>
//////////////////////////
#include <GLFW/glfw3.h>

#include <format>
#include <gfx_opengl.hpp>
#include <iostream>
#include <palladium>

#include "pd/core/vec2.hpp"
#include "pd/lithium/pools.hpp"
#include "pd/lithium/vertex.hpp"

class App {
 public:
  App() {
    PD::Os::UseDriver<PD::OsDriver>();
    PD::Gfx::UseDriver<PD::GfxOpenGL>();
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
    window = glfwCreateWindow(1280, 720, "gfx_tests", nullptr, nullptr);
    glfwMakeContextCurrent(window);
    gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress));
    glfwSwapInterval(1);
    PD::Gfx::Init();
    PD::Li::InitPools(8192);
    std::vector<PD::u8> img(16 * 16 * 4, 0xff);
    pTex = PD::Gfx::LoadTexture(img, 16, 16);
    std::cout << "GfxDriver: " << PD::Gfx::GetDriverName() << std::endl;
    pPool.Init(10);
    auto cmd = pPool.Allocate(1);
    cmd->Reserve(4, 6);
    cmd->Add(PD::Li::Vertex(PD::fvec2(0, 0), PD::fvec2(0, 0), 0xffffffff));
    cmd->Add(PD::Li::Vertex(PD::fvec2(50, 0), PD::fvec2(1, 0), 0xffffffff));
    cmd->Add(PD::Li::Vertex(PD::fvec2(50, 50), PD::fvec2(1, 1), 0xffffffff));
    cmd->Add(PD::Li::Vertex(PD::fvec2(0, 50), PD::fvec2(0, 1), 0xffffffff));
    cmd->Add(0, 1, 2);
    cmd->Add(0, 2, 3);
    cmd->Tex = pTex;
  }
  ~App() {
    PD::Gfx::Deinit();
    glfwDestroyWindow(window);
    glfwTerminate();
  }

  void Run() {
    while (!glfwWindowShouldClose(window)) {
      glViewport(0, 0, 1280, 720);
      glClearColor(0.1, 0.1, 0.1, 0.1);
      glClear(GL_COLOR_BUFFER_BIT);
      PD::Gfx::Reset();
      PD::Gfx::SetViewPort(1280, 720);
      PD::Gfx::Draw(pPool);
      glfwPollEvents();
      glfwSwapBuffers(window);
    }
  }

 private:
  GLFWwindow* window = nullptr;
  PD::Pool<PD::Li::Command> pPool;
  PD::ptr pTex = 0;
};

int main() {
  App app;
  app.Run();
  return 0;
}