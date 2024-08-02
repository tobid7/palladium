#include <filesystem>
#include <pd/Hid.hpp>
#include <pd/Message.hpp>
#include <pd/ThemeEditor.hpp>
#include <pd/UI7.hpp>

std::map<PDColor, std::string> color_names = {
    {PDColor_Background, "Background"},
    {PDColor_Button, "Button"},
    {PDColor_ButtonActive, "ButtonActive"},
    {PDColor_ButtonDisabled, "ButtonDisabled"},
    {PDColor_ButtonHovered, "ButtonHovered"},
    {PDColor_Checkmark, "Checkmark"},
    {PDColor_FrameBg, "FrameBg"},
    {PDColor_FrameBgHovered, "FrameBgHovered"},
    {PDColor_Header, "Header"},
    {PDColor_List0, "List0"},
    {PDColor_List1, "List1"},
    {PDColor_MessageBackground, "Message Background"},
    {PDColor_Progressbar, "Progressbar"},
    {PDColor_Selector, "Selector"},
    {PDColor_SelectorFade, "SelectorFade"},
    {PDColor_Text2, "Text Light"},
    {PDColor_Text, "Text Dark"},
    {PDColor_TextDisabled, "Text Disabled"},
};

Palladium::ThemeEditor::ThemeEditor() {
  // Backup active Theme and create New one to edit
  temp_theme = Palladium::ThemeActive();
  edit_theme = Palladium::Theme::New();
  edit_theme->CopyOther(temp_theme);
  Palladium::ThemeSet(edit_theme);
}

Palladium::ThemeEditor::~ThemeEditor() {
  // Set Back to Acrive Theme
  Palladium::ThemeSet(temp_theme);
}

void Palladium::ThemeEditor::Draw() const {
  Palladium::R2::OnScreen(R2Screen_Top);
  if (UI7::BeginMenu("Palladium -> Theme Editor")) {
    UI7::Label("Sample Text");
    UI7::Checkbox("Checkbox", cm);
    UI7::InputText("Input Text", inpt, "Input Text");
    UI7::Button("Button");
    UI7::Progressbar(0.5f);
    UI7::ColorSelector("Color Selector",
                       edit_theme->GetTableRef()[PDColor_Progressbar]);
    UI7::EndMenu();
  }
  Palladium::R2::OnScreen(R2Screen_Bottom);
  if (UI7::BeginMenu("Theme", NVec2(), UI7MenuFlags_Scrolling)) {
    if (menu == 0) {
      if (UI7::Button("Create New")) {
        menu = 1;
        edit_theme->Default();
      } else if (UI7::Button("Edit Current")) {
        menu = 1;
      } else if (UI7::Button("Select Theme")) {
        menu = 2;
        theme_list.clear();
        for (const auto& it : std::filesystem::directory_iterator(
                 Palladium::GetAppDirectory() + "/themes")) {
          theme_list.push_back(it.path().filename().string());
        }
      }
    } else if (menu == 1) {
      if (UI7::Button("Go back")) {
        edit_theme->CopyOther(temp_theme);
        menu = 0;
      } else if (UI7::Button("Save")) {
        Palladium::AddOvl(std::make_unique<Ovl_Keyboard>(kbd_text, kbd_state,
                                                         "<name>.theme"));
      }
      for (auto& it : color_names) {
        UI7::ColorSelector(it.second, edit_theme->GetTableRef()[it.first]);
      }
    } else if (menu == 2) {
      if (UI7::Button("Go back")) {
        menu = 0;
      }
      for (auto& it : theme_list) {
        if (UI7::Button(it)) {
          edit_theme->Load(Palladium::GetAppDirectory() + "/themes/" + it);
          menu = 1;
        }
        UI7::SameLine();
        if (UI7::Button("Make Current")) {
          edit_theme->Load(Palladium::GetAppDirectory() + "/themes/" + it);
          temp_theme->CopyOther(edit_theme);
          menu = 0;
        }
        UI7::SameLine();
        if (UI7::Button("Delete")) {
          if (std::string(it) != "Palladium.theme") {
            std::filesystem::remove(Palladium::GetAppDirectory() + "/themes/" +
                                    it);
            theme_list.clear();
            for (const auto& it : std::filesystem::directory_iterator(
                     Palladium::GetAppDirectory() + "/themes")) {
              theme_list.push_back(it.path().filename().string());
            }
          } else {
            Palladium::PushMessage("ThemeEditor",
                                   "Cannot Delete\nPalladium.theme!");
          }
        }
      }
    }
    UI7::EndMenu();
  }
}

void Palladium::ThemeEditor::Logic() {
  if (kbd_state) {
    if (kbd_state == PDKeyboardState_Confirm) {
      auto path =
          Palladium::GetAppDirectory() + "/themes/" + kbd_text + ".theme";
      kbd_text = "";
      if (std::filesystem::exists(path)) {
        // Prompt Override
        return;
      }
      edit_theme->Save(path);
    }
    kbd_state = PDKeyboardState_None;
  }
  if (Hid::IsEvent("cancel", Hid::Down)) {
    if (menu == 0) {
      Palladium::Scene::Back();
    } else {
      if (menu == 1) edit_theme->CopyOther(temp_theme);
      menu = 0;
    }
  }
}