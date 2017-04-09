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

#include <moba/msghandler.h>
#include <moba/systemhandler.h>
#include <moba/jsonabstractitem.h>

#include <gtkmm/window.h>
#include <gtkmm/comboboxtext.h>
#include <gtkmm/liststore.h>

class FrmMain : public Gtk::Window {
    public:
        FrmMain(moba::MsgHandlerPtr mhp);
        virtual ~FrmMain() {
        }

    protected:
        Gtk::Notebook m_Notebook;
        Gtk::ButtonBox m_ButtonBox;
        Gtk::Button m_Button_Emegerency;
        Gtk::Box m_VBox;

        // about
        Gtk::Button m_Button_About;
        Gtk::AboutDialog m_Dialog;

        // info-bar
        Gtk::InfoBar m_InfoBar;
        Gtk::Label m_Label_InfoBarMessage;

        // active-apps
        Gtk::ScrolledWindow m_ScrolledWindow;
        Gtk::TreeView m_TreeView;
        Glib::RefPtr<Gtk::ListStore> m_refTreeModel;

        // notices
        Gtk::ScrolledWindow m_ScrolledWindow_Notices;
        Gtk::TreeView m_TreeView_Notices;
        Glib::RefPtr<Gtk::ListStore> m_refTreeModel_Notices;

        // server-data
        Gtk::Label lblName[2][12];
        Gtk::VBox m_VBox_ServerDataKey;
        Gtk::VBox m_VBox_ServerDataValue;
        Gtk::HBox m_HBox_ServerData;

        // system-controll
        Gtk::ButtonBox m_ButtonBox_System;
        Gtk::Button m_Button_SystemStandby;
        Gtk::Button m_Button_SystemShutdown;
        Gtk::Button m_Button_SystemReset;

        class ModelColumnsActiveApps : public Gtk::TreeModel::ColumnRecord {
            public:

                ModelColumnsActiveApps() {
                    add(m_col_id);
                    add(m_col_name);
                    add(m_col_version);
                    add(m_col_ipAddr);
                    add(m_col_port);
                    add(m_col_uptime);
                }

                Gtk::TreeModelColumn<unsigned int>  m_col_id;
                Gtk::TreeModelColumn<Glib::ustring> m_col_name;
                Gtk::TreeModelColumn<Glib::ustring> m_col_version;
                Gtk::TreeModelColumn<Glib::ustring> m_col_ipAddr;
                Gtk::TreeModelColumn<unsigned int>  m_col_port;
                Gtk::TreeModelColumn<Glib::ustring> m_col_uptime;
        };

        ModelColumnsActiveApps m_Columns_ActiveApps;

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
        void initStatus();

        moba::MsgHandlerPtr msgHandler;
        moba::SystemHandler sysHandler;

        // Signal handlers:
        bool on_timeout(int timer_number);
        void on_button_about_clicked();
        void on_button_emegency_clicked();
        void on_button_system_shutdown_clicked();
        void on_button_system_reset_clicked();
        void on_button_system_standby_clicked();
        void on_about_dialog_response(int response_id);
        void on_infobar_response(int response);

        // msg-response
        void setServerInfoRes(moba::JsonItemPtr data);
        void setConClientsRes(moba::JsonItemPtr data);
        void setSystemNotice(moba::JsonItemPtr data);
};
