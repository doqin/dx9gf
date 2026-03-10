#include "TilesetEditorScene.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_dx9.h"
#include "imgui/imgui_impl_win32.h"

void Editor::TilesetEditorScene::Init()
{
}

void Editor::TilesetEditorScene::Update(unsigned long long deltaTime)
{
	ImGui_ImplDX9_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
	if (ImGui::Begin("Tileset tools")) {
		if (ImGui::Button("Finish")) {
			auto sm = editor->GetSceneManager();
			sm->PopScene();
			sm->GoToPrevious();
			ImGui::End();
			ImGui::EndFrame();
			return;
		}
		ImGui::End();
	}
	ImGui::EndFrame();
	camera.Update();
}

void Editor::TilesetEditorScene::Draw(unsigned long long deltaTime)
{
	auto dev = editor->GetGraphicsDevice();
	dev->Clear();
	if (SUCCEEDED(dev->BeginDraw())) {
		auto tilesetLock = tileset.lock();
		auto spritesheetLock = tilesetLock->spritesheet.lock();
		spritesheetLock->Begin();
		spritesheetLock->Draw(camera, deltaTime);
		spritesheetLock->End();
		ImGui::Render();
		ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
		dev->EndDraw();
	}

	dev->Present();
}
