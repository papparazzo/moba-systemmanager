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

class NoticeLogger final : public Gtk::Box {
public:

    NoticeLogger();
    NoticeLogger(const NoticeLogger&) = delete;
    NoticeLogger& operator=(const NoticeLogger&) = delete;

    ~NoticeLogger() noexcept override = default;

    void setNotice(Gtk::MessageType noticeType, std::string caption, std::string text);
    void setNotice(std::string timestamp, std::string type, std::string caption, std::string text);

private:
    struct ModelColumnsNotices : public Gtk::TreeModel::ColumnRecord {
        ModelColumnsNotices() {
            add(m_col_timestamp);
            add(m_col_type);
            add(m_col_caption);
            add(m_col_text);
        }

        Gtk::TreeModelColumn<Glib::ustring> m_col_timestamp;
        Gtk::TreeModelColumn<Glib::ustring> m_col_type;
        Gtk::TreeModelColumn<Glib::ustring> m_col_caption;
        Gtk::TreeModelColumn<Glib::ustring> m_col_text;
    };

    ModelColumnsNotices m_Columns_Notices;

    void on_button_notices_clear_clicked();

    // notice-logger
    Gtk::ScrolledWindow          m_ScrolledWindow_NoticeLogger;
    Gtk::TreeView                m_TreeView_Notices;
    Glib::RefPtr<Gtk::ListStore> m_refTreeModel_Notices;

    Gtk::Box                     m_ButtonBox_NoticeLogger{Gtk::Orientation::HORIZONTAL, 6};
    Gtk::Button                  m_Button_NoticesClear{"Liste leeren"};
};

