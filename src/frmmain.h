/*
 *  Project:    moba-systemmanager
 *
 *  Copyright (C) 2016 Stefan Paproth <pappi-@gmx.de>
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

#include "moba/registry.h"
#include "moba/servermessages.h"
#include "moba/guimessages.h"
#include "moba/clientmessages.h"
#include "moba/systemmessages.h"
#include "moba/timermessages.h"

#include <gtkmm/window.h>
#include <gtkmm/comboboxtext.h>
#include <gtkmm/liststore.h>

#include "activeapps.h"
#include "noticelogger.h"
#include "systemcontrol.h"
#include "automaticcontrol.h"

class FrmMain : public Gtk::Window {
    public:
        FrmMain(EndpointPtr mhp);
        virtual ~FrmMain() {
        }

    protected:
        Gtk::Notebook  m_Notebook;
        Gtk::ButtonBox m_ButtonBox;
        Gtk::Button    m_Button_Emergency{"Nothalt"};
        Gtk::Box       m_VBox{Gtk::ORIENTATION_VERTICAL, 6};
        Gtk::Box       m_HBox{Gtk::ORIENTATION_HORIZONTAL, 6};
        Gtk::Label     m_Label_Connectivity_HW{" \xe2\x96\x84"};
        Gtk::Label     m_Label_Connectivity_SW{" \xe2\x96\x84"};

        // about
        Gtk::Button      m_Button_About{"About..."};
        Gtk::AboutDialog m_Dialog;

        // info-bar
        Gtk::InfoBar m_InfoBar;
        Gtk::Label   m_Label_InfoBarMessage;

        // active-apps
        Gtk::ScrolledWindow m_ScrolledWindow;
        ActiveApps          m_ActiveApps;

        NoticeLogger     m_Notice_Logger;
        SystemControl    m_System_Control;
        AutomaticControl m_Automatic_Control;

        // server-data
        Gtk::Label lblName[2][11];
        Gtk::VBox  m_VBox_ServerDataKey{Gtk::ORIENTATION_VERTICAL, 6};
        Gtk::VBox  m_VBox_ServerDataValue{Gtk::ORIENTATION_VERTICAL, 6};
        Gtk::HBox  m_HBox_ServerData;

        void initAboutDialog();
        void initActiveApps();
        void initServerData();

        EndpointPtr msgEndpoint;
        Registry    registry;

        void setNotice(Gtk::MessageType noticeType, std::string caption, std::string text);

        // Signal handlers:
        bool on_timeout(int timer_number);
        void on_button_about_clicked();
        void on_button_emergency_clicked();
        void on_about_dialog_response(int response_id);
        void on_infobar_response(int response);

        // msg-response
        void setServerInfoRes(const ServerInfoRes &data);
        void setConClientsRes(const ServerConClientsRes &data);
        void setSystemNotice(const GuiSystemNotice &data);
        void setErrorNotice(const ClientError &data);
        void setHardwareState(const SystemHardwareStateChanged &data);
        void setNewClient(const ServerNewClientStarted &data);
        void setRemoveClient(const ServerClientClosed &data);
};
