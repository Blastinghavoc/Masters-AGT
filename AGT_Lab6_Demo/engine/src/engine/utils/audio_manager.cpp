#include "pch.h"
#include "audio_manager.h"

#include "track.h"
#include "event_sound.h"

#include <fmod_errors.h>

//#pragma comment(lib, "lib/fmodex_vc.lib")

//=============================================================================

FMOD::System * engine::audio_manager::m_fmod_system{ nullptr };

uint32_t engine::audio_manager::max_channels{ 32 };
uint32_t engine::audio_manager::used_channels{ 0 };

//-----------------------------------------------------------------------------

engine::audio_manager::~audio_manager()
{
	clean_all();
	SAFE_RELEASE(m_fmod_system);
}

//-----------------------------------------------------------------------------

bool engine::audio_manager::init()
{
	// Create an FMOD system
	result = FMOD::System_Create(&m_fmod_system);
	fmod_error_check(result);
	if (result != FMOD_OK)
		return false;

	// Initialise the system
	result = m_fmod_system->init(max_channels, FMOD_INIT_NORMAL, nullptr);
	fmod_error_check(result);

	return result == FMOD_OK;
}

void engine::audio_manager::on_update() const
{
	m_fmod_system->update();
}

bool engine::audio_manager::clean_all()
{
	for (auto sound : m_sounds)
	{
		SAFE_RELEASE(sound.second);
	}
	m_sounds.clear();

	result = m_fmod_system->close();
	SAFE_RELEASE(m_fmod_system);

	fmod_error_check(result);
	return result == FMOD_OK;
}

void engine::audio_manager::stop_all()
{
	for (uint32_t i = 0; i < max_channels; i++)
	{
		FMOD::Channel* pChannel = nullptr;
		result = m_fmod_system->getChannel(i, &pChannel);
		fmod_error_check(result);

		if (result == FMOD_OK && pChannel)
		{
			pChannel->stop();
		}
	}
}

//-----------------------------------------------------------------------------

bool engine::audio_manager::load_sound(const std::string& file_path, const engine::sound_type& type, const std::string& name)
{
	LOG_CORE_INFO("[sound] Creating sound '{0}'.", name);

	bool result = false;
	if (type == engine::sound_type::event)
		result = load_event(file_path, name);
	else if (type == engine::sound_type::track)
		result = load_track(file_path, name);

	return result;
}

// Play an event sound
void engine::audio_manager::play(const std::string &sound)
{
	if (m_sounds.find(sound) != m_sounds.end())
	{
		m_sounds[sound]->play();
		return;
	}

	LOG_CORE_ERROR("[sound] Could not play '{0}'.", sound);
}

engine::sound * engine::audio_manager::sound(const std::string& sound) const
{
	if (m_sounds.find(sound) != m_sounds.end())
	{
		return m_sounds.at(sound);
	}
	return nullptr;
}

//-----------------------------------------------------------------------------

bool engine::audio_manager::load_event(const std::string& file_path, const std::string& name)
{
	auto event = new engine::event_sound(name);
	if (event->load(file_path))
	{
		m_sounds[name] = event;
		return true;
	}

	LOG_CORE_ERROR("[sound] Could not load event '{0}'.", name);

	return false;
}

bool engine::audio_manager::load_track(const std::string& file_path, const std::string& name)
{
	// block track creation when no more channels are available
	if (used_channels >= max_channels)
	{
		LOG_CORE_ERROR("Cannot create a new track. No more channels available.");

		return false;
	}

	auto track = new engine::track(name);
	if (track->load(file_path))
	{
		m_sounds[name] = track;
		++used_channels;
		
		LOG_CORE_INFO("[sound] Channels count: '{0}'.", used_channels);

		return true;
	}

	LOG_CORE_ERROR("[sound] Could not load track '{0}'.", name);

	return false;
}

//-----------------------------------------------------------------------------

// Check for error
void engine::audio_manager::fmod_error_check(FMOD_RESULT result)
{
	if (result != FMOD_OK)
	{
		const std::string  &error_string = FMOD_ErrorString(result);
		LOG_CORE_ERROR("[sound] '{0}'", error_string);
		// Warning: error message commented out -- if headphones not plugged into computer in lab, error occurs
	}
}

FMOD::System* engine::audio_manager::system()
{
	return m_fmod_system;
}

engine::ref<engine::audio_manager> engine::audio_manager::instance()
{
	static engine::audio_manager instance;
	return std::make_shared<engine::audio_manager>(instance);
}

uint32_t engine::audio_manager::available_channels()
{
	return max_channels - used_channels;
}

//=============================================================================
