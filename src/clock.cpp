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
#include "clock.h"

Clock::Clock() /*: m_factor{60}*/ {
    Glib::signal_timeout().connect(sigc::mem_fun(*this, &Clock::on_timeout), 250);
}

Clock::~Clock() {
}

bool Clock::on_draw(const Cairo::RefPtr<Cairo::Context>& cr) {
    Gtk::Allocation allocation = get_allocation();
    const int width = allocation.get_width();
    const int height = allocation.get_height();

    const double radius = 0.42 * m_factor;

    // scale to unit square and translate (0, 0) to be (0.5, 0.5), i.e.
    // the center of the window
    cr->scale(height, height);
    cr->translate(0.5 * width / height, 0.5);
    cr->set_line_width(0.013 * m_factor);

    cr->arc(0, 0, 0.441 * m_factor, 0, 2 * M_PI);
    cr->save();
    cr->set_source_rgba(1.0, 1.0, 1.0, 0.8);
    cr->fill_preserve();
    cr->restore();
    cr->stroke_preserve();
    cr->clip();

    //clock ticks
    for (int i = 0; i < 60; i++) {
        double inset = 0.04 * m_factor;

        cr->save();

        if(i % 5 == 0) {
            inset *= 3;
            cr->set_line_width(0.03 * m_factor);
        }

        cr->move_to(
            (radius - inset) * cos (i * M_PI / 30),
            (radius - inset) * sin (i * M_PI / 30)
        );

        cr->line_to (
            radius * cos (i * M_PI / 30),
            radius * sin (i * M_PI / 30)
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
    cr->set_source_rgba(1.0, 0.0, 0.0, 0.8); // red
    cr->move_to(
        -sin(seconds) * (radius * 0.14),
        cos(seconds) * (radius * 0.14)
    );
    cr->line_to(
        sin(seconds) * (radius * 0.9),
        -cos(seconds) * (radius * 0.9)
    );
    cr->stroke();

    return true;
}

bool Clock::on_timeout() {

    if(!m_run) {
        return false;
    }

    if(++m_seconds % 60 == 0) {
        m_seconds = 0;
    }

    if(++m_ticks % 4 == 0) {
        m_ticks = 0;

        m_minutes += m_multiplier / 60  ;

        if(m_minutes % 60 == 0) {
            m_minutes = 0;
            if(++m_hours % 12) {
                m_hours = 0;
            }
        }
    }

    auto win = get_window();
    if(win) {
        Gdk::Rectangle r(
            0,
            0,
            get_allocation().get_width(),
            get_allocation().get_height()
        );
        win->invalidate_rect(r, false);
    }
    return true;
}

