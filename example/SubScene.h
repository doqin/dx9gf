#pragma once
#include "DX9GF.h"
#include "DX9GFExtras.h"
#include "ExampleGame.h"
#include "Rectangle.h"
#include "Ellipse.h"

class SubScene : public DX9GF::IScene {
private:
	ExampleGame* game = nullptr;
	DX9GF::InputManager* inputManager = nullptr;
	std::shared_ptr<DX9GF::TransformManager> transformManager;
	std::vector<std::weak_ptr<DX9GF::ICollider>> worldColliders;
	std::vector<std::shared_ptr<GO::Rectangle>> rects;
	std::vector<std::shared_ptr<GO::Ellipse>> ellipses;
public:
	SubScene(ExampleGame* game, int width, int height) : game(game), IScene(width, height) {}
	void Init() override;
	void Update(unsigned long long deltaTime) override;
	void Draw(unsigned long long deltaTime) override;
	void Dispose() override;
};