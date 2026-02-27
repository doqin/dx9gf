#pragma once
#include "dinput.h"
#define KEYBOARD_BUFFER_SIZE 1024
#define KEYBOARD_LAST_PRESS_TIME 125
#define MOUSE_LAST_PRESS_TIME 125

namespace DX9GF {
	class InputManager {
	public:
		enum MouseButton {
			Left = 0,
			Right = 1,
			Middle = 2
		};
	private:
		static InputManager* instance;

		LPDIRECTINPUT8 dinput = nullptr;
		LPDIRECTINPUTDEVICE8 diKeyboard = nullptr;
		LPDIRECTINPUTDEVICE8 diMouse = nullptr;
		DIMOUSESTATE diMouseState;
		BYTE mouseBuffer[4];
		POINT mousePos = {0, 0};
		POINT lastMousePos = {0, 0};
		POINT relativeMousePos = {0, 0};
		bool hasMousePos = false;

		InputManager() {}
		~InputManager() {}

		char keys[256];
		char keysBuffer[256];
		int lastKeyPressed = -1;
		int lastBufferPressed = -1;
		int lastMouseButtonPressed = -1;
		int lastMouseBufferPressed = -1;
		bool firstKeyCheck = true;
		bool firstMouseCheck = true;
		unsigned long long keysDelta = 0;
		unsigned long long mouseDelta = 0;

		bool CheckKeysBuffer(int DIKey) const;
		bool CheckMouseBuffer(MouseButton button);
		void UpdateMouseBuffer(int buttonIndex);
	public:
		static InputManager* GetInstance();
		void Init(HWND, HINSTANCE);
		void ReadKeyboard(unsigned long long deltaTime);
		bool KeyPress(int DIKey);
		bool KeyDown(int DIKey);
		bool KeyUp(int DIKey);
		void ReleaseLastPressed();
		void ReadMouse(unsigned long long deltaTime);
		bool MousePress(MouseButton button);
		bool MouseDown(MouseButton button);
		bool MouseUp(MouseButton button);
		long GetRelativeMouseX() const;
		long GetRelativeMouseY() const;
		POINT GetRelativeMousePos() const;
		long GetAbsoluteMouseX() const;
		long GetAbsoluteMouseY() const;
		POINT GetAbsoluteMousePos() const;
		void Dispose();
	};
}