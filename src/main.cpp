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

#include <gtkmm/application.h>

#include <moba-common/helper.h>

#include "frmmain.h"
#include "config.h"
#include "moba/endpoint.h"
#include "moba/socket.h"

namespace {
    moba::AppData appData = {
        PACKAGE_NAME,
        moba::Version{PACKAGE_VERSION},
        __DATE__,
        __TIME__,
        "::1",
        7000
    };
}

int main(int argc, char *argv[]) {

    auto socket = std::make_shared<Socket>(appData.host, appData.port);
    auto endpoint = EndpointPtr{new Endpoint{socket, appData.appName, appData.version, {Message::SERVER, Message::SYSTEM, Message::GUI, Message::TIMER}}};
    auto app = Gtk::Application::create(argc, argv, "org.moba.systemmanager");

    FrmMain frmMain{endpoint};
    frmMain.set_title(appData.appName);
    frmMain.set_border_width(10);
    frmMain.set_default_size(400, 200);
    frmMain.set_icon_name("moba-systemmanager");

    return app->run(frmMain);
}
