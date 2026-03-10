#include "DX9GFUtils.h"
#include "DX9GFApplication.h"

std::vector<RECT> DX9GF::Utils::CreateFrames(UINT sheetWidth, UINT sheetHeight, UINT frameWidth, UINT frameHeight, UINT frameCount, UINT offset)
{
	std::vector<RECT> frames;
	frames.reserve(frameCount);

	UINT framesPerRow = sheetWidth / frameWidth;

	for (UINT i = 0; i < frameCount; ++i)
	{
		UINT frameIndex = offset + i;
		UINT row = frameIndex / framesPerRow;
		UINT col = frameIndex % framesPerRow;

		RECT rect;
		rect.left = col * frameWidth;
		rect.top = row * frameHeight;
		rect.right = rect.left + frameWidth;
		rect.bottom = rect.top + frameHeight;

		frames.push_back(rect);
	}

	return frames;
}

bool DX9GF::Utils::IsWithinRectangle(float pointX, float pointY, float rectX, float rectY, float rectWidth, float rectHeight)
{
	return pointX > rectX 
		&& pointX < rectX + rectWidth
		&& pointY > rectY
		&& pointY < rectY + rectHeight;
}

void DX9GF::Utils::SetMousePos(int x, int y)
{
	POINT p;
	p.x = x;
	p.y = y;
	if (ClientToScreen(DX9GF::Application::GetInstance()->GetHWnd(), &p)) {
		SetCursorPos(p.x, p.y);
	}
}

std::tuple<float, float> DX9GF::Utils::WindowToWorldCoords(const DX9GF::Camera& camera, float windowX, float windowY)
{
	auto app = DX9GF::Application::GetInstance();
	auto [cameraX, cameraY] = camera.GetPosition();
	float worldX = -windowX + cameraX - app->GetScreenWidth() / 2;
	float worldY = -windowY + cameraY - app->GetScreenHeight() / 2;
	return std::make_tuple(worldX, worldY);
}

std::tuple<float, float> DX9GF::Utils::WorldToWindowCoords(const DX9GF::Camera& camera, float worldX, float worldY)
{
	auto app = DX9GF::Application::GetInstance();
	auto [cameraX, cameraY] = camera.GetPosition();
	float windowX = worldX - cameraX + app->GetScreenWidth() / 2;
	float windowY = worldY - cameraY + app->GetScreenHeight() / 2;
	return std::make_tuple(windowX, windowY);
}

std::wstring DX9GF::Utils::ToWide(const char* s)
{
	if (s == nullptr) return L"";

	int required = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, s, -1, nullptr, 0);
	UINT codePage = CP_UTF8;
	DWORD flags = MB_ERR_INVALID_CHARS;
	if (required == 0) {
		codePage = CP_ACP;
		flags = 0;
		required = MultiByteToWideChar(codePage, flags, s, -1, nullptr, 0);
	}
	if (required == 0) return L"";

	std::wstring result(static_cast<size_t>(required - 1), L'\0');
	MultiByteToWideChar(codePage, flags, s, -1, result.data(), required);
	return result;
}
