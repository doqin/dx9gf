#pragma once
#include "DX9GF.h"
#include "ExampleGame.h"

class SubScene : public DX9GF::IScene {
private:
	ExampleGame* game;
	DX9GF::InputManager* inputManager;
	
	DX9GF::StaticSprite* whiteSquare;

	bool isDragging = false;
	bool IsWithinBound(float srcX, float srcY, float dstX, float dstY, float dstW, float dstH);
public:
	SubScene(ExampleGame* game) : game(game) {}
	void Init() override;
	void Update(unsigned long long deltaTime) override;
	void Draw(unsigned long long deltaTime) override;
	void Dispose() override;
};