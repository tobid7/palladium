#include <pd.hpp>

PD::Net::Backend::Ref net_backend;
PD::LI::Backend::Ref backend;
PD::Hid::Ref inp;

#ifdef __3DS__
#include <3ds.h>

#include <li_backend_c3d.hpp>
#include <pd_hid_3ds.hpp>
#include <pd_net_3ds.hpp>

const u32 DisplayTransferFlags =
    (GX_TRANSFER_FLIP_VERT(0) | GX_TRANSFER_OUT_TILED(0) |
     GX_TRANSFER_RAW_COPY(0) | GX_TRANSFER_IN_FORMAT(GX_TRANSFER_FMT_RGBA8) |
     GX_TRANSFER_OUT_FORMAT(GX_TRANSFER_FMT_RGB8) |
     GX_TRANSFER_SCALING(GX_TRANSFER_SCALE_NO));
#else
#include <li_backend_gl2.hpp>
#include <pd_hid_glfw.hpp>
#include <pd_net_desktop.hpp>

bool vs = true;

void WindowIcn(GLFWwindow* win, const std::string& path) {
  auto img = PD::Image::New(path);
  if (img->Fmt() == img->RGB) {
    PD::Image::Convert(img, img->RGBA);
  }
  GLFWimage* i = new GLFWimage;
  i->pixels = new unsigned char[img->GetBuffer().size()];
  /** Use std::copy_n instead of memcpy (Weil nicht rot unterstrichen) */
  std::copy_n(img->GetBuffer().data(), img->GetBuffer().size(), i->pixels);
  i->width = img->Width();
  i->height = img->Height();
  glfwSetWindowIcon(win, 1, i);
}

#endif

class FileBrowserOderSo {
 public:
  struct Entry {
    std::string Name;
    std::string Path;
    bool Directory;
    std::shared_ptr<std::vector<Entry>> DirContent;
  };
  FileBrowserOderSo() { Listing = ParseDir(CurrentPath); }
  ~FileBrowserOderSo() = default;

  std::vector<Entry> ParseDir(const std::string& path) {
    std::vector<Entry> res;
    try {
      for (auto& it : std::filesystem::directory_iterator(path)) {
        res.push_back({it.path().filename().string(), it.path().string(),
                       it.is_directory()});
      }
    } catch (const std::filesystem::filesystem_error& e) {
      // Actually just keeping the App alive
    }
    return res;
  }

  /** Recursive Continue Tree View Generation */
  void RCTVG(PD::UI7::Menu::Ref m, std::vector<Entry>& listing,
             const std::string& cp) {
    if (m->BeginTreeNode(cp)) {
      for (auto& it : listing) {
        if (it.Directory) {
          if (!it.DirContent) {
            it.DirContent = std::make_shared<std::vector<Entry>>();
            *it.DirContent = ParseDir(it.Path);
          }
          RCTVG(m, *it.DirContent, it.Name);
        } else {
          m->Label(it.Name);
        }
      }
      m->EndTreeNode();
    }
  }

  void UI_Handle(PD::UI7::Context::Ref ui7) {
    if (ui7->BeginMenu("FileBrowser", UI7MenuFlags_Scrolling)) {
      auto m = ui7->GetCurrentMenu();
      RCTVG(m, Listing, CurrentPath);
      ui7->EndMenu();
    }
  }

  std::string CurrentPath = "/";
  std::vector<Entry> Listing;
};

#include <mutex>
#include <thread>

const int PORT = 8080;
const size_t MAX_LOGS = 100;

bool skill_thread = false;

std::mutex logMutex;
std::mutex coutCaptureMutex;
std::deque<std::string> logBuffer;

std::ostringstream capturedOutput;

class CoutRedirector : public std::streambuf {
 public:
  CoutRedirector(std::streambuf* original) : originalBuf(original) {}

 protected:
  int overflow(int c) override {
    if (c != EOF) {
      std::lock_guard<std::mutex> lock(coutCaptureMutex);  // separate mutex
      capturedOutput.put(c);
      if (c == '\n') {
        logBuffer.push_back(capturedOutput.str());
        capturedOutput.str("");
        capturedOutput.clear();
      }
    }
    return originalBuf->sputc(c);  // still forward to original
  }

 private:
  std::streambuf* originalBuf;
};

CoutRedirector* redirector = nullptr;

void startCoutCapture() {
  static std::streambuf* originalCoutBuffer = std::cout.rdbuf();
  redirector = new CoutRedirector(originalCoutBuffer);
  std::cout.rdbuf(redirector);
}

void logMessage(const std::string& msg) {
  std::lock_guard<std::mutex> lock(logMutex);
  if (logBuffer.size() >= MAX_LOGS) logBuffer.pop_front();
  logBuffer.push_back(msg);
  std::cout << msg << std::endl;
}

const char* css_file = R"(
html, body {
    font-family: 'Montserrat', sans-serif;
    background-color: #1e1e1e;
    color: #e0e0e0;
    margin: 0;
    padding: 0;
    height: 100%;
    width: 100%;
}

body {
    display: flex;
    flex-direction: column;
    min-height: 100vh;
    box-sizing: border-box;
}

main {
    flex: 1;
    padding: 2rem;
    box-sizing: border-box;
}

h2 {
    margin-bottom: 1rem;
}

pre {
    background-color: #2e2e2e;
    padding: 1rem;
    border-radius: 8px;
    overflow-x: auto;
    white-space: pre-wrap;
    word-wrap: break-word;
    box-shadow: 0 0 10px rgba(0, 0, 0, 0.3);
    margin-bottom: 20px;
}

footer {
    background-color: #333;
    color: #e0e0e0;
    padding: 1rem;
    text-align: center;
    width: 100%;
    box-shadow: 0 -2px 5px rgba(0, 0, 0, 0.3);
}

img {
    border-radius: 50%;
}

a {
    color: #3498db;
    text-decoration: none;
    font-weight: bold;
}

a:hover {
    text-decoration: underline;
}

footer a {
    color: #3498db;
    text-decoration: none;
    font-weight: bold;
}

footer a:hover {
    text-decoration: underline;
}
)";

const char* html_begin = R"(
<!DOCTYPE html>
<html lang="en">
<head>
  <meta charset="UTF-8">
  <title>HTTP Server</title>
  <link href="https://fonts.googleapis.com/css2?family=Montserrat&display=swap" rel="stylesheet">
  <link rel="stylesheet" href="/style.css">
</head>
)";

const char* html_404 = R"(
  <!DOCTYPE html>
  <html lang="en">
  <head>
    <meta charset="UTF-8">
    <meta http-equiv="refresh" content="5">
    <title>404 Not Found</title>
    <link href="https://fonts.googleapis.com/css2?family=Montserrat&display=swap" rel="stylesheet">
    <link rel="stylesheet" href="/style.css">
  </head>
  <body>
  <img src="/assets/icon.png" width="100">
  <h2>404 Page not found!</h2>
  <a href="/">Return to main Page</a>
  </body>
  </html>
  )";

const char* html_end = "<footer>2025 tobid7</footer></body></html>";

std::string getLogs() {
  std::lock_guard<std::mutex> lock(logMutex);
  std::ostringstream oss;
  for (const auto& line : logBuffer) oss << line;
  return oss.str();
}

std::string getIndexHTML() {
  std::lock_guard<std::mutex> lock(logMutex);
  std::ostringstream oss;

  oss << html_begin;
  oss << "<body><main><h1>Welcome</h1><ul><li><a href=\"/logs\">View "
         "Logs</a></li><li><a href=\"/sysinfo\">System "
         "Info</a></li></ul></main>";
  oss << html_end;
  return oss.str();
}

std::string getSystemInfo() {
  std::ostringstream oss;
  oss << "Compiler -> " << PD::Strings::GetCompilerVersion() << std::endl;
  oss << "Lithium -> " << backend->pName << std::endl;
  oss << "Input Driver -> " << inp->pName << std::endl;
  oss << "Network Driver -> " << net_backend->pName << std::endl;
  oss << "Lithium:" << std::endl;
  oss << "  Vertices: " << backend->VertexCounter << std::endl;
  oss << "  Indices: " << backend->IndexCounter << std::endl;
  oss << "  Triangles: " << backend->IndexCounter / 3 << std::endl;
#ifdef __3DS__
  oss << "Section 3DS" << std::endl;
  oss << "  linear_mem: " << PD::Strings::FormatBytes(linearSpaceFree())
      << std::endl;
#endif
  return oss.str();
}

void handleClient(PD::Net::Socket::Ref client) {
  std::string buf;
  int len = client->Receive(buf, 4096);
  if (len > 0) {
    buf[len] = '\0';
    if (buf.find("GET / ") == 0 || buf.find("GET /HTTP") == 0 ||
        buf.find("GET / HTTP") == 0) {
      std::string body = getIndexHTML();
      std::ostringstream response;
      response << "HTTP/1.1 200 OK\r\n"
               << "Content-Type: text/html\r\n"
               << "Content-Length: " << body.size() << "\r\n"
               << "Connection: close\r\n\r\n"
               << body;
      client->Send(response.str());
    } else if (buf.find("GET /logs") == 0) {
      std::ostringstream body;
      body << html_begin;
      body << R"(
        <body>
          <main>
            <h2>Logs</h2>
            <pre id="logs">Fetching logs...</pre>
            <script>
              // Simply fetch a string by using an api endpoint
              async function fetchLogs() {
                const res = await fetch('/logdata');
                const text = await res.text();
                document.getElementById('logs').innerText = text;
              }
              setInterval(fetchLogs, 2000);
              fetchLogs();
            </script>
          </main>
      )";
      body << html_end;
      std::ostringstream response;
      response << "HTTP/1.1 200 OK\r\n"
               << "Content-Type: text/html\r\n"
               << "Content-Length: " << body.str().size() << "\r\n"
               << "Connection: close\r\n\r\n"
               << body.str();
      client->Send(response.str());
    } else if (buf.find("GET /sysinfo ") == 0) {
      std::ostringstream body;
      body << html_begin;
      body << R"(
        <body>
          <main>
            <h2>System Info</h2>
            <pre id="sysinfo">Loading...</pre>
            <script>
              // Simply fetch a string by using an api endpoint
              async function fetchSysInfo() {
                const res = await fetch('/sysinfo/data');
                const text = await res.text();
                document.getElementById('sysinfo').innerText = text;
              }
              setInterval(fetchSysInfo, 2000);
              fetchSysInfo();
            </script>
          </main>
      )";
      body << html_end;

      std::ostringstream response;
      response << "HTTP/1.1 200 OK\r\n"
               << "Content-Type: text/html\r\n"
               << "Content-Length: " << body.str().size() << "\r\n"
               << "Connection: close\r\n\r\n"
               << body.str();
      client->Send(response.str());
    } else if (buf.find("GET /sysinfo/data") == 0) {
      std::string body = getSystemInfo();
      std::ostringstream response;
      response << "HTTP/1.1 200 OK\r\n"
               << "Content-Type: text/plain\r\n"
               << "Content-Length: " << body.size() << "\r\n"
               << "Cache-Control: no-cache\r\n"
               << "Connection: close\r\n\r\n"
               << body;
      client->Send(response.str());
    } else if (buf.find("GET /logdata") == 0) {
      std::string body = getLogs();
      std::ostringstream response;
      response << "HTTP/1.1 200 OK\r\n"
               << "Content-Type: text/plain\r\n"
               << "Content-Length: " << body.size() << "\r\n"
               << "Cache-Control: no-cache\r\n"
               << "Connection: close\r\n\r\n"
               << body;
      client->Send(response.str());
    } else if (buf.find("GET /style.css") == 0) {
      std::string body = css_file;
      std::ostringstream response;
      response << "HTTP/1.1 200 OK\r\n"
               << "Content-Type: text/css\r\n"
               << "Content-Length: " << body.size() << "\r\n"
               << "Connection: close\r\n\r\n"
               << body;
      client->Send(response.str());
    } else if (buf.find("GET /assets/icon.png") == 0) {
      std::ifstream f("test.png", std::ios::binary);
      if (f) {
        std::ostringstream body_stream;
        body_stream << f.rdbuf();
        std::string body = body_stream.str();
        std::ostringstream header;
        header << "HTTP/1.1 200 OK\r\n"
               << "Content-Type: image/png\r\n"
               << "Content-Length: " << body.size() << "\r\n"
               << "Connection: close\r\n\r\n";
        client->Send(header.str());
        client->Send(body);
      } else {
        client->Send("HTTP/1.1 404 Not Found\r\nConnection: close\r\n\r\n");
      }
      std::string body = css_file;
      std::ostringstream response;
      response << "HTTP/1.1 200 OK\r\n"
               << "Content-Type: text/css\r\n"
               << "Content-Length: " << body.size() << "\r\n"
               << "Connection: close\r\n\r\n"
               << body;
      client->Send(response.str());
    } else {
      std::string body = html_404;
      std::ostringstream response;
      response << "HTTP/1.1 200 OK\r\n"
               << "Content-Type: text/html\r\n"
               << "Content-Length: " << body.size() << "\r\n"
               << "Connection: close\r\n\r\n"
               << body;
      client->Send(response.str());
    }
  }
  client->Close();
}

void startServer() {
  auto server = PD::Net::Socket::New(net_backend, PORT);
  server->Listen();

  std::cout << "HTTP Logging Console running on http://localhost:" << PORT
            << "/logs\n";

  while (!skill_thread) {
    auto client = PD::Net::Socket::New(net_backend);
    server->Accept(client);
    std::thread(handleClient, client).detach();
  }
}

class HttpServer {
 public:
  HttpServer(PD::Net::Backend::Ref backend) {}

  void StartServer() {
    auto server = PD::Net::Socket::New(pBackend, PORT);
    server->Listen();
    while (true) {
      auto client = PD::Net::Socket::New(pBackend);
      server->Accept(client);
    }
  }

  PD::Net::Backend::Ref pBackend;
};

int main() {
  bool m__ = false;
  bool a__ = false;
  bool s__ = false;
#ifdef __3DS__
  net_backend = PD::NetBackend3DS::New();
  net_backend->Init();
  osSetSpeedupEnable(true);
  gfxInitDefault();
  cfguInit();
  cfguExit();
  // consoleInit(GFX_BOTTOM, nullptr);
  C3D_Init(C3D_DEFAULT_CMDBUF_SIZE * 2);
  C3D_RenderTarget* Top =
      C3D_RenderTargetCreate(240, 400, GPU_RB_RGBA8, GPU_RB_DEPTH24_STENCIL8);
  C3D_RenderTarget* Bottom =
      C3D_RenderTargetCreate(240, 320, GPU_RB_RGBA8, GPU_RB_DEPTH24_STENCIL8);
  C3D_RenderTargetSetOutput(Top, GFX_TOP, GFX_LEFT, DisplayTransferFlags);
  C3D_RenderTargetSetOutput(Bottom, GFX_BOTTOM, GFX_LEFT, DisplayTransferFlags);
  backend = PD::LI::Backend_C3D::New();
  inp = PD::Hid3DS::New();
#else
  net_backend = PD::NetBackendDesktop::New();
  net_backend->Init();
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 1);
  // glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_ANY_PROFILE);
#ifdef __APPLE__
  // Using This as frame buffer would be way too large
  glfwWindowHint(GLFW_COCOA_RETINA_FRAMEBUFFER, 0);
#endif
  glfwWindowHint(GLFW_SAMPLES, 8);  // Anti Aliasing
  auto win = glfwCreateWindow(1280, 720, "Palladium", nullptr, nullptr);
  glfwMakeContextCurrent(win);
  glfwSwapInterval(1);
  gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
  std::cout << "OpenGL: " << GLVersion.major << "." << GLVersion.minor
            << std::endl;
  if (!glCreateShader) {
    std::cout << "glCreateShader wurde nicht gefunden oder so keine ahnung"
              << std::endl;
  }

  WindowIcn(win, "test.png");
  backend = PD::LI::Backend_GL2::New();
  inp = PD::HidGLFW::New(win);
#endif
  startCoutCapture();
  std::thread server_http(startServer);
  server_http.detach();
#ifdef __3DS__
  std::cout << "System freezed... Press A!" << std::endl;
  while (aptMainLoop()) {
    hidScanInput();
    if (hidKeysDown() & KEY_A) {
      break;
    }
    gspWaitForVBlank();
    gfxSwapBuffers();
  }
#endif
  backend->Init();
  auto font = PD::LI::Font::New(backend);
  font->LoadTTF("ComicNeue.ttf", 32);
#ifdef __3DS__
  font->DefaultPixelHeight = 24;
#else
  font->DefaultPixelHeight = 32;
#endif
  auto ren = PD::LI::Renderer::New(backend);
  PD::Image img("test.png");
  PD::Image img2("logo.png");
  auto tex = backend->LoadTexture(
      img, img.Width(), img.Height(),
      img.Fmt() == img.RGBA ? PD::LI::Texture::RGBA32 : PD::LI::Texture::RGB24);
  auto tex2 =
      backend->LoadTexture(img2, img2.Width(), img2.Height(),
                           img2.Fmt() == img.RGBA ? PD::LI::Texture::RGBA32
                                                  : PD::LI::Texture::RGB24);
  auto r = PD::LI::DrawList::New(ren->WhitePixel);
  r->SetFont(font);
  PD::UI7::Context::Ref ui7 = PD::UI7::Context::New(ren, inp);
  ui7->GetIO()->Font = font;
  FileBrowserOderSo fb;
#ifdef __3DS__
  ui7->GetIO()->Theme->Set(UI7Color_Background, PD::Color("#222222aa"));
  while (aptMainLoop()) {
    backend->ViewPort = ivec2(320, 240);
#else
  while (!glfwWindowShouldClose(win)) {
    int wx, wy;
    glfwGetWindowSize(win, &wx, &wy);
    backend->ViewPort = ivec2(wx, wy);
#endif
    inp->Update();
    r->CurrentTex = ren->WhitePixel;
    ren->CurrentTex = ren->WhitePixel;
    ui7->DoMenuEx("TestInline", UI7MenuFlags_Scrolling,
                  [&](PD::UI7::ReMenu::Ref m) {
                    m->SeparatorText("Test");
                    m->Label("Test oder So");
                    static bool v;
                    static float v2;
                    m->Checkbox("Checkbox", v);
                    m->DragData("Test V2", &v2, 1);
                    m->Label("Test");
                    m->Separator();
                    if (m->Button("Hello1")) {
                      logMessage("Button Pressed");
                    }
                    m->Image(tex2);
                  });
    /*if (ui7->BeginMenuEx("TestMenu")) {
      auto m = ui7->GetCurrentMenuEx();
      m->SeparatorText("Hello World!");
      m->Separator();
      ui7->EndMenuEx();
    }*/
    if (ui7->BeginMenu("Test")) {
      auto m = ui7->GetCurrentMenu();
      m->Button("Button");
      m->Label("Label");
      if (m->Button("Show Metrics")) {
        m__ = true;
      }
      if (m->Button("Show About")) {
        a__ = true;
      }
      if (m->Button("Show Style")) {
        s__ = true;
      }
      m->PushAlignment(UI7Align_Mid);
      m->SeparatorText("Extra");
      m->PopAlignment();
      m->DragData("FontScale", &font->DefaultPixelHeight);
#ifndef __3DS__
      if (m->Button("Toggle VSYNC")) {
        vs = !vs;
        glfwSwapInterval(vs);
      }
#endif
      ui7->EndMenu();
    }
    // fb.UI_Handle(ui7);
    ui7->StyleEditor(&s__);
    ui7->AboutMenu(&a__);
    ui7->MetricsMenu(&m__);
    r->CurrentTex = tex;
    r->DrawCircleFilled(
        100, 24, 0xffffffff,
        3 + int(((std::sin(ui7->GetIO()->Time->GetSeconds()) + 1) / 2) * 50));
    ui7->Update(0);
    // r->CurrentTex = smdhtex;
    // r->DrawRectFilled(0, 48, 0xffffffff);
    ren->RegisterDrawList(r);

#ifdef __3DS__
    C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
    C3D_FrameDrawOn(Top);
    C3D_RenderTargetClear(Top, C3D_CLEAR_ALL, 0x00000000, 0);
    auto l = PD::LI::DrawList::New(ren->WhitePixel);
    l->DrawText(5, std::format("Tris: {}", ui7->GetIO()->NumIndices / 3),
                0xffffffff);
    ivec2 tvp = backend->ViewPort;
    backend->ViewPort = ivec2(400, 240);
    backend->NewFrame();
    backend->RenderDrawData(l->pDrawList);
    l->pDrawList.Clear();
    backend->ViewPort = tvp;
    C3D_FrameDrawOn(Bottom);
    C3D_RenderTargetClear(Bottom, C3D_CLEAR_ALL, 0x00000000, 0);
    PD::TT::Beg("R");
    // backend->NewFrame();
    backend->RenderDrawData(ui7->GetIO()->pRDL->pDrawList);
    ui7->GetIO()->pRDL->pDrawList.Clear();
    // ren->Render();
    PD::TT::End("R");
    C3D_FrameEnd(0);
#else
    PD::TT::Beg("R");
    ren->Render();
    PD::TT::End("R");
    glfwPollEvents();
    glfwSwapBuffers(win);
#endif
  }
  backend->Deinit();
  skill_thread = true;
  if (server_http.joinable()) {
    server_http.join();
  }
  net_backend->Deinit();
#ifdef __3DS__
  C3D_Fini();
  gfxExit();
#else
  glfwTerminate();
#endif
  return 0;
}