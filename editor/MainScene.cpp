#include "MainScene.h"
#include "imgui/imgui.h"
#include "imgui_impl_dx9.h"
#include "imgui_impl_win32.h"
#include "Win32.h"
#include <stdexcept>
#include "C:/Program Files (x86)/Microsoft DirectX SDK (June 2010)/Include/DxErr.h"

void Editor::MainScene::Init()
{
}

void Editor::MainScene::Update(unsigned long long deltaTime)
{
	// ImGui stuff
	ImGui_ImplDX9_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
	if (ImGui::Begin("Tilemap Tools")) {
		if (ImGui::Button("Load spritesheets")) { // returns true when you click it
			Win32::OpenFileDialog(L"Images (*.png;*.jpg;*.jpeg)\0*.png;*.jpg;*.jpeg\0All Files\0*.*\0\0");
		}
		ImGui::InputInt("Grid Spacing X", &gridSpacingX);
		ImGui::InputInt("Grid Spacing Y", &gridSpacingY);
		ImGui::SliderFloat("Grid Color Red", &gridColorR, 0, 1);
		ImGui::SliderFloat("Grid Color Green", &gridColorG, 0, 1);
		ImGui::SliderFloat("Grid Color Blue", &gridColorB, 0, 1);
		ImGui::SliderFloat("Grid Color Alpha", &gridColorA, 0, 1);
		ImGui::Checkbox("Show Grid", &showGrid);
		ImGui::End();
	}
	else {
		// throw std::runtime_error("Could not build tilemap tool UI");
	}
	ImGui::EndFrame();
	// Regular stuff
	auto input = DX9GF::InputManager::GetInstance();
	input->ReadMouse(deltaTime);
	input->ReadKeyboard(deltaTime);
	if (input->MousePress(DX9GF::InputManager::MouseButton::Middle)) {
		auto dX = input->GetRelativeMouseX();
		auto dY = input->GetRelativeMouseY();
		auto cameraPos = camera.GetPosition();
		camera.SetPosition(
			cameraPos.x - dX,
			cameraPos.y - dY
		);
	}
	camera.Update();
}

void Editor::MainScene::Draw(unsigned long long deltaTime)
{
	auto dev = editor->GetGraphicsDevice();
	dev->Clear();
	if (auto result = dev->BeginDraw(); SUCCEEDED(result)) {
		auto app = DX9GF::Application::GetInstance();
		// Normal renders
		if (showGrid) {
			DX9GF::Debug::DrawGrid(editor->GetGraphicsDevice(), camera, 0, 0, app->GetScreenWidth(), app->GetScreenWidth(), gridSpacingX, gridSpacingY, D3DXCOLOR(gridColorR * gridColorA, gridColorG * gridColorA, gridColorB * gridColorA, gridColorA));
		}
		// Render ImGui uis
		ImGui::Render();
		ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
		dev->EndDraw();
	}
	else {
		auto error = DXGetErrorDescription(result);
		std::string what = std::string(error, error + wcslen(error));
		throw std::runtime_error("Graphics device couldn't draw: " + what);
	}
	dev->Present();
}
