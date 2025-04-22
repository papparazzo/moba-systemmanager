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

#include <gtkmm.h>

#include "moba/endpoint.h"
#include "moba/systemmessages.h"
#include "moba/environmentmessages.h"

class EnvironmentControl: public Gtk::Box {
public:
    EnvironmentControl(EndpointPtr msgEndpoint);
    virtual ~EnvironmentControl() noexcept = default;

    EnvironmentControl(const EnvironmentControl&) = delete;
    EnvironmentControl& operator=(const EnvironmentControl&) = delete;

    void enable();
    void disable();

    void setHardwareState(SystemHardwareStateChanged::HardwareState state);

private:
    void on_button_curtain_up_clicked();
    void on_button_curtain_down_clicked();
    void on_button_main_light_on_clicked();
    void on_button_main_light_off_clicked();

    Gtk::Grid                     m_grid;

    Gtk::Box                      m_HBox_Curtain{Gtk::Orientation::HORIZONTAL, 6};
    Gtk::Box                      m_HBox_MainLight{Gtk::Orientation::HORIZONTAL, 6};

    Gtk::Label                    m_Label_Curtain{"Rolladen:"};
    Gtk::Button                   m_Button_Curtain_Up{"hoch"};
    Gtk::Button                   m_Button_Curtain_Down{"runter"};

    Gtk::Label                    m_Label_MainLight{"Licht:"};
    Gtk::Button                   m_Button_MainLight_On{"an"};
    Gtk::Button                   m_Button_MainLight_Off{"aus"};

    Gtk::Label                    m_Label_AbientLight[4];
    Gtk::Scale                    m_Scale_AbientLight[4];
    Glib::RefPtr<Gtk::Adjustment> m_adjustment;

    EndpointPtr msgEndpoint;
};
