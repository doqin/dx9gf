#pragma once
#include "DX9GFCamera.h"

namespace DX9GF {

	class IScene {
	protected:
		Camera camera;
		Camera uiCamera;
		/// <summary>
		/// Releases all resources used by the object.
		/// </summary>
		virtual void Dispose();
	public:
		IScene(int screenWidth, int screenHeight) : camera(screenWidth, screenHeight), uiCamera(screenWidth, screenHeight) {}		virtual ~IScene();
		Camera& GetCamera();
		Camera& GetUICamera();

		virtual bool IsOverlay() const { return false; }
		/// <summary>
		/// Initializes the object.
		/// </summary>
		virtual void Init() = 0;
		/// <summary>
		/// Updates the object's state
		/// </summary>
		/// <param name="deltaTime">The time elapsed since the last frame in milliseconds</param>
		virtual void Update(unsigned long long deltaTime) = 0;

		/// <summary>
		/// Draws the frame
		/// </summary>
		/// <param name="deltaTime">The time elapsed since the last frame in milliseconds</param>
		virtual void DrawWorld(unsigned long long deltaTime) = 0;
		virtual void DrawUI(unsigned long long deltaTime) = 0;

		/// <summary>
		/// Called after the graphics device has been Reset (e.g. a fullscreen/windowed toggle
		/// or window resize). D3DPOOL_DEFAULT resources come back empty - a scene holding a
		/// pre-rendered snapshot in one (an offscreen render target it drew once and kept,
		/// rather than a loaded asset) must redraw it here, or it stays blank until the scene
		/// itself is torn down and rebuilt.
		/// </summary>
		virtual void OnDeviceReset() {}
	};
};