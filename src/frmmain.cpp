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
#include <sys/timeb.h>

#include "frmmain.h"
#include "config.h"

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

FrmMain::FrmMain(EndpointPtr mhp) : m_ActiveApps{mhp}, msgEndpoint{mhp} {
    sigc::slot<bool> my_slot = sigc::bind(sigc::mem_fun(*this, &FrmMain::on_timeout), 1);
    sigc::connection conn = Glib::signal_timeout().connect(my_slot, 25); // 25 ms

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
    initServerData();
    initSystemControl();
    initAutomaticController();

    m_Notebook.append_page(m_Notice_Logger, "Notice Logger");

    m_Button_Emergency.set_sensitive(false);

    registry.registerHandler<ServerInfoRes>(std::bind(&FrmMain::setServerInfoRes, this, std::placeholders::_1));
    registry.registerHandler<ServerConClientsRes>(std::bind(&FrmMain::setConClientsRes, this, std::placeholders::_1));
    registry.registerHandler<GuiSystemNotice>(std::bind(&FrmMain::setSystemNotice, this, std::placeholders::_1));
    registry.registerHandler<ClientError>(std::bind(&FrmMain::setErrorNotice, this, std::placeholders::_1));
    registry.registerHandler<ClientEchoRes>(std::bind(&FrmMain::setPingResult, this, std::placeholders::_1));
    registry.registerHandler<ServerNewClientStarted>(std::bind(&FrmMain::setNewClient, this, std::placeholders::_1));
    registry.registerHandler<SystemHardwareStateChanged>(std::bind(&FrmMain::setHardwareState, this, std::placeholders::_1));
    registry.registerHandler<ServerClientClosed>(std::bind(&FrmMain::setRemoveClient, this, std::placeholders::_1));
    registry.registerHandler<TimerGlobalTimerEvent>(std::bind(&FrmMain::setTimerGlobalTimerEvent, this, std::placeholders::_1));
    registry.registerHandler<TimerSetGlobalTimer>(std::bind(&FrmMain::setTimerSetGlobalTimer, this, std::placeholders::_1));

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

void FrmMain::initServerData() {
    m_Notebook.append_page(m_HBox_ServerData, "Server Data");

    m_HBox_ServerData.set_border_width(2);

    m_HBox_ServerData.add(m_VBox_ServerDataKey);
    m_HBox_ServerData.add(m_VBox_ServerDataValue);

    for(int i = 0; i < 11; ++i) {
        m_VBox_ServerDataKey.pack_start(lblName[0][i]);
        m_VBox_ServerDataValue.pack_start(lblName[1][i]);
        lblName[0][i].set_justify(Gtk::JUSTIFY_RIGHT);
        lblName[0][i].set_justify(Gtk::JUSTIFY_LEFT);
    }
}

void FrmMain::initSystemControl() {
    m_Notebook.append_page(m_VBox_SystemControl, "System Control");

    m_Label_HardwareState.set_markup("<b>Hardwarestatus:</b> [unbekannt]");
    m_Label_PingResult[0].set_markup("<b>Ping 1:</b> [unbekannt]");
    m_Label_PingResult[1].set_markup("<b>Ping 2:</b> [unbekannt]");
    m_Label_PingResult[2].set_markup("<b>Ping 3:</b> [unbekannt]");
    m_Label_PingResult[3].set_markup("<b>Ping 4:</b> [unbekannt]");

    m_VBox_SystemControl.pack_start(m_Label_HardwareState, Gtk::PACK_SHRINK, 50);
    m_VBox_SystemControl.pack_start(m_Label_PingResult[0], Gtk::PACK_SHRINK);
    m_VBox_SystemControl.pack_start(m_Label_PingResult[1], Gtk::PACK_SHRINK);
    m_VBox_SystemControl.pack_start(m_Label_PingResult[2], Gtk::PACK_SHRINK);
    m_VBox_SystemControl.pack_start(m_Label_PingResult[3], Gtk::PACK_SHRINK);
    m_VBox_SystemControl.pack_end(m_ButtonBox_System, Gtk::PACK_SHRINK);

    m_ButtonBox_System.pack_start(m_Button_SystemShutdown);
    m_ButtonBox_System.pack_start(m_Button_SystemReset);
    m_ButtonBox_System.pack_start(m_Button_SystemStandby);
    m_ButtonBox_System.pack_start(m_Button_SystemPing);
    m_ButtonBox_System.set_layout(Gtk::BUTTONBOX_CENTER);
    m_ButtonBox_System.set_sensitive(false);

    m_Button_SystemShutdown.signal_clicked().connect(sigc::mem_fun(*this, &FrmMain::on_button_system_shutdown_clicked));
    m_Button_SystemReset.signal_clicked().connect(sigc::mem_fun(*this, &FrmMain::on_button_system_reset_clicked));
    m_Button_SystemStandby.signal_clicked().connect(sigc::mem_fun(*this, &FrmMain::on_button_system_standby_clicked));
    m_Button_SystemPing.signal_clicked().connect(sigc::mem_fun(*this, &FrmMain::on_button_system_ping_clicked));
}

void FrmMain::initAutomaticController() {
    m_Notebook.append_page(m_HBox_AutomaticControl, "Automatic Control");

    m_refListModel_CurModelDay = Gtk::ListStore::create(m_Columns_CurModelDay);
    m_Combo_CurModelDay.set_model(m_refListModel_CurModelDay);

    auto row = *(m_refListModel_CurModelDay->append());
    row[m_Columns_CurModelDay.m_col_id] = Day::MONDAY;
    row[m_Columns_CurModelDay.m_col_name] = "Montag";

    row = *(m_refListModel_CurModelDay->append());
    row[m_Columns_CurModelDay.m_col_id] = Day::TUESDAY;
    row[m_Columns_CurModelDay.m_col_name] = "Dienstag";

    row = *(m_refListModel_CurModelDay->append());
    row[m_Columns_CurModelDay.m_col_id] = Day::WEDNESDAY;
    row[m_Columns_CurModelDay.m_col_name] = "Mittwoch";

    row = *(m_refListModel_CurModelDay->append());
    row[m_Columns_CurModelDay.m_col_id] = Day::THURSDAY;
    row[m_Columns_CurModelDay.m_col_name] = "Donnerstag";

    row = *(m_refListModel_CurModelDay->append());
    row[m_Columns_CurModelDay.m_col_id] = Day::FRIDAY;
    row[m_Columns_CurModelDay.m_col_name] = "Freitag";

    row = *(m_refListModel_CurModelDay->append());
    row[m_Columns_CurModelDay.m_col_id] = Day::SATURDAY;
    row[m_Columns_CurModelDay.m_col_name] = "Samstag";

    row = *(m_refListModel_CurModelDay->append());
    row[m_Columns_CurModelDay.m_col_id] = Day::SUNDAY;
    row[m_Columns_CurModelDay.m_col_name] = "Sonntag";

    m_Combo_CurModelDay.pack_start(m_Columns_CurModelDay.m_col_name);

    m_refListModel_Multiplicator = Gtk::ListStore::create(m_Columns_Multiplicator);
    m_Combo_Multiplicator.set_model(m_refListModel_Multiplicator);

    for(unsigned int i = 60; i <= 240; i += 30) {
        std::stringstream ss;

        ss << "1min -> " << static_cast<float>(i) / 60.0 << "h";

        auto row1 = *(m_refListModel_Multiplicator->append());
        row1[m_Columns_Multiplicator.m_col_factor] = i;
        row1[m_Columns_Multiplicator.m_col_label] = ss.str();
    }

    m_Combo_Multiplicator.pack_start(m_Columns_Multiplicator.m_col_label);

    m_HBox_AutomaticControl.set_homogeneous(true);
    m_HBox_AutomaticControl.add(m_VBox_AutomaticControl);
    m_HBox_AutomaticControl.add(m_HBox_AutomaticControl_Clock);

    m_HBox_AutomaticControl_Clock.pack_start(m_Clock);
    m_HBox_AutomaticControl_Clock.pack_start(m_Label_Date, Gtk::PACK_SHRINK, 10);

    m_VBox_AutomaticControl.pack_start(m_HBox_CurModelDay, Gtk::PACK_SHRINK);
    m_VBox_AutomaticControl.pack_start(m_HBox_CurModelTime, Gtk::PACK_SHRINK);
    m_VBox_AutomaticControl.pack_start(m_HBox_Multiplicator, Gtk::PACK_SHRINK);
    m_VBox_AutomaticControl.pack_end(m_ButtonBox_AutomaticControl, Gtk::PACK_SHRINK, 15);

    m_ButtonBox_AutomaticControl.pack_start(m_Button_AutomaticControl_Enable);
    m_ButtonBox_AutomaticControl.pack_start(m_Button_AutomaticControl_Set, Gtk::PACK_EXPAND_WIDGET, 5);
    m_ButtonBox_AutomaticControl.set_layout(Gtk::BUTTONBOX_END);

    m_HBox_CurModelDay.pack_start(m_Label_CurModelDay, Gtk::PACK_SHRINK, 5);
    m_HBox_CurModelDay.pack_end(m_Combo_CurModelDay, Gtk::PACK_SHRINK, 5);

    m_HBox_CurModelTime.pack_start(m_Label_CurModelTime, Gtk::PACK_SHRINK, 5);
    m_HBox_CurModelTime.pack_end(m_Entry_CurModelTime, Gtk::PACK_SHRINK, 5);

    m_HBox_Multiplicator.pack_start(m_Label_Multiplicator, Gtk::PACK_SHRINK, 5);
    m_HBox_Multiplicator.pack_end(m_Combo_Multiplicator, Gtk::PACK_SHRINK, 5);

    m_ButtonBox_AutomaticControl.set_sensitive(false);

    m_Button_AutomaticControl_Set.signal_clicked().connect(sigc::mem_fun(*this, &FrmMain::on_button_time_control_set_clicked));
    m_Button_AutomaticControl_Enable.signal_clicked().connect(sigc::mem_fun(*this, &FrmMain::on_button_system_automatic_clicked));
}

void FrmMain::setHardwareStateLabel(const std::string &status) {
    std::stringstream ss;
    ss << "<b>Hardwarestatus:</b> " << status;
    m_Label_HardwareState.set_markup(ss.str());
}

void FrmMain::setClock(Day day, unsigned int hours) {

    std::stringstream ss;
    ss << "<b>";

    switch(day) {
        case Day::MONDAY:
            ss << "Montag";
            break;

        case Day::TUESDAY:
            ss << "Dienstag";
            break;

        case Day::WEDNESDAY:
            ss << "Mittwoch";
            break;

        case Day::THURSDAY:
            ss << "Donnerstag";
            break;

        case Day::FRIDAY:
            ss << "Freitag";
            break;

        case Day::SATURDAY:
            ss << "Samstag";
            break;

        case Day::SUNDAY:
            ss << "Sonntag";
            break;
    }

    /**
     * https://www.t-online.de/leben/familie/id_88749070/tageszeiten-in-deutschland-wann-beginnt-eigentlich-die-nacht-.html
     *
     * ab  7:00 Uhr morgens
     * ab 11:00 Uhr vormittags
     * ab 13:00 Uhr mittags
     * ab 15:00 Uhr nachmittags
     * ab 18:00 Uhr abends
     * ab 22:00 Uhr nachts
     *
     * https://www.laenderdaten.info/Europa/Deutschland/sonnenuntergang.php
     *
     * ab  4:00 Uhr Sonnenaufgang
     * ab  5:00 Uhr Tag
     * ab 21:30 Uhr Sonnenuntergang
     * ab 22:30 Uht Nacht
     */

    ss << "</b> (";

    /*
    if(data.hours >= 7 || data.hours < 11) {
        ss << "morgens";
    } else if(data.hours >= 11 || data.hours < 13) {
        ss << "vormittags";
    } else if(data.hours < 11 || data.hours > 9) {
        ss << "mittags";
    } else if(data.hours < 13 || data.hours > 11) {
        ss << "nachmittags";
    } else if(data.hours < 17 || data.hours > 13) {
        ss << "abends";
    } else if(data.hours < 21 || data.hours > 17) {
        ss << "nachts";
    }*/
    ss << ")";

    m_Label_Date.set_markup(ss.str());
    //m_Label_Date.set_tooltip_markup("<b>Faktor:</b> unbekannt");
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
            m_Button_AutomaticControl_Set.set_sensitive(true);
            m_ButtonBox_System.set_sensitive(true);
            m_ButtonBox_AutomaticControl.set_sensitive(true);
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
            m_Button_AutomaticControl_Set.set_sensitive(false);
            m_ButtonBox_System.set_sensitive(false);
            m_ButtonBox_AutomaticControl.set_sensitive(false);
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

void FrmMain::on_button_system_reset_clicked() {
    msgEndpoint->sendMsg(SystemHardwareReset{});
}

void FrmMain::on_button_system_shutdown_clicked() {
    msgEndpoint->sendMsg(SystemHardwareShutdown{});
}

void FrmMain::on_button_system_standby_clicked() {
    if(m_Button_SystemStandby.get_label() == "Standby (aus)") {
        msgEndpoint->sendMsg(SystemSetStandbyMode{true});
    } else {
        msgEndpoint->sendMsg(SystemSetStandbyMode{false});
    }
}

void FrmMain::on_button_system_automatic_clicked() {
    if(m_Button_AutomaticControl_Enable.get_label() == "Automatik (aus)") {
        msgEndpoint->sendMsg(SystemSetAutomaticMode{true});
    } else {
        msgEndpoint->sendMsg(SystemSetAutomaticMode{false});
    }
}

void FrmMain::on_button_system_ping_clicked() {
    msgEndpoint->sendMsg(ClientEchoReq{"test"});
    start = std::chrono::system_clock::now();
    pingctr = 0;
    m_Button_SystemPing.set_sensitive(false);
}

void FrmMain::on_button_time_control_set_clicked() {
    auto iter = m_Combo_CurModelDay.get_active();
    assert(iter);

    auto iter1 = m_Combo_Multiplicator.get_active();
    assert(iter1);

    msgEndpoint->sendMsg(TimerSetGlobalTimer{
        (*iter)[m_Columns_CurModelDay.m_col_id],
        m_Entry_CurModelTime.get_text(),
        (*iter1)[m_Columns_Multiplicator.m_col_factor]
    });
}
// </editor-fold>

////////////////////////////////////////////////////////////////////////////////
// <editor-fold defaultstate="collapsed" desc="msg-response">
void FrmMain::setServerInfoRes(const ServerInfoRes &data) {
    lblName[0][0].set_markup("<b>AppName:</b>");
    lblName[1][0].set_label(data.appName);

    lblName[0][1].set_markup("<b>Version:</b>");
    lblName[1][1].set_label(data.version.getString());

    lblName[0][2].set_markup("<b>Build-Date:</b>");
    lblName[1][2].set_label(data.buildDate);

    lblName[0][3].set_markup("<b>Start-Time:</b>");
    lblName[1][3].set_label(data.startTime);

    lblName[0][4].set_markup("<b>max. Clients:</b>");
    lblName[1][4].set_label(std::to_string(data.maxClients));

    lblName[0][5].set_markup("<b>connected Clients:</b>");
    lblName[1][5].set_label(std::to_string(data.connectedClients));

    lblName[0][6].set_markup("<b>osArch:</b>");
    lblName[1][6].set_label(data.osArch);

    lblName[0][7].set_markup("<b>osName:</b>");
    lblName[1][7].set_label(data.osName);

    lblName[0][8].set_markup("<b>osVersion:</b>");
    lblName[1][8].set_label(data.osVersion);

    lblName[0][9].set_markup("<b>fwType:</b>");
    lblName[1][9].set_label(data.fwType);

    lblName[0][10].set_markup("<b>fwVersion:</b>");
    lblName[1][10].set_label(data.fwVersion);
}

void FrmMain::setConClientsRes(const ServerConClientsRes &data) {
    m_ActiveApps.clearList();

    for(auto iter : data.endpoints) {
        m_ActiveApps.addActiveApp(
            iter.appId, iter.appInfo.appName, iter.appInfo.version.getString(), iter.addr, iter.port, iter.startTime
        );
    }
}

void FrmMain::setErrorNotice(const ClientError &data) {
    m_Notice_Logger.setNotice(Gtk::MESSAGE_ERROR, data.errorId, data.additionalMsg);
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
    m_Notice_Logger.setNotice(mt, data.caption, data.text);
}

void FrmMain::setHardwareState(const SystemHardwareStateChanged &data) {
    if(data.hardwareState == SystemHardwareStateChanged::HardwareState::ERROR) {
        m_Clock.stop();
        m_Label_Connectivity_HW.override_color(Gdk::RGBA("red"), Gtk::STATE_FLAG_NORMAL);
        m_Label_Connectivity_SW.override_color(Gdk::RGBA("red"), Gtk::STATE_FLAG_NORMAL);
        m_Label_Connectivity_HW.set_tooltip_markup("<b>Status:</b> Keine Verbindung zur Hardware");
        m_Label_Connectivity_SW.set_tooltip_markup("<b>Status:</b> Keine Verbindung zur Hardware");
        m_Button_Emergency.set_sensitive(false);
        m_Button_SystemStandby.set_sensitive(false);
        m_Button_AutomaticControl_Enable.set_sensitive(false);
        setHardwareStateLabel("Hardwarefehler");
        return;
    }
    m_Button_Emergency.set_sensitive(true);
    m_Button_SystemStandby.set_sensitive(true);
    m_Button_AutomaticControl_Enable.set_sensitive(true);
    if(data.hardwareState == SystemHardwareStateChanged::HardwareState::EMERGENCY_STOP) {
        m_Clock.stop();
        m_Label_Connectivity_HW.override_color(Gdk::RGBA("red"), Gtk::STATE_FLAG_NORMAL);
        m_Label_Connectivity_SW.override_color(Gdk::RGBA("gold"), Gtk::STATE_FLAG_NORMAL);
        m_Label_Connectivity_HW.set_tooltip_markup("<b>Status:</b> Nohalt ausgelöst");
        m_Label_Connectivity_SW.set_tooltip_markup("<b>Status:</b> Nohalt ausgelöst");
        m_Button_Emergency.set_label("Freigabe");
        m_Button_AutomaticControl_Enable.set_sensitive(false);
        m_Button_SystemStandby.set_sensitive(false);
        setHardwareStateLabel("Nothalt");
        return;
    }
    m_Button_Emergency.set_label("Nothalt");
    if(data.hardwareState == SystemHardwareStateChanged::HardwareState::STANDBY) {
        m_Clock.stop();
        m_Label_Connectivity_HW.override_color(Gdk::RGBA("gold"), Gtk::STATE_FLAG_NORMAL);
        m_Label_Connectivity_SW.override_color(Gdk::RGBA("gold"), Gtk::STATE_FLAG_NORMAL);
        m_Label_Connectivity_HW.set_tooltip_markup("<b>Status:</b> Energiesparmodus");
        m_Label_Connectivity_SW.set_tooltip_markup("<b>Status:</b> Energiesparmodus");
        m_Button_SystemStandby.set_label("Standby (an)");
        m_Button_Emergency.set_sensitive(false);
        m_Button_AutomaticControl_Enable.set_sensitive(false);
        setHardwareStateLabel("standby");
        return;
    }
    m_Button_SystemStandby.set_label("Standby (aus)");
    if(data.hardwareState == SystemHardwareStateChanged::HardwareState::MANUEL) {
        m_Clock.stop();
        m_Label_Connectivity_HW.override_color(Gdk::RGBA("green"), Gtk::STATE_FLAG_NORMAL);
        m_Label_Connectivity_SW.override_color(Gdk::RGBA("gold"), Gtk::STATE_FLAG_NORMAL);
        m_Label_Connectivity_HW.set_tooltip_markup("<b>Status:</b> manuell");
        m_Label_Connectivity_SW.set_tooltip_markup("<b>Status:</b> manuell");
        m_Button_AutomaticControl_Enable.set_label("Automatik (aus)");
        setHardwareStateLabel("manuell");
        return;
    }
    if(data.hardwareState == SystemHardwareStateChanged::HardwareState::AUTOMATIC) {
        m_Clock.run();
        m_Label_Connectivity_HW.override_color(Gdk::RGBA("green"), Gtk::STATE_FLAG_NORMAL);
        m_Label_Connectivity_SW.override_color(Gdk::RGBA("green"), Gtk::STATE_FLAG_NORMAL);
        m_Label_Connectivity_HW.set_tooltip_markup("<b>Status:</b> automatisch");
        m_Label_Connectivity_SW.set_tooltip_markup("<b>Status:</b> automatisch");
        m_Button_AutomaticControl_Enable.set_label("Automatik (an)");
        setHardwareStateLabel("automatisch");
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

void FrmMain::setTimerGlobalTimerEvent(const TimerGlobalTimerEvent &data) {
    m_Clock.setMultiplier(data.multiplicator);
    m_Clock.setTime(data.hours, data.minutes, false);

    setClock(data.curModelDay, data.hours);
}

void FrmMain::setTimerSetGlobalTimer(const TimerSetGlobalTimer &data) {
    m_Combo_CurModelDay.set_active(static_cast<int>(data.curModelDay));
    m_Entry_CurModelTime.set_text(data.curModelTime);

    auto children = m_refListModel_Multiplicator->children();

    for(auto iter : children) {
        if(data.multiplicator == (*iter)[m_Columns_Multiplicator.m_col_factor]) {
            m_Combo_Multiplicator.set_active(iter);
            break;
        }
    }

    m_Clock.setMultiplier(data.multiplicator);
    m_Clock.setTime(data.hours, data.minutes, true);

    setClock(data.curModelDay, data.hours);
}

void FrmMain::setPingResult(const ClientEchoRes&) {
    std::stringstream ss;
    timeb sTimeB;

    char buffer[25] = "";

    ftime(&sTimeB);
    strftime(buffer, 21, "%d.%m.%Y %H:%M:%S", localtime(&sTimeB.time));

    std::chrono::time_point<std::chrono::system_clock> end = std::chrono::system_clock::now();
    int elapsed_millis = std::chrono::duration_cast<std::chrono::milliseconds> (end - start).count();

    ss << "<b>Ping " << (pingctr + 1) << ":</b> " << buffer << " -> " << " elapsed time: " << elapsed_millis << " ms";
    m_Label_PingResult[pingctr].set_markup(ss.str());

    if(pingctr < 3) {
        msgEndpoint->sendMsg(ClientEchoReq{"test"});
        start = std::chrono::system_clock::now();
        pingctr++;
        return;
    }
    m_Button_SystemPing.set_sensitive(true);
}
// </editor-fold>
