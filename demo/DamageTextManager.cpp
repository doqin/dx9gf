#include "pch.h"
#include "DamageTextManager.h"
#include <sstream>
#include <iomanip>
namespace Demo
{
	const float DEFAULT_LIFETIME = 1500.0f;
	const float FADE_OUT_TIME = 500.0f;

	std::wstring FormatDamageNumber(float value)
	{
		std::wstringstream wss;

		if (value == (int)value)
		{
			wss << (int)value;
		}
		else
		{
			wss << std::fixed << std::setprecision(1) << value;
		}
		return wss.str();
	}

	void DamageTextManager::Init(Game* gameContext)
	{
		this->game = gameContext;
		//set font and size for damage text, should not use fontsize < 11
		font = std::make_unique<DX9GF::Font>(game->GetGraphicsDevice(), L"StatusPlz", 12);
		fontSprite = std::make_unique<DX9GF::FontSprite>(font.get());
	}

	//it is necessary to call this func when release the game
	void DamageTextManager::Release()
	{
		fontSprite.reset();
		font.reset();
		this->game = nullptr;
	}

	void DamageTextManager::Spawn(float damageValue, float x, float y, TextType type)
	{
		for (int i = 0; i < MAX_TEXTS; i++)
		{
			if (!pool[i].isActive)
			{
				pool[i].isActive = true;
				pool[i].displayX = x;
				pool[i].displayY = y;
				pool[i].lifeTime = DEFAULT_LIFETIME;
				pool[i].alpha = 255;
				pool[i].velocityX = ((rand() % 100) / 1000.0f) - 0.05f;
				switch (type)
				{
				case TextType::TakeDamage:
					pool[i].displayText = L"-" + FormatDamageNumber(damageValue);
					pool[i].displayColor = D3DCOLOR_ARGB(255, 255, 0, 0);
					pool[i].velocityY = -0.05f;
					pool[i].textSize = 1.0f;
					break;

				case TextType::Heal:
					pool[i].displayText = L"+" + FormatDamageNumber(damageValue);
					pool[i].displayColor = D3DCOLOR_ARGB(255, 0, 255, 0);
					pool[i].velocityY = -0.02f;
					pool[i].textSize = 1.0f;
					break;

				case TextType::Critical:
					pool[i].displayText = L"-" + FormatDamageNumber(damageValue) + L"!";
					pool[i].displayColor = D3DCOLOR_ARGB(255, 255, 165, 0);
					pool[i].velocityY = -0.08f;
					pool[i].textSize = 1.5f;
					break;

				case TextType::Dodge:
					pool[i].displayText = L"Miss";
					pool[i].displayColor = D3DCOLOR_ARGB(255, 200, 200, 200);
					pool[i].velocityY = -0.04f;
					pool[i].textSize = 0.8f;
					break;
				}

				break;
			}
		}
	}
	void DamageTextManager::Update(unsigned long long deltaTime)
	{
		for (int i = 0; i < MAX_TEXTS; i++)
		{
			if (pool[i].isActive)
			{
				pool[i].displayX += pool[i].velocityX * deltaTime;
				pool[i].displayY += pool[i].velocityY * deltaTime;
				pool[i].lifeTime -= deltaTime;

				if (pool[i].lifeTime <= 0)
				{
					pool[i].isActive = false;
					continue;
				}
				if (pool[i].lifeTime <= FADE_OUT_TIME)
				{
					pool[i].alpha = (pool[i].lifeTime / FADE_OUT_TIME) * 255.0f;
				}
			}
		}
	}
	void DamageTextManager::Draw(DX9GF::Camera& camera, unsigned long long deltaTime)
	{
		if (!fontSprite) return;
		fontSprite->Begin();

		for (int i = 0; i < MAX_TEXTS; i++)
		{
			if (pool[i].isActive)
			{
				D3DCOLOR addColor = pool[i].displayColor & 0x00FFFFFF;
				int alpha = (int)pool[i].alpha << 24;
				D3DCOLOR finalColor = addColor | alpha;

				fontSprite->SetColor(finalColor);
				fontSprite->SetText(std::wstring(pool[i].displayText));
				fontSprite->SetPosition(pool[i].displayX, pool[i].displayY);
				fontSprite->SetScale(pool[i].textSize, pool[i].textSize);
				fontSprite->Draw(camera, deltaTime);
			}
		}
		fontSprite->End();

	}
}