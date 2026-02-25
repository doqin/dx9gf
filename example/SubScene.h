#pragma once
#include "DX9GF.h"
#include "ExampleGame.h"

class SubScene : public DX9GF::IScene {
private:
	ExampleGame* game;
public:
	SubScene(ExampleGame* game) : game(game) {}
	void Init() override;
	void Update(unsigned long long deltaTime) override;
	void Draw(unsigned long long deltaTime) override;
};