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

#include "systemcontrol.h"
#include <sstream>
#include <ctime>
#include <iomanip>

#include "moba/clientmessages.h"

SystemControl::SystemControl(EndpointPtr msgEndpoint): Gtk::Box{Gtk::Orientation::VERTICAL, 6}, msgEndpoint{msgEndpoint} {

    append(m_HBox_Expander);

    m_Label_HardwareState.set_margin(50);
    m_HBox_Expander.append(m_Label_HardwareState);

    m_HBox_Expander.append(m_Label_PingResult[0]);
    m_HBox_Expander.append(m_Label_PingResult[1]);
    m_HBox_Expander.append(m_Label_PingResult[2]);
    m_HBox_Expander.append(m_Label_PingResult[3]);
    m_HBox_Expander.append(m_ButtonBox_System);

	m_ButtonBox_System.set_margin(25);

    m_Label_HardwareState.set_markup("<b>Hardwarestatus:</b> [unbekannt]");
    m_Label_PingResult[0].set_markup("<b>Ping 1:</b> [unbekannt]");
    m_Label_PingResult[1].set_markup("<b>Ping 2:</b> [unbekannt]");
    m_Label_PingResult[2].set_markup("<b>Ping 3:</b> [unbekannt]");
    m_Label_PingResult[3].set_markup("<b>Ping 4:</b> [unbekannt]");

    m_ButtonBox_System.append(m_Button_SystemShutdown);
    m_ButtonBox_System.append(m_Button_SystemReset);
    m_ButtonBox_System.append(m_Button_SystemStandby);
    m_ButtonBox_System.append(m_Button_SystemPing);

    m_ButtonBox_System.set_halign(Gtk::Align::CENTER);
    m_ButtonBox_System.set_sensitive(false);
    m_ButtonBox_System.set_margin(60);

    m_Button_SystemShutdown.signal_clicked().connect(sigc::mem_fun(*this, &SystemControl::on_button_shutdown_clicked));
    m_Button_SystemReset.signal_clicked().connect(sigc::mem_fun(*this, &SystemControl::on_button_reset_clicked));
    m_click_connection = m_Button_SystemStandby.signal_clicked().connect(sigc::mem_fun(*this, &SystemControl::on_button_standby_clicked));
    m_Button_SystemPing.signal_clicked().connect(sigc::mem_fun(*this, &SystemControl::on_button_ping_clicked));
}

void SystemControl::on_button_reset_clicked() {
    msgEndpoint->sendMsg(SystemHardwareReset{});
}

void SystemControl::on_button_shutdown_clicked() {
    msgEndpoint->sendMsg(SystemHardwareShutdown{});
}

void SystemControl::on_button_standby_clicked() {
    if(m_Button_SystemStandby.get_active()) {
        msgEndpoint->sendMsg(SystemSetStandbyMode{true});
    } else {
        msgEndpoint->sendMsg(SystemSetStandbyMode{false});
    }
}

void SystemControl::on_button_ping_clicked() {
    msgEndpoint->sendMsg(ClientEchoReq{"test"});
    start = std::chrono::system_clock::now();
    pingctr = 0;
    m_Button_SystemPing.set_sensitive(false);
}

void SystemControl::setHardwareStateLabel(const std::string &status) {
    std::stringstream ss;
    ss << "<b>Hardwarestatus:</b> " << status;
    m_Label_HardwareState.set_markup(ss.str());
}

void SystemControl::setPingResult() {
    auto t = std::time(nullptr);
    auto tm = *std::localtime(&t);

    std::ostringstream oss;

    std::chrono::time_point<std::chrono::system_clock> end = std::chrono::system_clock::now();
    int elapsed_millis = std::chrono::duration_cast<std::chrono::milliseconds> (end - start).count();

    oss << "<b>Ping " << (pingctr + 1) << ":</b> " << std::put_time(&tm, "%d.%m.%Y %H:%M:%S") << " -> " << " elapsed time: " << elapsed_millis << " ms";
    m_Label_PingResult[pingctr].set_markup(oss.str());

    if(pingctr < 3) {
        msgEndpoint->sendMsg(ClientEchoReq{"test"});
        start = std::chrono::system_clock::now();
        pingctr++;
        return;
    }
    m_Button_SystemPing.set_sensitive(true);
}

void SystemControl::enable() {
    m_ButtonBox_System.set_sensitive(true);
}

void SystemControl::disable() {
    m_ButtonBox_System.set_sensitive(false);
}

void SystemControl::setHardwareState(SystemState systemState) {
    switch(systemState) {
        case SystemState::ERROR:
            m_Button_SystemStandby.set_sensitive(false);
            setHardwareStateLabel("Hardwarefehler");
            break;

        case SystemState::STANDBY:
            m_click_connection.block();
            m_Button_SystemStandby.set_sensitive(true);
            m_Button_SystemStandby.set_active(true);
            m_click_connection.unblock();
            setHardwareStateLabel("standby");
            break;

        case SystemState::EMERGENCY_STOP:
            m_Button_SystemStandby.set_sensitive(false);
            setHardwareStateLabel("Nothalt");
            break;

        case SystemState::MANUEL:
            m_click_connection.block();
            m_Button_SystemStandby.set_sensitive(true);
            m_Button_SystemStandby.set_active(false);
            m_click_connection.unblock();
            setHardwareStateLabel("manuell");
            break;

        case SystemState::AUTOMATIC:
            m_click_connection.block();
            m_Button_SystemStandby.set_sensitive(true);
            m_Button_SystemStandby.set_active(false);
            m_click_connection.unblock();
            setHardwareStateLabel("automatisch");
            break;

        case SystemState::NO_CONNECT:
            m_Button_SystemStandby.set_sensitive(false);
            setHardwareStateLabel("Keine Verbindung");
            break;
    }
}
