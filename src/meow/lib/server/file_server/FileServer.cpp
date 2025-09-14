#pragma GCC optimize("O3")
#include "FileServer.h"
#include "./tmpl_html.cpp"
#include "meow/manager/WiFiManager.h"
#include "meow/manager/FileManager.h"


namespace meow
{
    FileServer::~FileServer()
    {
        stop();
    }

    bool FileServer::begin(const char *server_dir, ServerMode mode)
    {
        if (_is_working)
            return false;

        _server_dir = server_dir;

        if (!_server_dir.equals("/") && _server_dir.endsWith("/"))
            _server_dir.remove(1, -1);

        if (_server_dir.isEmpty())
            _server_dir = "/";

        if (!_sd.isMounted())
            return false;

        if (!_fs.dirExist(_server_dir.c_str()))
            return false;

        _server_ip = "http://";

        if (!_wifi.isConnected())
        {
            if (_ssid.isEmpty())
            {
                log_e("%s", STR_ERR_ROUTER_NOT_CONNECTED);
                return false;
            }
            else
            {
                _wifi.tryConnectTo(_ssid, _pwd);
                vTaskDelay(3000 / portTICK_PERIOD_MS);
            }

            if (!_wifi.isConnected())
            {
                log_e("Помилка підключення. Створюю власну AP");
                if (!_wifi.createAP(_ssid, _pwd, 1))
                    return false;

                _server_ip += _wifi.getAPIP();
            }
            else
            {
                _server_ip += _wifi.getLocalIP();
            }
        }
        else
        {
            _server_ip += _wifi.getLocalIP();
        }

        log_i("File server addr: %s", _server_ip.c_str());

        _server_mode = mode;

        BaseType_t result = xTaskCreatePinnedToCore(startWebServer, "fileServerTask", (1024 / 2) * 20, this, 10, NULL, 1);

        if (result == pdPASS)
        {
            _is_working = true;

            log_i("File server is working now");

            if (_server_mode == SERVER_MODE_RECEIVE)
                log_i("mode == SERVER_MODE_RECEIVE");
            else
                log_i("mode == SERVER_MODE_SEND");

            return true;
        }
        else
        {
            log_e("fileServerTask was not running");

            if (_wifi.isApEnabled())
                _wifi.disable();

            return false;
        }
    }

    void FileServer::stop()
    {
        if (!_is_working)
            return;

        _must_work = false;

        _server->close();

        if (_wifi.isApEnabled())
            _wifi.disable();

        delete _server;
    }

    String FileServer::getAddress() const
    {
        if (_is_working)
            return _server_ip;
        else
            return emptyString;
    }

    void FileServer::fileServerTask(void *params)
    {
        _server = new WebServer(80);

        if (_server_mode == SERVER_MODE_RECEIVE)
        {
            _server->on("/", HTTP_GET, [this]()
                        { this->handleReceive(); });
            _server->on("/upload", HTTP_POST, []() {}, [this]()
                        { this->handleFile(); });
        }
        else
        {
            _server->on("/", HTTP_GET, [this]()
                        { this->handleSend(); });
        }

        _server->onNotFound([this]()
                            { this->handle404(); });


        _server->begin();

        // cppcheck-suppress-begin knownConditionTrueFalse
        _must_work = true; 
        while (_must_work) 
        {
            _server->handleClient();
            vTaskDelay(1 / portTICK_PERIOD_MS);
        }
        // cppcheck-suppress-end knownConditionTrueFalse

        _is_working = false;
        vTaskDelete(NULL);
    }

    void FileServer::handleReceive()
    {
        String html = HEAD_HTML;
        html += RECEIVE_TITLE_STR;
        html += RECEIVE_FILE_HTML;
        _server->sendHeader("Cache-Control", "no-cache");
        _server->send(200, "text/html", html);
    }

    void FileServer::handleSend()
    {
        if (_server->args() > 0)
        {
            String path = _server_dir;
            path += "/";
            path += _server->arg(0);

            FILE *file = _fs.openFile(path.c_str(), "rb");

            if (!file || !_fs.fileExist(path.c_str()))
                handle404();
            else
            {
                size_t f_size = _fs.getFileSize(path.c_str());
                FileStream f_stream(file, _server->arg(0).c_str(), f_size);

                _server->sendHeader("Content-Type", "application/force-download");
                _server->sendHeader("Content-Disposition", "attachment; filename=\"" + _server->arg(0) + "\"");
                _server->sendHeader("Content-Transfer-Encoding", "binary");
                _server->sendHeader("Cache-Control", "no-cache");
                _server->streamFile(f_stream, "application/octet-stream");
            }
        }
        // Якщо відсутні параметри, відобразити список файлів в директорії
        else
        {
            if (!_fs.dirExist(_server_dir.c_str()))
            {
                log_e("Помилка відкриття директорії %s", _server_dir.c_str());
                _server->send(500, "text/html", "");
                return;
            }

            String html = HEAD_HTML;
            html += SEND_TITLE_STR; // Заголовок
            html += MID_HTML;

            std::vector<FileInfo> f_infos;
            _fs.indexFiles(f_infos, _server_dir.c_str());

            for (auto &info : f_infos)
            {
                html += HREF_HTML;
                html += info.getName();
                html += "\">";
                html += info.getName();
                html += "</a>";
            }

            html += FOOT_HTML;
            _server->sendHeader("Cache-Control", "no-cache");
            _server->send(200, "text/html", html);
        }
    }

    void FileServer::handleFile()
    {
        HTTPUpload &uploadfile = _server->upload();
        _server->setContentLength(CONTENT_LENGTH_UNKNOWN);

        if (uploadfile.status == UPLOAD_FILE_START)
        {
            String file_name = _server_dir;
            file_name += "/";
            file_name += uploadfile.filename;

            log_i("Запит на створення файлу %s", file_name.c_str());

            _fs.closeFile(in_file);

            if (_fs.exists(file_name.c_str(), true))
            {
                String temp_name = file_name;
                temp_name += "_copy";

                while (_fs.fileExist(temp_name.c_str(), true))
                    temp_name += "_copy";

                file_name = temp_name;
            }

            in_file = _fs.openFile(file_name.c_str(), "ab");

            if (!in_file)
            {
                log_e("Не можу відкрити файл %s на запис", file_name.c_str());
                _server->send(500, "text/html", "");
                return;
            }

            _last_delay_ts = millis();
        }
        else if (uploadfile.status == UPLOAD_FILE_WRITE)
        {
            _fs.writeToFile(in_file, static_cast<const void *>(uploadfile.buf), uploadfile.currentSize);
            if (millis() - _last_delay_ts > 1000)
            {
                vTaskDelay(1 / portTICK_PERIOD_MS);
                _last_delay_ts = millis();
            }
        }
        else if (uploadfile.status == UPLOAD_FILE_END || uploadfile.status == UPLOAD_FILE_ABORTED)
        {
            if (in_file)
            {
                _fs.closeFile(in_file);

                handleReceive();

                if (uploadfile.status == UPLOAD_FILE_END)
                    log_i("Файл отримано");
                else
                    log_i("Завантаження перервано");
            }
            else
            {
                log_e("Необроблений файл");
                _server->send(500, "text/html", "");
            }
        }
    }

    void FileServer::handle404()
    {
        String html = HEAD_HTML;
        html += NOT_FOUND_TITLE_STR;
        html += MID_HTML;
        html += NOT_FOUND_BODY_START;
        html += _server_ip;
        html += NOT_FOUND_BODY_END;
        html += FOOT_HTML;
        _server->send(404, "text/html", html);
    }

    void FileServer::startWebServer(void *params)
    {
        FileServer *instance = static_cast<FileServer *>(params);
        instance->fileServerTask(params);
        log_i("FileServer task finished");
        vTaskDelete(NULL);
    }
}