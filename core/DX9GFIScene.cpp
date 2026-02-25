#include "DX9GFIScene.h"

void DX9GF::IScene::Dispose()
{
}

DX9GF::IScene::~IScene() {
	Dispose();
}