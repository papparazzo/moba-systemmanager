/*
 *  Project:    moba-systemmanager
 *
 *  Copyright (C) 2023 Stefan Paproth <pappi-@gmx.de>
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

#include "moba/endpoint.h"
#include "moba/registry.h"

#include "moba/guimessages.h"
#include "moba/clientmessages.h"
#include "moba/systemmessages.h"

#include <gtkmm.h>
#include <atomic>

enum class SystemState {
    NO_CONNECT,
    ERROR,
    STANDBY,
    EMERGENCY_STOP,
    MANUEL,
    AUTOMATIC
};

class FrmBase: public Gtk::Window {
public:
    FrmBase(EndpointPtr mhp);
    virtual ~FrmBase();

protected:
    std::atomic<SystemState> systemState;

    EndpointPtr msgEndpoint;

    Registry registry;

    // about
    Gtk::Button      m_Button_About{"About..."};
    Gtk::AboutDialog m_Dialog;

    // info-bar
    Gtk::InfoBar     m_InfoBar;
    Gtk::Label       m_Label_InfoBarMessage;

    Gtk::Button      m_Button_Emergency{"Nothalt"};

    // status label
    Gtk::Label       m_Label_Connectivity_HW{" \xe2\x96\x84"};
    Gtk::Label       m_Label_Connectivity_SW{" \xe2\x96\x84"};

    Gtk::Box         m_ButtonBox;
    Gtk::Box         m_VBox{Gtk::Orientation::VERTICAL, 6};
    Gtk::Box         m_HBox{Gtk::Orientation::HORIZONTAL, 6};

    std::string getDisplayMessage(std::string caption, std::string text);

    void initAboutDialog();
    void finishForm();

    // message-response
    void setNotice(Gtk::MessageType noticeType, std::string caption, std::string text);
    void setSystemNotice(const GuiSystemNotice &data);
    void setErrorNotice(const ClientError &data);
    void setHardwareState(const SystemHardwareStateChanged &data);

    virtual void setSensitive(bool) {}
    virtual void initialSend() {}
    virtual void setSystemState(SystemState systemState) {}
    virtual void listNotice(Gtk::MessageType noticeType, std::string caption, std::string text) {}
    
    // Signal handlers:
    void on_about_dialog_response(int response_id);
    void on_button_about_clicked();
    void on_button_emergency_clicked();
    void on_infobar_response(int response);
    bool on_timeout(int timer_number);
    bool on_timeout_status(int);
};

