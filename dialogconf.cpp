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

#include "dialogconf.h"
#include "ui_dialogconf.h"

#include <QMessageBox>
#include <QNetworkInterface>

#include <iostream>

using std::string;

DialogConf::DialogConf(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogConf),
    ipAddress{"127.0.0.1"},
    ipAddressOct1{"127"},
    ipAddressOct2{"0"},
    ipAddressOct3{"0"},
    ipAddressOct4{"1"},
    port{8866},
    serverMode{false}

{
    ui->setupUi(this);

    ui->lineEditIp1->setValidator( new QIntValidator(0, 255, ui->lineEditIp1));
    ui->lineEditIp2->setValidator( new QIntValidator(0, 255, ui->lineEditIp2));
    ui->lineEditIp3->setValidator( new QIntValidator(0, 255, ui->lineEditIp3));
    ui->lineEditIp4->setValidator( new QIntValidator(0, 255, ui->lineEditIp4));

    ui->lineEditPort->setValidator( new QIntValidator(0, 65535, ui->lineEditPort));

    connect(ui->buttonBoxConf, &QDialogButtonBox::accepted, this, [&](){
         if(this->setConfig())
             this->hide();
    });

    connect(ui->buttonBoxConf, &QDialogButtonBox::rejected, this, [&](){
         ui->lineEditIp1->setText(ipAddressOct1);
         ui->lineEditIp2->setText(ipAddressOct2);
         ui->lineEditIp3->setText(ipAddressOct3);
         ui->lineEditIp4->setText(ipAddressOct4);
         ui->lineEditPort->setText(QString::number(port));
         ui->lineEditPwdConf->setText(password);
         ui->lineEditPwd->setText(password);
         serverMode ? ui->radioServer->setChecked(true) : ui->radioServer->setChecked(false);
         ui->lineEditIp1->setStyleSheet("QLineEdit {color: green;}");
         ui->lineEditIp2->setStyleSheet("QLineEdit {color: green;}");
         ui->lineEditIp3->setStyleSheet("QLineEdit {color: green;}");
         ui->lineEditIp4->setStyleSheet("QLineEdit {color: green;}");
         this->hide();
    });

    connect(ui->lineEditPort, &QLineEdit::textChanged, this, [&](){
                       if(!ui->lineEditPort->hasAcceptableInput())
                             ui->lineEditPort->setStyleSheet("QLineEdit {color: red;}");
                       else
                             ui->lineEditPort->setStyleSheet("QLineEdit {color: green;}");
    });

    connect(ui->lineEditIp1, &QLineEdit::textChanged, this, [&](){
                       if(!ui->lineEditIp1->hasAcceptableInput())
                             ui->lineEditIp1->setStyleSheet("QLineEdit {color: red;}");
                       else
                             ui->lineEditIp1->setStyleSheet("QLineEdit {color: green;}");
    });

    connect(ui->lineEditIp2, &QLineEdit::textChanged, this, [&](){
                       if(!ui->lineEditIp2->hasAcceptableInput())
                             ui->lineEditIp2->setStyleSheet("QLineEdit {color: red;}");
                       else
                             ui->lineEditIp2->setStyleSheet("QLineEdit {color: green;}");
    });

    connect(ui->lineEditIp3, &QLineEdit::textChanged, this, [&](){
                       if(!ui->lineEditIp3->hasAcceptableInput())
                             ui->lineEditIp3->setStyleSheet("QLineEdit {color: red;}");
                       else
                             ui->lineEditIp1->setStyleSheet("QLineEdit {color: green;}");
    });

    connect(ui->lineEditIp4, &QLineEdit::textChanged, this, [&](){
                       if(!ui->lineEditIp4->hasAcceptableInput())
                             ui->lineEditIp4->setStyleSheet("QLineEdit {color: red;}");
                       else
                             ui->lineEditIp4->setStyleSheet("QLineEdit {color: green;}");
    });


    ui->lineEditPwd->setEchoMode(QLineEdit::Password);
    ui->lineEditPwdConf->setEchoMode(QLineEdit::Password);
    ui->lineEditPwd->hide();
    ui->lineEditPwdConf->hide();
    ui->labelPwd->hide();
    ui->labelPwdConf->hide();

    connect(ui->radioServer, &QRadioButton::toggled, this, [&](){
                      if( ui->radioServer->isChecked() ){
                          ui->lineEditPwd->show();
                          ui->lineEditPwdConf->show();
                          ui->labelPwd->show();
                          ui->labelPwdConf->show();
                      }else{
                          ui->lineEditPwd->hide();
                          ui->lineEditPwdConf->hide();
                          ui->labelPwd->hide();
                          ui->labelPwdConf->hide();
                      }
    });

    connect(ui->lineEditPwdConf, &QLineEdit::textChanged, this, [&](){
                       if( ui->lineEditPwdConf->text().compare(ui->lineEditPwd->text()) != 0 )
                             ui->lineEditPwdConf->setStyleSheet("QLineEdit {color: red;}");
                       else
                             ui->lineEditPwdConf->setStyleSheet("QLineEdit {color: green;}");
    });
}

DialogConf::~DialogConf(){
    delete ui;
}

string DialogConf::getIpAddress(void) const{
   return  ipAddress.toStdString();
}

string DialogConf::getPassword(void) const{
   return password.toStdString();
}

uint16_t  DialogConf::getPort(void) const{
    return port;
}

bool  DialogConf::getServerMode(void) const{
    return serverMode;
}

void DialogConf::done(int r){
    if(!ui->lineEditPort->hasAcceptableInput() ||
       !ui->lineEditIp1->hasAcceptableInput()  ||
       !ui->lineEditIp2->hasAcceptableInput()  ||
       !ui->lineEditIp3->hasAcceptableInput()  ||
       !ui->lineEditIp4->hasAcceptableInput()  ||
        ui->lineEditPwdConf->text().compare(ui->lineEditPwd->text()) != 0 ||
        ui->lineEditPwd->text().size() == 0   ){
            return;
    }else{
            QDialog::done(r);
    }
}

bool  DialogConf::setConfig(void){

    if(!ui->lineEditPort->hasAcceptableInput()){
        std::cerr << "Invalid port\n";
        QMessageBox::warning(this, tr("Configuration"), tr("Invalid Port."),
                             QMessageBox::Ok);
        return false;
     }

    if(!ui->lineEditIp1->hasAcceptableInput() ){
        std::cerr << "Invalid Ip Address\n";
        QMessageBox::warning(this, tr("Configuration"), tr("Invalid Ip Format: first octet."),
                             QMessageBox::Ok);
        return false;
    }

    if(!ui->lineEditIp2->hasAcceptableInput() ){
        std::cerr << "Invalid Ip Address\n";
        QMessageBox::warning(this, tr("Configuration"), tr("Invalid Ip Format: second octet."),
                             QMessageBox::Ok);
        return false;
    }

    if( !ui->lineEditIp3->hasAcceptableInput() ){
        std::cerr << "Invalid Ip Address\n";
        QMessageBox::warning(this, tr("Configuration"), tr("Invalid Ip Format: third octet."),
                             QMessageBox::Ok);
        return false;
    }

    if( !ui->lineEditIp4->hasAcceptableInput() ){
        std::cerr << "Invalid Ip Address\n";
        QMessageBox::warning(this, tr("Configuration"), tr("Invalid Ip Format: fourth octet."),
                             QMessageBox::Ok);
        return false;
    }

    if( ui->radioServer->isChecked() ){
        if( ui->lineEditPwdConf->text().compare(ui->lineEditPwd->text()) != 0 ){
            QMessageBox::warning(this, tr("Configuration"), tr("Password field and confirmation filed don't match."),
                                 QMessageBox::Ok);
            return false;
        }

        //if( ui->lineEditPwd->text().size() == 0 ){
            //QMessageBox::warning(this, tr("Configuration"), tr("Empty password is forbidden."),
           //                    QMessageBox::Ok);
            //return;
        //}
        password   =  ui->lineEditPwd->text();
    }

    QString ipAddressTst   =   ui->lineEditIp1->text() + "." + ui->lineEditIp2->text() + "." + ui->lineEditIp3->text() + "." + ui->lineEditIp4->text();
    bool    serverModeTst  =   ui->radioServer->isChecked();

    if(serverModeTst){
        bool validServerIp  =  false;
        const QString allIfaces("0.0.0.0");

        if(ipAddressTst == allIfaces){
            validServerIp = true;
        }else{
            QList<QNetworkInterface> interfaces = QNetworkInterface::allInterfaces();
            for (auto iface = interfaces.begin(); iface != interfaces.end(); ++iface){
                QList<QNetworkAddressEntry> entries = iface->addressEntries();
                for (auto address = entries.begin(); address != entries.end(); ++address){
                    if (address->ip().protocol() == QAbstractSocket::IPv4Protocol )
                        if(address->ip().toString() == ipAddressTst)
                            validServerIp = true;
                }
            }
        }

        if( !validServerIp){
            ui->lineEditIp1->setStyleSheet("QLineEdit {color: red;}");
            ui->lineEditIp2->setStyleSheet("QLineEdit {color: red;}");
            ui->lineEditIp3->setStyleSheet("QLineEdit {color: red;}");
            ui->lineEditIp4->setStyleSheet("QLineEdit {color: red;}");
            QMessageBox::warning(this, tr("Configuration"), tr("This address is not valid to start the service."),
                                 QMessageBox::Ok);
            return false;
        }

        ui->lineEditIp1->setStyleSheet("QLineEdit {color: green;}");
        ui->lineEditIp2->setStyleSheet("QLineEdit {color: green;}");
        ui->lineEditIp3->setStyleSheet("QLineEdit {color: green;}");
        ui->lineEditIp4->setStyleSheet("QLineEdit {color: green;}");

    }

    port            =   static_cast<uint16_t>(ui->lineEditPort->text().toInt());
    ipAddressOct1   =   ui->lineEditIp1->text();
    ipAddressOct2   =   ui->lineEditIp2->text();
    ipAddressOct3   =   ui->lineEditIp3->text();
    ipAddressOct4   =   ui->lineEditIp4->text();
    ipAddress       =   ipAddressTst;
    serverMode      =   serverModeTst;

    return true;
}
