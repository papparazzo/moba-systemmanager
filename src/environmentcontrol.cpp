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

#include "environmentcontrol.h"

EnvironmentControl::EnvironmentControl(EndpointPtr msgEndpoint): m_adjustment(Gtk::Adjustment::create(0.0, 0.0, 4095.0)), msgEndpoint(msgEndpoint) {
    set_expand(true);
    append(m_grid);
    m_grid.set_margin(50);
    m_grid.set_halign(Gtk::Align::CENTER);

    m_grid.attach(m_Label_Curtain, 0, 0);
    m_grid.attach(m_HBox_Curtain, 1, 0);

    m_HBox_Curtain.append(m_Button_Curtain_Up);
    m_HBox_Curtain.append(m_Button_Curtain_Down);

    m_Button_Curtain_Up.set_margin(4);
    m_Button_Curtain_Down.set_margin(4);

    m_Label_Curtain.set_halign(Gtk::Align::START);

    m_grid.attach(m_Label_MainLight, 0, 1);
    m_grid.attach(m_HBox_MainLight, 1, 1);

    m_HBox_MainLight.append(m_Button_MainLight_On);
    m_HBox_MainLight.append(m_Button_MainLight_Off);

    m_Button_MainLight_On.set_margin(4);
    m_Button_MainLight_Off.set_margin(4);

    m_Label_MainLight.set_halign(Gtk::Align::START);

    m_Button_Curtain_Up.signal_clicked().connect(sigc::mem_fun(*this, &EnvironmentControl::on_button_curtain_up_clicked));
    m_Button_Curtain_Down.signal_clicked().connect(sigc::mem_fun(*this, &EnvironmentControl::on_button_curtain_down_clicked));
    m_Button_MainLight_On.signal_clicked().connect(sigc::mem_fun(*this, &EnvironmentControl::on_button_main_light_on_clicked));
    m_Button_MainLight_Off.signal_clicked().connect(sigc::mem_fun(*this, &EnvironmentControl::on_button_main_light_off_clicked));

    //m_HBox_Digits.append(*Gtk::make_managed<Gtk::Label>("Scale Digits:", 0));
/*
    AmbientLightData
  	Integer	red		1	1	roter Anteil (0-4095)
	Integer	blue		1	1	blauer Anteil (0-4095)
	Integer	green		1	1	grüner Anteil (0-4095)
	Integer	white		1	1	weißer Anteil (0-4095)
 */
}

void EnvironmentControl::on_button_curtain_up_clicked() {
    msgEndpoint->sendMsg(EnvironmentSetAmbience{ToggleState::ON, ToggleState::UNSET});
}

void EnvironmentControl::on_button_curtain_down_clicked() {
    msgEndpoint->sendMsg(EnvironmentSetAmbience{ToggleState::OFF, ToggleState::UNSET});
}

void EnvironmentControl::on_button_main_light_on_clicked() {
    msgEndpoint->sendMsg(EnvironmentSetAmbience{ToggleState::UNSET, ToggleState::ON});
}

void EnvironmentControl::on_button_main_light_off_clicked() {
    msgEndpoint->sendMsg(EnvironmentSetAmbience{ToggleState::UNSET, ToggleState::OFF});
}

void EnvironmentControl::enable() {
    set_sensitive(true);
}

void EnvironmentControl::disable() {
    set_sensitive(false);
}

void EnvironmentControl::setHardwareState(SystemHardwareStateChanged::HardwareState state) {
    switch(state) {
        case SystemHardwareStateChanged::HardwareState::MANUEL:
            set_sensitive(true);
            break;

        case SystemHardwareStateChanged::HardwareState::AUTOMATIC:
            set_sensitive(false);
            break;

        default:
            break;
    }
}
