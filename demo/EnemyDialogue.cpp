#include "pch.h"
#include "EnemyDialogue.h"
#include <unordered_map>

namespace Demo::EnemyDialogue {

	const Lines* Get(const std::string& enemyType)
	{
		static const std::unordered_map<std::string, Lines> table = {
			// ---- Bosses ---------------------------------------------------------------
			{ "SpamEnemy", {
				{
					L"You've got mail! Lots of it.",
					L"Congratulations! You've been selected to lose!",
					L"Don't bother unsubscribing. It never works.",
					L"One click. That's all I need.",
				},
				L"Error... my mailing list... is shrinking!",
				L"Your inbox is full! Want to upgrade your storage?",
				{
					{ L"Spam multiplies! (+12 HP)", L"Forward this to ten friends... or ten copies of me!" },
					{ L"Your best shots get buried in junk!", L"Your best shots just got sorted into junk!" },
				},
			} },
			{ "TrojanEnemy", {
				{
					L"Relax. I'm just a harmless little update.",
					L"Trust me. Have I ever lied to you? ...Recently?",
					L"Every system lets me in eventually.",
					L"You opened the door. I just walked through.",
				},
				L"Wait, wait! I can be a legitimate program, I swear!",
				L"Your defences are full of holes. I'd know.",
				{
					{ L"Trojan corrupts your systems!", L"Let me just... rewrite a few of your files." },
					{ L"Trojan fortifies its defenses!", L"Firewall? Oh, I brought my own." },
				},
			} },
			{ "CupidEnemy", {
				{
					L"Waka waka! Dinner is served!",
					L"Ooh, a fresh process. You look delicious.",
					L"I've eaten bigger bytes than you.",
					L"No power pellets here, sorry!",
				},
				L"Hey! That's not how the game works!",
				L"Running low? Just one more bite...",
				{
					{ L"Pacman heals 15 HP!", L"Nom nom nom... ahh, much better!" },
					{ L"Pacman breaks your guard!", L"Chomp! There goes your guard!" },
				},
			} },
			{ "KeyeproEnemy", {
				{
					L"So you made it to the core. Pointless.",
					L"Every key in this system answers to me.",
					L"You can't close what I keep open.",
					L"Access denied. Permanently.",
				},
				L"Impossible... my keys are slipping!",
				L"Your process is failing. Let it end.",
				{
					{ L"The boss spawned a minion!", L"Keye, deal with this intruder." },
					{ L"Boss locks your mind!", L"Locked. You no longer have permission." },
					{ L"Boss regenerates 45HP!", L"Restoring from backup." },
					{ L"Boss fortifies itself with 30DEF!", L"Raising every firewall I own." },
					{ L"Boss lowers your defenses!", L"Your guard? Revoked." },
					{ L"Boss weakens your attacks!", L"Downgrading your privileges." },
					{ L"Boss enters a frenzy!", L"Enough! Full system override!" },
				},
			} },

			// ---- Regular enemies ------------------------------------------------------
			{ "DemonEyeEnemy", { // Bug
				{
					L"It's not a bug, it's a feature!",
					L"Works on my machine.",
					L"*bzzt* Found you in production.",
					L"Nobody tested this path, huh?",
				},
				L"Wait... are you debugging me?!",
				L"Your HP is leaking like bad memory!",
				{
					{ L"Bug powers up its attack!", L"Undefined behaviour, maximum damage!" },
					{ L"Bug injects a vulnerability!", L"Let me just inject a tiny exploit..." },
				},
			} },
			{ "VampireBatEnemy", { // Shrimp
				{
					L"Shrimp-le as that, your data is mine!",
					L"I'm small, but my packet loss is huge.",
					L"*click click* Fresh bandwidth!",
					L"Don't let the size fool you. I bite.",
				},
				L"Ow! I'm getting cooked here!",
				L"You're leaking data like a sieve!",
				{
					{ L"Shrimp siphons your data! (+15 HP)", L"Slurp! Your data tastes great." },
					{ L"Shrimp emits a piercing screech!", L"SKREEEEEE!" },
				},
			} },
			{ "MimicEnemy", { // Not Responding
				{
					L"(Not Responding)",
					L"Please wait while we waste your time...",
					L"Click OK to continue. There is no OK.",
					L"You can't close me. Nobody can.",
				},
				L"Program has encountered a problem and needs to... no, wait!",
				L"Your system is lagging. Have you tried restarting?",
				{
					{ L"A giant pop-up blocks the way!", L"SURPRISE! You've won a free pop-up!" },
					{ L"Mimic installs malicious adware!", L"Installing 37 toolbars. Please don't turn off your PC." },
				},
			} },
			{ "WarlockEnemy", { // Crawler
				{
					L"Crawling... indexing... found you.",
					L"Every page, every byte. I see it all.",
					L"Your connection is mine now.",
					L"Loading... loading... still loading...",
				},
				L"Connection unstable... retrying...",
				L"Your HP is buffering. Hope you're patient.",
				{
					{ L"Crawler slows your connection!", L"Enjoy your 56k modem experience." },
					{ L"Crawler gathers corrupted data!", L"So much juicy corrupted data..." },
				},
			} },
			{ "KernelEnemy", {
				{
					L"Kernel online. Threat detected.",
					L"Unauthorised process. Terminating.",
					L"I am the core of this system. You are a bug.",
					L"Running integrity check... you failed.",
				},
				L"Kernel panic! Kernel panic!",
				L"Your process is unstable. Shutting it down.",
				{
					{ L"Kernel runs recovery protocol! (+20 HP)", L"Executing recovery protocol." },
					{ L"Kernel elevates privileges!", L"sudo: permission granted." },
				},
			} },
			{ "KeyeEnemy", { // Keyepro's minion
				{
					L"Keye sees you.",
					L"The master sends his regards.",
					L"Blink and you'll miss me.",
					L"All access points are watched.",
				},
				L"Master... Keye needs backup!",
				L"Keye sees weakness.",
				{
					{ L"Keye focuses its gaze!", L"Locking on..." },
					{ L"Keye finds your blind spot!", L"There. Your blind spot." },
				},
			} },
		};

		auto it = table.find(enemyType);
		return it != table.end() ? &it->second : nullptr;
	}
}