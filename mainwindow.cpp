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

#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <unistd.h>

#include <string>
#include <iostream>

#include <QScrollBar>
#include <QPushButton>

using std::to_string;
using std::string;
using sslconn::ChatContext;

Reader::Reader(MainWindow* manWind)
    : mainWindow{manWind},
      running{true},
      context{manWind->getCtx()}
{}

Reader::~Reader(void){}

void Reader::endLoops(void) noexcept{
    running = false;
}

void Reader::run(void) {
    while(running){
         static_cast<void>(usleep(POLLING_INTERVAL));
         if(context.getStatus() == sslconn::connected)
              mainWindow->receiverWrapper();
    }
}

Listener::Listener(MainWindow* manWind)
    : mainWindow{manWind},
      running{true},
      context{manWind->getCtx()}
{}

Listener::~Listener(void){}

void Listener::endLoops(void) noexcept{
    running = false;
}

void Listener::run(void) {
    while(running){
         static_cast<void>(usleep(POLLING_INTERVAL));

         if(context.getStatus() == sslconn::listening){
             if(!mainWindow->listenerWrapper())
                 break;
         }
    }
}

ReturnPress::ReturnPress(QObject *parent)
    : QObject(parent)
{}

bool ReturnPress::eventFilter(QObject *obj, QEvent *event){
    if (event->type() == QEvent::KeyPress) {
        QKeyEvent *key = static_cast<QKeyEvent *>(event);
        if((key->key()==Qt::Key_Enter) || (key->key()==Qt::Key_Return) ) {
             emit returnKeyPressed();
             return true;
        }
    }
    return QObject::eventFilter(obj, event);
}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    returnPress{nullptr},
    connectionStatus{false},
    statusLabel{nullptr},
    connection{context},
    reader{this},
    listener{this}
{

    ui->setupUi(this);
    ui->menuBar->setNativeMenuBar(false);
    diagConf  = new DialogConf(this);
    diagHelp  = new DialogHelp(this);

    connect(ui->quitButton, &QPushButton::clicked,   this, &MainWindow::quitApp);
    connect(ui->clearButton, &QPushButton::clicked,  this, &MainWindow::clearHistory);
    connect(ui->configButton, &QPushButton::clicked, this, &MainWindow::editConfig);

    qRegisterMetaType<std::string>();

    connect(this, &MainWindow::updateMsgSnd,         this, &MainWindow::appendMsgSnd);
    connect(this, &MainWindow::updateMsgRec,         this, &MainWindow::appendMsgRec);
    connect(this, &MainWindow::updateMsgStat,        this, &MainWindow::appendMsgStat);
    connect(this, &MainWindow::updateMsgErr,         this, &MainWindow::appendMsgErr);

    ui->received->setReadOnly(true);

    returnPress=new ReturnPress(this);
    ui->sent->installEventFilter(returnPress);
    connect(returnPress, &ReturnPress::returnKeyPressed, this, &MainWindow::transmit);

    connect(ui->connectButton, &QPushButton::clicked,    this, [&](){ if(connectionStatus)
                                                                         this->disconnectChat();
                                                                      else
                                                                         this->connectChat();
    });

    statusLabel = new QLabel("Disconnected", this);
    statusBar()->addPermanentWidget(statusLabel, 3);

    connect(ui->actionAbout, &QAction::triggered,        this, [&](){diagHelp->exec();});
}

sslconn::ChatContext& MainWindow::getCtx(void){
    return context;
}

void MainWindow::quitApp(void){
    reader.endLoops();
    listener.endLoops();
    static_cast<void>(usleep(POLLING_INTERVAL * 10));
    if(reader.isRunning()) reader.terminate();
    if(listener.isRunning()) listener.terminate();
    this->close();
    std::cerr << "Exit!\n";
}

void MainWindow::clearHistory(void){
    ui->received->clear();
    std::cerr << "Deleted: History!\n";
}

void  MainWindow::editConfig(void){
    diagConf->exec();
}

void  MainWindow::appendMsgSnd(const string& prompt){
        screenMtx.lock();
        ui->received->appendPlainText(prompt.c_str());
        ui->received->appendPlainText(ui->sent->toPlainText());
        ui->received->appendPlainText(" ");
        ui->received->verticalScrollBar()->setValue(ui->received->verticalScrollBar()->maximum());
        screenMtx.unlock();
}

void  MainWindow::appendMsgRec(const string& prompt, const string& msg){
        screenMtx.lock();
        ui->received->appendPlainText(prompt.c_str());
        ui->received->appendPlainText(msg.c_str());
        ui->received->appendPlainText(" ");
        ui->received->verticalScrollBar()->setValue(ui->received->verticalScrollBar()->maximum());
        screenMtx.unlock();
}

void  MainWindow::appendMsgStat(void){
    if(context.getInfoMsg().size() != 0){
        screenMtx.lock();
        ui->received->appendPlainText(INFO_PROMPT);
        ui->received->appendPlainText(context.getInfoMsg().c_str());
        ui->received->appendPlainText(" ");
        ui->received->verticalScrollBar()->setValue(ui->received->verticalScrollBar()->maximum());
        screenMtx.unlock();
    }
}

void  MainWindow::appendMsgErr(const string& err){
        screenMtx.lock();
        statusLabel->setText(err.c_str());
        ui->received->appendPlainText(context.getErrMsg().c_str());
        ui->received->verticalScrollBar()->setValue(ui->received->verticalScrollBar()->maximum());
        screenMtx.unlock();
}

bool  MainWindow::connectChat(void){
    context.setPwd(diagConf->getPassword());
    context.setPort(to_string(diagConf->getPort()));
    context.setIp(diagConf->getIpAddress());
    context.setServer(diagConf->getServerMode() ? sslconn::SERVER : sslconn::CLIENT);

    connectionStatus = true;

    if(connection.configure()){
        ui->connectButton->setText("Disconnect");
        statusLabel->setText("Connected");
    } else {
        updateMsgErr("Connect Error");
    }

    reader.start();

    if(diagConf->getServerMode()){
         std::cerr << "Starting Listener!\n";
         listener.start();
    }

    updateMsgStat();

    return true;
}

void MainWindow::disconnectChat(void){
    ui->connectButton->setText("Connect");
    statusLabel->setText("Disconnected");
    connectionStatus = false;
}

void MainWindow::transmit(void){
    updateMsgSnd("me:");
    if(connection.sendMessage(ui->sent->toPlainText().toStdString()))
       ui->sent->clear();
    else
       updateMsgErr("Error Sending Msg");
}

void  MainWindow::receiverWrapper(void){
    receive(context.getResponse());
}

bool  MainWindow::listenerWrapper(void){
    bool  ret  {  connection.listenIncoming() };

    if(!ret)
        updateMsgErr("Listener Error.");

    return ret;
}

void MainWindow::receive(const QString msg){
    bool res  { connection.readIncoming() };
    if(res)
       updateMsgRec("peer: ", msg.toStdString());
    else
       updateMsgErr("Error Reading Msg");

}

void MainWindow::receive(const char* msg){
    bool res { connection.readIncoming() };
    if(res)
        updateMsgRec("peer: ", msg);
    else
        updateMsgErr("Error Reading Msg");
}

MainWindow::~MainWindow(){

    delete ui;
}
