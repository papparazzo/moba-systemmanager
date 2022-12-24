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

AutomaticControl::AutomaticControl(EndpointPtr msgEndpoint): msgEndpoint{msgEndpoint} {

    m_refListModel_CurModelDay = Gtk::ListStore::create(m_Columns_CurModelDay);
    m_Combo_CurModelDay.set_model(m_refListModel_CurModelDay);

    auto row = *(m_refListModel_CurModelDay->append());
    row[m_Columns_CurModelDay.m_col_id] = Day::MONDAY;
    row[m_Columns_CurModelDay.m_col_name] = "Montag";

    row = *(m_refListModel_CurModelDay->append());
    row[m_Columns_CurModelDay.m_col_id] = Day::TUESDAY;
    row[m_Columns_CurModelDay.m_col_name] = "Dienstag";

    row = *(m_refListModel_CurModelDay->append());
    row[m_Columns_CurModelDay.m_col_id] = Day::WEDNESDAY;
    row[m_Columns_CurModelDay.m_col_name] = "Mittwoch";

    row = *(m_refListModel_CurModelDay->append());
    row[m_Columns_CurModelDay.m_col_id] = Day::THURSDAY;
    row[m_Columns_CurModelDay.m_col_name] = "Donnerstag";

    row = *(m_refListModel_CurModelDay->append());
    row[m_Columns_CurModelDay.m_col_id] = Day::FRIDAY;
    row[m_Columns_CurModelDay.m_col_name] = "Freitag";

    row = *(m_refListModel_CurModelDay->append());
    row[m_Columns_CurModelDay.m_col_id] = Day::SATURDAY;
    row[m_Columns_CurModelDay.m_col_name] = "Samstag";

    row = *(m_refListModel_CurModelDay->append());
    row[m_Columns_CurModelDay.m_col_id] = Day::SUNDAY;
    row[m_Columns_CurModelDay.m_col_name] = "Sonntag";

    m_Combo_CurModelDay.pack_start(m_Columns_CurModelDay.m_col_name);

    m_refListModel_Multiplicator = Gtk::ListStore::create(m_Columns_Multiplicator);
    m_Combo_Multiplicator.set_model(m_refListModel_Multiplicator);

    for(unsigned int i = 1; i <= 4; ++i) {
        std::stringstream ss;

        ss << "1min -> " << i << "h";

        auto row1 = *(m_refListModel_Multiplicator->append());
        row1[m_Columns_Multiplicator.m_col_factor] = i;
        row1[m_Columns_Multiplicator.m_col_label] = ss.str();
    }

    m_Combo_Multiplicator.pack_start(m_Columns_Multiplicator.m_col_label);

    set_homogeneous(true);
    add(m_VBox_Settings);
    add(m_VBox_Clock);

    m_VBox_Clock.pack_start(m_Clock);
    m_VBox_Clock.pack_start(m_Label_Date, Gtk::PACK_SHRINK, 10);

    m_VBox_Settings.pack_start(m_Label_Spacer, Gtk::PACK_SHRINK, 5);
    m_VBox_Settings.pack_start(m_HBox_Multiplicator, Gtk::PACK_SHRINK);
    m_VBox_Settings.pack_start(m_HBox_CurModelDay, Gtk::PACK_SHRINK);

    for(int i = 0; i < 5; ++i) {
        m_HBox[i].set_spacing(6);
        m_VBox_Settings.pack_start(m_HBox[i], Gtk::PACK_SHRINK);
        m_HBox[i].pack_start(m_Label_Time[i], Gtk::PACK_SHRINK, 5);
        m_HBox[i].pack_end(m_Entry_Time[i], Gtk::PACK_SHRINK, 5);
        m_Entry_Time[i].set_width_chars(5);
    }

    m_Label_Time[0].set_text("Uhrzeit (hh:mm):");
    m_Label_Time[1].set_text("Sonnenaufgang (hh:mm):");
    m_Label_Time[2].set_text("Tag (hh:mm):");
    m_Label_Time[3].set_text("Sonnenuntergang (hh:mm):");
    m_Label_Time[4].set_text("Nacht (hh:mm):");

    m_VBox_Settings.pack_end(m_ButtonBox_AutomaticControl, Gtk::PACK_SHRINK, 15);

    m_ButtonBox_AutomaticControl.pack_start(m_Button_AutomaticControl_Enable);
    m_ButtonBox_AutomaticControl.pack_start(m_Button_AutomaticControl_Set, Gtk::PACK_EXPAND_WIDGET, 5);
    m_ButtonBox_AutomaticControl.set_layout(Gtk::BUTTONBOX_END);

    m_HBox_CurModelDay.pack_start(m_Label_CurModelDay, Gtk::PACK_SHRINK, 5);
    m_HBox_CurModelDay.pack_end(m_Combo_CurModelDay, Gtk::PACK_SHRINK, 5);

    m_HBox_Multiplicator.pack_start(m_Label_Multiplicator, Gtk::PACK_SHRINK, 5);
    m_HBox_Multiplicator.pack_end(m_Combo_Multiplicator, Gtk::PACK_SHRINK, 5);

    m_ButtonBox_AutomaticControl.set_sensitive(false);

    m_Button_AutomaticControl_Set.signal_clicked().connect(sigc::mem_fun(*this, &AutomaticControl::on_button_time_control_set_clicked));
    m_click_connection = m_Button_AutomaticControl_Enable.signal_clicked().connect(sigc::mem_fun(*this, &AutomaticControl::on_button_automatic_clicked));
}

AutomaticControl::~AutomaticControl() {
}

void AutomaticControl::enable() {
    m_Button_AutomaticControl_Set.set_sensitive(true);
    m_ButtonBox_AutomaticControl.set_sensitive(true);
}

void AutomaticControl::disable() {
    m_Button_AutomaticControl_Set.set_sensitive(false);
    m_ButtonBox_AutomaticControl.set_sensitive(false);
}

void AutomaticControl::setHardwareState(SystemHardwareStateChanged::HardwareState state) {
    switch(state) {
        case SystemHardwareStateChanged::HardwareState::ERROR:
            m_Clock.stop();
            m_Button_AutomaticControl_Enable.set_sensitive(false);
            break;

        case SystemHardwareStateChanged::HardwareState::STANDBY:
            m_Clock.stop();
            m_Button_AutomaticControl_Enable.set_sensitive(false);
            break;

        case SystemHardwareStateChanged::HardwareState::EMERGENCY_STOP:
            m_Clock.stop();
            m_Button_AutomaticControl_Enable.set_sensitive(false);
            break;

        case SystemHardwareStateChanged::HardwareState::MANUEL:
            m_Clock.stop();
            m_click_connection.block();
            m_Button_AutomaticControl_Enable.set_sensitive(true);
            m_Button_AutomaticControl_Enable.set_active(false);
            m_click_connection.unblock();
            break;

        case SystemHardwareStateChanged::HardwareState::AUTOMATIC:
            m_Clock.run();
            m_click_connection.block();
            m_Button_AutomaticControl_Enable.set_sensitive(true);
            m_Button_AutomaticControl_Enable.set_active(true);
            m_click_connection.unblock();
            break;
    }
}

void AutomaticControl::setClock(Day day, Time time) {

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

void AutomaticControl::on_button_automatic_clicked() {
    if(m_Button_AutomaticControl_Enable.get_active()) {
        msgEndpoint->sendMsg(SystemSetAutomaticMode{true});
    } else {
        msgEndpoint->sendMsg(SystemSetAutomaticMode{false});
    }
}

void AutomaticControl::setTimerGlobalTimerEvent(const TimerGlobalTimerEvent &data) {
    m_Clock.setTime(data.time.getHours(), data.time.getMinutes(), false);
    setClock(data.curModelDay, data.time);
}

void AutomaticControl::setTimerSetGlobalTimer(const TimerSetGlobalTimer &data) {
    m_Combo_CurModelDay.set_active(static_cast<int>(data.curModelDay));

    nightStartTime   = Time{22, 30};
    sunriseStartTime = Time{4, 0};
    dayStartTime     = Time{5, 0};
    sunsetStartTime  = Time{21, 30};

    /*
    sunriseStartTime = data.sunriseStartTime;
    dayStartTime = data.dayStartTime;
    sunsetStartTime = data.sunsetStartTime;
    nightStartTime = data.nightStartTime;
     */

    m_Clock.setNightLight(sunsetStartTime, dayStartTime);

    m_Entry_Time[0].set_text(data.curModelTime.getTimeAsString());
    m_Entry_Time[1].set_text(sunriseStartTime.getTimeAsString());
    m_Entry_Time[2].set_text(dayStartTime.getTimeAsString());
    m_Entry_Time[3].set_text(sunsetStartTime.getTimeAsString());
    m_Entry_Time[4].set_text(nightStartTime.getTimeAsString());

    auto children = m_refListModel_Multiplicator->children();

    for(auto iter : children) {
        if(data.multiplicator == (*iter)[m_Columns_Multiplicator.m_col_factor]) {
            m_Combo_Multiplicator.set_active(iter);
            break;
        }
    }

    m_Clock.setMultiplier(data.multiplicator);
    m_Clock.setTime(data.curModelTime.getHours(), data.curModelTime.getMinutes(), true);
    setClock(data.curModelDay, data.curModelTime);

    std::stringstream ss;
    ss <<
        "<b>Faktor:</b> 1min -> " << data.multiplicator << "h (" << data.multiplicator << ")";

    m_Label_Date.set_tooltip_markup(ss.str());
}
