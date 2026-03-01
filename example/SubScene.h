#pragma once
#include "DX9GF.h"
#include "ExampleGame.h"

class SubScene : public DX9GF::IScene {
private:
	ExampleGame* game;
	DX9GF::InputManager* inputManager;

	bool isDraggingSquare = false;
	bool isDraggingCircle = false;
	float circleX, circleY;
	float squareX, squareY;
	bool IsWithinBoundRectangle(float srcX, float srcY, float dstX, float dstY, float dstW, float dstH);
	bool IsWithinBoundCircle(float srcX, float srcY, float centerX, float centerY, float radius);
public:
	SubScene(ExampleGame* game) : game(game) {}
	void Init() override;
	void Update(unsigned long long deltaTime) override;
	void Draw(unsigned long long deltaTime) override;
	void Dispose() override;
};