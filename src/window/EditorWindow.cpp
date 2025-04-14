#include "EditorWindow.h"
#include "Application.h"
#include "Resources.h"
#include "Utils.h"

#include <fstream>
#include <sstream>
#include <iostream>

namespace Crystal
{

static std::string ConvertTabsToSpaces(const std::string &input, int tabWidth)
{
    std::string result;
    int currentColumn = 0;

    for (char c : input)
	{
        if (c == '\t')
		{
            int spacesToNextTabStop = tabWidth - (currentColumn % tabWidth);
            result.append(spacesToNextTabStop, ' ');
            currentColumn += spacesToNextTabStop;
        }
		else
		{
            result += c;
            currentColumn++;
        }
    }

    return result;
}

EditorWindow::EditorWindow(const std::filesystem::path &filePath)
{
	SetFilePath(filePath);

    std::ifstream filestream(filePath);
    
    if (filestream.is_open())
	{
		std::ostringstream ss;
		ss << filestream.rdbuf();
        m_editor.SetText(ss.str());
		m_editor.SetTextChanged(false);
		filestream.close();
    }
}

void EditorWindow::OnWindowAdded(void)
{
	m_editor.SetPalette(m_application->GetPreferences().GetGlobalSettings().GetEditorPalette());

	BuildErrorHandler &errorHandler = m_application->GetBuildErrorHandler();

	if (!errorHandler.HasAppliedErrorMarkers())
		return;

	std::vector<BuildErrorHandler::BuildError> filteredErrors = errorHandler.FilterErrorsByPath(m_filePath);

	if (filteredErrors.empty())
		return;

	TextEditor::ErrorMarkers markers;
	for (const auto &error : filteredErrors)
		markers.insert(std::make_pair(error.m_lineNumber - 1, error.m_errorMessage));
	m_editor.SetErrorMarkers(markers);
}

/*void EditorWindow::GetCurrentWord(std::string &word, int& start, int& end)
{
    std::string line = m_editor.GetCurrentLineText();
    const int cursorPosition = m_editor.GetCursorPosition().mColumn;

	line = ConvertTabsToSpaces(line, m_editor.GetTabSize());

    if (line.empty())
	{
        word.clear();
        start = end = cursorPosition;
        return;
    }

    // Check if the cursor is at the beginning of a word
    if (cursorPosition < line.size() && !Utils::isDelimiter(line[cursorPosition]))
    {
        if (cursorPosition == 0 || Utils::isDelimiter(line[cursorPosition - 1]))
        {
            // The cursor is at the beginning of a word
            word.clear();
            start = end = cursorPosition;
            return;
        }
    }

    start = cursorPosition;
    while (start > 0 && !Utils::isDelimiter(line[start - 1]))
        --start;

    end = cursorPosition;
    while (end < line.size() && !Utils::isDelimiter(line[end]))
        ++end;

    if (start < end && start >= 0 && end <= line.size())
        word = line.substr(start, end - start);
    else
        word.clear();
}*/

void EditorWindow::RenderWindow(void)
{
	//int start, end;
	//ImVec2 windowPosition;

	ImGui::SetNextWindowDockID(m_application->GetLayoutHandler().GetMainDockID(), ImGuiCond_Appearing);

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 0.0f, 0.0f });

	ImGuiWindowFlags flags = m_editor.IsTextChanged() ? ImGuiWindowFlags_UnsavedDocument : 0;
	if (ImGui::Begin(m_titleBuffer, &m_opened, flags))
	{
		PlatformWindow &mainWindow = m_application->GetMainWindow();
		WindowManager &wm = m_application->GetWindowManager();
		Preferences &preferences = m_application->GetPreferences();

		//SuggestionHandler &suggestionHandler = m_application->GetSuggestionHandler();
		Preferences::EditorSettings &settings = preferences.GetEditorSettings();
		m_editor.SetTabSize(settings.tabSize);
		m_editor.SetLineSpacing(settings.lineSpacing);
		m_editor.SetShowLineNumbersEnabled(settings.showLineNumbers);
		m_editor.SetShowWhitespacesEnabled(settings.showWhitespaces);
		m_editor.SetShortTabsEnabled(settings.shortTabs);
		m_editor.SetAutoIndentEnabled(settings.autoIndent);

		//m_findReplaceHandler.Render(m_editor);

		/*std::string word;
		GetCurrentWord(word, start, end);
		suggestionHandler.FilterSuggestions(word);

		static bool focusSuggestions = false;
		const bool hasSuggestions = suggestionHandler.HasSuggestions();

		if (ImGui::IsWindowFocused())
			ImGui::SetNextWindowFocus();

		bool isWindowFocused = ImGui::IsWindowFocused(ImGuiFocusedFlags_ChildWindows);

		if (isWindowFocused)
		{
			if (wm.GetLastWindow() != this)
				wm.SetLastWindow(this);
			
			if (hasSuggestions && ImGui::IsKeyPressed(ImGuiKey_Tab))
			{
				m_editor.SetReadOnly(true);
				focusSuggestions = true;
			}
		}

		m_editor.Render(c_title);

		if (hasSuggestions)
		{
			m_editor.SetReadOnly(false);

			if (wm.GetLastWindow() == this)
			{
				if (m_editor.IsFocused())
					ImGui::OpenPopup("##Suggestions");
				suggestionHandler.RenderSuggestions(m_editor, word, ImGui::GetWindowPos(), focusSuggestions, start, end);
			}
		}*/

		if (ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows) && wm.GetLastEditorWindow() != this)
			wm.SetLastEditorWindow(this);

		ImGui::SetWindowFontScale(preferences.GetGlobalSettings().editorFontSize / 24.0f / ImGui::GetIO().FontGlobalScale);
		m_editor.Render(m_titleBuffer);
		ImGui::SetWindowFontScale(1.0f);
	}

	ImGui::End();

	/*if (wm.GetLastEditorWindow() == this)
	{
		if (ImGui::Begin("##BottomMenuBar", nullptr, ImGuiWindowFlags_NoScrollbar))
		{
			int32_t line, column;
			m_editor.GetCursorPosition(line, column);

			char buffer[128];
			sprintf(buffer, "Ln %i, Col %i  Tab Size: %i  %s",
				line + 1, column + 1, m_editor.GetTabSize(), m_editor.GetLanguageDefinitionName());

			ImVec2 windowSize = ImGui::GetWindowSize();

			ImVec2 textSize = ImGui::CalcTextSize(buffer, nullptr, true, windowSize.x);
			ImGui::SetCursorPosX((windowSize.x - textSize.x) * 0.5f);

			ImGui::TextUnformatted(buffer);
		}
		ImGui::End();
	}*/

	ImGui::PopStyleVar();
}

void EditorWindow::SetFilePath(const std::filesystem::path &path)
{
	m_filePath = path;
	sprintf(m_titleBuffer, "%s##%s", path.filename().string().c_str(), path.string().c_str());

	std::filesystem::path extension = m_filePath.extension();
	if (extension == ".cpp" || extension == ".hpp" || extension == ".h")
		m_editor.SetLanguageDefinition(TextEditor::LanguageDefinition::Cpp());
	else if (extension == ".c")
		m_editor.SetLanguageDefinition(TextEditor::LanguageDefinition::C());
	else if (extension == ".cs")
		m_editor.SetLanguageDefinition(TextEditor::LanguageDefinition::Cs());
	else if (extension == ".hlsl")
		m_editor.SetLanguageDefinition(TextEditor::LanguageDefinition::Hlsl());
	else if (extension == ".glsl" || extension == ".shader" || extension == ".vert" || extension == ".frag")
		m_editor.SetLanguageDefinition(TextEditor::LanguageDefinition::Glsl());
	else if (extension == ".py")
		m_editor.SetLanguageDefinition(TextEditor::LanguageDefinition::Python());
	else if (extension == ".lua")
		m_editor.SetLanguageDefinition(TextEditor::LanguageDefinition::Lua());
	else if (extension == ".rs")
		m_editor.SetLanguageDefinition(TextEditor::LanguageDefinition::Rust());
	else if (extension == ".js" || extension == ".ts")
		m_editor.SetLanguageDefinition(TextEditor::LanguageDefinition::JavaScript());
	else if (extension == ".json")
		m_editor.SetLanguageDefinition(TextEditor::LanguageDefinition::Json());
	else if (extension == ".ini")
		m_editor.SetLanguageDefinition(TextEditor::LanguageDefinition::Ini());
	else if (extension == ".html")
		m_editor.SetLanguageDefinition(TextEditor::LanguageDefinition::Html());
	else if (extension == ".css")
		m_editor.SetLanguageDefinition(TextEditor::LanguageDefinition::Css());
	else if (extension == ".gml")
		m_editor.SetLanguageDefinition(TextEditor::LanguageDefinition::Gml());
	else
		m_editor.SetLanguageDefinition(TextEditor::LanguageDefinition::Text());
}

void EditorWindow::SaveToFile(void)
{
	std::ofstream file(m_filePath);
	if (file.is_open())
	{
		file << m_editor.GetText();
		m_editor.SetTextChanged(false);
		file.close();
	}
}

void EditorWindow::FindReplaceHandler::FindNext(TextEditor &editor, const std::string &word)
{
		
}

void EditorWindow::FindReplaceHandler::FindPrevious(TextEditor &editor, const std::string &word)
{

}

void EditorWindow::FindReplaceHandler::Replace(TextEditor &editor, const std::string &word, const std::string &replacement)
{

}

void EditorWindow::FindReplaceHandler::ReplaceAll(TextEditor &editor, const std::string &word, const std::string &replacement)
{

}

void EditorWindow::FindReplaceHandler::Render(TextEditor &editor)
{
	if (ImGui::IsKeyPressed(ImGuiKey_Escape))
	{
		m_active = false;
	}

	if (!m_active)
		return;

	ImGuiStyle &style = ImGui::GetStyle();

	ImGui::BeginChild("##FindReplaceChild", ImVec2(0, 200), true);

	static char inputFindText[1024] = "";
	static char inputReplaceText[1024] = "";
	static bool matchCase = false;
	static bool matchWholeWord = false;

	ImGui::InputText("##Find", inputFindText, IM_ARRAYSIZE(inputFindText));
	if (!*inputFindText)
	{
		ImGui::SetCursorPosX(ImGui::GetCursorPosX() + style.WindowPadding.x * 0.5f + 1);
		float cursorPosY = ImGui::GetCursorPosY();
		ImGui::SetCursorPosY(cursorPosY - ImGui::GetFontSize() - style.WindowPadding.y * 0.5f);
		ImGui::TextDisabled("Find");
		ImGui::SetCursorPosY(cursorPosY);
	}
	ImGui::InputText("##Replace", inputReplaceText, IM_ARRAYSIZE(inputReplaceText));
	if (!*inputReplaceText)
	{
		ImGui::SetCursorPosX(ImGui::GetCursorPosX() + style.WindowPadding.x * 0.5f + 1);
		float cursorPosY = ImGui::GetCursorPosY();
		ImGui::SetCursorPosY(cursorPosY - ImGui::GetFontSize() - style.WindowPadding.y * 0.5f);
		ImGui::TextDisabled("Replace");
		ImGui::SetCursorPosY(cursorPosY);
	}
	ImGui::Checkbox("Match case", &matchCase);
	ImGui::SameLine();
	ImGui::Checkbox("Match whole word", &matchWholeWord);

	// Buttons for Find Next, Replace, and Replace All
	if (ImGui::Button("Find Next"))
	{
		FindNext(editor, inputFindText);
	}

	ImGui::SameLine();

	if (ImGui::Button("Find Previous"))
	{
		FindPrevious(editor, inputFindText);
	}

	ImGui::SameLine();
	if (ImGui::Button("Replace"))
	{
		Replace(editor, inputFindText, inputReplaceText);
	}

	ImGui::SameLine();
	if (ImGui::Button("Replace All"))
	{
		ReplaceAll(editor, inputFindText, inputReplaceText);
	}
	ImGui::EndChild();
}

}