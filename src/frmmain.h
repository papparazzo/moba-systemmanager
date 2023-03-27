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
#include <gtkmm/window.h>
#include <gtkmm/comboboxtext.h>
#include <gtkmm/liststore.h>

#include "moba/endpoint.h"
#include "moba/registry.h"
#include "moba/servermessages.h"
#include "moba/guimessages.h"
#include "moba/clientmessages.h"
#include "moba/systemmessages.h"
#include "moba/timermessages.h"

#include "activeapps.h"
#include "noticelogger.h"
#include "systemcontrol.h"
#include "automaticcontrol.h"
#include "environmentcontrol.h"
#include "serverdata.h"
#include "frmbase.h"

class FrmMain: public FrmBase {
public:
    FrmMain(EndpointPtr mhp);
    virtual ~FrmMain() {
    }

protected:
    Gtk::Notebook  m_Notebook;

    // active-apps
    Gtk::ScrolledWindow m_ScrolledWindow;
    ActiveApps          m_ActiveApps;

    NoticeLogger       m_Notice_Logger;
    SystemControl      m_System_Control;
    AutomaticControl   m_Automatic_Control;
    ServerData         m_Server_Data;
    EnvironmentControl m_Environment_Control;

    void initActiveApps();
    void setSensitive(bool);
    void initialSend();
    void listNotice(Gtk::MessageType noticeType, std::string caption, std::string text);
    void registerAdditionalHandler();

    // msg-response
    void setServerInfoRes(const ServerInfoRes &data);
    void setConClientsRes(const ServerConClientsRes &data);
    void setNewClient(const ServerNewClientStarted &data);
    void setRemoveClient(const ServerClientClosed &data);
    void setSystemState(SystemState systemState);
};
