#include "DX9GFInputManager.h"
#include <stdexcept>
#include <DxErr.h>

DX9GF::InputManager* DX9GF::InputManager::instance = nullptr;

bool DX9GF::InputManager::CheckBuffer(int DIKey) const
{
    return buffer[DIKey] & 0x80;
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

void DX9GF::InputManager::KeySnapShot(unsigned long long deltaTime)
{
    delta += deltaTime;
    if (delta > KEYBOARD_LAST_PRESS_TIME) {
        ReleaseLastPressed();
    }
    for (int i = 0; i < 256; i++) {
        buffer[i] = keys[i];
    }
    HRESULT result = diKeyboard->GetDeviceState(sizeof(keys), (LPVOID)&keys);
    while (result != DI_OK && (result == DIERR_INPUTLOST || result == DIERR_NOTACQUIRED)) {
        result = diKeyboard->Acquire();
    }
    firstCheck = true;
}

bool DX9GF::InputManager::KeyPress(int DIKey)
{
    if (keys[DIKey] & 0x80) {
        if (firstCheck && buffer[DIKey] != keys[DIKey]) {
            lastBufferPressed = lastKeyPressed;
            lastKeyPressed = DIKey;
            delta = 0;
            firstCheck = false;
        }
        return true;
    }
    return false;
}

bool DX9GF::InputManager::KeyDown(int DIKey)
{
    return KeyPress(DIKey) && !CheckBuffer(DIKey);
}

bool DX9GF::InputManager::KeyUp(int DIKey)
{
    return !KeyPress(DIKey) && CheckBuffer(DIKey);
}

void DX9GF::InputManager::ReleaseLastPressed()
{
    delta = 0;
    lastKeyPressed = -1;
    lastBufferPressed = -1;
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
