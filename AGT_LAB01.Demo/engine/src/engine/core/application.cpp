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


}

engine::application::~application()
{
}

void engine::application::run()
{
    while (s_running)
    {

        for (auto* layer : m_layers_stack)
        {
			layer->on_update();
            layer->on_render();
        }

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
