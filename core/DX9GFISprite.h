#pragma once
#include "C:\Program Files (x86)\Microsoft DirectX SDK (June 2010)\Include\d3dx9.h"
#include "DX9GFCamera.h"

namespace DX9GF {
	class GraphicsDevice;

	class ISprite {
	protected:
		GraphicsDevice* graphicsDevice = nullptr;
		ID3DXSprite* p_sprite = nullptr;
		D3DXVECTOR3 pos = { 0.0f, 0.0f, 0.0f };
		D3DCOLOR color = 0xFFFFFFFF;
	public:
		ISprite(GraphicsDevice* graphicsDevice) : graphicsDevice(graphicsDevice) {}

		virtual void Draw(const Camera& camera) = 0;

		/// <summary>
		/// Translates the current position by the specified x and y offsets.
		/// </summary>
		/// <param name="x">The horizontal offset to translate by.</param>
		/// <param name="y">The vertical offset to translate by.</param>
		void Translate(float x, float y);

		/// <summary>
		/// Sets the position of the sprite
		/// </summary>
		/// <param name="x">The position in the horizontal coordinate</param>
		/// <param name="y">The position in the vertical coordinate</param>
		void SetPosition(float x, float y);
		void SetPosition(float x, float y, float z);
		void SetPositionX(float x);
		void SetPositionY(float y);
		void SetPositionZ(float z);

		float GetPositionX() const;
		float GetPositionY() const;
		float GetPositionZ() const;
		D3DXVECTOR3 GetPosition() const;
	};
}