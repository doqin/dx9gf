#pragma once
#include "DX9GF.h"
#include "ExampleGame.h"
#include "Player.h"
#include "Rectangle.h"

class MainScene : public DX9GF::IScene 
{
private:
	ExampleGame* game;
	std::shared_ptr<GO::Player> player;
	std::shared_ptr<GO::Rectangle> rect;
	std::shared_ptr<DX9GF::TransformManager> transformManager;
	std::vector<std::weak_ptr<DX9GF::ICollider>> worldColliders;
	DX9GF::StaticSprite* colorRec;
	DX9GF::StaticSprite* textureRec;
	DX9GF::InputManager* inputManager;
public:
	MainScene(ExampleGame* game, int width, int height) : game(game), IScene(width, height) {}
	void Init() override;
	void Draw(unsigned long long deltaTime) override;
	void Update(unsigned long long deltaTime) override;
	void Dispose() override;
};