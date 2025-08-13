#include "SimpleRpgContext.h"
#include "../../WidgetCreator.h"

#include "../common_res/btn_back/btn_back_normal.h"
#include "../common_res/btn_back/btn_back_hover.h"

#include "../../resources/color.h"
#include "../../resources/const.h"
#include "../../resources/string.h"
// Заголовкові файли ігрових сцен
#include "./scene/OpenWorldScene.h"
#include "./scene/CaveScene.h"
//
#include "./SceneID.h" // Ідентифікатори ігрових сцен

namespace simple_rpg
{
    SimpleRpgContext::SimpleRpgContext()
    {
        showMainMenuTmpl(); // В момент створення контексту, першим шаблоном в нього буде додано головне меню гри
    }

    SimpleRpgContext::~SimpleRpgContext()
    {
        // Видалення віджетів зображень, що використовуються для фону у головному меню гри.
        // Видаляти ці два віджети потрібно вручну, тому що віджет текстової мітки не видаляє їх самостійно,
        // задля збереження можливості перевикористання.
        delete _img_back_normal;
        delete _img_back_hover;
    }

    bool SimpleRpgContext::loop()
    {
        if (_state_id == STATE_PREF_SUB_CXT) // Якщо поточний модуль знаходиться в стані керування підконтекстами
        {
            if (!_pref_context->isReleased()) // Якщо субмодуль налаштувань не звільнено
            {
                _pref_context->tick(); // Передаємо увесь час поточного контексту підконтексту
                return false;          // Означає, що поточний контекст не потребує оновлення вводу чи малювання буферу
            }
            else
            {
                delete _pref_context; // Інакше видаляємо субмодуль
                showMainMenuTmpl();   // Формуємо шаблон UI для поточного контексту
            }
        }

        return true; // Означає, що поточний контекст повинен самостійно оновити стан вводу та відмалювати UI
    }

    void SimpleRpgContext::update()
    {
        switch (_state_id)
        {
        case STATE_GAME:
            updateGame();
            break;
        case STATE_MAIN_MENU:     // Якщо контекст знаходиться у стані головного меню
            handleMainMenuBtns(); // Викликаємо відповідний обробник кнопок цього стану
            break;
        }
    }

    void SimpleRpgContext::showMainMenuTmpl()
    {
        // Встановлюється стан контексту
        _state_id = STATE_MAIN_MENU;

        WidgetCreator creator;

        // Створення головного контейнера шаблону
        EmptyLayout *layout = creator.getEmptyLayout();

        //  Створення віджетів фонових зображень для пунктів меню.

        // Зображення для звичайного стану пункта меню.
        _img_back_normal = new Image(1);
        _img_back_normal->init(IMG_W, IMG_H);
        _img_back_normal->setSrc(BTN_BACK_NORMAL);
        _img_back_normal->setBackColor(COLOR_MAIN_BACK);
        _img_back_normal->setTranspColor(TFT_TRANSPARENT);
        // Зображення для стану "У фокусі" пункта меню.
        _img_back_hover = new Image(1);
        _img_back_hover->init(IMG_W, IMG_H);
        _img_back_hover->setSrc(BTN_BACK_HOVER);
        _img_back_hover->setBackColor(COLOR_MAIN_BACK);
        _img_back_hover->setTranspColor(TFT_TRANSPARENT);

        // Створення віджета головного меню гри
        _main_menu = new FixedMenu(ID_MENU_MAIN);
        layout->addWidget(_main_menu); // Ось так кожен віджет додається до контейнера, в якому він повинен знаходитися
        _main_menu->setBackColor(COLOR_MAIN_BACK);
        _main_menu->setItemsSpacing(ITEMS_SPASING);
        _main_menu->setWidth(IMG_W + 2);
        _main_menu->setHeight((IMG_H + ITEMS_SPASING) * 3);
        _main_menu->setItemWidth(IMG_W);
        _main_menu->setItemHeight(IMG_H);
        _main_menu->setPos(getCenterX(_main_menu), TFT_CUTOUT);
        _main_menu->setLoopState(true);

        //  Створення віджетів елементів списку.

        // Пункт меню "Мультиплеєр"
        MenuItem *item_online = new MenuItem(ID_ITEM_ONLINE);
        _main_menu->addItem(item_online);
        item_online->setBackColor(COLOR_MAIN_BACK);

        Label *lbl_online = new Label(1);
        item_online->setLbl(lbl_online);
        lbl_online->setAlign(IWidget::ALIGN_CENTER);
        lbl_online->setGravity(IWidget::GRAVITY_CENTER);
        lbl_online->setBackImg(_img_back_hover);
        lbl_online->setText(STR_MULTI_GAME);
        lbl_online->setTextColor(TFT_BLACK);

        // Пункт меню "Один гравець"
        MenuItem *item_offline = new MenuItem(ID_ITEM_OFFLINE);
        _main_menu->addItem(item_offline);
        item_offline->setBackColor(COLOR_MAIN_BACK);

        Label *lbl_offline = lbl_online->clone(1);
        item_offline->setLbl(lbl_offline);
        lbl_offline->setBackImg(_img_back_normal);
        lbl_offline->setText(STR_SOLO_GAME);

        // Пункт меню "Налаштування"
        MenuItem *item_pref = new MenuItem(ID_ITEM_PREF);
        _main_menu->addItem(item_pref);
        item_pref->setBackColor(COLOR_MAIN_BACK);

        Label *lbl_pref = lbl_online->clone(1);
        item_pref->setLbl(lbl_pref);
        lbl_pref->setBackImg(_img_back_normal);
        lbl_pref->setText(STR_PREF);

        // Встановити цей UI-шаблон для поточного контексту.
        setLayout(layout);
    }

    void SimpleRpgContext::setItemBackNormal()
    {
        MenuItem *item = _main_menu->getCurrItem()->castTo<MenuItem>();
        item->getLbl()->setBackImg(_img_back_normal);
    }

    void SimpleRpgContext::setItemBackHover()
    {
        MenuItem *item = _main_menu->getCurrItem()->castTo<MenuItem>();
        item->getLbl()->setBackImg(_img_back_hover);
    }

    void SimpleRpgContext::handleMainMenuBtns()
    {
        if (_input.isHolded(BtnID::BTN_UP)) // Обробка стану кнопки "Вгору"
        {
            _input.lock(BtnID::BTN_UP, HOLD_LOCK); // Блокуємо стан кнопки, для уникнення випадкових спрацювань.
            setItemBackNormal();                   // Змінюємо фон поточного вибраного елемента меню на "Звичайний"
            _main_menu->focusUp();                 // Підіймаємо фокус на віджеті на одну позицію вгору
            setItemBackHover();                    // Змінюємо фон поточного вибраного елемента меню на "У фокусі"
        }
        else if (_input.isHolded(BtnID::BTN_DOWN)) // Обробка стану кнопки "Вниз"
        {
            _input.lock(BtnID::BTN_DOWN, HOLD_LOCK);
            setItemBackNormal();
            _main_menu->focusDown();
            setItemBackHover();
        }
        else if (_input.isReleased(BtnID::BTN_B)) // Обробка стану кнопки "Назад"
        {
            _input.lock(BtnID::BTN_B, CLICK_LOCK);
            openContextByID(ID_CONTEXT_GAMES); // Повертаємось до контексту вибору гри
        }
        else if (_input.isReleased(BtnID::BTN_A)) // Обробка стану кнопки "Вибір"
        {
            _input.lock(BtnID::BTN_A, CLICK_LOCK);

            switch (_main_menu->getCurrItemID()) // Читаємо ідентфиікатор віджета поточного обраного елемента списку меню
            {
            case ID_ITEM_ONLINE:
                // showSelectRole(); // Створюємо відповідний шаблон UI контексту в залежності від пункта меню
                break;
            case ID_ITEM_OFFLINE:
                showMainScene();
                break;
            case ID_ITEM_PREF:
                openPrefSubContext(); // Або відкриваємо підконтекст
                break;
            }
        }
    }

    void SimpleRpgContext::openPrefSubContext()
    {
        // Встановлємо ідентифікатор стану, який означає,
        // що поточний модуль передає увесь час субмодулю.
        _state_id = STATE_PREF_SUB_CXT;
        _pref_context = new RpgPrefContext(); // Створюємо субмодуль
    }

    void SimpleRpgContext::updateGame()
    {
        if (!_scene->isFinished())
        {
            // Якщо гра не повідомляла про її завершення

            if (!_scene->isReleased())
            {
                // Якщо сцена не просила про перемикання, виклаємо лише її оновлення

                _scene->update();
            }
            else
            {
                // Інакше, якщо сцена попросила про зміну поточного ігрового рівня

                // Отримуємо ідентифікатор наступної сцени
                const uint8_t next_scene_id = _scene->getNextSceneID();
                // Видаляємо поточну
                delete _scene;

                // Створюємо наступну сцену відповідно до ідентифікатора, чи будь-якого іншого механізму перемикання
                switch (next_scene_id)
                {
                case ID_SCENE_OPEN_WORLD:
                    _scene = new OpenWorldScene(_stored_objs);
                    break;
                case ID_SCENE_CAVE:
                    _scene = new CaveScene(_stored_objs);
                    break;
                default:
                    log_e("Невідомий ідентифікатор сцени %u", next_scene_id);
                    esp_restart();
                    break;
                }
            }
        }
        else
        {
            // Інакше звільняємо ресурси гри, та завантажуємо шаблон головного меню

            log_i("Вихід з гри");
            delete _scene;
            showMainMenuTmpl();
        }
    }

    void SimpleRpgContext::showMainScene()
    {
        getLayout()->delWidgets(); // Очистити віджети щоб вони не займали місце в пам'яті
        _scene = new OpenWorldScene(_stored_objs);
        _state_id = STATE_GAME;
    }
}