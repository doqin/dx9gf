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
			//a buffer with a context is an intro that is followed by more audio, not the end of the sound
			if (pBufferContext) return;
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

	enum class AudioType 
	{
		SFX,
		MUSIC
	};

	struct ActiveVoice
	{
		std::string name; //identify
		IXAudio2SourceVoice* pVoice;
		VoiceCallback* pCallback;
		AudioType type;
		float baseVolume; //save the volume level when play
		std::string group; //music fades act on a whole group (a stem set's voices share its name)
		float fadeMul = 1.0f; //crossfade envelope for the whole group
		float fadeFrom = 1.0f; //fadeMul when a fade-out started, so an interrupted fade doesn't jump
		int stemIndex = -1; //>= 0 when this voice is one stem of a stem set
		float stemT = 1.0f; //0..1 progress of this stem towards active
		float stemLevel = 1.0f; //equal-power curve of stemT
	};

	//dynamic music: every stem loops in sync, inactive ones sit at silence
	struct StemSet
	{
		std::vector<std::string> stems;
		std::vector<bool> active;
		float fadeTime = 2.0f;
	};

	//music that plays an intro once, then loops the main track (same wave format required)
	struct IntroLoop
	{
		std::string intro;
		std::string loop;
	};

	bool LoadWavFromResource(int resourceID, SoundBuffer& out_audio);

	class AudioManager {
	private:
		IXAudio2* pEngine = nullptr;              //control XAudio2
		IXAudio2MasteringVoice* pMasterVoice = nullptr; //output
		std::map<std::string, SoundBuffer*> cache; //cache will save the loaded file (avoid disk loading latency)
		std::map<std::string, std::vector<std::string>> soundBanks;
		std::vector<ActiveVoice*> activeVoices; //list of playing sound
		std::map<std::string, StemSet> stemSets;
		std::map<std::string, IntroLoop> introLoops;

		ActiveVoice* PlayInternal(std::string name, bool loop, float volume, AudioType type, std::string group, float fadeMul, bool startNow, const std::string& introName = "");
		void PlayStemSet(std::string setName, float volume);
		void ApplyVoiceVolume(ActiveVoice* av);

		//settings manager will push values to these vars
		float currentMasterVolume = 1.0f;
		float currentMusicVolume = 1.0f;
		float currentSfxVolume = 1.0f;

		bool isFading = false;
		float fadeTimer = 0.0f;
		float fadeDuration = 0.0f;

		std::string fadingOutSound = "";

		std::string fadingInSound = "";
		float fadingInTargetVolume = 1.0f;
		bool fadingInLoop = true;

		AudioManager() {}
		~AudioManager() {}
		static AudioManager* instance;
	public:
		static AudioManager* GetInstance();
		static void DestroyInstance();
		bool Init();

		//load sound from file to cache
		void Load(std::string name, int resID);

		void Play(std::string name, bool loop = false, float volume = 1.0f, AudioType type = AudioType::SFX);
		void Update(unsigned long long deltaTime);
		void Stop(std::string name);
		void StopAll();
		void Shutdown();

		//set game volume
		void SetMasterVolume(float volume);
		void SetMusicVolume(float volume);
		void SetSfxVolume(float volume);
		
		//manage various types of footstep
		void RegisterBank(std::string bankName, std::vector<std::string> soundNames);
		void PlayRandom(std::string bankName, float volume = 1.0f, AudioType type = AudioType::SFX);

		//background music
		void PlayBGM_Fade(std::string name, float targetVolume = 0.5f, float duration = 1.5f);
		void PlayRandomBGM_Fade(std::string bankName, float targetVolume = 0.5f, float duration = 1.5f);

		//intro-then-loop music: register already-loaded tracks, then play with PlayBGM_Fade(name).
		//the intro is heard at full level (no fade-in) and flows seamlessly into the looping track
		void RegisterIntroLoop(std::string name, std::string introName, std::string loopName);

		//dynamic music: register already-loaded, equal-length stems, then play the set with PlayBGM_Fade(setName)
		void RegisterStemSet(std::string setName, std::vector<std::string> stemNames);
		//which stems are audible (indices into the set); can be called before or while the set plays
		void SetActiveStems(std::string setName, std::vector<int> activeIndices, float fadeTime = 2.0f);
	};
}