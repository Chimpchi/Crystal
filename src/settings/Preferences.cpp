#include "Preferences.h"
#include "../Window/EditorWindow.h"
#include "Utils.h"
#include "Math/Math.h"

#include <mini/ini.h>
#include <nlohmann/json.hpp>

#include <filesystem>

#include <iostream>
#include <filesystem>

#define DEFAULT_FONT "Crystal Plex Mono"
#define DEFAULT_THEME "Crystal Dark"
#define GLOBAL_SETTINGS "globalSettings"
#define UI_SETTINGS "uiSettings"
#define EDITOR_SETTINGS "editorSettings"

namespace Crystal
{

static std::unordered_map<ImGuiCol, const char*> s_imguiColorMappings = {
    {ImGuiCol_Text, "TextPrimary"},
    {ImGuiCol_TextDisabled, "TextDisabled"},
    {ImGuiCol_WindowBg, "WindowBg"},
    {ImGuiCol_ChildBg, "ChildBg"},
    {ImGuiCol_PopupBg, "PopupBg"},
    {ImGuiCol_Border, "Border"},
    {ImGuiCol_FrameBg, "FrameBg"},
    {ImGuiCol_FrameBgHovered, "FrameBgHovered"},
    {ImGuiCol_FrameBgActive, "FrameBgActive"},
    {ImGuiCol_TitleBg, "TitleBg"},
    {ImGuiCol_TitleBgActive, "TitleBgActive"},
    {ImGuiCol_TitleBgCollapsed, "TitleBgCollapsed"},
    {ImGuiCol_MenuBarBg, "MenuBarBg"},
    {ImGuiCol_ScrollbarBg, "ScrollbarBg"},
    {ImGuiCol_ScrollbarGrab, "ScrollbarGrab"},
    {ImGuiCol_ScrollbarGrabHovered, "ScrollbarGrabHovered"},
    {ImGuiCol_ScrollbarGrabActive, "ScrollbarGrabActive"},
    {ImGuiCol_CheckMark, "CheckMark"},
    {ImGuiCol_SliderGrab, "SliderGrab"},
    {ImGuiCol_SliderGrabActive, "SliderGrabActive"},
    {ImGuiCol_Button, "Button"},
    {ImGuiCol_ButtonHovered, "ButtonHovered"},
    {ImGuiCol_ButtonActive, "ButtonActive"},
    {ImGuiCol_Header, "Header"},
    {ImGuiCol_HeaderHovered, "HeaderHovered"},
    {ImGuiCol_HeaderActive, "HeaderActive"},
    {ImGuiCol_Separator, "Separator"},
    {ImGuiCol_SeparatorHovered, "SeparatorHovered"},
    {ImGuiCol_SeparatorActive, "SeparatorActive"},
    {ImGuiCol_ResizeGrip, "ResizeGrip"},
    {ImGuiCol_ResizeGripHovered, "ResizeGripHovered"},
    {ImGuiCol_ResizeGripActive, "ResizeGripActive"},
    {ImGuiCol_Tab, "Tab"},
    {ImGuiCol_TabHovered, "TabHovered"},
    {ImGuiCol_TabSelected, "TabSelected"},
    {ImGuiCol_TabSelectedOverline, "TabSelectedOverline"},
    {ImGuiCol_TabDimmed, "TabDimmed"},
    {ImGuiCol_TabDimmedSelected, "TabDimmedSelected"},
    {ImGuiCol_TabDimmedSelectedOverline, "TabDimmedSelectedOverline"},
    {ImGuiCol_DockingPreview, "DockingPreview"},
    {ImGuiCol_DockingEmptyBg, "DockingEmptyBg"},
    {ImGuiCol_TextSelectedBg, "TextSelectedBg"},
    {ImGuiCol_NavHighlight, "NavHighlight"},
    {ImGuiCol_ModalWindowDimBg, "ModalWindowDimBg"},
    {ImGuiCol_DragDropTarget, "DragDropTarget"}
};

static ImVec4 hexToColor(const std::string& hex)
{
    if (hex.size() < 8)
        return ImVec4(0.0f, 0.0f, 0.0f, 0.0f);

    int32_t r, g, b, a;

    std::istringstream(hex.substr(0,2)) >> std::hex >> r;
    std::istringstream(hex.substr(2,2)) >> std::hex >> g;
    std::istringstream(hex.substr(4,2)) >> std::hex >> b;
    std::istringstream(hex.substr(6,2)) >> std::hex >> a;

    return ImVec4(r * 0.00392f, g * 0.00392f, b * 0.00392f, a * 0.00392f);
}

Preferences::Preferences(PlatformWindow *mainWindow, WindowManager &wm)
{
    m_globalSettings.m_mainWindow = mainWindow;
    m_globalSettings.m_wm = &wm;

    m_globalSettings.RefreshColorThemeList();
    LoadPreferences("Preferences.json");
}

Preferences::~Preferences(void)
{
    nlohmann::json json = {
        {"globalSettings", {
            {"fontSize",    m_globalSettings.fontSize},
            {"font",        m_globalSettings.m_fontName},
            {"colorTheme",  m_globalSettings.m_colorThemeName}
        }},
        {"uiSettings", {
            {"indentSpacing", m_uiSettings.indentSpacing}
        }},
        {"editorSettings", {
            {"tabSize",             m_editorSettings.tabSize},
            {"lineSpacing",         m_editorSettings.lineSpacing},
            {"showLineNumbers",     m_editorSettings.showLineNumbers},
            {"showWhitespaces",     m_editorSettings.showWhitespaces},
            {"shortTabs",           m_editorSettings.shortTabs},
            {"autoIndent",          m_editorSettings.autoIndent},
            //{"smoothScroll",        m_editorSettings.smoothScroll},
            //{"smoothScrollSpeed",   m_editorSettings.smoothScrollSpeed}
        }},
    };

    std::ofstream o("Preferences.json");
    o << std::setw(4) << json;
    o.close();
}

void Preferences::LoadPreferences(char* path) {
    if(!std::filesystem::exists(path))
    {
        std::cout << "Ivalid path (" << path << ")" << std::endl;
        m_globalSettings.SetFont(DEFAULT_FONT);
        m_globalSettings.SetColorTheme(DEFAULT_THEME);
        return;
    }

    std::ifstream stream(path);
    nlohmann::json json;
    stream >> json;

    std::string font = json[GLOBAL_SETTINGS]["font"].empty() ? DEFAULT_FONT : json[GLOBAL_SETTINGS]["font"];
    std::string colorTheme = json[GLOBAL_SETTINGS]["colorTheme"].empty() ? DEFAULT_FONT : json[GLOBAL_SETTINGS]["colorTheme"];

    m_globalSettings.SetFont(font);
    m_globalSettings.SetColorTheme(colorTheme);
    m_globalSettings.fontSize = json[GLOBAL_SETTINGS]["fontSize"];

    m_uiSettings.indentSpacing = json[UI_SETTINGS]["indentSpacing"];

    m_editorSettings.tabSize           = json[EDITOR_SETTINGS]["tabSize"];
    m_editorSettings.lineSpacing       = json[EDITOR_SETTINGS]["lineSpacing"];
    m_editorSettings.showLineNumbers   = json[EDITOR_SETTINGS]["showLineNumbers"];
    m_editorSettings.showWhitespaces   = json[EDITOR_SETTINGS]["showWhitespaces"];
    m_editorSettings.shortTabs         = json[EDITOR_SETTINGS]["shortTabs"];
    m_editorSettings.autoIndent        = json[EDITOR_SETTINGS]["autoIndent"];
    //m_editorSettings.smoothScroll      = json[EDITOR_SETTINGS]["smoothScroll"];
    //m_editorSettings.smoothScrollSpeed = json[EDITOR_SETTINGS]["smoothScrollSpeed"];
}

const std::vector<const char*> Preferences::GlobalSettings::GetColorThemeNameList(void) const
{
    std::vector<const char*> keys;
    keys.reserve(m_colorThemes.size());

    for (const auto& pair : m_colorThemes)
        keys.push_back(pair.first.c_str());

    return keys;
}

void Preferences::GlobalSettings::RefreshColorThemeList(void)
{
	namespace fs = std::filesystem;

    ImGuiIO &io = ImGui::GetIO();

    m_colorThemes.clear();
    m_fonts.clear();

	if (fs::exists("themes") && fs::is_directory("themes"))
        for (const auto &entry : fs::directory_iterator("themes"))
        {
            fs::path path = entry.path();

            if (!fs::is_regular_file(path))
                continue;
            
            fs::path name = path;
            name.replace_extension();
            m_colorThemes.insert({name.filename().string(), path});
        }

	if (fs::exists("fonts") && fs::is_directory("fonts"))
    {
        ImFontConfig fontConfig{};
        fontConfig.OversampleV = 2;
        fontConfig.OversampleH = 2;
        io.Fonts->Clear();
        for (const auto &entry : fs::directory_iterator("fonts"))
        {
            fs::path path = entry.path();

            if (!fs::is_regular_file(path))
                continue;
            
            fs::path name = path;
            name.replace_extension();
			std::string pathStr = path.string();

            m_fonts.insert({name.filename().string(), io.Fonts->AddFontFromFileTTF(pathStr.c_str(), 24.0f, &fontConfig)});
        }
    }
}

void Preferences::GlobalSettings::SetColorTheme(const std::string &name)
{
    m_colorThemeName = name;

    mINI::INIFile file(m_colorThemes[name].string());
    mINI::INIStructure ini;
    file.read(ini);

	ImVec4 *imColors = ImGui::GetStyle().Colors;

    for (const auto &[imguiCol, iniKey] : s_imguiColorMappings)
        imColors[imguiCol] = hexToColor(ini["UserInterface"][iniKey]);

#if CRYSTAL_PLATFORM_WINDOWS
    m_mainWindow->SetBorderColor(hexToColor(ini["UserInterface"]["Border"]));
	//#define PLATFORM_COLOR(_name) hexToPlatformColor(ini["Platform"][_name])
    /*m_mainWindow->SetColors(
        PLATFORM_COLOR("Titlebar"),
        PLATFORM_COLOR("Border"),
        PLATFORM_COLOR("Text")
    );*/
#endif

#define EDITOR_COLOR(_name) ImColor(hexToColor(ini["Editor"][_name]))

    m_editorPalette = {
        EDITOR_COLOR("Default"),
        EDITOR_COLOR("Keyword"),
        EDITOR_COLOR("Number"),
        EDITOR_COLOR("String"),
        EDITOR_COLOR("CharLiteral"),
        EDITOR_COLOR("Punctuation"),
        EDITOR_COLOR("Preprocessor"),
        EDITOR_COLOR("Identifier"),
        EDITOR_COLOR("KnownIdentifier"),
        EDITOR_COLOR("PreprocIdentifier"),
        EDITOR_COLOR("CommentSingleLine"),
        EDITOR_COLOR("CommentMultiLine"),
        EDITOR_COLOR("Background"),
        EDITOR_COLOR("Cursor"),
        EDITOR_COLOR("Selection"),
        EDITOR_COLOR("ErrorMarker"),
        EDITOR_COLOR("ControlCharacter"),
        EDITOR_COLOR("Breakpoint"),
        EDITOR_COLOR("LineNumber"),
        EDITOR_COLOR("CurrentLineFill"),
        EDITOR_COLOR("CurrentLineFillInactive"),
        EDITOR_COLOR("CurrentLineEdge")
    };

    m_wm->ForEachWindowOfType<EditorWindow>([&](EditorWindow *window) {
        TextEditor *editor = window->GetTextEditor();
        editor->SetPalette(m_editorPalette);
    });
}

void Preferences::GlobalSettings::SetFont(const std::string &name)
{
    m_fontName = name;
    ImGui::GetIO().FontDefault = m_fonts[name];
}

const char* Preferences::GlobalSettings::GetCurrentFontName(void) const
{
    return m_fontName.c_str();
}

const std::vector<const char*> Preferences::GlobalSettings::GetFontNameList(void) const
{
    std::vector<const char*> keys;
    keys.reserve(m_fonts.size());

    for (const auto& pair : m_fonts)
        keys.push_back(pair.first.c_str());

    return keys;
}

unsigned int Preferences::GlobalSettings::GetFontIndex(const char* fontName) const
{
    std::vector<const char*> list = GetFontNameList();
    for(int i = 0; i < list.size(); i++) {
        if(!strcmp(fontName, list[i]))
            return i;
    }

    return 0;
}

const char* Preferences::GlobalSettings::GetCurrentColorThemeName(void) const
{
    return m_colorThemeName.c_str();
}

unsigned int Preferences::GlobalSettings::GetColorThemeIndex(const char* fontName) const
{
    std::vector<const char*> list = GetColorThemeNameList();
    for(int i = 0; i < list.size(); i++)
    {
        if(!strcmp(fontName, list[i]))
            return i;
    }

    return 0;
}

void Preferences::Refresh(void)
{
    ImGuiIO &io = ImGui::GetIO();
    ImGuiStyle &style = ImGui::GetStyle();

    //io.FontDefault = m_globalSettings.m_fontPack->GetClosestFont(m_globalSettings.fontSize);

    io.FontGlobalScale = m_globalSettings.fontSize / 24.0f;
    style.IndentSpacing = m_uiSettings.indentSpacing;
}

}