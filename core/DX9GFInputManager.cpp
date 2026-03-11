#include "DX9GFInputManager.h"
#include <stdexcept>
#include <DxErr.h>
#include "DX9GFApplication.h"

DX9GF::InputManager* DX9GF::InputManager::instance = nullptr;

bool DX9GF::InputManager::CheckKeysBuffer(int DIKey) const
{
    return keysBuffer[DIKey] & 0x80;
}

bool DX9GF::InputManager::CheckMouseBuffer(MouseButton button)
{
    return mouseBuffer[button] & 0x80;
}

void DX9GF::InputManager::UpdateMouseBuffer(int buttonIndex)
{
    if (firstMouseCheck && (diMouseState.rgbButtons[buttonIndex] != mouseBuffer[buttonIndex])) {
        lastMouseBufferPressed = lastMouseButtonPressed;
        lastMouseButtonPressed = buttonIndex;
        mouseDelta = 0;
        firstMouseCheck = false;
    }
}

DX9GF::InputManager* DX9GF::InputManager::GetInstance()
{
    if (instance == nullptr) {
        instance = new InputManager();
    }
    return instance;
}

void DX9GF::InputManager::Init(HWND hWnd, HINSTANCE hInstance)
{
    HRESULT result = DirectInput8Create(hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&dinput, NULL);
    if (result != DI_OK) {
        auto error = DXGetErrorDescription(result);
        std::string what = std::string(error, error + wcslen(error));
        throw std::runtime_error(what);
    }
    result = dinput->CreateDevice(GUID_SysKeyboard, &diKeyboard, NULL);
    if (result != DI_OK) {
        auto error = DXGetErrorDescription(result);
        std::string what = std::string(error, error + wcslen(error));
        throw std::runtime_error(what);
    }
    diKeyboard->SetDataFormat(&c_dfDIKeyboard);
    diKeyboard->SetCooperativeLevel(hWnd, DISCL_NONEXCLUSIVE | DISCL_FOREGROUND);
    result = diKeyboard->Acquire();
    if (result != DI_OK) {
        auto error = DXGetErrorDescription(result);
        std::string what = std::string(error, error + wcslen(error));
        throw std::runtime_error(what);
    }

    result = dinput->CreateDevice(GUID_SysMouse, &diMouse, NULL);
    if (result != DI_OK) {
        auto error = DXGetErrorDescription(result);
        std::string what = std::string(error, error + wcslen(error));
        throw std::runtime_error(what);
    }
    diMouse->SetDataFormat(&c_dfDIMouse);
    diMouse->SetCooperativeLevel(hWnd, DISCL_NONEXCLUSIVE | DISCL_FOREGROUND);
    result = diMouse->Acquire();
    if (result != DI_OK) {
        auto error = DXGetErrorDescription(result);
        std::string what = std::string(error, error + wcslen(error));
        throw std::runtime_error(what);
    }
}

void DX9GF::InputManager::ReadKeyboard(unsigned long long deltaTime)
{
    keysDelta += deltaTime;
    if (keysDelta > KEYBOARD_LAST_PRESS_TIME) {
        ReleaseLastPressed();
    }
    for (int i = 0; i < 256; i++) {
        keysBuffer[i] = keys[i];
    }
    HRESULT result = diKeyboard->GetDeviceState(sizeof(keys), (LPVOID)&keys);
    while (result != DI_OK && (result == DIERR_INPUTLOST || result == DIERR_NOTACQUIRED)) {
        result = diKeyboard->Acquire();
    }
    firstKeyCheck = true;
}

bool DX9GF::InputManager::KeyPress(int DIKey)
{
    if (keys[DIKey] & 0x80) {
        if (firstKeyCheck && keysBuffer[DIKey] != keys[DIKey]) {
            lastBufferPressed = lastKeyPressed;
            lastKeyPressed = DIKey;
            keysDelta = 0;
            firstKeyCheck = false;
        }
        return true;
    }
    return false;
}

bool DX9GF::InputManager::KeyDown(int DIKey)
{
    return KeyPress(DIKey) && !CheckKeysBuffer(DIKey);
}

bool DX9GF::InputManager::KeyUp(int DIKey)
{
    return !KeyPress(DIKey) && CheckKeysBuffer(DIKey);
}

void DX9GF::InputManager::ReleaseLastPressed()
{
    keysDelta = 0;
    lastKeyPressed = -1;
    lastBufferPressed = -1;
}

void DX9GF::InputManager::ReadMouse(unsigned long long deltaTime)
{
    mouseDelta += deltaTime;
    if (mouseDelta > MOUSE_LAST_PRESS_TIME) {
        mouseDelta = 0;
        lastMouseButtonPressed = -1;
        lastMouseBufferPressed = -1;
    }
    for (int i = 0; i < 4; i++) {
        mouseBuffer[i] = diMouseState.rgbButtons[i];
    }
    GetCursorPos(&mousePos);
    ScreenToClient(DX9GF::Application::GetInstance()->GetHWnd(), &mousePos);
	if (!hasMousePos) {
		lastMousePos = mousePos;
		relativeMousePos = { 0, 0 };
		hasMousePos = true;
	} else {
		relativeMousePos = { mousePos.x - lastMousePos.x, mousePos.y - lastMousePos.y };
		lastMousePos = mousePos;
	}
    HRESULT result = diMouse->GetDeviceState(sizeof(diMouseState), (LPVOID)&diMouseState);
    while (result != DI_OK && (result == DIERR_INPUTLOST || result == DIERR_NOTACQUIRED)) {
        result = diMouse->Acquire();
    }
    firstMouseCheck = true;
}

bool DX9GF::InputManager::MousePress(MouseButton button)
{
    if (diMouseState.rgbButtons[button] & 0x80) {
        UpdateMouseBuffer(button);
        return true;
    }
    return false;
}

bool DX9GF::InputManager::MouseDown(MouseButton button)
{
    return MousePress(button) && !CheckMouseBuffer(button);
}

bool DX9GF::InputManager::MouseUp(MouseButton button)
{
    return !MousePress(button) && CheckMouseBuffer(button);
}

long DX9GF::InputManager::GetRelativeMouseX() const
{
	return relativeMousePos.x;
}

long DX9GF::InputManager::GetRelativeMouseY() const
{
	return relativeMousePos.y;
}

POINT DX9GF::InputManager::GetRelativeMousePos() const
{
	return relativeMousePos;
}

long DX9GF::InputManager::GetAbsoluteMouseX() const
{
    return mousePos.x;
}

long DX9GF::InputManager::GetAbsoluteMouseY() const
{
    return mousePos.y;
}

POINT DX9GF::InputManager::GetAbsoluteMousePos() const
{
    return mousePos;
}

long DX9GF::InputManager::GetMouseScroll() const
{
    return diMouseState.lZ;
}

void DX9GF::InputManager::Dispose()
{
    if (diKeyboard != nullptr) {
        diKeyboard->Unacquire();
        diKeyboard->Release();
        diKeyboard = nullptr;
    }
    if (diMouse != nullptr) {
        diKeyboard->Unacquire();
        diMouse->Release();
        diMouse = nullptr;
    }
    if (dinput != nullptr) {
        dinput->Release();
        dinput = nullptr;
    }
}
