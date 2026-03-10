#include "Editor.h"
#include "TilemapEditorScene.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_dx9.h"
#include "imgui/imgui_impl_win32.h"

void Editor::Editor::Init()
{
	IGame::Init();
	auto app = DX9GF::Application::GetInstance();
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGui::StyleColorsDark();
	ImGui_ImplWin32_Init(app->GetHWnd());
	ImGui_ImplDX9_Init(GetGraphicsDevice()->GetDevice());
	this->sceneManager->PushScene(new TilemapEditorScene(this, app->GetScreenWidth(), app->GetScreenHeight()));
	this->sceneManager->GoToNext();
}

void Editor::Editor::OnResize(UINT width, UINT height)
{
	// ImGui DX9 backend keeps D3DPOOL_DEFAULT resources that must be released before a device Reset.
	ImGui_ImplDX9_InvalidateDeviceObjects();
	IGame::OnResize(width, height);
	ImGui_ImplDX9_CreateDeviceObjects();
}

void Editor::Editor::Dispose()
{
	ImGui_ImplDX9_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
	IGame::Dispose();
}
