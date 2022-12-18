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

    for(unsigned int i = 60; i <= 240; i += 30) {
        std::stringstream ss;

        ss << "1min -> " << static_cast<float>(i) / 60.0 << "h";

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
    m_VBox_Settings.pack_start(m_HBox_CurModelDay, Gtk::PACK_SHRINK);
    m_VBox_Settings.pack_start(m_HBox_CurModelTime, Gtk::PACK_SHRINK);
    m_VBox_Settings.pack_start(m_HBox_Multiplicator, Gtk::PACK_SHRINK);
    m_VBox_Settings.pack_end(m_ButtonBox_AutomaticControl, Gtk::PACK_SHRINK, 15);

    m_ButtonBox_AutomaticControl.pack_start(m_Button_AutomaticControl_Enable);
    m_ButtonBox_AutomaticControl.pack_start(m_Button_AutomaticControl_Set, Gtk::PACK_EXPAND_WIDGET, 5);
    m_ButtonBox_AutomaticControl.set_layout(Gtk::BUTTONBOX_END);

    m_HBox_CurModelDay.pack_start(m_Label_CurModelDay, Gtk::PACK_SHRINK, 5);
    m_HBox_CurModelDay.pack_end(m_Combo_CurModelDay, Gtk::PACK_SHRINK, 5);

    m_HBox_CurModelTime.pack_start(m_Label_CurModelTime, Gtk::PACK_SHRINK, 5);
    m_HBox_CurModelTime.pack_end(m_Entry_CurModelTime, Gtk::PACK_SHRINK, 5);

    m_HBox_Multiplicator.pack_start(m_Label_Multiplicator, Gtk::PACK_SHRINK, 5);
    m_HBox_Multiplicator.pack_end(m_Combo_Multiplicator, Gtk::PACK_SHRINK, 5);

    m_ButtonBox_AutomaticControl.set_sensitive(false);

    m_Button_AutomaticControl_Set.signal_clicked().connect(sigc::mem_fun(*this, &AutomaticControl::on_button_time_control_set_clicked));
    m_Button_AutomaticControl_Enable.signal_clicked().connect(sigc::mem_fun(*this, &AutomaticControl::on_button_automatic_clicked));
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
            m_Button_AutomaticControl_Enable.set_sensitive(true);
            m_Button_AutomaticControl_Enable.set_label("Automatik (aus)");
            break;

        case SystemHardwareStateChanged::HardwareState::AUTOMATIC:
            m_Clock.run();
            m_Button_AutomaticControl_Enable.set_sensitive(true);
            m_Button_AutomaticControl_Enable.set_label("Automatik (an)");
            break;
    }
}

void AutomaticControl::setClock(Day day, unsigned int hours) {

    std::stringstream ss;
    ss << "<b>";

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

    ss << "</b> (";

    /*
    if(data.hours >= 7 || data.hours < 11) {
        ss << "morgens";
    } else if(data.hours >= 11 || data.hours < 13) {
        ss << "vormittags";
    } else if(data.hours < 11 || data.hours > 9) {
        ss << "mittags";
    } else if(data.hours < 13 || data.hours > 11) {
        ss << "nachmittags";
    } else if(data.hours < 17 || data.hours > 13) {
        ss << "abends";
    } else if(data.hours < 21 || data.hours > 17) {
        ss << "nachts";
    }*/
    ss << ")";

    m_Label_Date.set_markup(ss.str());
    //m_Label_Date.set_tooltip_markup("<b>Faktor:</b> unbekannt");
}

void AutomaticControl::on_button_time_control_set_clicked() {
    auto iter = m_Combo_CurModelDay.get_active();
    assert(iter);

    auto iter1 = m_Combo_Multiplicator.get_active();
    assert(iter1);

    msgEndpoint->sendMsg(TimerSetGlobalTimer{
        (*iter)[m_Columns_CurModelDay.m_col_id],
        m_Entry_CurModelTime.get_text(),
        (*iter1)[m_Columns_Multiplicator.m_col_factor]
    });
}

void AutomaticControl::on_button_automatic_clicked() {
    if(m_Button_AutomaticControl_Enable.get_label() == "Automatik (aus)") {
        msgEndpoint->sendMsg(SystemSetAutomaticMode{true});
    } else {
        msgEndpoint->sendMsg(SystemSetAutomaticMode{false});
    }
}

void AutomaticControl::setTimerGlobalTimerEvent(const TimerGlobalTimerEvent &data) {
    m_Clock.setMultiplier(data.multiplicator);
    m_Clock.setTime(data.hours, data.minutes, false);

    setClock(data.curModelDay, data.hours);
}

void AutomaticControl::setTimerSetGlobalTimer(const TimerSetGlobalTimer &data) {
    m_Combo_CurModelDay.set_active(static_cast<int>(data.curModelDay));
    m_Entry_CurModelTime.set_text(data.curModelTime);

    auto children = m_refListModel_Multiplicator->children();

    for(auto iter : children) {
        if(data.multiplicator == (*iter)[m_Columns_Multiplicator.m_col_factor]) {
            m_Combo_Multiplicator.set_active(iter);
            break;
        }
    }

    m_Clock.setMultiplier(data.multiplicator);
    m_Clock.setTime(data.hours, data.minutes, true);

    setClock(data.curModelDay, data.hours);
}

void AutomaticControl::setNightLight(bool activate) {
    Glib::RefPtr<Gio::Settings> s = Gio::Settings::create("org.gnome.settings-daemon.plugins.color");

    if(activate) {
        s->set_uint("night-light-temperature", 1000);
    }
    s->set_boolean("night-light-enabled", activate);
}