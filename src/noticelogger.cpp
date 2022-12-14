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
#include <sys/timeb.h>

NoticeLogger::NoticeLogger(): Gtk::Box{Gtk::ORIENTATION_VERTICAL, 2} {

    pack_start(m_ScrolledWindow_NoticeLogger);
    pack_end(m_ButtonBox_NoticeLogger, Gtk::PACK_SHRINK);

    // TreeView
    m_ScrolledWindow_NoticeLogger.add(m_TreeView_Notices);
    m_ScrolledWindow_NoticeLogger.set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);

    m_refTreeModel_Notices = Gtk::ListStore::create(m_Columns_Notices);
    m_TreeView_Notices.set_model(m_refTreeModel_Notices);

    m_TreeView_Notices.append_column("Timestamp", m_Columns_Notices.m_col_timestamp);
    m_TreeView_Notices.append_column("Type",      m_Columns_Notices.m_col_type);
    m_TreeView_Notices.append_column("Caption",   m_Columns_Notices.m_col_caption);
    m_TreeView_Notices.append_column("Text",      m_Columns_Notices.m_col_text);

    // HBox
    m_ButtonBox_NoticeLogger.pack_start(m_Button_NoticesClear, Gtk::PACK_EXPAND_WIDGET, 5);
    m_ButtonBox_NoticeLogger.set_layout(Gtk::BUTTONBOX_END);
    m_ButtonBox_NoticeLogger.set_sensitive(false);

    m_Button_NoticesClear.signal_clicked().connect(sigc::mem_fun(*this, &NoticeLogger::on_button_notices_clear_clicked));
}

NoticeLogger::~NoticeLogger() {
}

void NoticeLogger::setNotice(Gtk::MessageType noticeType, std::string caption, std::string text) {
    timeb sTimeB;
    char buffer[25] = "";

    ftime(&sTimeB);
    strftime(buffer, 21, "%d.%m.%Y %H:%M:%S", localtime(&sTimeB.time));

    Gtk::TreeModel::Row row = *(m_refTreeModel_Notices->append());
    row[m_Columns_Notices.m_col_timestamp] = std::string(buffer);
    row[m_Columns_Notices.m_col_caption  ] = caption;
    row[m_Columns_Notices.m_col_text     ] = text;

    switch(noticeType) {
        case Gtk::MESSAGE_ERROR:
            row[m_Columns_Notices.m_col_type] = "ERROR";
            break;

        case Gtk::MESSAGE_WARNING:
            row[m_Columns_Notices.m_col_type] = "WARNING";
            break;

        default:
            row[m_Columns_Notices.m_col_type] = "INFO";
            break;
    }

    std::replace(caption.begin(), caption.end(), '<', '"');
    std::replace(caption.begin(), caption.end(), '>', '"');
    std::replace(text.begin(), text.end(), '<', '"');
    std::replace(text.begin(), text.end(), '>', '"');

    std::stringstream ss;
    ss << "<b>" << caption << "!</b>\n" << text;

//    m_Label_InfoBarMessage.set_markup(ss.str());
//    m_InfoBar.set_message_type(noticeType);
//    m_InfoBar.show();
    m_ButtonBox_NoticeLogger.set_sensitive(true);
}

void NoticeLogger::on_button_notices_clear_clicked() {
    m_ButtonBox_NoticeLogger.set_sensitive(false);
    m_refTreeModel_Notices->clear();
}
