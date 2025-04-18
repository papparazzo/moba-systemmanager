/*
 *  Project:    moba-systemmanager
 *
 *  Copyright (C) 2022 Stefan Paproth <pappi-@gmx.de>
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Affero General Public License as
 *  published by the Free Software Foundation, either version 3 of the
 *  License, or (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU Affero General Public License for more details.
 *
 *  You should have received a copy of the GNU Affero General Public License
 *  along with this program. If not, see <http://www.gnu.org/licenses/agpl.txt>.
 *
 */

#include <cmath>
#include <cairomm/context.h>
#include <glibmm/main.h>
#include <giomm/settings.h>

#include "clock.h"

Clock::Clock() {
    Glib::signal_timeout().connect(sigc::mem_fun(*this, &Clock::on_timeout), 250);
    set_draw_func(sigc::mem_fun(*this, &Clock::on_draw));
}

void Clock::setTime(unsigned int hours, unsigned int minutes, bool draw) {
    m_hours = hours;
    m_minutes = minutes;
    if(draw) {
         queue_draw();
    }
}

void Clock::setMultiplier(unsigned int multiplier) {
    m_multiplier = multiplier;
}

void Clock::setNightLight(Time on_at, Time off_at) {
    m_nightlight_on_at = on_at;
    m_nightlight_off_at = off_at;
}

void Clock::run() {
    m_run = true;
}

void Clock::stop() {
    m_run = false;
    m_seconds = 0;
    queue_draw();
    setNightLight(false);
}

void Clock::on_draw(const Cairo::RefPtr<Cairo::Context>& cr, int width, int height) {
    const double radius = 0.42 * m_factor;

    // scale to unit square and translate (0, 0) to be (0.5, 0.5), i.e.
    // the center of the window
    cr->scale(height, height);
    cr->translate(0.5 * width / height, 0.5);
    cr->set_line_width(0.013 * m_factor);

    if(!m_run) {
        cr->set_source_rgba(0.5, 0.5, 0.5, 0.8);
    }
    cr->arc(0, 0, 0.441 * m_factor, 0, 2 * M_PI);
    cr->save();
    cr->set_source_rgba(1.0, 1.0, 1.0, 0.8);
    cr->fill_preserve();
    cr->restore();
    cr->stroke_preserve();
    cr->clip();

    if(!m_run) {
        cr->set_source_rgba(0.5, 0.5, 0.5, 0.8);
    }

    //clock ticks
    for (int i = 0; i < 60; i++) {
        double inset = 0.04 * m_factor;

        cr->save();

        if(i % 5 == 0) {
            inset *= 3;
            cr->set_line_width(0.03 * m_factor);
        }

        cr->move_to(
            (radius - inset) * cos(i * M_PI / 30),
            (radius - inset) * sin(i * M_PI / 30)
        );

        cr->line_to (
            radius * cos(i * M_PI / 30),
            radius * sin(i * M_PI / 30)
        );

        cr->stroke();
        cr->restore();
    }

    // compute the angles of the indicators of our clock
    double seconds= m_seconds * M_PI / 30;
    double minutes = m_minutes * M_PI / 30;
    double hours = m_hours * M_PI / 6;

    cr->save();

    // draw the minutes hand
    cr->set_line_width(0.03 * m_factor);
    cr->move_to(
        -sin(minutes) * (radius * 0.14),
        cos(minutes) * (radius * 0.14)
    );
    cr->line_to(
        sin(minutes) * (radius * 0.8),
        -cos(minutes) * (radius * 0.8)
    );
    cr->stroke();

    // draw the hours hand
    cr->set_line_width(0.039 * m_factor);
    cr->move_to(
        -sin(hours + minutes / 12.0) * (radius * 0.12),
        cos(hours + minutes / 12.0) * (radius * 0.12)
    );
    cr->line_to(
        sin(hours + minutes / 12.0) * (m_radius * 0.5),
        -cos(hours + minutes / 12.0) * (m_radius * 0.5)
    );
    cr->stroke();
    cr->restore();

    // draw the seconds hand
    if(m_run) {
        cr->set_source_rgba(1.0, 0.0, 0.0, 0.8); // red
    } else {
        cr->set_source_rgba(0.5, 0.5, 0.5, 0.8);
    }

    cr->move_to(
        -sin(seconds) * (radius * 0.14),
        cos(seconds) * (radius * 0.14)
    );
    cr->line_to(
        sin(seconds) * (radius * 0.9),
        -cos(seconds) * (radius * 0.9)
    );
    cr->stroke();
}

bool Clock::on_timeout() {
    if(!m_run) {
        return true;
    }

    m_seconds = (m_seconds + m_multiplier) % 60;

    m_ticks = (++m_ticks % 8);

    switch(m_ticks) {
        case 0:
        case 4:
            m_minutes++;
            break;

        case 1:
        case 3:
        case 5:
            if(m_multiplier > 2) {
                m_minutes++;
            } else {
                queue_draw();
                return true;
            }
            break;

        case 2:
            if(m_multiplier == 2 || m_multiplier == 4) {
                m_minutes++;
            } else {
                queue_draw();
                return true;
            }
            break;

        case 6:
            if(m_multiplier != 1 && m_multiplier != 3) {
                m_minutes++;
            } else {
                queue_draw();
                return true;
            }
            break;

        case 7:
            if(m_multiplier == 3 || m_multiplier == 4) {
                m_minutes++;
            } else {
                queue_draw();
                return true;
            }
            break;
    }

    /*
    1 Sek => 1 Min 0,      4
    1 Sek => 2 Min 0,  2,  4,  6
    1 Sek => 3 Min 0,1,  3,4,5,  7
    1 Sek => 4 Min 0,1,2,3,4,5,6,7
     */

    if(m_minutes % 60 == 0) {
        m_minutes = 0;
        if(++m_hours % 24 == 0) {
            m_hours = 0;
        }
    }

    Time t{m_hours, m_minutes};

    if((t >= m_nightlight_on_at || t <= m_nightlight_off_at) && !nightlightActive) {
        setNightLight(true);
    }

    if((t < m_nightlight_on_at && t > m_nightlight_off_at) && nightlightActive) {
        setNightLight(false);
    }

    queue_draw();
    return true;
}

void Clock::setNightLight(bool activate) {
    Glib::RefPtr<Gio::Settings> s1 = Gio::Settings::create("org.gnome.settings-daemon.plugins.color");
    Glib::RefPtr<Gio::Settings> s2 = Gio::Settings::create("org.gnome.desktop.interface");

    if(activate) {
        s2->set_string("gtk-theme", "Yaru-dark");
        s1->set_uint("night-light-temperature", 1700);
        s1->set_boolean("night-light-enabled", true);
    } else {
        s2->set_string("gtk-theme", "Yaru");
        s1->set_boolean("night-light-enabled", false);
    }

    nightlightActive = activate;

    //FIXME ACHTUNG: Wenn Applikation beendet wird, dann setNightLight(false) aufrufen!!!
}
