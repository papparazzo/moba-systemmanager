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

#include <chrono>

#include "moba/endpoint.h"

#include "moba/registry.h"
#include "moba/servermessages.h"
#include "moba/guimessages.h"
#include "moba/clientmessages.h"
#include "moba/systemmessages.h"

#include <gtkmm/window.h>
#include <gtkmm/comboboxtext.h>
#include <gtkmm/liststore.h>

#include "activeapps.h"
#include "clock.h"

class FrmMain : public Gtk::Window {
    public:
        FrmMain(EndpointPtr mhp);
        virtual ~FrmMain() {
        }

    protected:
        Gtk::Notebook  m_Notebook;
        Gtk::ButtonBox m_ButtonBox;
        Gtk::Button    m_Button_Emegerency;
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
        TreeView_ActiveApps m_TreeView_ActiveApps;

        // time-control
        Gtk::ScrolledWindow m_ScrolledWindow_TimeControl;
        Gtk::HBox           m_HBox_TimeControl;
        Clock               m_Clock;

        Gtk::Box            m_VBox_TimeControl{Gtk::ORIENTATION_VERTICAL, 6};
        Gtk::Box            m_HBox_CurModelTime{Gtk::ORIENTATION_HORIZONTAL, 6};
        Gtk::Box            m_HBox_Multiplicator{Gtk::ORIENTATION_HORIZONTAL, 6};

        Gtk::Entry          m_Entry_CurModelTime;
        Gtk::Label          m_Label_CurModelTime;

        Gtk::Entry          m_Entry_Multiplicator;
        Gtk::Label          m_Label_Multiplicator;

        // notice-logger
        Gtk::ScrolledWindow m_ScrolledWindow_NoticeLogger;
        Gtk::TreeView       m_TreeView_Notices;
        Glib::RefPtr<Gtk::ListStore> m_refTreeModel_Notices;

        Gtk::Box         m_VBox_NoticeLogger{Gtk::ORIENTATION_VERTICAL, 2};
        Gtk::ButtonBox   m_ButtonBox_NoticeLogger;
        Gtk::Button      m_Button_NoticesClear{"Liste leeren"};

        // server-data
        Gtk::Label lblName[2][11];
        Gtk::VBox  m_VBox_ServerDataKey{Gtk::ORIENTATION_VERTICAL, 6};
        Gtk::VBox  m_VBox_ServerDataValue{Gtk::ORIENTATION_VERTICAL, 6};
        Gtk::HBox  m_HBox_ServerData;

        // system-control
        Gtk::ButtonBox m_ButtonBox_System;
        Gtk::Button    m_Button_SystemStandby;
        Gtk::Button    m_Button_SystemAutomatic;
        Gtk::Button    m_Button_SystemShutdown;
        Gtk::Button    m_Button_SystemReset;
        Gtk::Button    m_Button_SystemPing;
        Gtk::Box       m_VBox_SystemControl{Gtk::ORIENTATION_VERTICAL, 6};
        Gtk::Label     m_Label_HardwareState;
        Gtk::Label     m_Label_PingResult[4];

        class ModelColumnsNotices : public Gtk::TreeModel::ColumnRecord {
            public:
                ModelColumnsNotices() {
                    add(m_col_timestamp);
                    add(m_col_type);
                    add(m_col_caption);
                    add(m_col_text);
                }

                Gtk::TreeModelColumn<Glib::ustring> m_col_timestamp;
                Gtk::TreeModelColumn<Glib::ustring> m_col_type;
                Gtk::TreeModelColumn<Glib::ustring> m_col_caption;
                Gtk::TreeModelColumn<Glib::ustring> m_col_text;
        };

        ModelColumnsNotices m_Columns_Notices;

        void initAboutDialog();
        void initActiveApps();
        void initServerData();
        void initSystemControl();
        void initTimeController();
        void initNoticeLogger();

        EndpointPtr msgEndpoint;
        Registry    registry;

        std::chrono::time_point<std::chrono::system_clock> start;
        int pingctr;

        void setNotice(Gtk::MessageType noticeType, std::string caption, std::string text);
        void setHardwareStateLabel(const std::string &status);

        // Signal handlers:
        bool on_timeout(int timer_number);
        void on_button_about_clicked();
        void on_button_emegency_clicked();
        void on_button_system_shutdown_clicked();
        void on_button_system_reset_clicked();
        void on_button_system_automatic_clicked();
        void on_button_system_standby_clicked();
        void on_button_system_ping_clicked();
        void on_button_notices_clear_clicked();
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
        void setPingResult(const ClientEchoRes&);
};
