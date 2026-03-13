#pragma once
#include <windows.h>
#include <xaudio2.h>
#include <mmsystem.h>
#include <vector>
#include <string>
#include <map>

#pragma comment(lib, "xaudio2.lib")
#pragma comment(lib, "winmm.lib")

namespace DX9GF {
	struct SoundBuffer
	{
		//using XAudio2 instead of directsound
		WAVEFORMATEX wfx{}; //sample rate, channels,...
		std::vector <BYTE> audioRawData; //raw data
		XAUDIO2_BUFFER buffer; //audio buffering to avoid disk I/O latency

		SoundBuffer()
		{
			ZeroMemory(&buffer, sizeof(XAUDIO2_BUFFER));
		}
	};

	class VoiceCallback : public IXAudio2VoiceCallback
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

	struct ActiveVoice
	{
		IXAudio2SourceVoice* pVoice;
		VoiceCallback* pCallback;
	};

	bool LoadWavFromResource(int resourceID, SoundBuffer& out_audio);

	class AudioManager {
	private:
		IXAudio2* pEngine = nullptr;              //control XAudio2
		IXAudio2MasteringVoice* pMasterVoice = nullptr; //output
		std::map<std::string, SoundBuffer*> cache; //cache will save the loaded file (avoid disk loading latency)
		std::vector<ActiveVoice*> activeVoices; //list of playing sound
		AudioManager() {}
		~AudioManager() { if (instance) delete instance; }
		static AudioManager* instance;
	public:
		static AudioManager* GetInstance();
		bool Init();

		//load sound from file to cache
		void Load(std::string name, int resID);

		void Play(std::string name, bool loop = false, float volume = 1.0f);
		void Update();
		void Shutdown();

		//set game volume
		void SetMasterVolume(float volume);
	};
}