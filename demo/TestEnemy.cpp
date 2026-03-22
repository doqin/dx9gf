#include "pch.h"
#include "TestEnemy.h"
#include "resource.h"

void Demo::TestEnemy::Init(DX9GF::GraphicsDevice* graphicsDevice, DX9GF::Camera* camera)
{
    texture = std::make_shared<DX9GF::Texture>(graphicsDevice);
    texture->LoadTexture(IDB_PNG2);
    sprite = std::make_shared<DX9GF::StaticSprite>(texture.get());
    sprite->SetOrigin(32, 32);
    InitCardSpawnTrigger(camera, 64.f, 64.f);
}

void Demo::TestEnemy::Draw(DX9GF::GraphicsDevice* graphicsDevice, DX9GF::Camera* camera, unsigned long long deltaTime)
{
    if (sprite) {
        sprite->Begin();
        auto [x, y] = GetWorldPosition();
        sprite->SetPosition(x, y);
        sprite->Draw(*camera, deltaTime);
        sprite->End();
    }
    IEnemy::Draw(graphicsDevice, camera, deltaTime);
}

std::vector<std::shared_ptr<DX9GF::ICommand>> Demo::TestEnemy::GetAttackCommand(std::shared_ptr<Player> player, std::function<void(void)> finishAttack)
{
  return {
    std::make_shared<DX9GF::CustomCommand>([&](std::function<void(void)> markFinished) {
        finishAttack();
        markFinished();
    })
  };
}
