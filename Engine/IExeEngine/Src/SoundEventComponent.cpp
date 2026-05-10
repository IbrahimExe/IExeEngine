#include "Precompiled.h"
#include "SoundEventComponent.h"
#include "SaveUtil.h"

using namespace IExeEngine;
using namespace IExeEngine::Audio;

void SoundEventComponent::Initialize()
{
	ASSERT(!mFileName.empty(), "SoundEventComponent: No sound file loaded!");
	mSoundId = SoundEffectManager::Get()->Load(mFileName);
}

void SoundEventComponent::Terminate()
{
	Stop();
}

void SoundEventComponent::DebugUI()
{
	ImGui::PushID(mFileName.c_str());
	ImGui::Text(mFileName.c_str());
	if (ImGui::Button("Play"))
	{
		Play();
	}

	ImGui::SameLine(); // Puts next element in the same line
	if (ImGui::Button("Stop"))
	{
		Stop();
	}
	ImGui::PopID();
}

void SoundEventComponent::Deserialize(const rapidjson::Value& value)
{
	SaveUtil::ReadString("FileName", mFileName, value);
	SaveUtil::ReadBool("Looping", mLooping, value);
}

void SoundEventComponent::Play()
{
	SoundEffectManager::Get()->Play(mSoundId, mLooping);
}

void SoundEventComponent::Stop()
{
	SoundEffectManager::Get()->Stop(mSoundId);
}