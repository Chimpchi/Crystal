#include "ExplorerWindow.h"
#include "EditorWindow.h"
#include "ImageWindow.h"
#include "Application.h"
#include "Resources.h"

#include <fstream>
#include <unordered_map>

#if _WIN32
#include <windows.h>
#endif

#include "imgui_internal.h"

namespace Crystal
{

namespace fs = std::filesystem;

std::unordered_map<fs::path, std::string> s_iconMap = {
    { ".txt", "txt" },
    { ".ini", "conf" },
    { ".cfg", "conf" },
    { ".png", "imagepng" },
    { ".jpg", "imagejpg" },
    { ".webp", "imagewebp" },
    { ".gif", "imagegif" },
    { ".ico", "imageico" },
    { ".svg", "svg" },
    { ".lua", "lua" },
    { ".cpp", "cpp" },
    { ".hpp", "hpp" },
    { ".c", "c" },
    { ".h", "h" },
    { ".hs", "haskell" },
    { ".cs", "csharp" },
    { ".cshtml", "cshtml" },
    { ".csproj", "csproj" },
    { ".fs", "fsharp" },
    { ".sln", "sln" },
    { ".zip", "archive" },
    { ".rar", "archive" },
    { ".tar", "archive" },
    { ".7z", "archive" },
    { ".gz", "archive" },
    { ".bat", "bat" },
    { ".sh", "sh" },
    { ".rs", "rust" },
    { ".cargo", "cargo" },
    { ".go", "go" },
    { ".py", "python" },
    { ".java", "java" },
    { ".java", "jar" },
    { ".kt", "kotlin" },
    { ".json", "json" },
    { ".js", "javascript" },
    { ".ts", "typescript" },
    { ".html", "html" },
    { ".css", "css" },
    { ".php", "php" },
	{ ".odin", "odin" },
    { ".md", "markdown" },
    { ".rb", "ruby" },
    { ".dart", "dart" },
    { ".mp4", "mp4" },
    { ".mov", "mov" },
    { ".avi", "video" },
    { ".wmv", "video" },
    { ".flv", "video" },
    { ".mkv", "video" },
    { ".mp3", "audiomp3" },
    { ".wav", "audiowav" },
    { ".ogg", "audioogg" },
    { ".flac", "audio" },
    { ".db", "database" },
    { ".pdb", "database" },
    { ".psd", "photoshop" },
    { ".otf", "fontotf" },
    { ".ttf", "fontttf" },
    { ".woff", "fontwoff" },
    { ".woff2", "fontwoff2" },
    { ".obj", "obj" },
    { ".bin", "binary" },
    { ".exe", "binary" },
    { ".glsl", "glsl" },
    { ".gml", "gml" },
    { ".gitignore", "git" },
    { "makefile", "makefile" },
    { "Makefile", "makefile" }
};

static bool FileTreeNode(const char *label, const fs::path &path)
{
    const ImGuiStyle& style = ImGui::GetStyle();

    ImU32 id = ImGui::GetID(label);
    float x = ImGui::GetCursorPosX();
    ImGui::BeginGroup();
    bool clicked = ImGui::InvisibleButton(label, ImVec2(-1, ImGui::GetFontSize() + style.FramePadding.y*0.5f));
    bool hovered = ImGui::IsItemHovered();
    bool active = ImGui::IsItemActive();
    if (hovered || active )
        ImGui::GetWindowDrawList()->AddRectFilled(ImGui::GetItemRectMin(), ImGui::GetItemRectMax(), ImColor(ImGui::GetStyle().Colors[active ? ImGuiCol_HeaderActive : ImGuiCol_HeaderHovered]));

    ImGui::SameLine(x);

    std::string iconTag = s_iconMap[path.extension()];
    if (iconTag.empty() && (iconTag = s_iconMap[path.filename()]).empty())
        iconTag = "file";

    const float scale = ImGui::GetFontSize();
    const float initialY = ImGui::GetCursorPosY();
    ImGui::SetCursorPosY(initialY + style.FramePadding.y * 0.25f);
    ImGui::Image(Resources::GetTextureByName(iconTag).id, ImVec2(scale, scale),
        ImVec2(0.0f, 0.0f), ImVec2(1.0f, 1.0f));

    ImGui::SetCursorPosY(initialY);

    ImGui::SameLine();
    ImGui::Text(label);
    ImGui::EndGroup();

    if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_AcceptBeforeDelivery))
    {
        ImGui::SetDragDropPayload("FILE_PATH", path.string().c_str(), path.string().size() + 1, ImGuiCond_Appearing);
        ImGui::Text("%s", label);
        ImGui::EndDragDropSource();
    }

    return clicked;
}

static bool DirectoryTreeNode(const char* label, const fs::path &path)
{
    const ImGuiStyle& style = ImGui::GetStyle();
    ImGuiStorage* storage = ImGui::GetStateStorage();

    ImU32 id = ImGui::GetID(label);
    bool opened = storage->GetInt(id, 0);
    float x = ImGui::GetCursorPosX();
    ImGui::BeginGroup();
    if (ImGui::InvisibleButton(label, ImVec2(-1, ImGui::GetFontSize() + style.FramePadding.y*0.5f)))
    {
        int* p_opened = storage->GetIntRef(id, 0);
        opened = *p_opened = !*p_opened;
    }
    bool hovered = ImGui::IsItemHovered();
    bool active = ImGui::IsItemActive();
    if (hovered || active)
        ImGui::GetWindowDrawList()->AddRectFilled(ImGui::GetItemRectMin(), ImGui::GetItemRectMax(), ImColor(ImGui::GetStyle().Colors[active ? ImGuiCol_HeaderActive : ImGuiCol_HeaderHovered]));

    ImGui::SameLine(x);

    const float scale = ImGui::GetFontSize();
    const float initialY = ImGui::GetCursorPosY();
    ImGui::SetCursorPosY(initialY + style.FramePadding.y * 0.25f);
    ImGui::Image(opened ? Resources::GetTextureByName("folder_open").id :
        Resources::GetTextureByName("folder").id, ImVec2(scale, scale),
        ImVec2(0.0f, 0.0f), ImVec2(1.0f, 1.0f));

    ImGui::SetCursorPosY(initialY);

    ImGui::SameLine();
    ImGui::Text(label);
    ImGui::EndGroup();

    if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_AcceptBeforeDelivery))
    {
        ImGui::SetDragDropPayload("DIRECTORY_PATH", path.string().c_str(), path.string().size() + 1, ImGuiCond_Appearing);
        ImGui::Text("%s", label);
        ImGui::EndDragDropSource();
    }

    if (opened)
        ImGui::TreePush(label);

    return opened;
};

static fs::path s_currentPath;

void ExplorerWindow::RenderBranch(const fs::path &directory)
{
    if (!fs::exists(directory) || !fs::is_directory(directory))
        return;

    if (ImGui::IsWindowHovered() && !ImGui::IsAnyItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Right))
    {
        s_currentPath = m_application->GetMainDirectoryPath();
        ImGui::OpenPopup("##ExplorerMainDirectoryPopup");
    }

    for (const auto &entry : fs::directory_iterator(directory))
    {
        fs::path path = entry.path();

        if (fs::is_directory(path))
        {
            bool isOpened = DirectoryTreeNode(path.filename().string().c_str(), path);
            m_application->GetDragDropHandler().BeginDragDropFileMoveTarget(path);

            if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Right))
            {
                s_currentPath = path;
                ImGui::OpenPopup("##ExplorerDirectoryPopup");
            }
            else if (isOpened)
            {
                ExplorerWindow::RenderBranch(path);
                ImGui::TreePop();
            }
            m_mainDragDropTargetPosition = ImGui::GetCursorPosY();
        }
    }

    for (const auto &entry : fs::directory_iterator(directory))
    {
        fs::path path = entry.path();

        if (fs::is_regular_file(path))
        {
            if (FileTreeNode(path.filename().string().c_str(), path))
                m_application->OpenFile(path);
            else if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Right))
            {
                s_currentPath = path;
                ImGui::OpenPopup("##ExplorerFilePopup");
            }
        }
    }

    static bool confirmDeletePopupOpened = false;
    static bool createFilePopupOpened = false;
    static bool createDirectoryPopupOpened = false;
    static bool renameItemPopupOpened = false;

    ImGuiViewport *viewport = ImGui::GetMainViewport();

    if (viewport->PlatformWindowCreated && GImGui->PlatformIO.Platform_GetWindowFocus(viewport))
        ImGui::SetNextWindowViewport(viewport->ID);

    if (ImGui::BeginPopup("##ExplorerFilePopup"))
    {
#if _WIN32
        if (ImGui::MenuItem("View in File Explorer")) { ShellExecuteA(NULL, "open", s_currentPath.remove_filename().string().c_str(), NULL, NULL, SW_SHOWDEFAULT); }
#endif
        if (ImGui::MenuItem("Rename"))
            renameItemPopupOpened = true;

        if (ImGui::MenuItem("Delete"))
            confirmDeletePopupOpened = true;

        ImGui::EndPopup();
    }

    if (viewport->PlatformWindowCreated && GImGui->PlatformIO.Platform_GetWindowFocus(viewport))
        ImGui::SetNextWindowViewport(viewport->ID);

    if (ImGui::BeginPopup("##ExplorerDirectoryPopup"))
    {
        if (ImGui::MenuItem("New File"))
            createFilePopupOpened = true;

        if (ImGui::MenuItem("New Folder"))
            createDirectoryPopupOpened = true;

#if _WIN32
        if (ImGui::MenuItem("View in File Explorer")) { ShellExecuteA(NULL, "open", s_currentPath.remove_filename().string().c_str(), NULL, NULL, SW_SHOWDEFAULT); }
#endif
        if (ImGui::MenuItem("Rename"))
            renameItemPopupOpened = true;

        if (ImGui::MenuItem("Delete"))
            confirmDeletePopupOpened = true;

        ImGui::EndPopup();
    }

    if (viewport->PlatformWindowCreated && GImGui->PlatformIO.Platform_GetWindowFocus(viewport))
        ImGui::SetNextWindowViewport(viewport->ID);

    if (ImGui::BeginPopup("##ExplorerMainDirectoryPopup"))
    {
        if (ImGui::MenuItem("New File"))
            createFilePopupOpened = true;

        if (ImGui::MenuItem("New Folder"))
            createDirectoryPopupOpened = true;

#if _WIN32
        if (ImGui::MenuItem("View in File Explorer")) { ShellExecuteA(NULL, "open", s_currentPath.remove_filename().string().c_str(), NULL, NULL, SW_SHOWDEFAULT); }
#endif

        ImGui::EndPopup();
    }

    static char inputText[64];

    if (confirmDeletePopupOpened)
    {
        ImGui::OpenPopup("##ConfirmDeletePopup");
        confirmDeletePopupOpened = false;
    }

    if (createFilePopupOpened)
    {
        ImGui::OpenPopup("##CreateFilePopupOpened");
        memset(inputText, 0, sizeof(inputText));
        createFilePopupOpened = false;
    }

    if (createDirectoryPopupOpened)
    {
        ImGui::OpenPopup("##CreateDirectoryPopupOpened");
        memset(inputText, 0, sizeof(inputText));
        createDirectoryPopupOpened = false;
    }

    if (renameItemPopupOpened)
    {
        ImGui::OpenPopup("##RenameItemPopupOpened");
        memcpy(inputText, s_currentPath.filename().string().c_str(), sizeof(inputText));
        renameItemPopupOpened = false;
    }

    if (viewport->PlatformWindowCreated && GImGui->PlatformIO.Platform_GetWindowFocus(viewport))
        ImGui::SetNextWindowViewport(viewport->ID);

    const ImVec2 mouseCurosrPosition = ImGui::GetMousePos();

    ImGui::SetNextWindowPos(mouseCurosrPosition, ImGuiCond_Appearing);
    if (ImGui::BeginPopupModal("##ConfirmDeletePopup", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_AlwaysAutoResize))
    {
        const std::string title = "Are you sure you want to delete " + s_currentPath.filename().string() + "?";
        ImGui::Text(title.c_str());

        if (!ImGui::IsWindowHovered() && !ImGui::IsAnyItemHovered() && ImGui::IsAnyMouseDown() || ImGui::IsKeyPressed(ImGuiKey_Escape, false))
            ImGui::CloseCurrentPopup();

        if (ImGui::Button("Yes"))
        {
            fs::remove_all(s_currentPath);
            ImGui::CloseCurrentPopup();
        }
        ImGui::SameLine();
        if (ImGui::Button("Never mind"))
            ImGui::CloseCurrentPopup();

        ImGui::EndPopup();
    }

    if (viewport->PlatformWindowCreated && GImGui->PlatformIO.Platform_GetWindowFocus(viewport))
        ImGui::SetNextWindowViewport(viewport->ID);

    ImGui::SetNextWindowPos(mouseCurosrPosition, ImGuiCond_Appearing);
    if (ImGui::BeginPopupModal("##CreateFilePopupOpened", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_AlwaysAutoResize))
    {
        if (!ImGui::IsWindowHovered() && !ImGui::IsAnyItemHovered() && ImGui::IsAnyMouseDown() || ImGui::IsKeyPressed(ImGuiKey_Escape, false))
            ImGui::CloseCurrentPopup();

        if (!ImGui::IsAnyItemActive()) 
            ImGui::SetKeyboardFocusHere();

        ImGui::InputText("Enter Name", inputText, IM_ARRAYSIZE(inputText));

        if (ImGui::Button("Create") || ImGui::IsKeyPressed(ImGuiKey_Enter, false))
        {
            std::ofstream file(s_currentPath / inputText);
			file.close();
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }

    if (viewport->PlatformWindowCreated && GImGui->PlatformIO.Platform_GetWindowFocus(viewport))
        ImGui::SetNextWindowViewport(viewport->ID);

    ImGui::SetNextWindowPos(mouseCurosrPosition, ImGuiCond_Appearing);
    if (ImGui::BeginPopupModal("##CreateDirectoryPopupOpened", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_AlwaysAutoResize))
    {
        if (!ImGui::IsWindowHovered() && !ImGui::IsAnyItemHovered() && ImGui::IsAnyMouseDown() || ImGui::IsKeyPressed(ImGuiKey_Escape, false))
            ImGui::CloseCurrentPopup();

        if (!ImGui::IsAnyItemActive()) 
            ImGui::SetKeyboardFocusHere();

        ImGui::InputText("Enter Name", inputText, IM_ARRAYSIZE(inputText));

        if (ImGui::Button("Create") || ImGui::IsKeyPressed(ImGuiKey_Enter, false))
        {
            fs::create_directory(s_currentPath / inputText);
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }

    if (viewport->PlatformWindowCreated && GImGui->PlatformIO.Platform_GetWindowFocus(viewport))
        ImGui::SetNextWindowViewport(viewport->ID);

    ImGui::SetNextWindowPos(mouseCurosrPosition, ImGuiCond_Appearing);
    if (ImGui::BeginPopupModal("##RenameItemPopupOpened", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_AlwaysAutoResize))
    {
        if (!ImGui::IsWindowHovered() && !ImGui::IsAnyItemHovered() && ImGui::IsAnyMouseDown() || ImGui::IsKeyPressed(ImGuiKey_Escape, false))
            ImGui::CloseCurrentPopup();

        if (!ImGui::IsAnyItemActive()) 
            ImGui::SetKeyboardFocusHere();

        ImGui::InputText("Enter Name", inputText, IM_ARRAYSIZE(inputText));

        if (ImGui::Button("Rename") || ImGui::IsKeyPressed(ImGuiKey_Enter, false))
        {
            if (EditorWindow *editorWindow = dynamic_cast<EditorWindow*>(m_application->GetWindowManager().CheckForWindowWithPath(s_currentPath)))
                editorWindow->SetFilePath(s_currentPath.parent_path() / inputText);
            else if (ImageWindow *imageWindow = dynamic_cast<ImageWindow*>(m_application->GetWindowManager().CheckForWindowWithPath(s_currentPath)))
                imageWindow->SetFilePath(s_currentPath.parent_path() / inputText);

            try {
                fs::rename(s_currentPath, s_currentPath.parent_path() / inputText);
            }
            catch (const std::filesystem::filesystem_error& e) {
                std::cerr << "Error renaming file: " << e.what() << '\n';
            }

            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }
}

void ExplorerWindow::RenderWindow(void)
{
	ImGui::PushStyleColor(ImGuiCol_TabSelectedOverline, {0.0f, 0.0f, 0.0f, 0.0f});
	ImGui::PushStyleColor(ImGuiCol_TabDimmedSelectedOverline, {0.0f, 0.0f, 0.0f, 0.0f});

	ImGui::SetNextWindowDockID(m_application->GetLayoutHandler().GetLeftDockID(), ImGuiCond_FirstUseEver);

    if (ImGui::Begin("Explorer", &m_opened))
	{
        fs::path directory = m_application->GetMainDirectoryPath();

        ImGuiStyle &style = ImGui::GetStyle();
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { style.ItemSpacing.x, style.ItemSpacing.y * 1.2f });

        if (!directory.empty())
		    RenderBranch(directory);

        ImGui::SetCursorPosY(m_mainDragDropTargetPosition);
        ImGui::Dummy(ImVec2(0.0f, ImGui::GetScrollY()) + ImGui::GetContentRegionAvail());

        m_application->GetDragDropHandler().BeginDragDropFileMoveTarget(directory);

        ImGui::PopStyleVar();
	}
    ImGui::End();

	ImGui::PopStyleColor(2);
}

}