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

class ActiveApps final: public Gtk::Box {
public:
    explicit ActiveApps(EndpointPtr msgEndpoint);
    ~ActiveApps() noexcept override = default;

    void clearList() const;

    void addActiveApp(
        long id,
        const std::string &name,
        const std::string &description,
        const std::string &version,
        const std::string &addr,
        long port,
        const std::string &startTime
    ) const;
    void removeActiveApp(long id) const;

private:
    struct ModelColumnsActiveApps final : Gtk::TreeModel::ColumnRecord {
        ModelColumnsActiveApps() {
            add(m_col_id);
            add(m_col_name);
            add(m_col_description);
            add(m_col_version);
            add(m_col_ipAddr);
            add(m_col_port);
            add(m_col_startTime);
        }

        Gtk::TreeModelColumn<unsigned int>  m_col_id;
        Gtk::TreeModelColumn<Glib::ustring> m_col_name;
        Gtk::TreeModelColumn<Glib::ustring> m_col_description;
        Gtk::TreeModelColumn<Glib::ustring> m_col_version;
        Gtk::TreeModelColumn<Glib::ustring> m_col_ipAddr;
        Gtk::TreeModelColumn<unsigned int>  m_col_port;
        Gtk::TreeModelColumn<Glib::ustring> m_col_startTime;
    };

    ModelColumnsActiveApps m_Columns_ActiveApps;

    EndpointPtr msgEndpoint;

    Glib::RefPtr<Gtk::ListStore> m_refTreeModel_ActiveApps;

    Gtk::ScrolledWindow          m_ScrolledWindow;
    Gtk::TreeView                m_TreeView_ActiveApps;

    Gtk::PopoverMenu m_Menu_Popup;

    void on_menu_popup_reset();
    void on_menu_popup_reset_hardware();
    void on_menu_popup_selftest();
    void on_popup_button_pressed(int n_press, double x, double y);
};
