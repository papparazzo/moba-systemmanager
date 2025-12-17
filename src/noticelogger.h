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
#include <string>

#include "moba/endpoint.h"

class NoticeLogger final : public Gtk::Box {
public:

    explicit NoticeLogger(EndpointPtr msgEndpoint);
    NoticeLogger(const NoticeLogger&) = delete;
    NoticeLogger& operator=(const NoticeLogger&) = delete;

    ~NoticeLogger() noexcept override = default;

    void setNotice(
        const std::string &timestamp,
        const std::string &level,
        const std::string &type,
        const std::string &caption,
        const std::string &text,
        const std::string &origin,
        const std::string &source
    );

    void clearList();

private:
    struct ModelColumnsNotices final : Gtk::TreeModel::ColumnRecord {
        ModelColumnsNotices() {
            add(m_col_timestamp);
            add(m_col_level);
            add(m_col_type);
            add(m_col_caption);
            add(m_col_text);
            add(m_col_origin);
            add(m_col_source);
        }

        Gtk::TreeModelColumn<Glib::ustring> m_col_timestamp;
        Gtk::TreeModelColumn<Glib::ustring> m_col_level;
        Gtk::TreeModelColumn<Glib::ustring> m_col_type;
        Gtk::TreeModelColumn<Glib::ustring> m_col_caption;
        Gtk::TreeModelColumn<Glib::ustring> m_col_text;
        Gtk::TreeModelColumn<Glib::ustring> m_col_origin;
        Gtk::TreeModelColumn<Glib::ustring> m_col_source;
    };

    ModelColumnsNotices m_Columns_Notices;

    void on_button_notices_clear_clicked() const;

    // notice-logger
    Gtk::ScrolledWindow          m_ScrolledWindow_NoticeLogger;
    Gtk::TreeView                m_TreeView_Notices;
    Glib::RefPtr<Gtk::ListStore> m_refTreeModel_Notices;

    Gtk::Box                     m_ButtonBox_NoticeLogger{Gtk::Orientation::HORIZONTAL, 6};
    Gtk::Button                  m_Button_NoticesClear{"Liste leeren"};

    EndpointPtr msgEndpoint;
};

