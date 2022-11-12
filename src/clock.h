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

#pragma once

#include <gtkmm/drawingarea.h>
#include <atomic>

class Clock : public Gtk::DrawingArea
{
public:
    Clock();
    virtual ~Clock();

    void setHours(unsigned int hours) {
        m_hours = hours;
    }

    void setMinutes(unsigned int minutes) {
        m_minutes = minutes;
    }

    void setMultiplier(unsigned int multiplier) {
        m_multiplier = multiplier;
    }

    void stop() {
        //m_run = false;
    }

protected:
    //Override default signal handler:
    bool on_draw(const Cairo::RefPtr<Cairo::Context>& cr) override;

    bool on_timeout();

    int m_seconds;
    std::atomic<unsigned int> m_minutes;
    std::atomic<unsigned int> m_hours;
    std::atomic<unsigned int> m_multiplier{180}; // factor 60 bedeutet: 1 Sek Echtzeit = 60 Sek Modellbahnzeit

    std::atomic<bool> m_run;

    unsigned int m_ticks{0};

    const double m_factor = 1;
    const double m_radius = 0.42 * m_factor;
};