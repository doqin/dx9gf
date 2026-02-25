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
public:
	MainScene(ExampleGame* game) : game(game) {}
	void Init() override;
	void Draw(unsigned long long deltaTime) override;
	void Update(unsigned long long deltaTime) override;
	void Dispose() override;
};