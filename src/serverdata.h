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
#include "moba/servermessages.h"

class ServerData: public Gtk::HBox {
public:
    ServerData();
    ServerData(const ServerData&) = delete;
    ServerData& operator=(const ServerData&) = delete;

    virtual ~ServerData();

    void setServerInfoRes(const ServerInfoRes &data);

    void clear();

private:
    // server-data
    Gtk::Label lblName[2][11];
    Gtk::VBox  m_VBox_ServerDataKey{Gtk::ORIENTATION_VERTICAL, 6};
    Gtk::VBox  m_VBox_ServerDataValue{Gtk::ORIENTATION_VERTICAL, 6};
};

