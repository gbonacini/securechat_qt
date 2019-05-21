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

#include "sslconn.h"

#include <algorithm>
#include <iostream>
#include <cstring>

#include "types.h"

namespace  sslconn {

    using std::string;
    using std::vector;
    using std::string;
    using std::to_string;
    using std::fill;
    using std::strlen;
    using std::strcpy;

    using typeutils::safeInt;

    ChatContext::ChatContext(void)
       :    connectionMode{UNDEFINED},
            biop{nullptr},
            abiop{nullptr},
            mbiop{nullptr},
            ctxp{nullptr},
            sslp{nullptr},
            configIP{""},
            sConfigPort{""},
            errMessage{"None"},
            infoMessage{""},
            baseDir{""},
            handShakeSummary{""},
            blackList{"!aNULL:!eNULL:!3DES:!SHA1:!EXPORT:!EXPORT56:MEDIUM"},
            incomingBufferp(MEDIUM_BUFFER, 0),
            errBuffer(MEDIUM_BUFFER, 0),
            password(MEDIUM_BUFFER, 0),
            status{inactive}
    {
        const string envvar    {"SCBLACKLIST"};
        const char   *envconf  {getenv(envvar.c_str())};
        if(envconf != nullptr)
            blackList  = envconf;
    }

    PasswdVect ChatContext::getPwd(void) const noexcept{
        return password;
    }

    const char*  ChatContext::getResponse(void) const noexcept{
        return  incomingBufferp.data();
    }

    Status   ChatContext::getStatus(void) const noexcept{
        return status;
    }

    const string&  ChatContext::getErrMsg(void)  const noexcept{
        return errMessage;
    }

    const string&  ChatContext::getInfoMsg(void)  const noexcept{
        return infoMessage;
    }

    void ChatContext::setIp(const string& par) noexcept{
       configIP = par;
    }

    void ChatContext::setPort(const string& par) noexcept{
       sConfigPort = par;
    }

    void ChatContext::setPwd(const string& par) noexcept{
       fill(password.begin(), password.end(), 0);
       password.insert(password.begin(), par.begin(), par.end());
    }

    void ChatContext::setServer(Conntype mod) noexcept{
        connectionMode = mod;
    }

    void  ChatContext::appendInfo(const char* const msg) noexcept{
        infoMessage.append(msg);
    }

    void  ChatContext::appendInfo(const string&  msg) noexcept{
        infoMessage.append(msg);
    }

    SslConn::SslConn(ChatContext& ctx)
        : context{ctx},
          errStatus{false}
    {
        SSL_load_error_strings();
        ERR_load_BIO_strings();
        ERR_load_SSL_strings();
        OpenSSL_add_all_algorithms();
        static_cast<void>(SSL_library_init());
    }

    SslConn::~SslConn(void){
        if(context.biop != nullptr){
            #pragma clang diagnostic push
            #pragma clang diagnostic ignored "-Wold-style-cast"

            static_cast<void>(BIO_flush(context.biop));
            static_cast<void>(BIO_reset(context.biop));

            #pragma clang diagnostic pop
        }
    }

    string SslConn::getSslErrStrings(void) const noexcept{

         string res;

         try{
             BIO  *bio  { BIO_new (BIO_s_mem ()) };
             char *buf  { nullptr };

             ERR_print_errors (bio);

             #pragma clang diagnostic push
             #pragma clang diagnostic ignored "-Wold-style-cast"

             static_cast<void>(BIO_get_mem_data (bio, &buf));

             #pragma clang diagnostic pop

             res   =  buf;

             BIO_free (bio);
         }catch(...){
             res = "getSslErrStrings error.";
         }

         return res;
    }

    void  SslConn::cleanContext(void) noexcept{
        if(context.biop!=nullptr){
            BIO_free_all(context.biop);
            context.biop=nullptr;
        }
        if(context.mbiop!=nullptr){
            BIO_free_all(context.mbiop);
            context.mbiop=nullptr;
        }
        if(context.abiop!=nullptr){
            free(context.abiop);
            context.abiop=nullptr;
        }
        if(context.ctxp!=nullptr){
            SSL_CTX_free(context.ctxp);
            context.ctxp=nullptr;
        }

        context.status  =  inactive;
    }

    void   SslConn::setErrMsg(const char* const msg, bool clearBuff )  noexcept{
         if(clearBuff) context.errMessage.clear();
         context.errMessage.append(ERROR_PROMPT).append(msg);
    }

    void   SslConn::setErrMsg(const string& msg, bool clearBuff )  noexcept{
         if(clearBuff) context.errMessage.clear();
         context.errMessage.append(ERROR_PROMPT).append(msg);
    }

    bool SslConn::configure(void) noexcept{
        bool ret  { true };
        if(context.status==inactive && context.connectionMode == CLIENT){
               if(!setClientMode()){
                    setErrMsg("Error setting client mode", false);
                    ret  =  false;
               }
        }else if(context.status==inactive && context.connectionMode == SERVER){
               if(!setServerMode()){
                    setErrMsg("Error setting server mode", false);
                    ret  =  false;
               }
        }else{
               setErrMsg("Already Connected.");
               ret  =  false;
        }

        return true;
    }

    string  SslConn::getSslError(unsigned long errCode) const noexcept{
        try{
           string errMsg(ERR_error_string(errCode, nullptr));
           return errMsg;
        }catch(...){
            return string("Error getting SSL error string.");
        }
    }

    bool  SslConn::sendMessage(const string& msg) noexcept{

        if( context.status  ==  connected) {

            static_cast<void>(BIO_write(context.biop, msg.c_str(), safeInt(msg.size())));

            #pragma clang diagnostic push
            #pragma clang diagnostic ignored "-Wold-style-cast"

            static_cast<void>(BIO_flush(context.biop));

            #pragma clang diagnostic pop

        } else {
             errStatus   =  true;
             setErrMsg("Unconnected.");

             return false;
        }

        return true;
    }

    bool  SslConn::setContext(void) noexcept {
        bool ret { true };

        string       trustStore,
                     serverPem,
                     serverKey;

        context.baseDir.append(getenv("HOME")).append("/.securechat/");

        if(context.connectionMode == CLIENT){
            context.ctxp = SSL_CTX_new(SSLv23_client_method());
            if(context.ctxp == nullptr ){
                setErrMsg("SSL context failure.");
                ret  =  false;
            }

            trustStore.append(context.baseDir).append("TrustStore.pem");

            if(ret){
                if(SSL_CTX_load_verify_locations(context.ctxp, trustStore.data(), nullptr)==0){
                    cleanContext();
                    setErrMsg("Error loading trust store");
                    ret  =  false;
                }

                context.biop = BIO_new_ssl_connect(context.ctxp);
            }

        }else{
            context.ctxp = SSL_CTX_new(SSLv23_server_method());
            if(context.ctxp == nullptr){
                setErrMsg(" SSL_CTX_new failed. Aborting.");
                ret  =  false;
            }

            if(ret){
                int (*callback)(char *, int, int, void *) = [](char *buf, int size, int rwflag, void *chatContext)
                               {
                                     static_cast<void>(size);
                                     static_cast<void>(rwflag);

                                     ChatContext *context  { reinterpret_cast<ChatContext*>(chatContext) };

                                     if(strlen(context->getPwd().data()) != 0)
                                          strcpy(buf, context->getPwd().data());
                                     else
                                          strcpy(buf, "dummy");

                                     return 1;
                                };

                SSL_CTX_set_default_passwd_cb(context.ctxp, callback);
                SSL_CTX_set_default_passwd_cb_userdata(context.ctxp, reinterpret_cast<void*>(&context));

                serverPem.append(context.baseDir).append("server.pem");

                if(SSL_CTX_use_certificate_file(context.ctxp, serverPem.data(), SSL_FILETYPE_PEM)!=1){
                    setErrMsg(string("Failed SSL_CTX_use_certificate_file: ").append(getSslErrStrings()));
                    cleanContext();
                    ret  =  false;
                }
            }

            if(ret){
                serverKey.append(context.baseDir).append("server.key");

                if(SSL_CTX_use_PrivateKey_file(context.ctxp, serverKey.data(), SSL_FILETYPE_PEM)!=1){
                    setErrMsg(string("Failed SSL_CTX_use_PrivateKey_file.").append(getSslErrStrings()));
                    cleanContext();
                    ret  =  false;
                }
            }

            if(ret){
                context.mbiop = BIO_new_ssl(context.ctxp, 0);
                if(context.mbiop == nullptr){
                    setErrMsg(string("Context creation error: BIO_new_ssl() failed. Aborting.\n").append(getSslErrStrings()));
                    cleanContext();
                    ret  =  false;
                }
            }
        }

        return ret;
    }

    bool SslConn::setClientMode(void) noexcept{
        bool localStatus { true };
        int  bits        {  0   };

        if(setContext()){
            #pragma clang diagnostic push
            #pragma clang diagnostic ignored "-Wold-style-cast"

            static_cast<void>(BIO_get_ssl(context.biop, &(context.sslp)));
            static_cast<void>(SSL_set_mode(context.sslp, SSL_MODE_AUTO_RETRY));

            static_cast<void>(BIO_set_conn_hostname(context.biop, context.configIP.c_str()));
            static_cast<void>(BIO_set_conn_port(context.biop, context.sConfigPort.c_str()));
            #pragma clang diagnostic pop

            if(BIO_do_connect(context.biop) <= 0){
                setErrMsg("Set Client Mode: Error attempting to connect");
                localStatus = false;
            }else{
                // Check the certificate
                if(SSL_get_verify_result(context.sslp) != X509_V_OK){
                    setErrMsg(string("Certificate verification error: ").append(to_string( SSL_get_verify_result(context.sslp))));
                    localStatus = false;
                }
            }

            if(localStatus){
                #pragma clang diagnostic push
                #pragma clang diagnostic ignored "-Wold-style-cast"

                context.handShakeSummary.clear();
                context.handShakeSummary.append("Info - ").append(SSL_state_string_long((const SSL*)context.sslp))\
                                        .append(" - Algorithms: ").append(SSL_get_cipher_name((const SSL*)context.sslp))\
                                        .append(" - Algorithm bits: ").append(to_string( SSL_get_cipher_bits((const SSL*)context.sslp, &bits)))\
                                        .append(" - Connection Protocol Version: ").append(SSL_get_version((const SSL*)context.sslp));

                #pragma clang diagnostic pop

                context.appendInfo(context.handShakeSummary.c_str());
                context.status = connected;
            }else{
                cleanContext();
                context.status = error;
            }
        }
        return localStatus;
    }

    bool  SslConn::setServerMode(void) noexcept{
        bool         status             { true };
        vector<char> connectionString;

        connectionString.clear();
        connectionString.insert(connectionString.end(), context.configIP.begin(), context.configIP.end());
        connectionString.push_back(':');
        connectionString.insert(connectionString.end(), context.sConfigPort.begin(), context.sConfigPort.end());
        connectionString.push_back(0);

        context.appendInfo("Connection String: ");
        context.appendInfo(connectionString.data());

        if(setContext()){
            #pragma clang diagnostic push
            #pragma clang diagnostic ignored "-Wold-style-cast"

            static_cast<void>(BIO_get_ssl(context.mbiop, &(context.sslp)));
            static_cast<void>(SSL_set_mode(context.sslp, SSL_MODE_AUTO_RETRY));
            context.abiop = BIO_new_accept(connectionString.data());
            static_cast<void>(BIO_set_accept_bios(context.abiop, context.mbiop));

            #pragma clang diagnostic pop

            if(BIO_do_accept(context.abiop) <= 0){
                setErrMsg("Accept failed.");
                status = false;
            }

            if(status)
                context.status = listening;
            else
                cleanContext();
        }

        return status;
    }

    bool SslConn::readIncoming(void)  noexcept{
        int  incomingSize  {  0  };
        bool  ret          {  true };

        if(context.status==connected){
            fill(context.incomingBufferp.begin(), context.incomingBufferp.end(), 0);

            incomingSize  =  BIO_read(context.biop, context.incomingBufferp.data(), safeInt(context.incomingBufferp.size() - 1));

            if(incomingSize <= 0){
                setErrMsg(string("BIO_read() error: ").append(to_string(incomingSize)).append("\n"));
                ret = false;
            }

            if(incomingSize==0){
                errStatus   =  true;
                setErrMsg("Client sent eof.");

                #pragma clang diagnostic push
                #pragma clang diagnostic ignored "-Wold-style-cast"

                if(context.biop   !=  nullptr) static_cast<void>(BIO_reset(context.biop));
                if(context.abiop  !=  nullptr) static_cast<void>(BIO_reset(context.abiop));
                if(context.mbiop  !=  nullptr) static_cast<void>(BIO_reset(context.mbiop));

                #pragma clang diagnostic pop

                context.status=inactive;
            }
        }

        return ret;
    }

    bool  SslConn::listenIncoming(void) noexcept{
        bool         ret        {  true  };

        if(BIO_do_accept(context.abiop) <= 0){
            setErrMsg(string("BIO_do_accept error:").append(getSslErrStrings()));
            ret  =  false;
            cleanContext();
        }else{
            context.biop = BIO_pop(context.abiop);

            if(BIO_do_handshake(context.biop) > 0){
                context.status=connected;

                #pragma clang diagnostic push
                #pragma clang diagnostic ignored "-Wold-style-cast"

                SSL          *tempSsl  {  nullptr };
                static_cast<void>(BIO_get_ssl(context.biop, &tempSsl));

                #pragma clang diagnostic pop

                SSL_CIPHER   *cipher  { const_cast<SSL_CIPHER*>(SSL_get_current_cipher(tempSsl)) };

                if(cipher!=nullptr){
                    vector<char> buffer(MEDIUM_BUFFER, 0);
                    SSL_CIPHER_description(cipher,buffer.data(), safeInt(buffer.size() - 1));
                    context.handShakeSummary.clear();
                    context.handShakeSummary.append("Info - ").append(SSL_state_string_long(static_cast<const SSL*>(context.sslp)))\
                                            .append(" - Algorithms: ").append(cipher->name)\
                                            .append(" - Algorithm bits: ").append(to_string( cipher->alg_bits))\
                                            .append(" - Connection Protocol Version: ").append(SSL_get_version(reinterpret_cast<const SSL*>(context.sslp)));

                    context.appendInfo(context.handShakeSummary.c_str());
                }else{
                    int       bits  {  0  };
                    string    buffer;
                    context.appendInfo("No cipher informations.");
                    context.appendInfo(SSL_get_cipher(static_cast<const SSL*>(context.sslp)));
                    context.appendInfo("Bits:");
                    SSL_CIPHER_get_bits((SSL_get_current_cipher(static_cast<const SSL*>(context.sslp))), &bits);
                    buffer.append("#").append(to_string(bits)).append("\n");
                    context.appendInfo(buffer);
                }
            }else{
                cleanContext();
                setErrMsg("Handshake failed.");
                ret  =  false;
            }
        }

        return ret;
    }

} // End namespace sslconn
