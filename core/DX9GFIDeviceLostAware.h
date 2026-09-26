#pragma once

namespace DX9GF {
	// Implemented by anything holding a D3DPOOL_DEFAULT-backed D3DX helper object
	// (ID3DXSprite, ID3DXFont). Both require OnLostDevice() before IDirect3DDevice9::Reset
	// and OnResetDevice() after, or Reset can fail outright - see GraphicsDevice's
	// RegisterVolatileResource, which is what actually calls these around a Reset.
	class IDeviceLostAware {
	public:
		virtual ~IDeviceLostAware() = default;
		virtual void OnLostDevice() = 0;
		virtual void OnResetDevice() = 0;
	};
}
