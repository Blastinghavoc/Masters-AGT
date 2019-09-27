#pragma once

#include "engine/core.h"

#include "window.h"
#include "engine/core/layers_stack.h"
#include "engine/events/event.h"
#include "engine/events/application_event.h"
#include "engine/core/timestep.h"


namespace engine
{
    class ENGINE_API application
    {
    public:
        application();
        virtual ~application();

        /// \brief
        void run();

		/// \brief
		void run_fixed();

        /// \brief
        virtual void on_event(event &event);

        /// \brief Adds a layer to the stack.
        void push_layer(layer *layer);
        /// \brief Adds a overlay to the stack.
        void push_overlay(layer *overlay);

        /// \brief Returns a reference to the application window.
        static window& window() { return *(s_instance->m_window); } 

        /// \brief Returns a reference to the application.
        static application& instance() { return *s_instance; }

    private:
        bool on_window_close(window_closed_event &event);

    private:
        scope<engine::window> m_window;
        layers_stack          m_layers_stack;
		float m_last_frame_time;

    private:                  
        static application*   s_instance;
        static bool           s_running; 

    public: 
        static void exit(); 

    };


    application* create_application();
}
