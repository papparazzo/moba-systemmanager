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

#include "serverdata.h"

ServerData::ServerData() {
    set_border_width(2);

    add(m_VBox_ServerDataKey);
    add(m_VBox_ServerDataValue);

    for(int i = 0; i < 11; ++i) {
        m_VBox_ServerDataKey.pack_start(lblName[0][i]);
        m_VBox_ServerDataValue.pack_start(lblName[1][i]);
        lblName[0][i].set_justify(Gtk::JUSTIFY_RIGHT);
        lblName[0][i].set_justify(Gtk::JUSTIFY_LEFT);
    }

    lblName[0][0].set_markup("<b>AppName:</b>");
    lblName[0][1].set_markup("<b>Version:</b>");
    lblName[0][2].set_markup("<b>Build-Date:</b>");
    lblName[0][3].set_markup("<b>Start-Time:</b>");
    lblName[0][4].set_markup("<b>max. Clients:</b>");
    lblName[0][5].set_markup("<b>connected Clients:</b>");
    lblName[0][6].set_markup("<b>osArch:</b>");
    lblName[0][7].set_markup("<b>osName:</b>");
    lblName[0][8].set_markup("<b>osVersion:</b>");
    lblName[0][9].set_markup("<b>fwType:</b>");
    lblName[0][10].set_markup("<b>fwVersion:</b>");
}

ServerData::~ServerData() {
}

void ServerData::setServerInfoRes(const ServerInfoRes &data) {
    lblName[1][0].set_label(data.appName);
    lblName[1][1].set_label(data.version.getString());
    lblName[1][2].set_label(data.buildDate);
    lblName[1][3].set_label(data.startTime);
    lblName[1][4].set_label(std::to_string(data.maxClients));
    lblName[1][5].set_label(std::to_string(data.connectedClients));
    lblName[1][6].set_label(data.osArch);
    lblName[1][7].set_label(data.osName);
    lblName[1][8].set_label(data.osVersion);
    lblName[1][9].set_label(data.fwType);
    lblName[1][10].set_label(data.fwVersion);
}

void ServerData::clear() {
   for(auto i = 0; i <= 10; ++i) {
       lblName[1][i].set_label("");
   }
}