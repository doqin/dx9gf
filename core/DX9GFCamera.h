#pragma once
#include "C:\Program Files (x86)\Microsoft DirectX SDK (June 2010)\Include\d3dx9.h"
#include <tuple>
namespace DX9GF {
	class Camera {
	private:
		D3DXVECTOR2 position;
		float rotation; // in radians
		float zoom; // scale
		int screenWidth;
		int screenHeight;
		mutable D3DXMATRIX transformMatrix;
		mutable bool needUpdate;
	protected:
	public:
		Camera(int screenWidth, int screenHeight);
		~Camera() {}

		void Update() const;
		void SetScreenWidth(int width);
		void SetScreenHeight(int height);
		void SetScreenResolution(int width, int height);
		std::tuple<int, int> GetScreenResolution() const;
		void SetPosition(const D3DXVECTOR2& position);
		void SetPosition(float x, float y);
		D3DXVECTOR2 GetPosition() const;
		void SetRotation(float radians);
		float GetRotation() const;
		void SetZoom(float zoom);
		float GetZoom() const;
		D3DXMATRIX GetTransformMatrix() const;
	};
}