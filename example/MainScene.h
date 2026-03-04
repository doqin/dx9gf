#pragma once
#include "DX9GF.h"
#include "ExampleGame.h"

class MainScene : public DX9GF::IScene 
{
private:
	ExampleGame* game;
	DX9GF::StaticSprite* colorRec;
	DX9GF::StaticSprite* textureRec;
	DX9GF::AnimatedSprite* mario;
	DX9GF::InputManager* inputManager;
public:
	MainScene(ExampleGame* game, int width, int height) : game(game), IScene(width, height) {}
	void Init() override;
	void Draw(unsigned long long deltaTime) override;
	void Update(unsigned long long deltaTime) override;
	void Dispose() override;
};