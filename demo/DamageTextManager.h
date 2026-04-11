#pragma once
#include <string>
#include <array>
#include <memory>
#include "DX9GF.h"
#include "Game.h"
namespace Demo
{
	enum class TextType
	{
		TakeDamage,
		Heal, 
		Critical,
		Dodge
	};
	struct DamageText
	{
		float displayX, displayY;
		float velocityX, velocityY;
		std::wstring displayText;
		float lifeTime;
		float alpha;
		float textSize = 1.0f;
		bool isActive = false;
		D3DCOLOR displayColor = D3DCOLOR_ARGB(255, 255, 0, 0); //red
	};
	class DamageTextManager
	{
	private:
		Game* game = nullptr;
		static const int MAX_TEXTS = 100;
		std::array<DamageText, MAX_TEXTS> pool;

		//i want to prevent leaking memory by using unique_ptr, just switch to shared_ptr if u want to
		std::unique_ptr<DX9GF::Font> font;
		std::unique_ptr<DX9GF::FontSprite> fontSprite;

		DamageTextManager() {}
	public:
		static DamageTextManager* GetInstance()
		{
			static DamageTextManager instance;
			return &instance;
		}

		//block copy functions
		DamageTextManager(const DamageTextManager&) = delete;
		DamageTextManager& operator=(const DamageTextManager&) = delete;

		void Init(Game* gameContext);
		void Release();
		//use const and pass by ref to enhance perf 
		void Spawn(int damageValue, float x, float y, TextType type);
		void Update(unsigned long long deltaTime);
		void Draw(DX9GF::Camera& camera, unsigned long long deltaTime);
	};
}

