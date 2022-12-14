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

ActiveApps::ActiveApps(EndpointPtr msgEndpoint) : msgEndpoint(msgEndpoint) {
    m_refTreeModel_ActiveApps = Gtk::ListStore::create(m_Columns_ActiveApps);
    set_model(m_refTreeModel_ActiveApps);

    append_column("ID",         m_Columns_ActiveApps.m_col_id);
    append_column("Name",       m_Columns_ActiveApps.m_col_name);
    append_column("Version",    m_Columns_ActiveApps.m_col_version);
    append_column("IP-Adresse", m_Columns_ActiveApps.m_col_ipAddr);
    append_column("Port",       m_Columns_ActiveApps.m_col_port);
    append_column("Start-Time", m_Columns_ActiveApps.m_col_startTime);

    auto item = Gtk::manage(new Gtk::MenuItem("_Selftest", true));

    item->signal_activate().connect(sigc::mem_fun(*this, &ActiveApps::on_menu_popup_selftest));
    m_Menu_Popup.append(*item);

    item = Gtk::manage(new Gtk::MenuItem("_Reset", true));
    item->signal_activate().connect(sigc::mem_fun(*this, &ActiveApps::on_menu_popup_reset));
    m_Menu_Popup.append(*item);

    m_Menu_Popup.accelerate(*this);
    m_Menu_Popup.show_all();
}

ActiveApps::~ActiveApps() {
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
    int id, const std::string &name, const std::string &version,
    const std::string &addr, int port, const std::string startTime
) {
    Gtk::TreeModel::Row row;
    row = *(m_refTreeModel_ActiveApps->append());
    row[m_Columns_ActiveApps.m_col_id       ] = id;
    row[m_Columns_ActiveApps.m_col_name     ] = name;
    row[m_Columns_ActiveApps.m_col_version  ] = version;
    row[m_Columns_ActiveApps.m_col_ipAddr   ] = addr;
    row[m_Columns_ActiveApps.m_col_port     ] = port;
    row[m_Columns_ActiveApps.m_col_startTime] = startTime;
}

bool ActiveApps::on_button_press_event(GdkEventButton *button_event) {
    bool return_value = Gtk::TreeView::on_button_press_event(button_event);

    if((button_event->type == GDK_BUTTON_PRESS) && (button_event->button == 3)) {
        m_Menu_Popup.popup(button_event->button, button_event->time);
    }

    return return_value;
}

void ActiveApps::on_menu_popup_reset() {
    auto refSelection = get_selection();
    if(!refSelection) {
        return;
    }
    Gtk::TreeModel::iterator iter = refSelection->get_selected();
    if(!iter) {
        return;
    }
    long id = (*iter)[m_Columns_ActiveApps.m_col_id];
    msgEndpoint->sendMsg(ServerResetClient{id});
}

void ActiveApps::on_menu_popup_selftest() {
    auto refSelection = get_selection();
    if(!refSelection) {
        return;
    }
    Gtk::TreeModel::iterator iter = refSelection->get_selected();
    if(!iter) {
        return;
    }
    long id = (*iter)[m_Columns_ActiveApps.m_col_id];
    msgEndpoint->sendMsg(ServerSelfTestingClient{id});
}
