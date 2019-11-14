#include "pch.h"
#include "cross_fade.h"

void cross_fade::activate(float duration)
{
	m_remaining = duration;
	m_duration = duration;
}

void cross_fade::on_update(const engine::timestep& ts)
{
	if (m_remaining > 0)
	{
		float transparency = 0.8f*( m_remaining / m_duration);
		m_panel->set_transparency(transparency);
		m_panel->show();

		m_remaining -= ts.seconds();
		if (m_remaining <= 0)
		{
			m_panel->hide();
		}
	}
}


