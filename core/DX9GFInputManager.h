#pragma once
#include "dinput.h"
#define KEYBOARD_BUFFER_SIZE 1024
#define KEYBOARD_LAST_PRESS_TIME 125

namespace DX9GF {
	class InputManager {
	private:
		static InputManager* instance;

		LPDIRECTINPUT8 dinput = nullptr;
		LPDIRECTINPUTDEVICE8 diKeyboard = nullptr;
		LPDIRECTINPUTDEVICE8 diMouse = nullptr;
		DIMOUSESTATE diMouseState;

		InputManager() {}
		~InputManager() {}

		char keys[256];
		char buffer[256];
		int lastKeyPressed = -1;
		int lastBufferPressed = -1;
		bool firstCheck = true;
		unsigned long long delta;

		bool CheckBuffer(int DIKey) const;
	public:
		static InputManager* GetInstance();
		void Init(HWND, HINSTANCE);
		void KeySnapShot(unsigned long long deltaTime);
		bool KeyPress(int DIKey);
		bool KeyDown(int DIKey);
		bool KeyUp(int DIKey);
		void ReleaseLastPressed();
		
		void Dispose();
	};
}