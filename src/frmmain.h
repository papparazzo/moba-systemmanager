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

#include "activeapps.h"
#include "moba/endpoint.h"
#include "moba/servermessages.h"

#include "notificationlogger.h"
#include "systemcontrol.h"
#include "automaticcontrol.h"
#include "environmentcontrol.h"
#include "serverdata.h"
#include "frmbase.h"

class FrmMain final : public FrmBase {
public:
    explicit FrmMain(EndpointPtr mhp);
    ~FrmMain() noexcept override = default;

protected:
    Gtk::Notebook  m_Notebook;

    // active-apps
    ActiveApps         m_ActiveApps;
    NotificationLogger m_Notification_Logger;
    SystemControl      m_System_Control;
    AutomaticControl   m_Automatic_Control;
    ServerData         m_Server_Data;
    EnvironmentControl m_Environment_Control;

    void initActiveApps();
    void setSensitive(bool) override;
    void initialSend() override;
    void listNotice(
        const std::string &timestamp,
        const std::string &level,
        const std::string &type,
        const std::string &caption,
        const std::string &text,
        const std::string &origin,
        const std::string &source
    ) override;

    // msg-response
    void setConClientsRes(const ServerConClientsRes &data);
    void setNewClient(const ServerNewClientStarted &data);
    void setRemoveClient(const ServerClientClosed &data) const;
    void setSystemState(SystemState systemState) override;
};
