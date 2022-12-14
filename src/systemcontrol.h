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
#include <chrono>
#include <string>

#include "moba/endpoint.h"
#include "moba/systemmessages.h"

class SystemControl: public Gtk::Box {
public:
    SystemControl(EndpointPtr msgEndpoint);
    SystemControl(const SystemControl&) = delete;
    SystemControl& operator=(const SystemControl&) = delete;

    virtual ~SystemControl();

    void enable();
    void disable();

    void setPingResult();

    void setHardwareState(SystemHardwareStateChanged::HardwareState state);

private:
    void setHardwareStateLabel(const std::string &status);

    void on_button_shutdown_clicked();
    void on_button_reset_clicked();
    void on_button_automatic_clicked();
    void on_button_standby_clicked();
    void on_button_ping_clicked();

    Gtk::ButtonBox m_ButtonBox_System;
    Gtk::Button    m_Button_SystemStandby{"Standby"};
    Gtk::Button    m_Button_SystemShutdown{"Shutdown"};
    Gtk::Button    m_Button_SystemReset{"Reset"};
    Gtk::Button    m_Button_SystemPing{"Ping"};
    Gtk::Label     m_Label_HardwareState;
    Gtk::Label     m_Label_PingResult[4];

    std::chrono::time_point<std::chrono::system_clock> start;
    int pingctr;

    EndpointPtr msgEndpoint;
};

