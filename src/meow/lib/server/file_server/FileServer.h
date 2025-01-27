#pragma once
#pragma GCC optimize("O3")

#include <Arduino.h>
#include <WebServer.h>
#include "../../../manager/files/FileManager.h"

namespace meow
{
    class FileServer
    {

    public:
        enum ServerMode : uint8_t
        {
            SERVER_MODE_RECEIVE = 0,
            SERVER_MODE_SEND
        };

        ~FileServer();

        bool begin(const char *server_dir, ServerMode mode);
        void stop();
        bool isWorking() const { return _is_working; }

        void setSSID(const char *ssid) { _ssid = ssid; }
        void setPWD(const char *pwd) { _pwd = pwd; }
        void setDomainName(const char *domain_name);

        String getAddress() const;
        ServerMode getMode() const { return _mode; }

    private:
        bool _is_working{false};

        String _server_addr;
        String _server_dir;
        String _domain_name;
        String _ssid;
        String _pwd;

        ServerMode _mode;
        WebServer *_server = nullptr;
        bool _must_work = false;

        FileManager _f_mgr;
        FILE *in_file{nullptr};

        static void startWebServer(void *params);
        void fileServerTask(void *params);

        void handleReceive();
        void handleSend();
        void handleFile();
        void handle404();
    };
}