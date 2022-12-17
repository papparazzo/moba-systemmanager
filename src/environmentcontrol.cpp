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

    add(m_VBox_Left);
    add(m_VBox_Right);

    m_VBox_Left.pack_start(m_Label_Spacer, Gtk::PACK_SHRINK, 5);
    m_VBox_Left.pack_start(m_HBox_Curtain, Gtk::PACK_SHRINK);
    m_VBox_Left.pack_start(m_HBox_MainLight, Gtk::PACK_SHRINK);

    m_HBox_Curtain.pack_start(m_Label_Curtain, Gtk::PACK_SHRINK, 5);
    m_HBox_Curtain.pack_end(m_Button_Curtain_Up, Gtk::PACK_SHRINK, 5);
    m_HBox_Curtain.pack_end(m_Button_Curtain_Down, Gtk::PACK_SHRINK, 5);

    m_HBox_MainLight.pack_start(m_Label_MainLight, Gtk::PACK_SHRINK, 5);
    m_HBox_MainLight.pack_end(m_Button_MainLight, Gtk::PACK_SHRINK, 5);


    m_Button_Curtain_Up.signal_clicked().connect(sigc::mem_fun(*this, &EnvironmentControl::on_button_curtain_up_clicked));
    m_Button_Curtain_Down.signal_clicked().connect(sigc::mem_fun(*this, &EnvironmentControl::on_button_curtain_down_clicked));
    m_Button_MainLight.signal_clicked().connect(sigc::mem_fun(*this, &EnvironmentControl::on_button_main_light_clicked));

    //m_HBox_Digits.append(*Gtk::make_managed<Gtk::Label>("Scale Digits:", 0));
/*
    AmbientLightData
  	Integer	red		1	1	roter Anteil (0-4095)
	Integer	blue		1	1	blauer Anteil (0-4095)
	Integer	green		1	1	grüner Anteil (0-4095)
	Integer	white		1	1	weißer Anteil (0-4095)
 */
}

EnvironmentControl::~EnvironmentControl() {
}

void EnvironmentControl::on_button_curtain_up_clicked() {
    //msgEndpoint->sendMsg();
}

void EnvironmentControl::on_button_curtain_down_clicked() {
    //msgEndpoint->sendMsg();
}

void EnvironmentControl::on_button_main_light_clicked() {
    if(m_Button_MainLight.get_active()) {
        m_Button_MainLight.set_label("aus");
    } else {
        m_Button_MainLight.set_label("an");
    }
    //msgEndpoint->sendMsg();
}

void EnvironmentControl::enable() {
    set_sensitive(true);
}

void EnvironmentControl::disable() {
    set_sensitive(false);
}

void EnvironmentControl::setMainLightActive(bool active) {
    if(active) {
        m_Button_MainLight.set_label("aus");
    } else {
        m_Button_MainLight.set_label("an");
    }
    m_Button_MainLight.set_active(active);
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