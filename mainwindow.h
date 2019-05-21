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

#include "dialogconf.h"
#include "dialoghelp.h"

#include "sslconn.h"

#include <QThread>
#include <QMutex>
#include <QMainWindow>

#include <QString>
#include <QLabel>
#include <QMetaType>

Q_DECLARE_METATYPE(std::string)

namespace Ui {
class MainWindow;
}

class ReturnPress : public QObject {
    Q_OBJECT

protected:
    bool eventFilter(QObject *obj, QEvent *event);

public:
    explicit ReturnPress(QObject *parent=nullptr);

    signals:
        void returnKeyPressed(void);
};

class MainWindow;

class Reader : public QThread  {

public:
    explicit                      Reader(MainWindow* manWind);
    virtual                       ~Reader(void)             override;
    void                          endLoops(void)            noexcept;

private:
    MainWindow*                   mainWindow;
    bool                          running;
    sslconn::ChatContext&         context;

    void                          run(void)                 override;
};

class Listener : public QThread {

public:
    explicit                      Listener(MainWindow* manWind);
    virtual                       ~Listener(void)           override;
    void                          endLoops(void)            noexcept;

private:
    MainWindow*                   mainWindow;
    bool                          running;
    sslconn::ChatContext&         context;

    void                          run(void)                 override;
};

class MainWindow : public QMainWindow {
    Q_OBJECT

public:

    explicit MainWindow(QWidget *parent=nullptr);
    ~MainWindow();

    DialogConf  *diagConf;

private:
    friend Reader;
    friend Listener;

    Ui::MainWindow             *ui;
    ReturnPress                *returnPress;
    QMutex                     screenMtx;
    bool                       connectionStatus;
    QLabel                     *statusLabel;
    DialogHelp                 *diagHelp;
    sslconn::ChatContext       context;
    sslconn::SslConn           connection;
    Reader                     reader;
    Listener                   listener;

    void quitApp(void);
    void clearHistory(void);
    void editConfig(void);

    void receive(const QString msg);
    void receive(const char* msg);
    void receiverWrapper(void);
    bool listenerWrapper(void);

    bool connectChat(void);
    void disconnectChat(void);
    sslconn::ChatContext& getCtx(void);


private slots:
    void transmit(void);

    void appendMsgSnd(const std::string& prompt);
    void appendMsgRec(const std::string& prompt,  const std::string& msg);
    void appendMsgStat(void);
    void appendMsgErr(const std::string& err);

signals:

    void updateMsgSnd(const std::string& prompt);
    void updateMsgRec(const std::string& prompt,  const std::string& msg);
    void updateMsgStat(void);
    void updateMsgErr(const std::string& err);

};
