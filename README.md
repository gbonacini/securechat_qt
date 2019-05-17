Description:
============

Securechat is a QT based client/server chat utility  using  ssl/tls (openssl) to establish an encrypted communication between the two parties. This program is a porting of my chat program securechat from ncurses to QT. <BR>
![alt text](images/qtchat1.png "securechat_qt")

This program is fully interoperable with its ncurses counterpart.<BR>
![alt text](images/qtchat2.png "securechat_qt with securechat")

The ncurses version is also available in my github site.

images qtchat1.png


Dependencies:
-------------

- SSL 1.0 (will be upgraded asap) <BR>
- QT5 <BR>

Dependencies intallation (Ubuntu example):
------------------------------------------

* sudo apt install libssl1.0-dev<BR>
<BR>
* apt-get install qt5-default<BR>


Build:
------

* From the source directory:

   qmake<BR>
   make<BR>

Server Certificates Configuration:
==================================

1. Create Certification Authority:<BR>
$ mkdir $HOME/.securechat <BR>
$ cd $HOME/.securechat <BR>
$ openssl req -out ca.pem -new -x509<BR>

2. Server Private Key:<BR>
$ openssl genrsa -out server.key 4096<BR>

3. Certificate Request:<BR>
$ openssl req -key server.key -new -out server.req<BR>

4. Certificate Sequence (a progressive number): <BR>
$ echo "11" > file.seq<BR>

5. Public Key:<BR>
$ openssl x509 -req -in server.req -CA ca.pem -CAkey privkey.pem -CAserial file.seq -out server.pem

5. Only the owner of the server.key and the other private archives can be able to read or modify those files, check the permissions and change it in case of wrong privileges!<BR>
$ chmod -R 600 * <BR>

* Note: This program support the certificate passphrase, if it is used on a shared computer, it is strongly recommended to use this feature. 

Client Certificates Configuration:
==================================

Request the server.pem certificate to the server counterpart and insert that file in:<BR>
$HOME/.securechat/TrustStore.pem <BR>

