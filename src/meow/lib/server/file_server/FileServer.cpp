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

    bool FileServer::begin(const char *server_path, ServerMode mode)
    {
        if (_is_working)
            return false;

        _server_path = server_path;
        _server_mode = mode;

        if (!_sd.isMounted())
            return false;

        if (_server_mode != SERVER_MODE_SEND_FILE)
        {
            if (!_server_path.equals("/") && _server_path.endsWith("/"))
                _server_path.remove(1, -1);

            if (_server_path.isEmpty())
                _server_path = "/";

            if (!_fs.dirExist(_server_path.c_str()))
                return false;
        }
        else if (!_fs.fileExist(_server_path.c_str()))
            return false;

        _server_ip = "http://";

        if (!_wifi.isConnected())
        {
            log_i("%s", STR_ROUTER_NOT_CONNECTED);
            log_i("Створюю власну AP");

            if (_ssid.isEmpty())
            {
                log_e("Не вказано SSID");
                return false;
            }

            if (!_wifi.createAP(_ssid, _pwd, 1))
                return false;

            _server_ip += _wifi.getAPIP();
        }
        else
        {
            _server_ip += _wifi.getLocalIP();
        }

        log_i("File server addr: %s", _server_ip.c_str());

        _must_work = true;

        BaseType_t result = xTaskCreatePinnedToCore(startWebServer, "fileServerTask", (1024 / 2) * 20, this, 10, NULL, 1);

        if (result == pdPASS)
        {
            _is_working = true;
            log_i("File server is working now");
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
        else if (_server_mode == SERVER_MODE_SEND)
        {
            _server->on("/", HTTP_GET, [this]()
                        { this->handleSend(); });
        }
        else
        {
            _server->on("/", HTTP_GET, [this]()
                        { this->handleSendFile(); });
        }

        _server->onNotFound([this]()
                            { this->handle404(); });

        _server->begin();

        while (_must_work)
        {
            _server->handleClient();
            vTaskDelay(1 / portTICK_PERIOD_MS);
        }

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
            String path = _server_path;
            path += "/";
            path += _server->arg(0);

            FILE *file = _fs.openFile(path.c_str(), "rb");

            if (!file)
            {
                handle404();
            }
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
            if (!_fs.dirExist(_server_path.c_str()))
            {
                log_e("Помилка відкриття директорії %s", _server_path.c_str());
                _server->send(500, "text/html", "");
                return;
            }

            String html = HEAD_HTML;
            html += SEND_TITLE_STR; // Заголовок
            html += MID_HTML;

            std::vector<FileInfo> f_infos;
            _fs.indexFiles(f_infos, _server_path.c_str());

            for (const auto &info : f_infos)
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

    void FileServer::handleSendFile()
    {
        FILE *file = _fs.openFile(_server_path.c_str(), "rb");

        if (!file)
        {
            handle404();
        }
        else
        {
            size_t f_size = _fs.getFileSize(_server_path.c_str());

            int pos = _server_path.lastIndexOf('/');
            String filename = (pos == -1) ? _server_path : _server_path.substring(pos + 1);

            FileStream f_stream(file, filename.c_str(), f_size);

            _server->sendHeader("Content-Type", "application/force-download");
            _server->sendHeader("Content-Disposition", "attachment; filename=\"" + filename + "\"");
            _server->sendHeader("Content-Transfer-Encoding", "binary");
            _server->sendHeader("Cache-Control", "no-cache");
            _server->streamFile(f_stream, "application/octet-stream");
        }
    }

    void FileServer::handleFile()
    {
        HTTPUpload &uploadfile = _server->upload();
        _server->setContentLength(CONTENT_LENGTH_UNKNOWN);

        if (uploadfile.status == UPLOAD_FILE_START)
        {
            String filename = _server_path;
            filename += "/";
            filename += uploadfile.filename;

            log_i("Запит на створення файлу %s", filename.c_str());

            _fs.closeFile(in_file);

            if (_fs.exists(filename.c_str(), true))
            {
                String temp_name = filename;
                temp_name += "_copy";

                while (_fs.fileExist(temp_name.c_str(), true))
                    temp_name += "_copy";

                filename = temp_name;
            }

            in_file = _fs.openFile(filename.c_str(), "ab");

            if (!in_file)
            {
                log_e("Не можу відкрити файл %s на запис", filename.c_str());
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