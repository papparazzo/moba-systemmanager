/*
 *  Project:    moba-systemmanager
 *
 *  Copyright (C) 2023 Stefan Paproth <pappi-@gmx.de>
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

#include "frmbase.h"

#include "config.h"

namespace {
    const char license[] =
        "Project:    " PACKAGE_NAME "\n"
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

FrmBase::FrmBase(EndpointPtr mhp): systemState{SystemState::NO_CONNECT}, msgEndpoint{mhp} {

    set_icon_name(PACKAGE_NAME);
    set_title(PACKAGE_NAME);
    //set_border_width(10);
    //set_position(Gtk::WIN_POS_CENTER);
/*
    sigc::slot<bool> my_slot1 = sigc::bind(sigc::mem_fun(*this, &FrmBase::on_timeout), 1);
    sigc::connection conn1 = Glib::signal_timeout().connect(my_slot1, 25); // 25 ms

    sigc::slot<bool> my_slot2 = sigc::bind(sigc::mem_fun(*this, &FrmBase::on_timeout_status), 1);
    sigc::connection conn2 = Glib::signal_timeout().connect(my_slot2, 850, Glib::PRIORITY_DEFAULT_IDLE); // 25 ms
    
    // Add the message label to the InfoBar:
    auto infoBarContainer = dynamic_cast<Gtk::Container*>(m_InfoBar.get_content_area());
    if(infoBarContainer) {
        infoBarContainer->add(m_Label_InfoBarMessage);
    }
*/
    set_child(m_VBox);
    
    m_VBox.append(m_InfoBar);

    m_HBox_Status.set_margin(6);
    m_HBox_Status.append(m_Label_Connectivity_SW);
    m_Label_Connectivity_SW.set_justify(Gtk::Justification::LEFT);
    m_Label_Connectivity_SW.set_markup("<span color=\"gray\"> \xe2\x96\x84</span>");

    m_HBox_Status.append(m_Label_Connectivity_HW);
    m_Label_Connectivity_HW.set_justify(Gtk::Justification::LEFT);
    m_Label_Connectivity_HW.set_markup("<span color=\"gray\"> \xe2\x96\x84</span>");

    m_HBox_Status.append(m_HButtonBox);

    m_InfoBar.signal_response().connect(sigc::mem_fun(*this, &FrmBase::on_infobar_response));
    m_InfoBar.add_button("_OK", 0);

    m_HBox_Expander.set_hexpand();
    m_HButtonBox.append(m_HBox_Expander);

    // about-dialog
    m_HButtonBox.append(m_Button_About);
    m_Button_About.signal_clicked().connect(sigc::mem_fun(*this, &FrmBase::on_button_about_clicked));

    m_HButtonBox.append(m_Button_Emergency);
    m_Button_Emergency.signal_clicked().connect(sigc::mem_fun(*this, &FrmBase::on_button_emergency_clicked));

    setSensitive(false);
    m_Button_Emergency.set_sensitive(false);
    initAboutDialog();

    registry.registerHandler<GuiSystemNotice>(std::bind(&FrmBase::setSystemNotice, this, std::placeholders::_1));
    registry.registerHandler<ClientError>(std::bind(&FrmBase::setErrorNotice, this, std::placeholders::_1));
    registry.registerHandler<SystemHardwareStateChanged>(std::bind(&FrmBase::setHardwareState, this, std::placeholders::_1));
}

void FrmBase::finishForm() {
   // show_all_children();
    m_VBox.append(m_HBox_Status);
    m_InfoBar.hide();
}
 
void FrmBase::initAboutDialog() {
    m_Dialog.set_transient_for(*this);

    m_Dialog.set_program_name(PACKAGE_NAME);
    m_Dialog.set_version(PACKAGE_VERSION);
    m_Dialog.set_copyright("Stefan Paproth");
    m_Dialog.set_comments("Application for controlling the moba-server.");
    m_Dialog.set_license(license);

    m_Dialog.set_website("<pappi-@gmx.de>");
    m_Dialog.set_website_label("pappi-@gmx.de");

   // m_Dialog.set_logo(Gdk::Pixbuf::create_from_file("/usr/local/share/icons/hicolor/scalable/apps/" PACKAGE_NAME ".svg"));

    std::vector<Glib::ustring> list_authors;
    list_authors.push_back("Stefan Paproth");
    m_Dialog.set_authors(list_authors);

   // m_Dialog.signal_response().connect(sigc::mem_fun(*this, &FrmBase::on_about_dialog_response));

    m_Button_About.grab_focus();
}

std::string FrmBase::getDisplayMessage(std::string caption, std::string text) {
    std::replace(caption.begin(), caption.end(), '<', '"');
    std::replace(caption.begin(), caption.end(), '>', '"');
    std::replace(text.begin(), text.end(), '<', '"');
    std::replace(text.begin(), text.end(), '>', '"');

    std::stringstream ss;
    ss << "<b>" << caption << "!</b>\n" << text;
    return std::move(ss.str());
}

void FrmBase::setNotice(Gtk::MessageType noticeType, std::string caption, std::string text) {
    listNotice(noticeType, caption, text);

    m_Label_InfoBarMessage.set_markup(getDisplayMessage(caption, text));
    m_InfoBar.set_message_type(noticeType);
    m_InfoBar.show();
}

void FrmBase::setErrorNotice(const ClientError &data) {
    setNotice(
        Gtk::MessageType::ERROR, 
        errorIdEnumToString(data.errorId),    
        data.additionalMsg
    );
}

void FrmBase::setSystemNotice(const GuiSystemNotice &data) {
    
    Gtk::MessageType mt;
    switch(data.noticeType) {
        case GuiSystemNotice::NoticeType::ERROR:
            mt = Gtk::MessageType::ERROR;
            break;

        case GuiSystemNotice::NoticeType::WARNING:
            mt = Gtk::MessageType::WARNING;
            break;

        case GuiSystemNotice::NoticeType::INFO:
        default:
            mt = Gtk::MessageType::INFO;
            break;
    }
    setNotice(mt, data.caption, data.text);
}

void FrmBase::setHardwareState(const SystemHardwareStateChanged &data) {
    if(data.hardwareState == SystemHardwareStateChanged::HardwareState::ERROR) {
        m_Label_Connectivity_HW.set_tooltip_markup("<b>Status:</b> Keine Verbindung zur Hardware");
        m_Label_Connectivity_SW.set_tooltip_markup("<b>Status:</b> Keine Verbindung zur Hardware");
        systemState = SystemState::ERROR;
        m_Button_Emergency.set_sensitive(false);
        setSystemState(systemState);
        return;
    }
    m_Button_Emergency.set_sensitive(true);
    if(data.hardwareState == SystemHardwareStateChanged::HardwareState::EMERGENCY_STOP) {
        m_Label_Connectivity_HW.set_tooltip_markup("<b>Status:</b> Nohalt ausgelöst");
        m_Label_Connectivity_SW.set_tooltip_markup("<b>Status:</b> Nohalt ausgelöst");
        systemState = SystemState::EMERGENCY_STOP;
        m_Button_Emergency.set_label("Freigabe");
        setSystemState(systemState);
        return;
    }
    m_Button_Emergency.set_label("Nothalt");
    if(data.hardwareState == SystemHardwareStateChanged::HardwareState::STANDBY) {
        m_Label_Connectivity_HW.set_tooltip_markup("<b>Status:</b> Energiesparmodus");
        m_Label_Connectivity_SW.set_tooltip_markup("<b>Status:</b> Energiesparmodus");
        systemState = SystemState::STANDBY;
        m_Button_Emergency.set_sensitive(false);
    }

    if(data.hardwareState == SystemHardwareStateChanged::HardwareState::MANUEL) {
        m_Label_Connectivity_HW.set_tooltip_markup("<b>Status:</b> manuell");
        m_Label_Connectivity_SW.set_tooltip_markup("<b>Status:</b> manuell");
        systemState = SystemState::MANUEL;
    }
    if(data.hardwareState == SystemHardwareStateChanged::HardwareState::AUTOMATIC) {
        m_Label_Connectivity_HW.set_tooltip_markup("<b>Status:</b> automatisch");
        m_Label_Connectivity_SW.set_tooltip_markup("<b>Status:</b> automatisch");
        systemState = SystemState::AUTOMATIC;
    }
    setSystemState(systemState);
}

////////////////////////////////////////////////////////////////////////////////
// <editor-fold defaultstate="collapsed" desc="call-back-methodes">
void FrmBase::on_about_dialog_response(int) {
    m_Dialog.hide();
}

bool FrmBase::on_timeout(int) {
    static bool connected = false;

    try {
        if(!connected) {
            msgEndpoint->connect();
            systemState = SystemState::ERROR;
            m_Label_Connectivity_HW.set_tooltip_markup("<b>Status:</b> Keine Verbindung zur Hardware");
            m_Label_Connectivity_SW.set_tooltip_markup("<b>Status:</b> Keine Verbindung zur Hardware");
            initialSend();
            setSensitive(true);

            connected = true;
            return true;
        }
        registry.handleMsg(msgEndpoint->receiveMsg());
    } catch(std::exception &e) {
        if(connected) {
            m_Button_Emergency.set_sensitive(false);
            systemState = SystemState::NO_CONNECT;
            m_Label_Connectivity_HW.set_tooltip_markup("<b>Status:</b> Keine Verbindung zum Server");
            m_Label_Connectivity_SW.set_tooltip_markup("<b>Status:</b> Keine Verbindung zum Server");

            m_InfoBar.set_message_type(Gtk::MessageType::ERROR);
            std::stringstream ss;
            ss << "<b>msg-handler exception:</b>\n" << e.what();
            m_Label_InfoBarMessage.set_markup(ss.str());
            m_InfoBar.show();
            setSensitive(false);
            connected = false;
        }
    }
    return true;
}

bool FrmBase::on_timeout_status(int) {
    static bool on = false;

    on = !on;
    switch(systemState) {
        case SystemState::NO_CONNECT:
            if(on) {
                m_Label_Connectivity_SW.set_markup("<span color=\"red\"> \xe2\x96\x84</span>");
            } else {
                m_Label_Connectivity_SW.set_markup("<span color=\"gray\"> \xe2\x96\x84</span>");
            }
            m_Label_Connectivity_HW.set_markup("<span color=\"gray\"> \xe2\x96\x84</span>");
            break;

        case SystemState::ERROR:
            m_Label_Connectivity_SW.set_markup("<span color=\"green\"> \xe2\x96\x84</span>");
            if(on) {
                m_Label_Connectivity_HW.set_markup("<span color=\"red\"> \xe2\x96\x84</span>");
            } else {
                m_Label_Connectivity_HW.set_markup("<span color=\"gray\"> \xe2\x96\x84</span>");
            }
            break;

        case SystemState::STANDBY:
            m_Label_Connectivity_SW.set_markup("<span color=\"green\"> \xe2\x96\x84</span>");
            if(on) {
                m_Label_Connectivity_HW.set_markup("<span color=\"gold\"> \xe2\x96\x84</span>");
            } else {
                m_Label_Connectivity_HW.set_markup("<span color=\"gray\"> \xe2\x96\x84</span>");
            }
            break;

        case SystemState::EMERGENCY_STOP:
            if(on) {
                m_Label_Connectivity_HW.set_markup("<span color=\"gold\"> \xe2\x96\x84</span>");
                m_Label_Connectivity_SW.set_markup("<span color=\"gold\"> \xe2\x96\x84</span>");
            } else {
                m_Label_Connectivity_HW.set_markup("<span color=\"gray\"> \xe2\x96\x84</span>");
                m_Label_Connectivity_SW.set_markup("<span color=\"gray\"> \xe2\x96\x84</span>");
            }
            break;

        case SystemState::MANUEL:
            m_Label_Connectivity_HW.set_markup("<span color=\"green\"> \xe2\x96\x84</span>");
            if(on) {
                m_Label_Connectivity_SW.set_markup("<span color=\"green\"> \xe2\x96\x84</span>");
            } else {
                m_Label_Connectivity_SW.set_markup("<span color=\"gray\"> \xe2\x96\x84</span>");
            }
            break;

        case SystemState::AUTOMATIC:
            m_Label_Connectivity_HW.set_markup("<span color=\"green\"> \xe2\x96\x84</span>");
            m_Label_Connectivity_SW.set_markup("<span color=\"green\"> \xe2\x96\x84</span>");
            break;
    }
    return true;

    /*
                SW              HW
    -           rot / blink     grau
    ERROR       grün            rot / blink
    STANDBY     grün            gelb / blink
    EMERGENCY   gelb / blink    gelb / blink
    MANUELL     grün / blink    grün
    AUTOMATIC   grün            grün

     */
}

void FrmBase::on_button_about_clicked() {
    m_Dialog.show();
    m_Dialog.present();
}

void FrmBase::on_button_emergency_clicked() {
    if(m_Button_Emergency.get_label() == "Nothalt") {
        msgEndpoint->sendMsg(SystemTriggerEmergencyStop{});
    } else {
        msgEndpoint->sendMsg(SystemReleaseEmergencyStop{});
    }
}

void FrmBase::on_infobar_response(int) {
    m_Label_InfoBarMessage.set_text("");
    m_InfoBar.hide();
}
// </editor-fold>
