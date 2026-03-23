#include "pch.h"
#include "TestEnemy.h"
#include "resource.h"
#include "RoundProjectile.h"

void Demo::TestEnemy::Init(DX9GF::GraphicsDevice* graphicsDevice, DX9GF::Camera* camera)
{
    texture = std::make_shared<DX9GF::Texture>(graphicsDevice);
    texture->LoadTexture(IDB_PNG2);
    sprite = std::make_shared<DX9GF::StaticSprite>(texture.get());
    sprite->SetOrigin(32, 32);
    roundProjectileTexture = std::make_shared<DX9GF::Texture>(graphicsDevice);
    roundProjectileTexture->LoadTexture(IDB_PNG3);
    roundProjectileSprite = std::make_shared<DX9GF::StaticSprite>(roundProjectileTexture.get());
    roundProjectileSprite->SetOrigin(8, 8);
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

void Demo::TestEnemy::StartAttack(std::shared_ptr<Player> player) {
    this->player = player;
    for (size_t i = 0; i < 10; i++) {
        commandBuffer.PushCommand(std::make_shared<DX9GF::CustomCommand>([this, i](std::function<void(void)> markFinished) {
            if (auto lock = this->player.lock()) {
                projectiles.push_back(
                    RoundProjectile::Builder(
                        transformManager,
                        lock,
                        roundProjectileSprite,
                        16, 16,
                        i * 20, 0
                    )
                    .SetTargetPosition(lock->GetWorldX(), lock->GetWorldY())
                    .SetDelay(1.f)
                    .SetDecayTime(4.f)
                    .SetVelocity(100.f)
                    .Build()
                );
                projectiles.back()->Init();
                transformManager.lock()->RebuildHierarchy();
            }
            else {
                throw std::runtime_error("player is null");
            }
            markFinished();
        }));
        commandBuffer.PushCommand(std::make_shared<DX9GF::DelayCommand>(1.f));
    }
    commandBuffer.PushCommand(std::make_shared<DX9GF::DelayCommand>(1.f));
}
