#pragma once
#include "DX9GF.h"
#include "DX9GFExtras.h"
#include "ExampleGame.h"
#include "Rectangle.h"

class SubScene : public DX9GF::IScene {
private:
	ExampleGame* game;
	DX9GF::InputManager* inputManager;
	std::shared_ptr<GO::Rectangle> square;
	bool isDraggingCircle = false;
	float circleX, circleY;
	bool IsWithinBoundCircle(float srcX, float srcY, float centerX, float centerY, float radius);
public:
	SubScene(ExampleGame* game) : game(game) {}
	void Init() override;
	void Update(unsigned long long deltaTime) override;
	void Draw(unsigned long long deltaTime) override;
	void Dispose() override;
};