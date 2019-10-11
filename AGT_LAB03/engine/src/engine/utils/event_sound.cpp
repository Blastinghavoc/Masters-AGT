#include "pch.h"
#include "event_sound.h"
#include "audio_manager.h"


//=============================================================================

engine::event_sound::event_sound(const std::string& name)
{
	m_name = name;
}

bool engine::event_sound::load(const std::string& filePath, bool loop /*= false*/)
{
	auto result = engine::audio_manager::system()->createSound(filePath.c_str(), FMOD_DEFAULT | (FMOD_LOOP_OFF + loop), nullptr, &m_sound);
	engine::audio_manager::fmod_error_check(result);
	return result == FMOD_OK;
}

bool engine::event_sound::play()
{
	FMOD::Channel *channel = NULL;
	auto result = engine::audio_manager::system()->playSound(m_sound, NULL, false, &channel);
	engine::audio_manager::fmod_error_check(result);
	return result == FMOD_OK;
}

//=============================================================================
