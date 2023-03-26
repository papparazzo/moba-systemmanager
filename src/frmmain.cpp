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

#include <iostream>
#include <exception>
#include <algorithm>
#include <functional>
#include <string>

#include <cassert>
#include <ctime>

#include "frmmain.h"
#include "config.h"

#include "moba/environmentmessages.h"

FrmMain::FrmMain(EndpointPtr mhp): FrmBase{mhp}, m_ActiveApps{mhp}, m_System_Control{mhp}, m_Automatic_Control{mhp},
m_Environment_Control{mhp} {
    
    set_size_request(675, 450);
    set_resizable(false);

    initActiveApps();
    finishForm();

    registry.registerHandler<ServerInfoRes>(std::bind(&ServerData::setServerInfoRes, &m_Server_Data, std::placeholders::_1));
    registry.registerHandler<ServerConClientsRes>(std::bind(&FrmMain::setConClientsRes, this, std::placeholders::_1));
    registry.registerHandler<ClientEchoRes>([this]{m_System_Control.setPingResult();});
    registry.registerHandler<ServerNewClientStarted>(std::bind(&FrmMain::setNewClient, this, std::placeholders::_1));
    registry.registerHandler<ServerClientClosed>(std::bind(&FrmMain::setRemoveClient, this, std::placeholders::_1));
    registry.registerHandler<TimerGlobalTimerEvent>(std::bind(&AutomaticControl::setTimerGlobalTimerEvent, &m_Automatic_Control, std::placeholders::_1));
    registry.registerHandler<TimerSetGlobalTimer>(std::bind(&AutomaticControl::setTimerSetGlobalTimer, &m_Automatic_Control, std::placeholders::_1));
}

void FrmMain::initActiveApps() {
    m_VBox.pack_start(m_Notebook);

    m_Notebook.set_border_width(10);

    m_Notebook.append_page(m_ScrolledWindow, "Active Apps");
    m_Notebook.append_page(m_Server_Data, "Server Info");
    m_Notebook.append_page(m_System_Control, "Systemsteuerung");
    m_Notebook.append_page(m_Automatic_Control, "Automatic Control");
    m_Notebook.append_page(m_Environment_Control, "Umgebung");
    m_Notebook.append_page(m_Notice_Logger, "Notice Logger");

    m_ScrolledWindow.add(m_ActiveApps);
    m_ScrolledWindow.set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);
}

void FrmMain::setSensitive(bool sensitive) {
    if(sensitive) {
        m_Automatic_Control.enable();
        m_System_Control.enable();
        m_Environment_Control.enable();
    } else {
        m_Automatic_Control.disable();
        m_System_Control.disable();
        m_Environment_Control.disable();
        m_ActiveApps.clearList();
        m_Server_Data.clear();
    }
}

void FrmMain::listNotice(Gtk::MessageType noticeType, std::string caption, std::string text) {
    m_Notice_Logger.setNotice(noticeType, caption, text);
}

void FrmMain::initialSend() {
    msgEndpoint->sendMsg(ServerInfoReq{});
    msgEndpoint->sendMsg(ServerConClientsReq{});
    msgEndpoint->sendMsg(SystemGetHardwareState{});
    msgEndpoint->sendMsg(TimerGetGlobalTimer{});
}

////////////////////////////////////////////////////////////////////////////////
// <editor-fold defaultstate="collapsed" desc="msg-response">
void FrmMain::setConClientsRes(const ServerConClientsRes &data) {
    m_ActiveApps.clearList();

    for(auto iter : data.endpoints) {
        m_ActiveApps.addActiveApp(
            iter.appId, iter.appInfo.appName, iter.appInfo.version.getString(), iter.addr, iter.port, iter.startTime
        );
    }
}

void FrmMain::setNewClient(const ServerNewClientStarted &data) {
    m_ActiveApps.addActiveApp(
        data.endpoint.appId, data.endpoint.appInfo.appName, data.endpoint.appInfo.version.getString(),
        data.endpoint.addr, data.endpoint.port, data.endpoint.startTime
    );
}

void FrmMain::setRemoveClient(const ServerClientClosed &data) {
    m_ActiveApps.removeActiveApp(data.clientId);
}

void FrmMain::setSystemState(SystemState systemState) {
    m_System_Control.setHardwareState(systemState);
    m_Automatic_Control.setHardwareState(systemState);
}

// </editor-fold>
