#include "IAudioManager.h"


bool loadWavFromResource(int resourceID, SoundBuffer& out_audio)
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

	//embedded resource can't use mmio, gotta use this technique (just use for pcm wav - 44 bytes header)

	//take rate and channels,... from file
	memcpy(&out_audio.wfx, (BYTE*)pResourceData + 20, sizeof(WAVEFORMATEX));

	//take raw audio data from file
	out_audio.audioRawData.resize(resourceSize - 44);
	memcpy(out_audio.audioRawData.data(), (BYTE*)pResourceData + 44, resourceSize - 44);

	//set the buffer on
	out_audio.buffer.AudioBytes = out_audio.audioRawData.size();
	out_audio.buffer.pAudioData = out_audio.audioRawData.data();
	out_audio.buffer.Flags = XAUDIO2_END_OF_STREAM;

	return true;
}


bool cAudioManager::Init()
{
	//turn on COM of Windows
	CoInitializeEx(NULL, COINIT_MULTITHREADED);

	if (FAILED(XAudio2Create(&pEngine, 0, XAUDIO2_DEFAULT_PROCESSOR)))
		return false;

	if (FAILED(pEngine->CreateMasteringVoice(&pMasterVoice)))
		return false;

	return true;
}

void cAudioManager::Load(string name, int resID)
{
	if (cache.count(name)) return;
	SoundBuffer* ad = new SoundBuffer();
	if (loadWavFromResource(resID, *ad))
	{
		cache[name] = ad;
	}
	else
	{
		delete ad;
	}
}

void cAudioManager::Play(string name, bool loop, float volume)
{
	//can't find sound name from cache
	if (!cache.count(name)) return;

	SoundBuffer* data = cache[name];

	//create a callback for this turn
	cVoiceCallback* cb = new cVoiceCallback();
	IXAudio2SourceVoice* pVoice = nullptr;

	if (FAILED(pEngine->CreateSourceVoice(&pVoice, &data->wfx, 0, XAUDIO2_DEFAULT_FREQ_RATIO, cb, NULL, NULL)))
	{
		delete cb;
		return;
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

	pVoice->SetVolume(volume);

	//play the sound
	pVoice->SubmitSourceBuffer(&data->buffer);
	pVoice->Start(0);

	//save into list to control it easily
	activeVoices.push_back(new activeVoice{ pVoice, cb });
}

void cAudioManager::Update() {
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

void cAudioManager::Shutdown() {
	//destroy playing sound
	for (auto av : activeVoices)
	{
		av->pVoice->Stop();
		av->pVoice->DestroyVoice();
		delete av->pCallback;
		delete av;
	}
	activeVoices.clear();

	//clear the cache
	for (auto pair : cache) delete pair.second;
	cache.clear();

	//free what it need to be freed
	if (pMasterVoice)
		pMasterVoice->DestroyVoice();
	if (pEngine)
		pEngine->Release();
	CoUninitialize();
}

void cAudioManager::setMasterVolume(float volume)
{
	if (pMasterVoice)
	{
		pMasterVoice->SetVolume(volume);
	}
}