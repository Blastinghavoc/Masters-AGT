#pragma once
#include "../FMOD/include/fmod.hpp"
#include "engine/core.h"

namespace engine
{
	class sound;

	enum class sound_type
	{
		event,
		track
	};

	class audio_manager
	{
		//-------------------------------------------------------------------------
	private:
		FMOD_RESULT                                 result{};
		/// \brief the global variable for talking to FMOD
		static FMOD::System *                       m_fmod_system;
		/// \brief A map of all the events
		hash_map<std::string, sound *>               m_sounds{};

		/// \brief Number of total channels available
		static uint32_t                             max_channels;

		/// \brief Number of total channels already in use;
		static uint32_t                                  used_channels;

		//-------------------------------------------------------------------------

		audio_manager() = default;

		//-------------------------------------------------------------------------

	public:
		~audio_manager();

		//-------------------------------------------------------------------------

		bool init();
		void on_update() const;
		bool clean_all();
		void stop_all();

		//-------------------------------------------------------------------------

		/// \brief 
		/// \param filePath Path to the sound file.
		/// \param type Specifies the properties of the file.
		/// \param name Unique identifier of the sound.
		/// \return True if the sound has been loaded correctly.
		bool load_sound(const std::string &filePath, const sound_type &type, const std::string &name);

		/// \brief plays the specified sound
		void play(const std::string &sound);

		/// \brief Retrieves a pointer to the specified sound
		sound * sound(const std::string &sound) const;

		//-------------------------------------------------------------------------
	private:
		bool load_event(const std::string &filePath, const std::string &name);
		bool load_track(const std::string &filePath, const std::string &name);

		//-------------------------------------------------------------------------
	public:
		static void fmod_error_check(FMOD_RESULT result);
		static FMOD::System * system();
		static ref<audio_manager> instance();
		static uint32_t available_channels();

	}; // Audio
}
