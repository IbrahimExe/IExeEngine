#include "Precompiled.h"

#include "AudioSystem.h"

using namespace DirectX;
using namespace IExeEngine;
using namespace IExeEngine::Audio;

namespace 
{
    std::unique_ptr<AudioSystem> sAudioSystem;
}

void AudioSystem::StaticInitialize()
{    
    ASSERT(sAudioSystem == nullptr, "AudioSystem: System already initialized!");
    sAudioSystem = std::make_unique<AudioSystem>();
    sAudioSystem->Initialize();
}    
     
void AudioSystem::StaticTerminate()
{
    if (sAudioSystem != nullptr)
    {
        sAudioSystem->Terminate();
        sAudioSystem.reset();
    }
}

AudioSystem* AudioSystem::Get()
{
    ASSERT(sAudioSystem != nullptr, "AudioSystem: Is not Initialized!");
    return sAudioSystem.get();
}

AudioSystem::AudioSystem()
{

}

AudioSystem::~AudioSystem()
{
    ASSERT(mAudioEngine == nullptr, "AudioSystem: Must call terminate!");
}

void AudioSystem::Initialize()
{
    AUDIO_ENGINE_FLAGS flags = AudioEngine_Default;

#if defined(_DEBUG)
    flags |= AudioEngine_Debug;
#endif
    mAudioEngine = new AudioEngine(flags);
}

void AudioSystem::Terminate()
{
    if (mAudioEngine != nullptr)
    {
        delete mAudioEngine;
    }
    mAudioEngine = nullptr;
}

void AudioSystem::Update()
{
    if (mAudioEngine != nullptr && mAudioEngine->Update())
    {
        if (mAudioEngine->IsCriticalError())
        {
            LOG("AudioSystem: Critical error encountered!!! No more audio for you lol...");
            Terminate();
        }
    }
}

void AudioSystem::Suspend()
{
    ASSERT(mAudioEngine != nullptr, "AudioSystem: Not initialized!");
    mAudioEngine->Suspend();
}