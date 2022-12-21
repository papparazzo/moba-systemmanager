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
#include <giomm/settings.h>

#include "clock.h"

#include "moba/endpoint.h"
#include "moba/systemmessages.h"
#include "moba/timermessages.h"
#include "moba/day.h"

class AutomaticControl: public Gtk::HBox {
public:
    AutomaticControl(EndpointPtr msgEndpoint);
    AutomaticControl(const AutomaticControl&) = delete;
    AutomaticControl& operator=(const AutomaticControl&) = delete;

    virtual ~AutomaticControl();

    void enable();
    void disable();

    void setHardwareState(SystemHardwareStateChanged::HardwareState state);
    void setTimerGlobalTimerEvent(const TimerGlobalTimerEvent &data);
    void setTimerSetGlobalTimer(const TimerSetGlobalTimer &data);

private:
    void setClock(Day day, unsigned int hours);
    void setNightLight(bool activate);

    void on_button_time_control_set_clicked();
    void on_button_automatic_clicked();

    Gtk::VBox    m_VBox_Clock;
    Gtk::Label   m_Label_Date;

    Clock        m_Clock;

    Gtk::Box     m_VBox_Settings{Gtk::ORIENTATION_VERTICAL, 6};

    Gtk::Box     m_HBox_CurModelDay{Gtk::ORIENTATION_HORIZONTAL, 6};
    Gtk::Box     m_HBox_CurModelTime{Gtk::ORIENTATION_HORIZONTAL, 6};
    Gtk::Box     m_HBox_Multiplicator{Gtk::ORIENTATION_HORIZONTAL, 6};

    struct ModelColumns_CurModelDay: public Gtk::TreeModel::ColumnRecord {
        ModelColumns_CurModelDay() {
            add(m_col_name);
            add(m_col_id);
        }

        Gtk::TreeModelColumn<Glib::ustring> m_col_name; //The data to choose - this must be text.
        Gtk::TreeModelColumn<Day>           m_col_id;
    };

    struct ModelColumns_Multiplicator: public Gtk::TreeModel::ColumnRecord {
        ModelColumns_Multiplicator() {
            add(m_col_label);
            add(m_col_factor);
        }

        Gtk::TreeModelColumn<Glib::ustring> m_col_label;
        Gtk::TreeModelColumn<unsigned int>  m_col_factor;
    };


    ModelColumns_CurModelDay     m_Columns_CurModelDay;
    ModelColumns_Multiplicator   m_Columns_Multiplicator;

    Glib::RefPtr<Gtk::ListStore> m_refListModel_CurModelDay;
    Glib::RefPtr<Gtk::ListStore> m_refListModel_Multiplicator;

    Gtk::Label        m_Label_Spacer;

    Gtk::ComboBox     m_Combo_CurModelDay;
    Gtk::Label        m_Label_CurModelDay{"Tag:"};

    Gtk::Entry        m_Entry_CurModelTime;
    Gtk::Label        m_Label_CurModelTime{"Uhrzeit (hh:mm):"};

    Gtk::ComboBox     m_Combo_Multiplicator;
    Gtk::Label        m_Label_Multiplicator{"Multiplikator"};

    Gtk::ButtonBox    m_ButtonBox_AutomaticControl;
    Gtk::Button       m_Button_AutomaticControl_Set{"Werte setzen"};
    Gtk::ToggleButton m_Button_AutomaticControl_Enable{"Automatik"};

    sigc::connection  m_click_connection;

    EndpointPtr msgEndpoint;
};

