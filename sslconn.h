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

#include <openssl/bio.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

#include <unistd.h>
#include <stdlib.h>

#include <vector>
#include <string>

#define SMALL_BUFFER 64
#define MEDIUM_BUFFER 256
#define BIG_BUFFER 2048
#define HUGE_BUFFER 10240

#define ERROR_PROMPT "Error:---> "
#define INFO_PROMPT "Info:---> "
#define LOCAL_PROMPT "Local:---> "
#define REMOTE_PROMPT "Remote:---> "

#define POLLING_INTERVAL 100000

namespace  sslconn {

enum Status   { inactive, connected, listening, error };
enum Conntype { CLIENT, SERVER, UNDEFINED };

using PasswdVect  = const std::vector<char>&;

class ChatContext{
    friend class SslConn;
    friend class SslConnReader;
    friend class SslConnListener;

public:
    ChatContext(void);

    PasswdVect              getPwd(void)                  const noexcept;
    const char*             getResponse(void)             const noexcept;
    Status                  getStatus(void)               const noexcept;
    const std::string&      getErrMsg(void)               const noexcept;
    const std::string&      getInfoMsg(void)              const noexcept;

    void        setIp(const std::string& par)                   noexcept;
    void        setPort(const std::string& par)                 noexcept;
    void        setPwd(const std::string& par)                  noexcept;
    void        setServer(Conntype mod)                         noexcept;
    void        appendInfo(const char* const msg)               noexcept;
    void        appendInfo(const std::string& msg)              noexcept;

private:

    Conntype           connectionMode;

    BIO                *biop,
                       *abiop,
                       *mbiop;
    SSL_CTX            *ctxp;
    SSL                *sslp;
    std::string        configIP,
                       sConfigPort,
                       errMessage,
                       infoMessage,
                       baseDir,
                       handShakeSummary,
                       blackList;
    std::vector<char>  incomingBufferp,
                       errBuffer,
                       password;
    Status             status;                // Status: Valid values:
                                              // inactive, connected, listening.
};

class SslConn {
    public:
        explicit SslConn(ChatContext& ctx);
        ~SslConn(void);

        bool            sendMessage(const std::string& msg)                 noexcept;
        bool            configure(void)                                     noexcept;
        void            cleanContext(void)                                  noexcept;
        std::string     getSslError(unsigned long errCode)       const      noexcept;
        bool            listenIncoming(void)                                noexcept;
        bool            readIncoming(void)                                  noexcept;

    private:

        ChatContext&    context;
        bool            errStatus;

        bool            setClientMode(void)                                 noexcept;
        bool            setContext(void)                                    noexcept;
        bool            setServerMode(void)                                 noexcept;
        void            setErrMsg(const char* const msg,
                                  bool clearBuff=true)                      noexcept;
        void            setErrMsg(const std::string& msg,
                                  bool clearBuff=true)                      noexcept;
        std::string     getSslErrStrings(void)                   const      noexcept;
};

} // End namespace sslconn
