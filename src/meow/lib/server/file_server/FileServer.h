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

        enum NetMode : uint8_t
        {
            NET_MODE_AP = 0,
            NET_MODE_LOCAL
        };

        // TODO docs

        ~FileServer();

        bool begin(const char *server_dir, ServerMode mode);
        void stop();
        bool isWorking() const { return _is_working; }

        void setSSID(const char *ssid) { _ssid = ssid; }
        void setPWD(const char *pwd) { _pwd = pwd; }
        void setNetMode(NetMode mode) { _net_mode = mode; }
        NetMode getNetMode() const { return _net_mode; }

        String getAddress() const;
        ServerMode getServerMode() const { return _server_mode; }

    private:
        bool _is_working{false};

        unsigned long _last_delay_time = 0;

        String _server_ip;
        String _server_dir;
        String _ssid;
        String _pwd;

        ServerMode _server_mode{SERVER_MODE_RECEIVE};
        NetMode _net_mode{NET_MODE_AP};
        //;
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