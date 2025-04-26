#pragma GCC optimize("O3")
#include "FileServer.h"
#include "./tmpl_html.cpp"

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

        if (!_f_mgr.isSdMounted())
            return false;

        if (!_f_mgr.dirExist(_server_dir.c_str()))
            return false;

        WiFi.softAP(_ssid, _pwd, 1, 0, 1);
        delay(10);

        IPAddress IP = WiFi.softAPIP();
        _server_addr = "http://";
        _server_addr += IP.toString();

        log_i("AP addr: %s", _server_addr.c_str());

        _mode = mode;

        BaseType_t result = xTaskCreatePinnedToCore(startWebServer, "fileServerTask", (1024 / 2) * 20, this, 10, NULL, 1);

        if (result == pdPASS)
        {
            _is_working = true;

            log_i("File server is working now");

            if (_mode == SERVER_MODE_RECEIVE)
                log_i("mode == SERVER_MODE_RECEIVE");
            else
                log_i("mode == SERVER_MODE_SEND");

            return true;
        }
        else
        {
            log_e("fileServerTask was not running");
            WiFi.disconnect(true);
            WiFi.mode(WIFI_OFF);
            return false;
        }
    }

    void FileServer::stop()
    {
        if (!_is_working)
            return;

        _must_work = false;

        _server->close();

        WiFi.disconnect(true);
        WiFi.mode(WIFI_OFF);

        delete _server;
    }

    String FileServer::getAddress() const
    {
        if (_is_working)
            return _server_addr;
        else
            return emptyString;
    }

    void FileServer::fileServerTask(void *params)
    {
        _server = new WebServer(80);

        if (_mode == SERVER_MODE_RECEIVE)
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

        _must_work = true;

        _server->begin();

        while (_must_work)
        {
            _server->handleClient();
            vTaskDelay(1 / portTICK_PERIOD_MS);
        }

        _is_working = false;
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

            FILE *file = _f_mgr.getFileDescriptor(path.c_str(), "rb");

            if (!file || !_f_mgr.fileExist(path.c_str()))
                handle404();
            else
            {
                size_t f_size = _f_mgr.getFileSize(path.c_str());
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
            if (!_f_mgr.dirExist(_server_dir.c_str()))
            {
                log_e("Помилка відкриття директорії %s", _server_dir.c_str());
                _server->send(500, "text/html", "");
                return;
            }

            String html = HEAD_HTML;
            html += SEND_TITLE_STR; // Заголовок
            html += MID_HTML;

            std::vector<FileInfo> f_infos;
            _f_mgr.indexFiles(f_infos, _server_dir.c_str());

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

            _f_mgr.closeFile(in_file);

            if (_f_mgr.exists(file_name.c_str(), true))
            {
                String temp_name = file_name;
                temp_name += "_copy";

                while (_f_mgr.fileExist(temp_name.c_str(), true))
                    temp_name += "_copy";

                file_name = temp_name;
            }

            in_file = _f_mgr.getFileDescriptor(file_name.c_str(), "ab");

            if (!in_file)
            {
                log_e("Не можу відкрити файл %s на запис", file_name.c_str());
                _server->send(500, "text/html", "");
                return;
            }

            _last_delay_time = millis();
        }
        else if (uploadfile.status == UPLOAD_FILE_WRITE)
        {
            _f_mgr.writeToFile(in_file, (const char *)uploadfile.buf, uploadfile.currentSize);
            if (millis() - _last_delay_time > 1000)
            {
                vTaskDelay(1 / portTICK_PERIOD_MS);
                _last_delay_time = millis();
            }
        }
        else if (uploadfile.status == UPLOAD_FILE_END || uploadfile.status == UPLOAD_FILE_ABORTED)
        {
            if (in_file)
            {
                _f_mgr.closeFile(in_file);

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
        html += _server_addr;
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