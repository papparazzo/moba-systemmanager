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

namespace {
    const char license[] =
        "Project:    moba-systemmanager\n"
        "\n"
        "Copyright (C) 2018 Stefan Paproth <pappi-@gmx.de>\n"
        "\n"
        "This program is free software: you can redistribute it and/or modify\n"
        "it under the terms of the GNU Affero General Public License as\n"
        "published by the Free Software Foundation, either version 3 of the\n"
        "License, or (at your option) any later version.\n"
        "\n"
        "This program is distributed in the hope that it will be useful,\n"
        "but WITHOUT ANY WARRANTY; without even the implied warranty of\n"
        "MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the\n"
        "GNU Affero General Public License for more details.\n"
        "\n"
        "You should have received a copy of the GNU Affero General Public License\n"
        "along with this program. If not, see <http://www.gnu.org/licenses/agpl.txt>.";
}

FrmMain::FrmMain(EndpointPtr mhp):
m_ActiveApps{mhp}, m_System_Control{mhp}, m_Automatic_Control{mhp},
m_Environment_Control{mhp}, msgEndpoint{mhp}
{
    sigc::slot<bool> my_slot = sigc::bind(sigc::mem_fun(*this, &FrmMain::on_timeout), 1);
    sigc::connection conn = Glib::signal_timeout().connect(my_slot, 25); // 25 ms

    //Glib::signal_timeout().connect(sigc::mem_fun(*this, &Clock::on_timeout), 250);

    set_title(PACKAGE_NAME);

    set_border_width(10);
    set_size_request(600, 400);
    set_resizable(false);
    set_position(Gtk::WIN_POS_CENTER);

    add(m_VBox);

    // Add the message label to the InfoBar:
    auto infoBarContainer = dynamic_cast<Gtk::Container*>(m_InfoBar.get_content_area());
    if(infoBarContainer) {
        infoBarContainer->add(m_Label_InfoBarMessage);
    }

    m_InfoBar.signal_response().connect(sigc::mem_fun(*this, &FrmMain::on_infobar_response));
    m_InfoBar.add_button("_OK", 0);

    m_VBox.pack_start(m_InfoBar, Gtk::PACK_SHRINK);

    m_Notebook.set_border_width(10);
    m_VBox.pack_start(m_Notebook);
    m_VBox.pack_start(m_HBox, Gtk::PACK_SHRINK);
    m_HBox.pack_end(m_ButtonBox, Gtk::PACK_SHRINK);

    m_HBox.pack_start(m_Label_Connectivity_HW, Gtk::PACK_SHRINK);
    m_Label_Connectivity_HW.set_justify(Gtk::JUSTIFY_LEFT);
    m_Label_Connectivity_HW.override_color(Gdk::RGBA("Gray"), Gtk::STATE_FLAG_NORMAL);
    m_Label_Connectivity_HW.set_tooltip_markup("<b>Status:</b> unbekannt");

    m_HBox.pack_start(m_Label_Connectivity_SW, Gtk::PACK_SHRINK);
    m_Label_Connectivity_SW.set_justify(Gtk::JUSTIFY_LEFT);
    m_Label_Connectivity_SW.override_color(Gdk::RGBA("Gray"), Gtk::STATE_FLAG_NORMAL);
    m_Label_Connectivity_SW.set_tooltip_markup("<b>Status:</b> unbekannt");

    // about-dialog
    m_ButtonBox.pack_start(m_Button_About, Gtk::PACK_EXPAND_WIDGET, 5);
    m_ButtonBox.set_layout(Gtk::BUTTONBOX_END);
    m_Button_About.signal_clicked().connect(sigc::mem_fun(*this, &FrmMain::on_button_about_clicked));

    m_ButtonBox.pack_start(m_Button_Emergency, Gtk::PACK_EXPAND_WIDGET, 5);
    m_Button_Emergency.signal_clicked().connect(sigc::mem_fun(*this, &FrmMain::on_button_emergency_clicked));

    initAboutDialog();
    initActiveApps();
    m_Notebook.append_page(m_Server_Data, "Server Info");
    m_Notebook.append_page(m_System_Control, "Systemsteuerung");
    m_Notebook.append_page(m_Automatic_Control, "Automatic Control");
    m_Notebook.append_page(m_Environment_Control, "Umgebung");
    m_Notebook.append_page(m_Notice_Logger, "Notice Logger");

    m_Button_Emergency.set_sensitive(false);

    registry.registerHandler<ServerInfoRes>(std::bind(&ServerData::setServerInfoRes, &m_Server_Data, std::placeholders::_1));
    registry.registerHandler<ServerConClientsRes>(std::bind(&FrmMain::setConClientsRes, this, std::placeholders::_1));
    registry.registerHandler<GuiSystemNotice>(std::bind(&FrmMain::setSystemNotice, this, std::placeholders::_1));
    registry.registerHandler<ClientError>(std::bind(&FrmMain::setErrorNotice, this, std::placeholders::_1));
    registry.registerHandler<ClientEchoRes>([this]{m_System_Control.setPingResult();});
    registry.registerHandler<ServerNewClientStarted>(std::bind(&FrmMain::setNewClient, this, std::placeholders::_1));
    registry.registerHandler<SystemHardwareStateChanged>(std::bind(&FrmMain::setHardwareState, this, std::placeholders::_1));
    registry.registerHandler<ServerClientClosed>(std::bind(&FrmMain::setRemoveClient, this, std::placeholders::_1));
    registry.registerHandler<TimerGlobalTimerEvent>(std::bind(&AutomaticControl::setTimerGlobalTimerEvent, &m_Automatic_Control, std::placeholders::_1));
    registry.registerHandler<TimerSetGlobalTimer>(std::bind(&AutomaticControl::setTimerSetGlobalTimer, &m_Automatic_Control, std::placeholders::_1));

    show_all_children();
    m_InfoBar.hide();
}

void FrmMain::initAboutDialog() {
    m_Dialog.set_transient_for(*this);

    m_Dialog.set_program_name(PACKAGE_NAME);
    m_Dialog.set_version(PACKAGE_VERSION);
    m_Dialog.set_copyright("Stefan Paproth");
    m_Dialog.set_comments("Application for controlling the moba-server.");
    m_Dialog.set_license(license);

    m_Dialog.set_website("<pappi-@gmx.de>");
    m_Dialog.set_website_label("pappi-@gmx.de");

    m_Dialog.set_logo(Gdk::Pixbuf::create_from_file("/usr/local/share/icons/hicolor/scalable/apps/moba-systemmanager.svg"));

    std::vector<Glib::ustring> list_authors;
    list_authors.push_back("Stefan Paproth");
    m_Dialog.set_authors(list_authors);

    m_Dialog.signal_response().connect(sigc::mem_fun(*this, &FrmMain::on_about_dialog_response));

    m_Button_About.grab_focus();
}

void FrmMain::initActiveApps() {
    m_Notebook.append_page(m_ScrolledWindow, "Active Apps");

    m_ScrolledWindow.add(m_ActiveApps);
    m_ScrolledWindow.set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);
}

////////////////////////////////////////////////////////////////////////////////
// <editor-fold defaultstate="collapsed" desc="call-back-methodes">
void FrmMain::on_button_about_clicked() {
    m_Dialog.show();
    m_Dialog.present();
}

void FrmMain::on_button_emergency_clicked() {
    if(m_Button_Emergency.get_label() == "Nothalt") {
        msgEndpoint->sendMsg(SystemTriggerEmergencyStop{});
    } else {
        msgEndpoint->sendMsg(SystemReleaseEmergencyStop{});
    }
}

void FrmMain::on_about_dialog_response(int) {
    m_Dialog.hide();
}

bool FrmMain::on_timeout(int) {
    static bool connected = false;

    try {
        if(!connected) {
            msgEndpoint->connect();
            m_Label_Connectivity_HW.override_color(Gdk::RGBA("red"), Gtk::STATE_FLAG_NORMAL);
            m_Label_Connectivity_HW.set_tooltip_markup("<b>Status:</b> Keine Verbindung zur Hardware");
            m_Label_Connectivity_SW.override_color(Gdk::RGBA("red"), Gtk::STATE_FLAG_NORMAL);
            m_Label_Connectivity_SW.set_tooltip_markup("<b>Status:</b> Keine Verbindung zur Hardware");
            m_Automatic_Control.enable();
            m_System_Control.enable();
            m_Environment_Control.enable();
            msgEndpoint->sendMsg(ServerInfoReq{});
            msgEndpoint->sendMsg(ServerConClientsReq{});
            msgEndpoint->sendMsg(SystemGetHardwareState{});
            msgEndpoint->sendMsg(TimerGetGlobalTimer{});
            connected = true;
            return true;
        }
        registry.handleMsg(msgEndpoint->recieveMsg());

    } catch(std::exception &e) {
        if(connected) {
            m_Automatic_Control.disable();
            m_System_Control.disable();
            m_Environment_Control.disable();
            m_Button_Emergency.set_sensitive(false);
            m_Label_Connectivity_HW.override_color(Gdk::RGBA("gray"), Gtk::STATE_FLAG_NORMAL);
            m_Label_Connectivity_HW.set_tooltip_markup("<b>Status:</b> Keine Verbindung zum Server");
            m_Label_Connectivity_SW.override_color(Gdk::RGBA("gray"), Gtk::STATE_FLAG_NORMAL);
            m_Label_Connectivity_SW.set_tooltip_markup("<b>Status:</b> Keine Verbindung zum Server");
            m_InfoBar.set_message_type(Gtk::MESSAGE_ERROR);
            std::stringstream ss;
            ss << "<b>msg-handler exception:</b>\n" << e.what();
            m_Label_InfoBarMessage.set_markup(ss.str());
            m_InfoBar.show();
            connected = false;
        }
    }
    return true;
}

void FrmMain::on_infobar_response(int) {
    m_Label_InfoBarMessage.set_text("");
    m_InfoBar.hide();
}

// </editor-fold>

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

void FrmMain::setNotice(Gtk::MessageType noticeType, std::string caption, std::string text) {
    m_Notice_Logger.setNotice(noticeType, caption, text);

    std::replace(caption.begin(), caption.end(), '<', '"');
    std::replace(caption.begin(), caption.end(), '>', '"');
    std::replace(text.begin(), text.end(), '<', '"');
    std::replace(text.begin(), text.end(), '>', '"');

    std::stringstream ss;
    ss << "<b>" << caption << "!</b>\n" << text;

    m_Label_InfoBarMessage.set_markup(ss.str());
    m_InfoBar.set_message_type(noticeType);
    m_InfoBar.show();
}

void FrmMain::setErrorNotice(const ClientError &data) {
    setNotice(Gtk::MESSAGE_ERROR, data.errorId, data.additionalMsg);
}

void FrmMain::setSystemNotice(const GuiSystemNotice &data) {
    Gtk::MessageType mt;
    switch(data.noticeType) {
        case GuiSystemNotice::NoticeType::ERROR:
            mt = Gtk::MESSAGE_ERROR;
            break;

        case GuiSystemNotice::NoticeType::WARNING:
            mt = Gtk::MESSAGE_WARNING;
            break;

        case GuiSystemNotice::NoticeType::INFO:
        default:
            mt = Gtk::MESSAGE_INFO;
            break;
    }
    setNotice(mt, data.caption, data.text);
}

void FrmMain::setHardwareState(const SystemHardwareStateChanged &data) {
    m_System_Control.setHardwareState(data.hardwareState);
    m_Automatic_Control.setHardwareState(data.hardwareState);

    if(data.hardwareState == SystemHardwareStateChanged::HardwareState::ERROR) {
        m_Label_Connectivity_HW.override_color(Gdk::RGBA("red"), Gtk::STATE_FLAG_NORMAL);
        m_Label_Connectivity_SW.override_color(Gdk::RGBA("red"), Gtk::STATE_FLAG_NORMAL);
        m_Label_Connectivity_HW.set_tooltip_markup("<b>Status:</b> Keine Verbindung zur Hardware");
        m_Label_Connectivity_SW.set_tooltip_markup("<b>Status:</b> Keine Verbindung zur Hardware");
        m_Button_Emergency.set_sensitive(false);
        return;
    }
    m_Button_Emergency.set_sensitive(true);
    if(data.hardwareState == SystemHardwareStateChanged::HardwareState::EMERGENCY_STOP) {
        m_Label_Connectivity_HW.override_color(Gdk::RGBA("red"), Gtk::STATE_FLAG_NORMAL);
        m_Label_Connectivity_SW.override_color(Gdk::RGBA("gold"), Gtk::STATE_FLAG_NORMAL);
        m_Label_Connectivity_HW.set_tooltip_markup("<b>Status:</b> Nohalt ausgelöst");
        m_Label_Connectivity_SW.set_tooltip_markup("<b>Status:</b> Nohalt ausgelöst");
        m_Button_Emergency.set_label("Freigabe");
        return;
    }
    m_Button_Emergency.set_label("Nothalt");
    if(data.hardwareState == SystemHardwareStateChanged::HardwareState::STANDBY) {
        m_Label_Connectivity_HW.override_color(Gdk::RGBA("gold"), Gtk::STATE_FLAG_NORMAL);
        m_Label_Connectivity_SW.override_color(Gdk::RGBA("gold"), Gtk::STATE_FLAG_NORMAL);
        m_Label_Connectivity_HW.set_tooltip_markup("<b>Status:</b> Energiesparmodus");
        m_Label_Connectivity_SW.set_tooltip_markup("<b>Status:</b> Energiesparmodus");
        m_Button_Emergency.set_sensitive(false);
        return;
    }

    if(data.hardwareState == SystemHardwareStateChanged::HardwareState::MANUEL) {
        m_Label_Connectivity_HW.override_color(Gdk::RGBA("green"), Gtk::STATE_FLAG_NORMAL);
        m_Label_Connectivity_SW.override_color(Gdk::RGBA("gold"), Gtk::STATE_FLAG_NORMAL);
        m_Label_Connectivity_HW.set_tooltip_markup("<b>Status:</b> manuell");
        m_Label_Connectivity_SW.set_tooltip_markup("<b>Status:</b> manuell");
        return;
    }
    if(data.hardwareState == SystemHardwareStateChanged::HardwareState::AUTOMATIC) {
        m_Label_Connectivity_HW.override_color(Gdk::RGBA("green"), Gtk::STATE_FLAG_NORMAL);
        m_Label_Connectivity_SW.override_color(Gdk::RGBA("green"), Gtk::STATE_FLAG_NORMAL);
        m_Label_Connectivity_HW.set_tooltip_markup("<b>Status:</b> automatisch");
        m_Label_Connectivity_SW.set_tooltip_markup("<b>Status:</b> automatisch");
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

// </editor-fold>
