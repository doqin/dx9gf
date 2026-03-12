#pragma once
#include <windows.h>
#include <xaudio2.h>
#include <mmsystem.h>
#include <vector>
#include <string>
#include <map>
using namespace std;

#pragma comment(lib, "xaudio2.lib")
#pragma comment(lib, "winmm.lib")

struct SoundBuffer
{
	//using XAudio2 instead of directsound
	WAVEFORMATEX wfx; //sample rate, channels,...
	vector <BYTE> audioRawData; //raw data
	XAUDIO2_BUFFER buffer; //audio buffering to avoid disk I/O latency

	SoundBuffer()
	{
		ZeroMemory(&buffer, sizeof(XAUDIO2_BUFFER));
	}
};

class cVoiceCallback : public IXAudio2VoiceCallback
{
public:
	bool isFinished = false; //finish voice flag

	//play last sound byte will trigger XAudio2 to use this function
	void STDMETHODCALLTYPE OnBufferEnd(void* pBufferContext) override
	{
		isFinished = true;
	}

	//Interface must-have function
	void STDMETHODCALLTYPE OnVoiceProcessingPassStart(UINT32) override {}
	void STDMETHODCALLTYPE OnVoiceProcessingPassEnd() override {}
	void STDMETHODCALLTYPE OnStreamEnd() override {}
	void STDMETHODCALLTYPE OnBufferStart(void*) override {}
	void STDMETHODCALLTYPE OnLoopEnd(void*) override {}
	void STDMETHODCALLTYPE OnVoiceError(void*, HRESULT) override {}
};

struct activeVoice
{
	IXAudio2SourceVoice* pVoice;
	cVoiceCallback* pCallback;
};

bool loadWavFromResource(int resourceID, SoundBuffer& out_audio);

class cAudioManager {
private:
	IXAudio2* pEngine = nullptr;              //control XAudio2
	IXAudio2MasteringVoice* pMasterVoice = nullptr; //output
	map<string, SoundBuffer*> cache; //cache will save the loaded file (avoid disk loading latency)
	vector<activeVoice*> activeVoices; //list of playing sound

public:

	bool Init();

	//load sound from file to cache
	void Load(string name, int resID);

	void Play(string name, bool loop = false, float volume = 1.0f);
	void Update();
	void Shutdown();

	//set game volume
	void setMasterVolume(float volume);
};
