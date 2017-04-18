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
#include <moba/msgendpoint.h>
#include <moba/msgserverhandler.h>

class TreeView_ActiveApps : public Gtk::TreeView {
    public:
        TreeView_ActiveApps(moba::MsgEndpointPtr msgEndpoint);
        virtual ~TreeView_ActiveApps();

        void clearList();

        void addActiveApp(
            int id, const std::string &name, const std::string &version,
            const std::string &addr, int port, const std::string upTime
        );
        void removeActiveApp(long id);

    protected:

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

        Glib::RefPtr<Gtk::ListStore> m_refTreeModel_ActiveApps;

        Gtk::Menu m_Menu_Popup;

        moba::MsgEndpointPtr msgEndpoint;
        moba::MsgServerHandler serverHandler;

        void on_menu_popup_reset();
        void on_menu_popup_selftest();
        bool on_button_press_event(GdkEventButton* button_event) override;
        void on_menu_file_popup_generic();

};
