#include <pd/overlays/settings.hpp>

namespace PD {
int SettingsMenu::too = 0;
void SettingsMenu::Update(float delta, LI::Renderer::Ref ren, Hid::Ref inp) {
  if (!ctx) {
    ctx = UI7::Context::New(ren, inp);
    ctx->RootLayer(70);
  }
  flymgr.Update(delta);
  if (rem && flymgr.IsFinished()) {
    this->Kill();
  }
  ren->OnScreen(ren->GetScreen(false));
  if (ctx->BeginMenu("Palladium - Settings", UI7MenuFlags_CenterTitle)) {
    auto m = ctx->GetCurrentMenu();
    m->SeparatorText("Library Info");
    m->Label(LibInfo::CompiledWith());
    m->AfterAlignCenter();
    m->Label(LibInfo::CxxVersion());
    m->AfterAlignCenter();
    m->Label("Version: " + LibInfo::Version() + "[" + LibInfo::Commit() + "]");
    m->AfterAlignCenter();
    m->Label("Build Time: " + LibInfo::BuildTime());
    m->AfterAlignCenter();
    ctx->EndMenu();
  }
  ren->OnScreen(ren->GetScreen(true));
  if (ctx->BeginMenu("pdovlssettings", UI7MenuFlags_NoTitlebar)) {
    auto m = ctx->GetCurrentMenu();
    m->SeparatorText("Settings");
    if (m->Button("Exit")) {
      this->Rem();
    }
    ctx->EndMenu();
  }
  ctx->Update(delta);
}
}  // namespace PD