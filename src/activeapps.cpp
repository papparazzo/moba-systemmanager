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

#include "activeapps.h"
#include "moba/servermessages.h"

ActiveApps::ActiveApps(EndpointPtr msgEndpoint): msgEndpoint(msgEndpoint) {
    append(m_ScrolledWindow);

    m_ScrolledWindow.set_child(m_TreeView_ActiveApps);
    m_ScrolledWindow.set_policy(Gtk::PolicyType::AUTOMATIC, Gtk::PolicyType::AUTOMATIC);
    m_ScrolledWindow.set_expand();

    m_refTreeModel_ActiveApps = Gtk::ListStore::create(m_Columns_ActiveApps);
    m_TreeView_ActiveApps.set_model(m_refTreeModel_ActiveApps);

    m_TreeView_ActiveApps.append_column("Id",           m_Columns_ActiveApps.m_col_id);
    m_TreeView_ActiveApps.append_column("Name",         m_Columns_ActiveApps.m_col_name);
    m_TreeView_ActiveApps.append_column("Beschreibung", m_Columns_ActiveApps.m_col_description);
    m_TreeView_ActiveApps.append_column("Version",      m_Columns_ActiveApps.m_col_version);
    m_TreeView_ActiveApps.append_column("IP-Adresse",   m_Columns_ActiveApps.m_col_ipAddr);
    m_TreeView_ActiveApps.append_column("Port",         m_Columns_ActiveApps.m_col_port);
    m_TreeView_ActiveApps.append_column("Start-Time",   m_Columns_ActiveApps.m_col_startTime);

    auto refGesture = Gtk::GestureClick::create();
    refGesture->set_button(GDK_BUTTON_SECONDARY);
    refGesture->signal_pressed().connect(sigc::mem_fun(*this, &ActiveApps::on_popup_button_pressed));
    add_controller(refGesture);

    // Fill popup menu:
    auto gmenu = Gio::Menu::create();
    gmenu->append("_Selftest", "popup.selftest");
    gmenu->append("_Reset", "popup.reset");

    m_Menu_Popup.set_parent(*this);
    m_Menu_Popup.set_menu_model(gmenu);
    m_Menu_Popup.set_has_arrow(false);

    // Create actions:
    auto refActionGroup = Gio::SimpleActionGroup::create();

    refActionGroup->add_action("selftest", sigc::mem_fun(*this, &ActiveApps::on_menu_popup_selftest));
    refActionGroup->add_action("reset", sigc::mem_fun(*this, &ActiveApps::on_menu_popup_reset));

    insert_action_group("popup", refActionGroup);
}

void ActiveApps::removeActiveApp(long id) {
    Gtk::TreeModel::Children children = m_refTreeModel_ActiveApps->children();

    for(auto iter = children.begin(); iter != children.end(); ++iter) {
        Gtk::TreeModel::Row row = *iter;
        if(row[m_Columns_ActiveApps.m_col_id] == id) {
            m_refTreeModel_ActiveApps->erase(iter);
            return;
        }
    }
}

void ActiveApps::clearList() {
    m_refTreeModel_ActiveApps->clear();
}

void ActiveApps::addActiveApp(
    const long id, const std::string &name, const std::string &description, const std::string &version,
    const std::string &addr, const long port, const std::string &startTime
) const {
    Gtk::TreeModel::Row row = *m_refTreeModel_ActiveApps->append();
    row[m_Columns_ActiveApps.m_col_id         ] = id;
    row[m_Columns_ActiveApps.m_col_name       ] = name;
    row[m_Columns_ActiveApps.m_col_description] = description;
    row[m_Columns_ActiveApps.m_col_version    ] = version;
    row[m_Columns_ActiveApps.m_col_ipAddr     ] = addr;
    row[m_Columns_ActiveApps.m_col_port       ] = port;
    row[m_Columns_ActiveApps.m_col_startTime  ] = startTime;
}

void ActiveApps::on_popup_button_pressed(int n_press, double x, double y) {
    const Gdk::Rectangle rect(x, y, 1, 1);
    m_Menu_Popup.set_pointing_to(rect);
    m_Menu_Popup.popup();
}

void ActiveApps::on_menu_popup_reset() {

    auto refSelection = m_TreeView_ActiveApps.get_selection();
    if(!refSelection) {
        return;
    }
    auto iter = refSelection->get_selected();
    if(!iter) {
        return;
    }
    long id = (*iter)[m_Columns_ActiveApps.m_col_id];
    msgEndpoint->sendMsg(ServerResetClient{id});
}

void ActiveApps::on_menu_popup_selftest() {
    auto refSelection = m_TreeView_ActiveApps.get_selection();
    if(!refSelection) {
        return;
    }
    auto iter = refSelection->get_selected();
    if(!iter) {
        return;
    }
    long id = (*iter)[m_Columns_ActiveApps.m_col_id];
    msgEndpoint->sendMsg(ServerSelfTestingClient{id});
}
