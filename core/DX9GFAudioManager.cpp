#include "pch.h"
#include "DX9GFAudioManager.h"
#include <cmath>
#include <algorithm>

// chunk structure of WAV files
#pragma pack(push, 1)
struct ChunkHeader {
	char id[4];
	DWORD size;
};
#pragma pack(pop)

bool DX9GF::LoadWavFromResource(int resourceID, DX9GF::SoundBuffer& out_audio)
{
	//find embedded resource in this project by its ID
	HRSRC hResInfo = FindResource(NULL, MAKEINTRESOURCE(resourceID), L"WAVE");
	if (hResInfo == NULL)
		return false;

	//load resource
	HGLOBAL hResData = LoadResource(NULL, hResInfo);
	if (hResData == NULL)
		return false;

	//lock the resource to point the pointer into resource data
	void* pResourceData = LockResource(hResData);
	DWORD resourceSize = SizeofResource(NULL, hResInfo);
	if (pResourceData == NULL)
		return false;

	//embedded resource can't use mmio, gotta use this technique (just use for pcm wav)

	BYTE* pData = (BYTE*)pResourceData;
	DWORD offset = 12; //skip "RIFF", size, "WAVE"

	bool foundFmt = false;
	bool foundData = false;

	while (offset < resourceSize) {
		ChunkHeader* header = (ChunkHeader*)(pData + offset);
		offset += sizeof(ChunkHeader);

		if (strncmp(header->id, "fmt ", 4) == 0) {
			//take rate and channels,... from file
			memcpy(&out_audio.wfx, pData + offset, (header->size < sizeof(WAVEFORMATEX)) ? header->size : sizeof(WAVEFORMATEX)); //avoid include lib
			foundFmt = true;
		}
		else if (strncmp(header->id, "data", 4) == 0) {
			//take raw audio data from file
			out_audio.audioRawData.resize(header->size);
			memcpy(out_audio.audioRawData.data(), pData + offset, header->size);

			//set the buffer on
			out_audio.buffer.AudioBytes = header->size;
			out_audio.buffer.pAudioData = out_audio.audioRawData.data();
			out_audio.buffer.Flags = XAUDIO2_END_OF_STREAM;

			foundData = true;
			break;
		}

		offset += header->size;
	}

	return foundFmt && foundData;
}

DX9GF::AudioManager* DX9GF::AudioManager::instance = nullptr;

DX9GF::AudioManager* DX9GF::AudioManager::GetInstance()
{
	if (!instance) {
		instance = new AudioManager();
	}
	return instance;
}

void DX9GF::AudioManager::DestroyInstance()
{
	if (instance) {
		instance->Shutdown();
		delete instance;
		instance = nullptr;
	}
}

bool DX9GF::AudioManager::Init()
{
	//turn on COM of Windows
	if (FAILED(CoInitializeEx(NULL, COINIT_MULTITHREADED)))
		return false;

	if (FAILED(XAudio2Create(&pEngine, 0, XAUDIO2_DEFAULT_PROCESSOR)))
		return false;

	if (FAILED(pEngine->CreateMasteringVoice(&pMasterVoice)))
		return false;

	return true;
}

void DX9GF::AudioManager::Load(std::string name, int resID)
{
	if (cache.count(name)) return;
	SoundBuffer* ad = new SoundBuffer();
	if (LoadWavFromResource(resID, *ad))
	{
		cache[name] = ad;
	}
	else
	{
		delete ad;
	}
}

void DX9GF::AudioManager::ApplyVoiceVolume(ActiveVoice* av)
{
	float typeVol = (av->type == AudioType::MUSIC) ? currentMusicVolume : currentSfxVolume;
	av->pVoice->SetVolume(av->baseVolume * av->fadeMul * av->stemLevel * typeVol * currentMasterVolume);
}

void DX9GF::AudioManager::Play(std::string name, bool loop, float volume, AudioType type)
{
	PlayInternal(name, loop, volume, type, name, 1.0f, true);
}

DX9GF::ActiveVoice* DX9GF::AudioManager::PlayInternal(std::string name, bool loop, float volume, AudioType type, std::string group, float fadeMul, bool startNow, const std::string& introName)
{
	//set a limit voice count to protect the engine
	if (activeVoices.size() > 64) {
		return nullptr;
	}

	//can't find sound name from cache
	if (!cache.count(name)) return nullptr;

	SoundBuffer* data = cache[name];

	//create a callback for this turn
	DX9GF::VoiceCallback* cb = new VoiceCallback();
	IXAudio2SourceVoice* pVoice = nullptr;

	if (FAILED(pEngine->CreateSourceVoice(&pVoice, &data->wfx, 0, XAUDIO2_DEFAULT_FREQ_RATIO, cb, NULL, NULL)))
	{
		delete cb;
		return nullptr;
	}

	//set the loop
	if (loop == true)
	{
		//for background music to loop(?)
		data->buffer.LoopCount = XAUDIO2_LOOP_INFINITE;
	}
	else
	{
		//for normal sound
		data->buffer.LoopCount = 0;
	}

	ActiveVoice* av = new ActiveVoice{ name, pVoice, cb, type, volume };
	av->group = group;
	av->fadeMul = fadeMul;
	av->fadeFrom = fadeMul;
	ApplyVoiceVolume(av);
	//optional intro: queued first and played once, the main buffer follows gaplessly
	auto introIt = introName.empty() ? cache.end() : cache.find(introName);
	if (introIt != cache.end())
	{
		XAUDIO2_BUFFER introBuffer = introIt->second->buffer;
		introBuffer.LoopCount = 0;
		introBuffer.Flags = 0; //the stream continues into the main buffer
		introBuffer.pContext = cb; //non-null context marks "not the final buffer" for OnBufferEnd
		pVoice->SubmitSourceBuffer(&introBuffer);
	}
	//play the sound
	pVoice->SubmitSourceBuffer(&data->buffer);
	if (startNow) pVoice->Start(0);

	//save into list to control it easily
	activeVoices.push_back(av);
	return av;
}

void DX9GF::AudioManager::Update(unsigned long long deltaTime) {

	if (isFading) {
		fadeTimer += (deltaTime / 1000.0f);

		float progress = fadeTimer / fadeDuration;
		if (progress > 1.0f) progress = 1.0f;

		for (auto av : activeVoices) {
			if (av->type == AudioType::MUSIC && !av->pCallback->isFinished) {

				if (av->group == fadingOutSound) {
					av->fadeMul = av->fadeFrom * (1.0f - progress);
					ApplyVoiceVolume(av);
				}

				//intro music starts at full level so its first notes aren't faded away
				if (av->group == fadingInSound && !introLoops.count(av->group)) {
					av->fadeMul = progress;
					ApplyVoiceVolume(av);
				}
			}
		}

		if (progress >= 1.0f) {
			isFading = false;
			if (fadingOutSound != "") {
				Stop(fadingOutSound);
			}
		}
	}

	//crossfade the stems of any playing stem set towards their active state
	const float dt = deltaTime / 1000.0f;
	for (auto av : activeVoices) {
		if (av->stemIndex < 0 || av->pCallback->isFinished) continue;
		auto setIt = stemSets.find(av->group);
		if (setIt == stemSets.end()) continue;
		const StemSet& set = setIt->second;
		float target = set.active[av->stemIndex] ? 1.0f : 0.0f;
		float step = dt / (std::max)(set.fadeTime, 0.001f);
		if (av->stemT < target) av->stemT = (std::min)(av->stemT + step, target);
		else if (av->stemT > target) av->stemT = (std::max)(av->stemT - step, target);
		//equal-power curve: a stem fading in and one fading out keep constant loudness
		av->stemLevel = std::sin(av->stemT * 1.5707963f);
		ApplyVoiceVolume(av);
	}

	for (auto it = activeVoices.begin(); it != activeVoices.end(); )
	{
		if ((*it)->pCallback->isFinished) //finished sound should be destroyed
		{
			(*it)->pVoice->DestroyVoice();
			delete (*it)->pCallback;
			delete (*it);
			it = activeVoices.erase(it);
		}
		else
		{
			++it;
		}
	}
}

void DX9GF::AudioManager::Stop(std::string name)
{
	for (auto av : activeVoices)
	{
		if ((av->name == name || av->group == name) && !av->pCallback->isFinished)
		{
			av->pVoice->Stop();
			av->pCallback->isFinished = true;
		}
	}
}

void DX9GF::AudioManager::StopAll()
{
	for (auto av : activeVoices)
	{
		if (!av->pCallback->isFinished)
		{
			av->pVoice->Stop();
			av->pCallback->isFinished = true;
		}
	}
}

void DX9GF::AudioManager::Shutdown() {
	//destroy playing sound
	for (auto av : activeVoices)
	{
		av->pVoice->Stop();
		av->pVoice->DestroyVoice();
		delete av->pCallback;
		delete av;
	}
	activeVoices.clear();
	soundBanks.clear();

	//clear the cache
	for (auto& pair : cache) delete pair.second;
	cache.clear();

	//free what it need to be freed
	if (pMasterVoice)
		pMasterVoice->DestroyVoice();
	if (pEngine)
		pEngine->Release();
	CoUninitialize();
}

void DX9GF::AudioManager::SetMasterVolume(float volume)
{
	currentMasterVolume = volume;
	for (auto av : activeVoices)
	{
		ApplyVoiceVolume(av);
	}
}

void DX9GF::AudioManager::SetMusicVolume(float volume)
{
	currentMusicVolume = volume;
	for (auto av : activeVoices)
	{
		if (av->type == AudioType::MUSIC)
		{
			ApplyVoiceVolume(av);
		}
	}
}

void DX9GF::AudioManager::SetSfxVolume(float volume)
{
	currentSfxVolume = volume;
	for (auto av : activeVoices)
	{
		if (av->type == AudioType::SFX)
		{
			ApplyVoiceVolume(av);
		}
	}
}

void DX9GF::AudioManager::RegisterBank(std::string bankName, std::vector<std::string> soundNames)
{
	soundBanks[bankName] = soundNames;
}

void DX9GF::AudioManager::PlayRandom(std::string bankName, float volume, AudioType type)
{
	if (soundBanks.find(bankName) == soundBanks.end() || soundBanks[bankName].empty())
		return;

	int index = rand() % soundBanks[bankName].size();
	std::string selectedSound = soundBanks[bankName][index];

	Play(selectedSound, false, volume, type);
}

void DX9GF::AudioManager::PlayBGM_Fade(std::string name, float targetVolume, float duration)
{
	if (!activeVoices.empty()) {
		bool isAlreadyPlaying = false;
		for (auto av : activeVoices) {
			if (av->group == name && av->type == AudioType::MUSIC && !av->pCallback->isFinished) {
				isAlreadyPlaying = true;
				break;
			}
		}
		if (isAlreadyPlaying) return;
	}

	isFading = true;
	fadeTimer = 0.0f;
	fadeDuration = duration;
	fadingInSound = name;
	fadingInTargetVolume = targetVolume;

	//the loudest playing music group is the one that fades out
	fadingOutSound = "";
	float loudest = -1.0f;
	for (auto av : activeVoices) {
		if (av->type == AudioType::MUSIC && !av->pCallback->isFinished && av->fadeMul > loudest) {
			loudest = av->fadeMul;
			fadingOutSound = av->group;
		}
	}
	//any other music left over from an interrupted fade is cut
	for (auto av : activeVoices) {
		if (av->type != AudioType::MUSIC || av->pCallback->isFinished) continue;
		if (av->group == fadingOutSound) av->fadeFrom = av->fadeMul;
		else Stop(av->group);
	}

	// Start the incoming track silent right away so it crossfades with the outgoing one
	if (stemSets.count(name)) PlayStemSet(name, targetVolume);
	else if (introLoops.count(name)) {
		const IntroLoop& il = introLoops[name];
		PlayInternal(il.loop, true, targetVolume, AudioType::MUSIC, name, 1.0f, true, il.intro);
	}
	else PlayInternal(name, true, targetVolume, AudioType::MUSIC, name, 0.0f, true);
}

void DX9GF::AudioManager::PlayStemSet(std::string setName, float volume)
{
	const StemSet& set = stemSets[setName];
	std::vector<ActiveVoice*> started;
	for (size_t i = 0; i < set.stems.size(); ++i) {
		ActiveVoice* av = PlayInternal(set.stems[i], true, volume, AudioType::MUSIC, setName, 0.0f, false);
		if (!av) continue;
		av->stemIndex = static_cast<int>(i);
		//start at the current active state so there is no fade-in pop on the stems
		av->stemT = set.active[i] ? 1.0f : 0.0f;
		av->stemLevel = std::sin(av->stemT * 1.5707963f);
		ApplyVoiceVolume(av);
		started.push_back(av);
	}
	//start every stem in one operation set so they are sample-aligned
	const UINT32 opSet = 1;
	for (auto av : started) av->pVoice->Start(0, opSet);
	pEngine->CommitChanges(opSet);
}

void DX9GF::AudioManager::RegisterIntroLoop(std::string name, std::string introName, std::string loopName)
{
	introLoops[name] = IntroLoop{ introName, loopName };
}

void DX9GF::AudioManager::RegisterStemSet(std::string setName, std::vector<std::string> stemNames)
{
	StemSet set;
	set.stems = stemNames;
	set.active.assign(stemNames.size(), true);
	stemSets[setName] = set;
}

void DX9GF::AudioManager::SetActiveStems(std::string setName, std::vector<int> activeIndices, float fadeTime)
{
	auto it = stemSets.find(setName);
	if (it == stemSets.end()) return;
	StemSet& set = it->second;
	set.active.assign(set.stems.size(), false);
	for (int idx : activeIndices) {
		if (idx >= 0 && idx < static_cast<int>(set.active.size())) set.active[idx] = true;
	}
	set.fadeTime = fadeTime;
}

void DX9GF::AudioManager::PlayRandomBGM_Fade(std::string bankName, float targetVolume, float duration)
{
	if (soundBanks.find(bankName) == soundBanks.end() || soundBanks[bankName].empty())
		return;

	int index = rand() % soundBanks[bankName].size();
	std::string selectedSound = soundBanks[bankName][index];

	PlayBGM_Fade(selectedSound, targetVolume, duration);
}