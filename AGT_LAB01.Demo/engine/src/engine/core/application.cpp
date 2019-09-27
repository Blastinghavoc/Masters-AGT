#include "pch.h"
#include "application.h"
#include "engine/renderer/renderer.h"
#include "GLFW/glfw3.h"
#include <gl\gl.h>

//----------------------------------------------------------------------------- 

engine::application* engine::application::s_instance{ nullptr }; 
bool engine::application::s_running{ true }; 

//----------------------------------------------------------------------------- 

engine::application::application()
{
    CORE_ASSERT(!s_instance, "Application already exists!");
    s_instance = this;

    m_window = window::create();
    m_window->event_callback(BIND_EVENT_FN(application::on_event));

    renderer::init();

	/*char* pVersion = (char*)glGetString(GL_VERSION);
	char* pVendor = (char*)glGetString(GL_VENDOR);
	char* pRenderer = (char*)glGetString(GL_RENDERER);*/
}

engine::application::~application()
{
}

void engine::application::run()
{
	m_last_frame_time = static_cast<float>(glfwGetTime());
    while (s_running)
    {
		const auto time = static_cast<float>(glfwGetTime());
		timestep time_step = time - m_last_frame_time;
		m_last_frame_time = time;
        for (auto* layer : m_layers_stack)
        {
			layer->on_update(time_step);
            layer->on_render();
        }

        m_window->on_update();		
    }

}

void engine::application::run_fixed()
{
	const timestep TICK_TIME = 1.f / 30.f;
	m_last_frame_time = static_cast<float>(glfwGetTime());
	LOG_INFO("Tick time = {}" , TICK_TIME);
	while (s_running)
	{
		const auto time = static_cast<float>(glfwGetTime());
		timestep time_elapsed = time - m_last_frame_time;

		//if (time_elapsed < TICK_TIME)
		//{
		//	LOG_INFO("busy waiting,time:{} elapsed:{} last frame was: {}",time,time_elapsed,m_last_frame_time);
		//}

		/*if (time_elapsed >= TICK_TIME *2)
		{
			LOG_INFO("We are behind by {} ticks!", time_elapsed / TICK_TIME);
		}*/

		//While we are behind on updates, do them.
		//Ideally we should only be behind by 1 tick.
		while (time_elapsed >= TICK_TIME)
		{
			time_elapsed = time_elapsed - TICK_TIME;
			//Update all the layers by 1 tick.
			for (auto* layer : m_layers_stack)
			{
				layer->on_update(TICK_TIME);
			}
			//We have now done one frame/tick
			m_last_frame_time = m_last_frame_time + TICK_TIME;
			//LOG_INFO("Last frame time = {}" , m_last_frame_time);			
		}
		//Once we're caught up on updates, perform a render operation.
		for (auto* layer : m_layers_stack)
		{			
			layer->on_render();
		}

		const auto end_time = static_cast<float>(glfwGetTime());
		const timestep remaining_time = TICK_TIME - (end_time - time);
		//LOG_INFO("Sleeping for {}", std::max(0.0f, remaining_time.seconds()));
		//Sleep remaining time until next tick.
		Sleep(std::max(0.0f, remaining_time.milliseconds()));

		m_window->on_update();
	}
}

void engine::application::on_event(event& event) 
{ 
    event_dispatcher dispatcher(event); 
    // dispatch event on window X pressed 
    dispatcher.dispatch<window_closed_event>(BIND_EVENT_FN(application::on_window_close)); 

    //LOG_CORE_TRACE("{0}", event); 

    // events are executed from top of the stack to bottom (aka end to start of the list) 
    for (auto it = m_layers_stack.end(); it != m_layers_stack.begin(); ) 
    { 
        (*--it)->on_event(event); 
        // stop event propagation to next layer if flagged as handled 
        if (event.handled) 
            break; 
    } 
} 

void engine::application::push_layer(layer* layer) 
{ 
    m_layers_stack.push_layer(layer); 
} 

void engine::application::push_overlay(layer* overlay) 
{ 
    m_layers_stack.push_overlay(overlay); 
} 

bool engine::application::on_window_close(window_closed_event&) 
{ 
    exit(); 
    return true; 
} 

void engine::application::exit() 
{ 
    s_running = false; 
} 
