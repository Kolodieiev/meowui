#include "MainScene.h"
// Додати зображення плиток
#include "../../common_res/tile_img/tile_0.h"
#include "../../common_res/tile_img/tile_1.h"
//
#include "../map/template/map_scene_main.h"
//
#include "../obj/blue_ball/BlueBallObj.h"
#include "../obj/green_ball/GreenBallObj.h"

namespace test_server
{
    MainScene::MainScene(DataStream &stored_objs)
        : IGameScene(stored_objs), _server{stub_serv}, _client{stub_client}
    {
        _mode = MODE_SOLO;
        init();
    }

    MainScene::MainScene(DataStream &stored_objs, GameServer &server)
        : IGameScene(stored_objs), _server{server}, _client{stub_client}
    {
        _mode = MODE_SERVER;
        _server.onData(onClientData, this);
        _server.onDisconnect(onClientDisconn, this);
        init();
        uint8_t START_GAME_DATA_CMD = 100;
        _server.sendBroadcast(UdpPacket::TYPE_DATA, &START_GAME_DATA_CMD, 1);
    }

    MainScene::MainScene(DataStream &stored_objs, GameClient &client)
        : IGameScene(stored_objs), _server{stub_serv}, _client{client}
    {
        _mode = MODE_CLIENT;
        _client.onData(onServerData, this);
        _client.onDisconnect(onServerDisconn, this);
        init();
    }

    MainScene::~MainScene()
    {
        if (_mode == MODE_SERVER)
        {
            _server.onDisconnect(nullptr, nullptr);
            _server.onData(nullptr, nullptr);
            _server.stop();
        }
        else if (_mode == MODE_CLIENT)
        {
            _client.onDisconnect(nullptr, nullptr);
            _server.onData(nullptr, nullptr);
            _client.disconnect();
        }
    }

    void MainScene::init()
    {
        if (_mode == MODE_SERVER || _mode == MODE_SOLO)
        {
            BlueBallObj *ball = createObject<BlueBallObj>();
            ball->init();
            _ball = ball;
            _ball->_x_global = 34;
            _ball->_y_global = 34;

            if (_mode == MODE_SERVER)
                _ball->setName(_server.getName());
            else
                _ball->setName("Solo");

            _main_obj = _ball;
            _game_objs.emplace(_ball->getId(), _ball);
        }

        buildTerrain();

        _ui = new SceneUI();
        _game_UI = _ui;

        if (_mode == MODE_SOLO)
        {
            _ui->hideStartMsg();
        }

        if (_mode == MODE_CLIENT) // Якщо клієнт, надсилаємо інформацію серверу про готовність приймати дані
        {
            _ui->hideStartMsg();
            log_i("Надсилаю ready");
            uint8_t data = CMD_READY;
            _client.send(UdpPacket::TYPE_DATA, &data, 1);
        }
    }

    void MainScene::buildTerrain()
    {
        _terrain.addTileDesc(0, Tile::TYPE_GROUND, IMG_TILE_0);
        _terrain.addTileDesc(1, Tile::TYPE_WALL, IMG_TILE_1);

        uint8_t tiles_x_num{11}; // Кількість плиток мапи по ширині
        uint8_t tiles_y_num{11}; // Кількість плиток мапи по висоті
        uint8_t tile_width{32};  // Ширина або висота плитки. Плитки повинні бути квадратними

        _terrain.build(tiles_x_num, tiles_y_num, tile_width, MAP_SCENE_MAIN); // Побудувати мапу на основі даних шаблону
    }

    void MainScene::update()
    {
        // Обробка кнопки виходу
        if (_input.isPressed(BtnID::BTN_EXIT))
        {
            _input.lock(BtnID::BTN_EXIT, 500);
            _is_finished = true;
            return;
        }

        // Обробка кнопок джойстика
        IGameObject::MovingDirection direction = IGameObject::DIRECTION_NONE;

        if (_input.isHolded(BtnID::BTN_UP))
        {
            _input.lock(BtnID::BTN_UP, 100);
            direction = IGameObject::DIRECTION_UP;
        }
        else if (_input.isHolded(BtnID::BTN_DOWN))
        {
            _input.lock(BtnID::BTN_DOWN, 100);
            direction = IGameObject::DIRECTION_DOWN;
        }
        else if (_input.isHolded(BtnID::BTN_RIGHT))
        {
            _input.lock(BtnID::BTN_RIGHT, 100);
            direction = IGameObject::DIRECTION_RIGHT;
        }
        else if (_input.isHolded(BtnID::BTN_LEFT))
        {
            _input.lock(BtnID::BTN_LEFT, 100);
            direction = IGameObject::DIRECTION_LEFT;
        }

        // Виконуємо дії залежно від режиму
        if (_mode == MODE_SERVER) // Серверні перевірки повинні бути завжди першими для меншої кількості промахів
        {
            if (_is_clients_connected)
            {
                _ball->move(direction);
                doServerStuff();
            }
        }
        else if (_mode == MODE_CLIENT)
            doClientStuff(direction);
        else
            _ball->move(direction);

        IGameScene::update(); // Необхідно обов'язково перевикликати метод кожен кадр у базовму класі. Інакше сцена не буде перемальовуватися.
    }

    // ----------------------------------------------------------------------------------------------------------------------------- Сервер

    void MainScene::onClientData(ClientWrapper *cl_wrap, UdpPacket *packet, void *arg)
    {
        MainScene *self = static_cast<MainScene *>(arg);
        self->handleClientData(cl_wrap, packet);
    }

    // --------------------------------------------------

    void MainScene::handleClientDisconn(ClientWrapper *cl_wrap)
    {
        // Тут реалізовувати обробку відключення клієнта
    }

    void MainScene::onClientDisconn(ClientWrapper *cl_wrap, void *arg)
    {
        MainScene *self = static_cast<MainScene *>(arg);
        self->handleClientDisconn(cl_wrap);
    }

    // --------------------------------------------------

    void MainScene::doServerStuff()
    {
        // Розсилаємо дані клієнтам

        size_t data_size = getObjsSize();
        if (data_size > 1000)
        {
            log_e("Перевищено максимально допустимий розмір пакету");
            return;
        }
        UdpPacket packet(data_size);
        packet.write(CMD_UPD_OBJ);
        serialize(packet);

        _server.sendBroadcast(packet);
    }

    // ----------------------------------------------------------------------------------------------------------------------------- Клієнт

    void MainScene::onServerData(UdpPacket *packet, void *arg)
    {
        MainScene *self = static_cast<MainScene *>(arg);
        self->handleServerData(packet);
    }
    // --------------------------------------------------

    void MainScene::handleServerDisconn()
    {
        _is_finished = true;
    }

    void MainScene::onServerDisconn(void *arg)
    {
        MainScene *self = static_cast<MainScene *>(arg);
        self->handleServerDisconn();
    }

    // --------------------------------------------------

    void MainScene::doClientStuff(IGameObject::MovingDirection direction)
    {
        if (direction == IGameObject::MovingDirection::DIRECTION_NONE)
            return;

        // Якщо натиснута кнопка руху, передаємо команду на сервер
        UdpPacket pack(SIZE_CMD_MOVE);
        pack.write(CMD_MOVE);
        pack.write(direction);
        _client.sendPacket(pack);
    }

    IGameObject *MainScene::createObjByClass(ClassID id)
    {
        //  По такому прикладу можна додати створення будь-якого типу
        if (id == GREEN_BALL)
        {
            GreenBallObj *ball = createObject<GreenBallObj>();
            ball->init();
            ball->_x_global = 70; // Можна додати зміщення, якщо клієнтів декілька
            ball->_y_global = 34;
            return ball;
        }
        else if (id == BLUE_BALL)
        {
            BlueBallObj *ball = createObject<BlueBallObj>();
            ball->init();
            ball->_x_global = 34;
            ball->_y_global = 34;
            return ball;
        }

        log_e("Невідомий class_id: %d", id);
        esp_restart();
    }

    // -----------------------------------------------------------------------------------------------------------------------------

    void MainScene::handleClientData(ClientWrapper *cl_wrap, UdpPacket *packet)
    {
        // Дані повинні містити як мінімум команду
        if (packet->dataLen() < 1)
        {
            log_e("Некоректний розмір пакету");
            return;
        }

        // Зчитуємо саме ігрову команду, а не тип пакета. Тип оброблює сервер самостійно.
        uint8_t cmd = packet->getData()[0];

        // log_i("CMD: %d", cmd);
        // log_i("data len %d", packet->dataLen());

        if (cmd == CMD_READY)
        {
            if (_is_clients_connected) // Ігноруємо, якщо вже всі клієнти підключилися
                return;

            // Створити об'єкт для клієнта
            GreenBallObj *ball = createObject<GreenBallObj>();
            ball->init();
            ball->_x_global = 80;
            ball->_y_global = 34;
            ball->setName(cl_wrap->getName());

            // Додати клієнта в таблицю
            auto result = _client_id.emplace(cl_wrap->getIP(), ball);
            if (!result.second)
            {
                delete ball;
                return;
            }

            // Додати об'єкт клієнта на ігрову мапу
            _game_objs.emplace(ball->getId(), ball);

            // Надіслати клієнту його id
            UdpPacket resp(SIZE_CMD_SET_ID); // Розмір команди в даних + розмір id
            resp.write(CMD_SET_ID);
            resp.write(ball->getId());
            _server.sendPacket(cl_wrap, resp);

            // Первірити чи всі клієнти підключилися
            if (_client_id.size() == _server.getClients()->size())
            {
                _ui->hideStartMsg();          // Приховати повідомлення про очікування
                _is_clients_connected = true; // Розблокувати рух

                // Розіслати ініціалізацію
                size_t data_size = getObjsSize();
                if (data_size > 1000)
                {
                    log_e("Перевищено максимально допустимий розмір пакету");
                    return;
                }
                UdpPacket packet(1 + data_size);
                packet.write(CMD_INIT);
                serialize(packet);

                _server.sendBroadcast(packet);
            }
        }
        else if (cmd == CMD_MOVE)
        {
            // Перевіряємо довжину паету на відповідність команді
            if (packet->dataLen() < SIZE_CMD_MOVE)
                return;

            // Шукаємо ігровий об'єкт клієнта
            auto it = _client_id.find(cl_wrap->getIP());
            if (it == _client_id.end())
                return;

            // Тут можна перевірити чи може об'єкт виконати цю команду
            // ...

            // Читаємо дані напрямку руху
            IGameObject::MovingDirection direction = static_cast<IGameObject::MovingDirection>(packet->getData(1)[0]);
            // Приводимо клієнтський об'єкт до відповідного типу, так як ми знаємо його тип. Якщо тип може бути випадковим
            // дані про тип потрібно передати додатково в пакеті
            BallObj *client_obj = static_cast<BallObj *>(it->second);

            // Застосовуємо дію до клієнтського об'єкта
            client_obj->move(direction);
        }
        else
        {
            log_i("Unknown CMD");
            // packet->printToLog();
        }
    }

    void MainScene::handleServerData(UdpPacket *packet)
    {
        // Очікуємо, що сервер надсилає завжди валідні дані, тому довжину даних не перевіряємо ніде.

        uint8_t cmd;
        packet->read(&cmd, 1);

        // log_i("CMD: %d", cmd);
        // log_i("data len %d", packet->dataLen());

        if (cmd == CMD_SET_ID)
        {
            // Якщо отримали ідентифікатор прив'язки, зберігаємо його
            packet->extractBytes(&_client_obj_id, 1, 4);
            log_i("CMD_SET_ID: %d", _client_obj_id);
        }
        else if (cmd == CMD_INIT)
        {
            // Запускаємо стартову ініціалізацію об'єктів на клієнті
            log_i("CMD_INIT");

            while (packet->space() > 5) // В роділі даних повинно зберігатися мінімум (obj_id(4) + class_id(1) + 1 ) для відновлення
            {
                uint8_t class_id;
                packet->extractBytes(&class_id, packet->index() + 4, 1); // class_id зберігається після ob_id
                log_i("class_id: %d", class_id);

                // Створюємо для кожного із образів новий об'єкт
                IGameObject *obj = createObjByClass(static_cast<ClassID>(class_id));
                // Записуємо ключові поля в об'єкт
                obj->deserialize(*packet);

                // Перевіряємо ідентифікатор об'єкта. Якщо співпадає із клієнтським, прив'язуємо камеру до цього об'єкта
                if (obj->getId() == _client_obj_id)
                {
                    log_i("_client_obj_id");
                    _main_obj = obj;
                }

                // Додаємо його в ігровий світ клієнта
                _game_objs.emplace(obj->getId(), obj);
            }
        }
        else if (cmd == CMD_UPD_OBJ)
        {
            // Виконуємо оновлення стану ігрових об'єктів

            while (packet->space() > 5)
            {
                uint32_t id;
                packet->extractBytes(&id, packet->index(), 4); // Читаємо перші 4 байти після команди

                // Шукаємо об'єкт в ігровому світі
                auto it = _game_objs.find(id);

                // Якщо об'єкт знайдено, десеріалізуємо дані в нього
                if (it != _game_objs.end())
                    it->second->deserialize(*packet);
                else
                {
                    log_i("not found");
                    // packet->printToLog(false);

                    // Інакше створюємо новий об'єкт за вказаним ід класу
                    uint8_t class_id;
                    packet->extractBytes(&class_id, packet->index() + 4, 1); // Читаємо один байт після команди + ід об'єкта
                    log_i("class_id: %d", class_id);

                    // Створюємо об'єкт по цьому id
                    IGameObject *obj = createObjByClass(static_cast<ClassID>(class_id));
                    // десеріалізуємо дані в об'єкт
                    obj->deserialize(*packet);

                    // додаємо його в ігровий світ клієнта
                    _game_objs.emplace(obj->getId(), obj);
                }
            }
        }
        else
        {
            log_i("Unknown CMD");
            // packet->printToLog();
        }
    }

    // -----------------------------------------------------------------------------------------------------------------------------
} // namespace test_server