#pragma once
#include "DX9GF.h"
#include "DX9GFExtras.h"
#include "ExampleGame.h"
#include "Player.h"
#include "Rectangle.h"

class MainScene : public DX9GF::IScene 
{
private:
	ExampleGame* game;
	DX9GF::InputManager* inputManager = nullptr;
	DX9GF::CommandBuffer commandBuffer;
	std::shared_ptr<DX9GF::Font> fontArial;
	std::shared_ptr<DX9GF::FontSprite> fontSpriteArial;
	std::vector<std::shared_ptr<GO::Player>> players;
	std::shared_ptr<GO::Rectangle> rect;
	std::shared_ptr<DX9GF::TransformManager> transformManager;
	std::shared_ptr<DX9GF::ColliderManager> colliderManager;
	std::shared_ptr<DX9GF::Texture> whiteTexture;
	std::shared_ptr<DX9GF::Texture> dawgTexture;
	std::shared_ptr<DX9GF::StaticSprite> colorRec;
	std::shared_ptr<DX9GF::StaticSprite> textureRec;
	std::shared_ptr<DX9GF::Map> map;
	DX9GF::AudioManager* audioManager;
public:
	MainScene(ExampleGame* game, int width, int height) : game(game), IScene(width, height) {}
	void Init() override;
	void Draw(unsigned long long deltaTime) override;
	void Update(unsigned long long deltaTime) override;
	void Dispose() override;
};