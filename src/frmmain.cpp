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

#include <ctime>
#include <sys/timeb.h>

#include "frmmain.h"
#include "config.h"

namespace {
    const char license[] =
        "Project:    moba-systemmanager\n"
        "\n"
        "Copyright (C) 2016 Stefan Paproth <pappi-@gmx.de>\n"
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

    moba::Message::MessageType type[] = {

        moba::Message::MT_ECHO_REQ,

        moba::Message::MT_RESET_CLIENT,
        moba::Message::MT_SERVER_INFO_REQ,
        moba::Message::MT_CON_CLIENTS_REQ,
        moba::Message::MT_SELF_TESTING_CLIENT,

        moba::Message::MT_EMERGENCY_STOP,
        moba::Message::MT_EMERGENCY_STOP_CLEARING,
        moba::Message::MT_GET_HARDWARE_STATE,
        moba::Message::MT_SET_HARDWARE_STATE,
        moba::Message::MT_HARDWARE_SHUTDOWN,
        moba::Message::MT_HARDWARE_RESET,
        moba::Message::MT_HARDWARE_SWITCH_STANDBY,
    };
}

FrmMain::FrmMain(moba::MsgHandlerPtr mhp) :
    msgHandler(mhp), sysHandler(mhp), m_VBox(Gtk::ORIENTATION_VERTICAL, 6),
    m_Button_About("About..."), m_VBox_SystemControl(Gtk::ORIENTATION_VERTICAL, 6),
    m_VBox_ServerDataKey(Gtk::ORIENTATION_VERTICAL, 6), m_VBox_ServerDataValue(Gtk::ORIENTATION_VERTICAL, 6)
{
    sigc::slot<bool> my_slot = sigc::bind(sigc::mem_fun(*this, &FrmMain::on_timeout), 1);
    sigc::connection conn = Glib::signal_timeout().connect(my_slot, 250); // 250 ms

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

    m_VBox.pack_start(m_ButtonBox, Gtk::PACK_SHRINK);

    // about-dialog
    m_ButtonBox.pack_start(m_Button_About, Gtk::PACK_EXPAND_WIDGET, 5);
    m_ButtonBox.set_layout(Gtk::BUTTONBOX_END);
    m_Button_About.signal_clicked().connect(sigc::mem_fun(*this, &FrmMain::on_button_about_clicked));

    m_ButtonBox.pack_start(m_Button_Emegerency, Gtk::PACK_EXPAND_WIDGET, 5);
    m_Button_Emegerency.signal_clicked().connect(sigc::mem_fun(*this, &FrmMain::on_button_emegency_clicked));
    m_Button_Emegerency.set_label("Nothalt");

    initAboutDialog();
    initActiveApps();
    initServerData();
    initSystemControl();
    initStatus();

    msgHandler->sendServerInfoReq();
    msgHandler->sendConClientsReq();
    sysHandler.sendGetEmergencyStopState();
    sysHandler.sendGetHardwareState();
    show_all_children();
    m_InfoBar.hide();
}

void FrmMain::initAboutDialog() {
    m_Dialog.set_transient_for(*this);

    m_Dialog.set_program_name(PACKAGE_NAME);
    m_Dialog.set_version(PACKAGE_VERSION);
    m_Dialog.set_copyright("Stefan Paproth");
    m_Dialog.set_comments("This is just an application for testing purpose.");
    m_Dialog.set_license(license);

    m_Dialog.set_website("<pappi-@gmx.de>");
    m_Dialog.set_website_label("pappi-@gmx.de");

    std::vector<Glib::ustring> list_authors;
    list_authors.push_back("Stefan Paproth");
    m_Dialog.set_authors(list_authors);

    m_Dialog.signal_response().connect(sigc::mem_fun(*this, &FrmMain::on_about_dialog_response));

    m_Button_About.grab_focus();
}

void FrmMain::initActiveApps() {
    m_Notebook.append_page(m_ScrolledWindow, "Active Apps");

    m_ScrolledWindow.add(m_TreeView);
    m_ScrolledWindow.set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);

    //Create the Tree model:
    m_refTreeModel = Gtk::ListStore::create(m_Columns_ActiveApps);
    m_TreeView.set_model(m_refTreeModel);

    m_TreeView.append_column("ID",         m_Columns_ActiveApps.m_col_id);
    m_TreeView.append_column("Name",       m_Columns_ActiveApps.m_col_name);
    m_TreeView.append_column("Version",    m_Columns_ActiveApps.m_col_version);
    m_TreeView.append_column("IP-Adresse", m_Columns_ActiveApps.m_col_ipAddr);
    m_TreeView.append_column("Port",       m_Columns_ActiveApps.m_col_port);
    m_TreeView.append_column("Uptime",     m_Columns_ActiveApps.m_col_uptime);
}

void FrmMain::initServerData() {
    m_Notebook.append_page(m_HBox_ServerData, "Server Data");

    m_HBox_ServerData.set_border_width(2);

    m_HBox_ServerData.add(m_VBox_ServerDataKey);
    m_HBox_ServerData.add(m_VBox_ServerDataValue);

    for(int i = 0; i < 12; ++i) {
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

    m_Button_SystemShutdown.set_label("Shutdown");
    m_Button_SystemReset.set_label("Reset");
    m_Button_SystemStandby.set_label("Standby");
    m_Button_SystemPing.set_label("Ping");

    m_Button_SystemShutdown.signal_clicked().connect(sigc::mem_fun(*this, &FrmMain::on_button_system_shutdown_clicked));
    m_Button_SystemReset.signal_clicked().connect(sigc::mem_fun(*this, &FrmMain::on_button_system_reset_clicked));
    m_Button_SystemStandby.signal_clicked().connect(sigc::mem_fun(*this, &FrmMain::on_button_system_standby_clicked));
    m_Button_SystemPing.signal_clicked().connect(sigc::mem_fun(*this, &FrmMain::on_button_system_ping_clicked));
}

void FrmMain::initStatus() {
    m_Notebook.append_page(m_ScrolledWindow_Notices, "Notice Logger");

    m_ScrolledWindow_Notices.add(m_TreeView_Notices);
    m_ScrolledWindow_Notices.set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);

    m_refTreeModel_Notices = Gtk::ListStore::create(m_Columns_Notices);
    m_TreeView_Notices.set_model(m_refTreeModel_Notices);

    m_TreeView_Notices.append_column("Timestamp", m_Columns_Notices.m_col_timestamp);
    m_TreeView_Notices.append_column("Type",      m_Columns_Notices.m_col_type);
    m_TreeView_Notices.append_column("Caption",   m_Columns_Notices.m_col_caption);
    m_TreeView_Notices.append_column("Text",      m_Columns_Notices.m_col_text);
}

////////////////////////////////////////////////////////////////////////////////
// Call-back-methodes

void FrmMain::on_button_about_clicked() {
    m_Dialog.show();
    m_Dialog.present();
}

void FrmMain::on_button_emegency_clicked() {
    if(m_Button_Emegerency.get_label() == "Nothalt") {
        sysHandler.sendEmergencyStop();
    } else {
        sysHandler.sendEmergencyStopClearing();
    }
}

void FrmMain::on_about_dialog_response(int) {
    m_Dialog.hide();
}

bool FrmMain::on_timeout(int) {
    moba::MessagePtr msg;
    try {
        msg = msgHandler->recieveMsg();
    } catch(std::exception &e) {
        Gtk::MessageDialog dialog(
            *this,
            "msg-handler exception:",
            false,
            Gtk::MESSAGE_ERROR,
            Gtk::BUTTONS_OK
        );
        dialog.set_secondary_text(e.what());
        dialog.run();
        msgHandler->connect();
        return true;
    }

    if(!msg) {
        return true;
    }

    switch(msg->getMsgType()) {
        case moba::Message::MT_SERVER_INFO_RES:
            setServerInfoRes(msg->getData());
            break;

        case moba::Message::MT_CON_CLIENTS_RES:
            setConClientsRes(msg->getData());
            break;

        case moba::Message::MT_SYSTEM_NOTICE:
            setSystemNotice(msg->getData());
            break;

        case moba::Message::MT_EMERGENCY_STOP:
            m_Button_Emegerency.set_label("Freigabe");
            break;

        case moba::Message::MT_EMERGENCY_STOP_CLEARING:
            m_Button_Emegerency.set_label("Nothalt");
            break;

        case moba::Message::MT_HARDWARE_STATE_CHANGED:
            setHardwareState(msg->getData());
            break;

        case moba::Message::MT_ECHO_RES:
            setPingResult();
            break;
    }
    return true;
}

void FrmMain::on_infobar_response(int) {
    m_Label_InfoBarMessage.set_text("");
    m_InfoBar.hide();
}

void FrmMain::on_button_system_reset_clicked() {
    sysHandler.sendHardwareReset();
}

void FrmMain::on_button_system_shutdown_clicked() {
    sysHandler.sendHardwareShutdown();
}

void FrmMain::on_button_system_standby_clicked() {
    sysHandler.sendHardwareSwitchStandby();
}

void FrmMain::on_button_system_ping_clicked() {
    msgHandler->sendEchoReq("test");
    start = std::chrono::system_clock::now();
    pingctr = 0;
    m_Button_SystemPing.set_sensitive(false);
}

void FrmMain::setServerInfoRes(moba::JsonItemPtr data) {
    moba::JsonObjectPtr o = boost::dynamic_pointer_cast<moba::JsonObject>(data);
    lblName[0][0].set_markup("<b>AppName:</b>");
    lblName[1][0].set_label(moba::castToString(o->at("appName")));

    lblName[0][1].set_markup("<b>Version:</b>");
    lblName[1][1].set_label(moba::castToString(o->at("version")));

    lblName[0][2].set_markup("<b>Build-Date:</b>");
    lblName[1][2].set_label(moba::castToString(o->at("buildDate")));

    lblName[0][3].set_markup("<b>Start-Time:</b>");
    lblName[1][3].set_label(moba::castToString(o->at("startTime")));

    lblName[0][4].set_markup("<b>Up-Time:</b>");
    lblName[1][4].set_label(moba::castToString(o->at("upTime")));

    lblName[0][5].set_markup("<b>max. Clients:</b>");
    lblName[1][5].set_label(std::to_string(moba::castToInt(o->at("maxClients"))));

    lblName[0][6].set_markup("<b>connected Clients:</b>");
    lblName[1][6].set_label(std::to_string(moba::castToInt(o->at("connectedClients"))));

    lblName[0][7].set_markup("<b>osArch:</b>");
    lblName[1][7].set_label(moba::castToString(o->at("osArch")));

    lblName[0][8].set_markup("<b>osName:</b>");
    lblName[1][8].set_label(moba::castToString(o->at("osName")));

    lblName[0][9].set_markup("<b>osVersion:</b>");
    lblName[1][9].set_label(moba::castToString(o->at("osVersion")));

    lblName[0][10].set_markup("<b>fwType:</b>");
    lblName[1][10].set_label(moba::castToString(o->at("fwType")));

    lblName[0][11].set_markup("<b>fwVersion:</b>");
    lblName[1][11].set_label(moba::castToString(o->at("fwVersion")));
}

void FrmMain::setConClientsRes(moba::JsonItemPtr data) {
    moba::JsonArrayPtr a = boost::dynamic_pointer_cast<moba::JsonArray>(data);
    Gtk::TreeModel::Row row;

    for(auto iter = a->begin(); iter != a->end(); ++iter) {
        moba::JsonObjectPtr o = boost::dynamic_pointer_cast<moba::JsonObject>(*iter);
        moba::JsonObjectPtr oi =
                boost::dynamic_pointer_cast<moba::JsonObject>(o->at("appInfo"));

        row = *(m_refTreeModel->append());
        row[m_Columns_ActiveApps.m_col_id     ] = moba::castToInt(o->at("appID"));
        row[m_Columns_ActiveApps.m_col_name   ] = moba::castToString(oi->at("appName"));
        row[m_Columns_ActiveApps.m_col_version] = moba::castToString(oi->at("version"));
        row[m_Columns_ActiveApps.m_col_ipAddr ] = moba::castToString(o->at("addr"));
        row[m_Columns_ActiveApps.m_col_port   ] = moba::castToInt(o->at("port"));
        row[m_Columns_ActiveApps.m_col_uptime ] = moba::castToString(o->at("upTime"));
    }
}

void FrmMain::setSystemNotice(moba::JsonItemPtr data) {
    moba::JsonObjectPtr o = boost::dynamic_pointer_cast<moba::JsonObject>(data);
    std::string type = moba::castToString(o->at("type"));
    std::string caption = moba::castToString(o->at("caption"));
    std::string text = moba::castToString(o->at("text"));

    if(type == "INFO") {
        m_InfoBar.set_message_type(Gtk::MESSAGE_INFO);
    }
    if(type == "WARNING") {
        m_InfoBar.set_message_type(Gtk::MESSAGE_WARNING);
    }
    if(type == "ERROR") {
        m_InfoBar.set_message_type(Gtk::MESSAGE_ERROR);
    }

    std::stringstream ss;
    ss << "<b>" << caption << "!</b>\n" << text;

    m_Label_InfoBarMessage.set_markup(ss.str());
    m_InfoBar.show();

    timeb sTimeB;

    char buffer[25] = "";

    ftime(&sTimeB);
    strftime(buffer, 21, "%d.%m.%Y %H:%M:%S", localtime(&sTimeB.time));

    Gtk::TreeModel::Row row = *(m_refTreeModel_Notices->append());
    row[m_Columns_Notices.m_col_timestamp] = std::string(buffer);
    row[m_Columns_Notices.m_col_type     ] = type;
    row[m_Columns_Notices.m_col_caption  ] = caption;
    row[m_Columns_Notices.m_col_text     ] = text;
}

void FrmMain::setHardwareState(moba::JsonItemPtr data) {
    std::stringstream ss;
    ss << "<b>Hardwarestatus:</b> " << moba::castToString(data);
    m_Label_HardwareState.set_markup(ss.str());
}

void FrmMain::setPingResult() {
    std::stringstream ss;
    timeb sTimeB;

    char buffer[25] = "";

    ftime(&sTimeB);
    strftime(buffer, 21, "%d.%m.%Y %H:%M:%S", localtime(&sTimeB.time));

    std::chrono::time_point<std::chrono::system_clock>
        end = std::chrono::system_clock::now();

    int elapsed_millis = std::chrono::duration_cast<std::chrono::milliseconds> (end - start).count();


    ss << "<b>Ping " << (pingctr + 1) << ":</b> " << buffer << " -> " << " elapsed time: " << elapsed_millis << " ms";
    m_Label_PingResult[pingctr].set_markup(ss.str());

    if(pingctr < 3) {
        msgHandler->sendEchoReq("test");
        start = std::chrono::system_clock::now();
        pingctr++;
        return;
    }
    m_Button_SystemPing.set_sensitive(true);
}
