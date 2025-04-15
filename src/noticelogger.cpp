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

#include "noticelogger.h"

#include <ctime>
#include <sstream>
#include <iomanip>

NoticeLogger::NoticeLogger(): Gtk::Box{Gtk::Orientation::VERTICAL} {

    append(m_ScrolledWindow_NoticeLogger);
    append(m_ButtonBox_NoticeLogger);

    // TreeView
    m_ScrolledWindow_NoticeLogger.set_child(m_TreeView_Notices);
    m_ScrolledWindow_NoticeLogger.set_policy(Gtk::PolicyType::AUTOMATIC, Gtk::PolicyType::AUTOMATIC);
    m_ScrolledWindow_NoticeLogger.set_expand();

    m_refTreeModel_Notices = Gtk::ListStore::create(m_Columns_Notices);
    m_TreeView_Notices.set_model(m_refTreeModel_Notices);

    m_TreeView_Notices.append_column("Timestamp", m_Columns_Notices.m_col_timestamp);
    m_TreeView_Notices.append_column("Type",      m_Columns_Notices.m_col_type);
    m_TreeView_Notices.append_column("Caption",   m_Columns_Notices.m_col_caption);
    m_TreeView_Notices.append_column("Text",      m_Columns_Notices.m_col_text);

    // HBox
    m_ButtonBox_NoticeLogger.append(m_Button_NoticesClear);
    m_ButtonBox_NoticeLogger.set_halign(Gtk::Align::END);
    m_ButtonBox_NoticeLogger.set_sensitive(false);

    m_Button_NoticesClear.signal_clicked().connect(sigc::mem_fun(*this, &NoticeLogger::on_button_notices_clear_clicked));
}

NoticeLogger::~NoticeLogger() {
}

void NoticeLogger::setNotice(Gtk::MessageType noticeType, std::string caption, std::string text) {
    auto t = std::time(nullptr);
    auto tm = *std::localtime(&t);

    std::ostringstream oss;

    oss << std::put_time(&tm, "%d.%m.%Y %H:%M:%S");

    std::string type;

    switch(noticeType) {
        case Gtk::MessageType::ERROR:
            type = "ERROR";
            break;

        case Gtk::MessageType::WARNING:
            type = "WARNING";
            break;

        default:
            type = "INFO";
            break;
    }
    setNotice(oss.str(), type, caption, text);
}

void NoticeLogger::setNotice(std::string timestamp, std::string type, std::string caption, std::string text) {
    Gtk::TreeModel::Row row = *(m_refTreeModel_Notices->append());
    row[m_Columns_Notices.m_col_timestamp] = timestamp;
    row[m_Columns_Notices.m_col_caption  ] = caption;
    row[m_Columns_Notices.m_col_text     ] = text;
    row[m_Columns_Notices.m_col_type     ] = type;
    m_ButtonBox_NoticeLogger.set_sensitive(true);
}

void NoticeLogger::on_button_notices_clear_clicked() {
    m_ButtonBox_NoticeLogger.set_sensitive(false);
    m_refTreeModel_Notices->clear();
}
