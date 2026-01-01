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

#pragma once

#include "moba/endpoint.h"
#include "moba/registry.h"

#include "moba/messagingmessages.h"
#include "moba/clientmessages.h"
#include "moba/systemmessages.h"

#include <gtkmm.h>
#include <atomic>

enum class SystemState {
    INITIALIZING,
    INCIDENT,
    NO_CONNECTION,
    STANDBY,
    MANUAL,
    READY,
    AUTOMATIC,
    SHUTDOWN
};

class FrmBase: public Gtk::Window {
public:
    explicit FrmBase(EndpointPtr mhp);
    ~FrmBase() noexcept override = default;

protected:
    std::atomic<SystemState> systemState;

    EndpointPtr msgEndpoint;

    Registry registry;

    // about
    Gtk::Button      m_Button_About{"About..."};
    Gtk::AboutDialog m_Dialog;

    // info-bar
    Gtk::Box         m_InfoBar{Gtk::Orientation::HORIZONTAL, 6};
    Gtk::Button      m_Button_OK{"OK", true};
    Gtk::Label       m_Label_InfoBarMessage;

    Gtk::Button      m_Button_Emergency{"Nothalt"};

    // status label
    Gtk::Label       m_Label_Connectivity_HW{" \xe2\x96\x84"};
    Gtk::Label       m_Label_Connectivity_SW{" \xe2\x96\x84"};

    Gtk::Box         m_HBox_Expander{Gtk::Orientation::HORIZONTAL, 6};

    Gtk::Box         m_HButtonBox{Gtk::Orientation::HORIZONTAL, 6};
    Gtk::Box         m_VBox{Gtk::Orientation::VERTICAL, 6};
    Gtk::Box         m_HBox_Status{Gtk::Orientation::HORIZONTAL, 6};

    std::string getDisplayMessage(std::string caption, std::string text);

    void initAboutDialog();
    void finishForm();

    // message-response
    void setNotice(Gtk::MessageType noticeType, std::string caption, std::string text);
    void handlesendNotification(const MessagingSendNotification &data);
    void handleSetNotificationList(const MessagingSetNotificationList &data) ;
    void handleError(const ClientError &data);
    void handleHardwareState(const SystemHardwareStateChanged &data);

    virtual void setSensitive(bool) {}
    virtual void initialSend() {}
    virtual void setSystemState(SystemState systemState) {}
    virtual void listNotice(
        const std::string &timestamp,
        const std::string &level,
        const std::string &type,
        const std::string &caption,
        const std::string &text,
        const std::string &origin,
        const std::string &source
    ) {};

    // Signal handlers:
    void on_about_dialog_response(int response_id);
    void on_button_about_clicked();
    void on_button_emergency_clicked() const;
    void on_infobar_response();
    bool on_timeout(int timer_number);
    bool on_timeout_status(int);
    void on_window_closing() const;
};

