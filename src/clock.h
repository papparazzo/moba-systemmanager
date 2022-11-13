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

    void setTime(unsigned int hours, unsigned int minutes, bool draw) {
        m_hours = hours;
        m_minutes = minutes;
        if(draw) {
            invalidateRect();
        }
    }

    void setMultiplier(unsigned int multiplier) {
        m_multiplier = multiplier;
    }

    void run() {
        m_run = true;
    }

    void stop() {
        m_run = false;
        m_seconds = 0;
    }

protected:

    void invalidateRect();

    bool on_draw(const Cairo::RefPtr<Cairo::Context>& cr) override;
    bool on_timeout();

    std::atomic<unsigned int> m_seconds{0};
    std::atomic<unsigned int> m_minutes{0};
    std::atomic<unsigned int> m_hours{0};
    std::atomic<unsigned int> m_multiplier{180}; // factor 60 bedeutet: 1 Sek Echtzeit = 60 Sek Modellbahnzeit

    std::atomic<bool> m_run{false};

    std::atomic<unsigned int> m_ticks{0};

    const double m_factor = 1;
    const double m_radius = 0.42 * m_factor;
};