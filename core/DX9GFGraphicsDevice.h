#pragma once
#include <d3d9.h>

namespace DX9GF {
	class GraphicsDevice {
	private:
		IDirect3DDevice9* d3ddev = NULL; // Đại diện cho card đồ họa máy tính
		IDirect3DSurface9* backbuffer = NULL;
		D3DVIEWPORT9 viewport;
	public:
		GraphicsDevice() {};
		GraphicsDevice(IDirect3DDevice9* d3ddev, IDirect3DSurface9* backbuffer);
		~GraphicsDevice();

		/// <summary>
		/// Gets a reference to the Direct3D device pointer.
		/// </summary>
		/// <returns>A reference to the Direct3D device pointer.</returns>
		IDirect3DDevice9*& GetDevice();

		/// <summary>
		/// Retrieves a reference to the back buffer surface.
		/// </summary>
		/// <returns>A reference to a pointer to the Direct3D surface representing the back buffer.</returns>
		IDirect3DSurface9*& GetBackBuffer();

		/// <summary>
		/// Sets the viewport parameters for rendering.
		/// </summary>
		/// <param name="x">The x-coordinate of the upper-left corner of the viewport.</param>
		/// <param name="y">The y-coordinate of the upper-left corner of the viewport.</param>
		/// <param name="width">The width of the viewport in pixels.</param>
		/// <param name="height">The height of the viewport in pixels.</param>
		/// <param name="minZ">The minimum depth value of the viewport range (typically 0.0).</param>
		/// <param name="maxZ">The maximum depth value of the viewport range (typically 1.0).</param>
		/// <returns>S_OK on success, or an error code on failure.</returns>
		HRESULT SetViewport(DWORD x, DWORD y, DWORD width, DWORD height, float minZ, float maxZ);

		/// <summary>
		/// Sets the viewport for rendering.
		/// </summary>
		/// <param name="viewport">The viewport parameters to set.</param>
		/// <returns>Returns S_OK if successful, or an error code otherwise.</returns>
		HRESULT SetViewport(D3DVIEWPORT9 viewport);

		/// <summary>
		/// Initiates drawing operations.
		/// </summary>
		/// <returns>Returns S_OK if successful, or an error code otherwise.</returns>
		HRESULT BeginDraw();

		/// <summary>
		/// Ends a drawing operation and returns the result of any drawing operations performed since the last call to BeginDraw.
		/// </summary>
		/// <returns>An HRESULT indicating success or failure of the drawing operations. Returns S_OK if successful, or an error code otherwise.</returns>
		HRESULT EndDraw();

		/// <summary>
		/// Updates the object's state or data.
		/// </summary>
		/// <returns>An HRESULT status code indicating success or failure of the update operation.</returns>
		HRESULT Present();

		HRESULT IsValid();
	};
};