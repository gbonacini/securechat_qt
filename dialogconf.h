// -----------------------------------------------------------------
// securechat_qt - an encrypted chat using OpenSSL, with a QT interface
// Copyright (C) 2019  Gabriele Bonacini
//
// This program is free software for no profit use; you can redistribute
// it and/or modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2 of
// the License, or (at your option) any later version.
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software Foundation,
// Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301  USA
// A commercial license is also available for a lucrative use.
// -----------------------------------------------------------------

#pragma once

#include <QDialog>
#include <QString>
#include <QIntValidator>

#include <cstdint>
#include <string>

#include "sslconn.h"

namespace Ui {
class DialogConf;
}

class DialogConf : public QDialog
{
    Q_OBJECT

public:
    explicit DialogConf(QWidget *parent = nullptr);
    ~DialogConf();

    std::string    getIpAddress(void)     const;
    std::string    getPassword(void)      const;
    uint16_t       getPort(void)          const;
    bool           getServerMode(void)    const;

private:
    Ui::DialogConf *ui;
    QString        ipAddress,
                   ipAddressOct1,
                   ipAddressOct2,
                   ipAddressOct3,
                   ipAddressOct4,
                   password;
    uint16_t       port;
    bool           serverMode;

    bool           setConfig(void);
    void           done(int r);
};
