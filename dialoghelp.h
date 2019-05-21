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

#define SECURECHAT_INFO_MSG      \
"<html>"\
"   <header>"\
"       <title>"\
"          General Information"\
"       </title>"\
"   </header> "\
"   <body> "\
"        <h3>Description</h3>"\
"            SecurechatsQt is a porting of Securechat is a text-based client/server chat utility that uses ssl/tls (openssl) to establish an encrypted communication between the two parties. "\
"            It wants to be a \"modern\" version of the talk program, the historical text chat program on Unix."\
"        <h3>Dependencies</h3>"\
"            This program requires:<BR><BR> - libssl1.0-dev <BR><BR> or equivalent package for your distribution."\
"            I'm planning an upgrade to 1.1 as soon I have free time."\
"         <h3>Configuration</h3>"\
"            The program requires the generation of some certificates to operate:<BR><BR>"\
"           <h4>* Server Certificates Configuration:<h4>"\
"               <h5>** Create Certification Authority</h5>"\
"                  $ mkdir $HOME/.securechat<BR>"\
"                  $ cd $HOME/.securechat<BR>"\
"                  $ openssl req -out ca.pem -new -x509<BR>"\
"               <h5>** Server Private Key</h5>"\
"                  $ openssl genrsa -out server.key 4096<BR>"\
"               <h5>** Certificate Request</h5>"\
"                  $ openssl req -key server.key -new -out server.req"\
"               <h5>** Certificate Sequence (a progressive number)</h5>"\
"                  $ echo \"11\" > file.seq<BR>"\
"               <h5>** Public Key</h5>"\
"                  $ openssl x509 -req -in server.req -CA ca.pem -CAkey privkey.pem -CAserial file.seq -out server.pem<BR><BR>"\
"                  Only the owner of the server.key and the other private archives can be able to read or modify those files, check the permissions and change it in case of wrong privileges!"\
"                  $ chmod -R 600 *<BR><BR>"\
"                  <B>Note</B>: This program support the certificate passphrase, if it is used on a shared computer, it is strongly recommended to use this feature."\
"           <h4>* Client Certificates Configuration</h4>"\
"                   Request the server.pem certificate to the server counterpart and insert that file in:<BR><BR>"\
"                   $HOME/.securechat/TrustStore.pem<BR>"\
"                   as explained in the man page.<BR>"\
"        <h3>Using Securechat</h3>"\
"            After the certificates configuration, start the program from the shell, for example:<BR><BR>"\
"            $ ./securechat<BR><BR>"\
"        <B>* Note:</B> Check the $TERM environment variable if the initial screen is not correctly printed."\
"        <h3>More Info</h3>"\
"            See the github page of this project: <BR><BR>"\
"            <a href=\"https://github.com/gbonacini/securechatqt\">securechatqt</a>"\
"   </body>"\
"</html>"


namespace Ui {
class DialogHelp;
}

class DialogHelp : public QDialog
{
    Q_OBJECT

public:
    explicit DialogHelp(QWidget *parent = nullptr);
    ~DialogHelp();

private:
    Ui::DialogHelp *ui;
};
