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

#include "automaticcontrol.h"
#include <iostream>

AutomaticControl::AutomaticControl(EndpointPtr msgEndpoint): msgEndpoint{msgEndpoint} {
    set_expand(true);

    append(m_grid_control_panel);
    append(m_VBox_Clock);

    m_Label_Date.set_margin(5);

    m_Clock.set_content_width(400);
    m_Clock.set_content_height(400);
    m_VBox_Clock.set_expand(true);

    m_VBox_Clock.append(m_Clock);
    m_VBox_Clock.append(m_Label_Date);

    m_grid_control_panel.set_margin(25);
    m_grid_control_panel.set_halign(Gtk::Align::CENTER);

    m_grid_control_panel.attach(m_Label_AutomaticControl, 0, 0);
    m_grid_control_panel.attach(m_Button_AutomaticControl_Ready, 1, 0);

    m_Button_AutomaticControl_Ready.set_halign(Gtk::Align::START);
    m_Button_AutomaticControl_Ready.set_margin(4);
    m_Label_AutomaticControl.set_halign(Gtk::Align::START);
    m_Label_AutomaticControl.set_margin(4);

//    m_grid_control_panel.attach(m_Label_Multiplicator, 0, 0);
//    m_grid_control_panel.attach(m_Combo_Multiplicator, 1, 0);

    m_grid_control_panel.attach(m_Label_CurModelDay, 0, 1);
    m_grid_control_panel.attach(m_Combo_CurModelDay, 1, 1);

    m_Label_Multiplicator.set_halign(Gtk::Align::START);
    m_Combo_Multiplicator.set_margin(4);
    m_Label_CurModelDay.set_halign(Gtk::Align::START);
    m_Combo_CurModelDay.set_margin(4);

    m_Label_Time[0].set_text("Startuhrzeit (hh:mm):");
    m_Label_Time[1].set_text("Sonnenaufgang (hh:mm):");
    m_Label_Time[2].set_text("Tag (hh:mm):");
    m_Label_Time[3].set_text("Sonnenuntergang (hh:mm):");
    m_Label_Time[4].set_text("Nacht (hh:mm):");

    for(int i = 0; i < 5; ++i) {
        m_grid_control_panel.attach(m_Label_Time[i], 0, 2 + i);
        m_grid_control_panel.attach(m_Entry_Time[i], 1, 2 + i);
        m_Entry_Time[i].set_width_chars(5);
        m_Label_Time[i].set_halign(Gtk::Align::START);
        m_Entry_Time[i].set_halign(Gtk::Align::START);
        m_Entry_Time[i].set_margin(4);
    }

    m_Combo_Multiplicator.pack_start(m_Columns_Multiplicator.m_col_label);

    m_grid_control_panel.attach_next_to(m_ButtonBox_AutomaticControl, m_Label_Time[4], Gtk::PositionType::BOTTOM, 2, 1);

    m_Button_AutomaticControl_Enable.set_margin(10);
    m_Button_AutomaticControl_Set.set_margin(10);

    m_ButtonBox_AutomaticControl.append(m_Button_AutomaticControl_Enable);
    m_ButtonBox_AutomaticControl.append(m_Button_AutomaticControl_Set);
    m_ButtonBox_AutomaticControl.set_margin(10);

    m_ButtonBox_AutomaticControl.set_sensitive(false);

    m_Button_AutomaticControl_Set.signal_clicked().connect(sigc::mem_fun(*this, &AutomaticControl::on_button_time_control_set_clicked));
    m_click_connection = m_Button_AutomaticControl_Enable.signal_clicked().connect(sigc::mem_fun(*this, &AutomaticControl::on_button_automatic_clicked));
    m_checkbox_connection = m_Button_AutomaticControl_Ready.signal_toggled().connect(sigc::mem_fun(*this, &AutomaticControl::on_button_automatic_ready_toggle));

    m_refListModel_CurModelDay = Gtk::ListStore::create(m_Columns_CurModelDay);
    m_Combo_CurModelDay.set_model(m_refListModel_CurModelDay);

    auto row = *m_refListModel_CurModelDay->append();
    row[m_Columns_CurModelDay.m_col_id] = Day::MONDAY;
    row[m_Columns_CurModelDay.m_col_name] = "Montag";

    row = *m_refListModel_CurModelDay->append();
    row[m_Columns_CurModelDay.m_col_id] = Day::TUESDAY;
    row[m_Columns_CurModelDay.m_col_name] = "Dienstag";

    row = *m_refListModel_CurModelDay->append();
    row[m_Columns_CurModelDay.m_col_id] = Day::WEDNESDAY;
    row[m_Columns_CurModelDay.m_col_name] = "Mittwoch";

    row = *m_refListModel_CurModelDay->append();
    row[m_Columns_CurModelDay.m_col_id] = Day::THURSDAY;
    row[m_Columns_CurModelDay.m_col_name] = "Donnerstag";

    row = *m_refListModel_CurModelDay->append();
    row[m_Columns_CurModelDay.m_col_id] = Day::FRIDAY;
    row[m_Columns_CurModelDay.m_col_name] = "Freitag";

    row = *m_refListModel_CurModelDay->append();
    row[m_Columns_CurModelDay.m_col_id] = Day::SATURDAY;
    row[m_Columns_CurModelDay.m_col_name] = "Samstag";

    row = *m_refListModel_CurModelDay->append();
    row[m_Columns_CurModelDay.m_col_id] = Day::SUNDAY;
    row[m_Columns_CurModelDay.m_col_name] = "Sonntag";

    m_Combo_CurModelDay.pack_start(m_Columns_CurModelDay.m_col_name);

    m_refListModel_Multiplicator = Gtk::ListStore::create(m_Columns_Multiplicator);
    m_Combo_Multiplicator.set_model(m_refListModel_Multiplicator);

    for(unsigned int i = 1; i <= 4; ++i) {
        std::stringstream ss;

        ss << "1min -> " << i << "h";

        auto row1 = *m_refListModel_Multiplicator->append();
        row1[m_Columns_Multiplicator.m_col_factor] = i;
        row1[m_Columns_Multiplicator.m_col_label] = ss.str();
    }
}

void AutomaticControl::enable() {
    m_Button_AutomaticControl_Set.set_sensitive(true);
    m_ButtonBox_AutomaticControl.set_sensitive(true);
}

void AutomaticControl::disable() {
    m_Button_AutomaticControl_Set.set_sensitive(false);
    m_ButtonBox_AutomaticControl.set_sensitive(false);
}

void AutomaticControl::setHardwareState(const SystemState systemState) {
    m_click_connection.block();
    m_checkbox_connection.block();

    switch(systemState) {
        case SystemState::INITIALIZING:
        case SystemState::INCIDENT:
        case SystemState::STANDBY:
        case SystemState::SHUTDOWN:
        case SystemState::NO_CONNECTION:
            m_Clock.stop();
            m_Button_AutomaticControl_Ready.set_sensitive(false);
            m_Button_AutomaticControl_Enable.set_sensitive(false);
            break;

        case SystemState::READY:
            m_Clock.stop();
            automaticModeState = AutomaticMode::READY;
            m_Button_AutomaticControl_Ready.set_sensitive(true);
            m_Button_AutomaticControl_Ready.set_active(true);
            m_Button_AutomaticControl_Enable.set_sensitive(true);
            m_Button_AutomaticControl_Enable.set_active(false);
            break;

        case SystemState::MANUAL:
            m_Clock.stop();

            automaticModeState = AutomaticMode::NOT_READY;
            m_Button_AutomaticControl_Ready.set_sensitive(true);
            m_Button_AutomaticControl_Ready.set_active(false);
            m_Button_AutomaticControl_Enable.set_sensitive(false);
            m_Button_AutomaticControl_Enable.set_active(false);
            break;

        case SystemState::AUTOMATIC:
            m_Clock.run();
            automaticModeState = AutomaticMode::ACTIVE;
            m_Button_AutomaticControl_Ready.set_sensitive(false);
            m_Button_AutomaticControl_Ready.set_active(true);
            m_Button_AutomaticControl_Enable.set_sensitive(true);
            m_Button_AutomaticControl_Enable.set_active(true);
            break;
    }
    m_checkbox_connection.unblock();
    m_click_connection.unblock();
}

void AutomaticControl::setClock(const Day day, const Time time) {
    std::stringstream ss;

    switch(day) {
        case Day::MONDAY:
            ss << "Montag";
            break;

        case Day::TUESDAY:
            ss << "Dienstag";
            break;

        case Day::WEDNESDAY:
            ss << "Mittwoch";
            break;

        case Day::THURSDAY:
            ss << "Donnerstag";
            break;

        case Day::FRIDAY:
            ss << "Freitag";
            break;

        case Day::SATURDAY:
            ss << "Samstag";
            break;

        case Day::SUNDAY:
            ss << "Sonntag";
            break;
    }

    /**
     * https://www.t-online.de/leben/familie/id_88749070/tageszeiten-in-deutschland-wann-beginnt-eigentlich-die-nacht-.html
     *
     * ab  7:00 Uhr morgens
     * ab 11:00 Uhr vormittags
     * ab 13:00 Uhr mittags
     * ab 15:00 Uhr nachmittags
     * ab 18:00 Uhr abends
     * ab 22:00 Uhr nachts
     *
     * https://www.laenderdaten.info/Europa/Deutschland/sonnenuntergang.php
     *
     * ab  4:00 Uhr Sonnenaufgang
     * ab  5:00 Uhr Tag
     * ab 21:30 Uhr Sonnenuntergang
     * ab 22:30 Uht Nacht
     */

    auto hours = time.getHours();
    if(hours >= 5 && hours < 10) {
        ss << " morgens";
    } else if(hours >= 10 && hours < 12) {
        ss << " vormittags";
    } else if(hours >= 12 && hours < 15) {
        ss << " mittags";
    } else if(hours >= 15 && hours < 18) {
        ss << " nachmittags";
    } else if(hours >= 18 && hours < 22) {
        ss << " abends";
    } else if(hours >= 22 || hours < 5) {
        ss << " nachts";
    }

    /* Geht so nicht, da "AutomaticControl::setClock" 'stündlich' aufgerufen wird
     * aber Sonnenauf und untergänge zu jeder Zeit stattfinden können
     *
    ss << "<b>";
    if(time >= sunriseStartTime && time < dayStartTime) {
        ss << " (Sonnenaufgang)";
    } else if(time >= sunsetStartTime && time < nightStartTime) {
        ss << " (Sonnenuntergang)";
    }
    ss << "</b>";
    */
    m_Label_Date.set_markup(ss.str());
}

void AutomaticControl::on_button_time_control_set_clicked() {
    auto iter = m_Combo_CurModelDay.get_active();
    assert(iter);

    auto iter1 = m_Combo_Multiplicator.get_active();
    assert(iter1);

    msgEndpoint->sendMsg(TimerSetGlobalTimer{
        (*iter)[m_Columns_CurModelDay.m_col_id],
        Time{m_Entry_Time[0].get_text()},
        (*iter1)[m_Columns_Multiplicator.m_col_factor],
        Time{m_Entry_Time[1].get_text()},
        Time{m_Entry_Time[2].get_text()},
        Time{m_Entry_Time[3].get_text()},
        Time{m_Entry_Time[4].get_text()}
    });
}

void AutomaticControl::on_button_automatic_clicked() const {
    if(automaticModeState == AutomaticMode::ACTIVE) {
        msgEndpoint->sendMsg(SystemSetAutomaticMode{false});
        return;
    }

    if(automaticModeState == AutomaticMode::READY) {
        msgEndpoint->sendMsg(SystemSetAutomaticMode{true});
    }
}

void AutomaticControl::on_button_automatic_finish(const Glib::RefPtr<Gio::AsyncResult>& result) const {
    try {
        switch(m_pDialog->choose_finish(result)) {
            case 0:
                return;

            case 1:
                msgEndpoint->sendMsg(SystemReadyForAutomaticMode{true});
                break;

            default:
                std::cout << "Unexpected response: " << std::endl;
                break;
        }
    } catch(const Gtk::DialogError& err) {
        // TODO: Propper error handling!
         std::cout << "DialogError, " << err.what() << std::endl;
    } catch(const Glib::Error& err) {
        // TODO: Propper error handling!
         std::cout << "Unexpected exception. " << err.what() << std::endl;
    }
}

void AutomaticControl::setTimerGlobalTimerEvent(const TimerGlobalTimerEvent &data) {
    m_Clock.setTime(data.time.getHours(), data.time.getMinutes(), false);
    setClock(data.curModelDay, data.time);
}

void AutomaticControl::setTimerSetGlobalTimer(const TimerSetGlobalTimer &data) {
    m_Combo_CurModelDay.set_active(static_cast<int>(data.curModelDay));

    sunriseStartTime = data.sunriseStartTime;
    dayStartTime = data.dayStartTime;
    sunsetStartTime = data.sunsetStartTime;
    nightStartTime = data.nightStartTime;

    m_Clock.setNightLight(sunsetStartTime, dayStartTime);

    m_Entry_Time[0].set_text(data.curModelTime.getTimeAsString());
    m_Entry_Time[1].set_text(sunriseStartTime.getTimeAsString());
    m_Entry_Time[2].set_text(dayStartTime.getTimeAsString());
    m_Entry_Time[3].set_text(sunsetStartTime.getTimeAsString());
    m_Entry_Time[4].set_text(nightStartTime.getTimeAsString());

    m_Combo_Multiplicator.set_active(data.multiplicator - 1);

    m_Clock.setMultiplier(data.multiplicator);
    m_Clock.setTime(data.curModelTime.getHours(), data.curModelTime.getMinutes(), true);
    setClock(data.curModelDay, data.curModelTime);

    std::stringstream ss;
    ss << "<b>Faktor:</b> 1min -> " << data.multiplicator << "h (" << data.multiplicator << ")";

    m_Label_Date.set_tooltip_markup(ss.str());
}

void AutomaticControl::on_button_automatic_ready_toggle() {
    if(automaticModeState == AutomaticMode::ACTIVE) {
        return;
    }

    if(automaticModeState == AutomaticMode::READY) {
        msgEndpoint->sendMsg(SystemReadyForAutomaticMode{false});
        return;
    }

    m_checkbox_connection.block();
    m_Button_AutomaticControl_Ready.set_active(false);
    m_checkbox_connection.unblock();

    if(!m_pDialog) {
        m_pDialog = Gtk::AlertDialog::create();
    }

    m_pDialog->set_message("Aktivierung Automatik");
    m_pDialog->set_detail("Befinden sich alle Züge in definierten Positionen?");
    m_pDialog->set_buttons({"Abbrechen", "OK"});
    m_pDialog->set_default_button(1);
    m_pDialog->set_cancel_button(0);
    if(auto* window = dynamic_cast<Gtk::Window*>(get_root())) {
        m_pDialog->choose(*window, sigc::mem_fun(*this, &AutomaticControl::on_button_automatic_finish));
    }
}
