-- Приклад створення віджета меню з фіксованою кількістю елементів списку.
-- Піни кнопок.
BTN_EXIT = 0 
BTN_UP = 38
BTN_DOWN = 41
BTN_A = 5

-- Ідентифікатори віджетів повинні бути унікальними в межах одного контейнера та повинні бути більшими за 0.
HEADER_LABEL_ID = 1
MAIN_MENU_ID = 2

-- Це саме стосується ідентифікаторів елементів списку меню.
SIMPLE_ITEM_ID = 1
TOGGLE_ITEM_ID = 2

-- Кольори віжетів
COLOR_LIME = 0xA7E0
COLOR_ITEM_FOCUS_BACK = 0x39E7;

-- Інші константи
CUTOUT_SIZE = 40 -- Висота вирізів(скруглення) екрану. 

-- Завантажити зображення із карти пам'яті в PSRAM. У змінній зберігається його ID.
-- Пам'ять зображення може бути звільнена вручну за потреби або буде звільнена автоматично після завершення виходу зі скрипта.
img_id_plus = loadImg("/plus.bmp"); 

initType("ToggleItem") -- Підключаємо клас ToggleItem, який автоматично підключить для себе класи ToggleSwitch, MenuItem, Label, Image, IWidget.
initType("Menu") -- Під час підключння Menu автоматично підключить також IWidgetContainer, який потрібний для його коректного функціонування.

main_layout = context.getLayout() -- Отримати вказівник на головний віджет-макет контексту.
main_layout:setBackColor(TFT_BLACK) -- Встановити фоновий колір макету.

-------------------------------------------------------- Заголовок

header_label = Label:new(HEADER_LABEL_ID) -- Створюємо Label для заголовка контексту, та розміщуємо його зверху між вирізами екрану.
main_layout:addWidget(header_label) -- Додаємо текстову мітку до головного макету.
header_label:setWidth(gl:width() - CUTOUT_SIZE * 2) -- Встановлюємо ширину на увесь екран мінус вирізи з боків.
header_label:setHeight(CUTOUT_SIZE) -- Встановлюємо висоту мітки.
header_label:setText("Приклад меню") -- Задаємо текст мітки.
header_label:setAlign(ALIGN_CENTER) -- Вирівнюємо текст по центру відносно віджета по горизонталі.
header_label:setGravity(GRAVITY_CENTER) -- Вирівнюємо текст по центру віджета по вертикалі.
header_label:setPos(CUTOUT_SIZE, 0) -- Задаємо позицію віджета.

---------------------------------------------------------- Меню

main_menu = Menu:new(MAIN_MENU_ID) -- Створюємо віджет головного меню.
main_layout:addWidget(main_menu) -- Додаємо меню до головного макету.
main_menu:setWidth(gl:width()) -- Встановлюємо ширину на увесь екран.
main_menu:setHeight(gl:height() - CUTOUT_SIZE * 2) -- Встановлюємо висоту на увесь екран мінус висота вирізів екрану з верху та низу.
main_menu:setPos(0, CUTOUT_SIZE) -- Встановлюємо позицію віджета відносно його батьківського контейнера.
main_menu:setItemHeight((main_menu:getHeight() - 2) / 4) -- Встановлюємо висоту елементів, в залежності від того, скільки елементів потрібно відображати за раз.
main_menu:setLoopState(true) -- Зациклюємо меню, щоб фокус переходив автоматично з першого елемента на останній та навпаки.

---------------------------------------------------------- Елементи меню

simple_item = MenuItem:new(SIMPLE_ITEM_ID) -- Створюємо простий елемент списку.
main_menu:addWidget(simple_item) -- Додаємо елемент до меню.
simple_item:setFocusBorderColor(COLOR_LIME)
simple_item:setFocusBackColor(COLOR_ITEM_FOCUS_BACK) -- Колір фону елемента при потраплянні фокусу на нього.
simple_item:setBackColor(TFT_BLACK) -- Чорний колір фону встановлено по замовчуванню. Тут його встановлюється для прикладу.
simple_item:setChangingBorder(true) -- Встановлюємо прапор зміни кольору межі віджета, у разі його потрапляння у фокус.
simple_item:setChangingBack(true) -- Встановлюємо прапор зміни кольору фону віджета, у разі його потрапляння у фокус.

simple_item_label = simple_item:getLbl() -- Отримуємо вказівник на віджет текстової мітки. Можна також створити нову мітку і присвоїти її елементу.
simple_item:setLbl(simple_item_label)
simple_item_label:setText("Простий елемент списку")
simple_item_label:setGravity(GRAVITY_CENTER)
simple_item_label:setTickerInFocus(true) -- Встановлюємо прапор автоматичного прокручування тексту віджета, у разі його потрапляння у фокус.

-- ----

toggle_item = ToggleItem:new(TOGGLE_ITEM_ID) -- Створюємо елемент списку меню з перемикачем та зображенням, якщо його було успішно завантажено з SD-карти.
main_menu:addWidget(toggle_item) -- Додаємо елемент до меню.
toggle_item:setFocusBorderColor(COLOR_LIME)
toggle_item:setFocusBackColor(COLOR_ITEM_FOCUS_BACK)
toggle_item:setChangingBorder(true)
toggle_item:setChangingBack(true)

toggle_item_label = simple_item_label:clone(1) -- Клонуємо текстову мітку з попереднього елемента списку, щоб не налаштовувати повторно параметри.
toggle_item:setLbl(toggle_item_label) -- Присвоюємо клоновану мітку елементу меню.
toggle_item_label:setText("Довгий текст елемента списку меню з перемикачем") -- Змінюємо текст клонованої мітки на інший.

toggle_switch = toggle_item:getToggle() -- Отримуємо вказівник на перемикача. Можна також створити новий перемикач та присвоїти його елементу.
toggle_switch:setWidth(40)
toggle_switch:setHeight(20)
toggle_switch:setCornerRadius(7)

if(img_id_plus > 0) then -- Якщо ресурс зображення успішно завантажено з карти пам'яті, додаємо зображення до текстової мітки.
    toggle_item_img = Image:new(1) -- Створюємо віджет зображення.
    toggle_item_img:setSrc(img_id_plus); -- Встановлюємо ресурс. Розмір віджета встановиться таким, як і у ресурса зображення.
    simple_item:setImg(toggle_item_img)
    toggle_item:setImg(toggle_item_img)
    context.manageWidget(toggle_item_img) -- Передаємо керування віджетом до контексту. Тому що елемент меню не видаляє самосітйно віджет зобрраження.
end

-------------------------------------------------------- Обробка вводу

function update() -- Функція викликається автоматично контекстом кожен кадр.
     if input.is_pressed(BTN_EXIT) then -- Обробка кнопки виходу.
        input.lock(BTN_EXIT, 1000) -- Заблокувати спрацьовування кнопки на n мс.
		context.exit() -- Завершити роботу скрипта.
        return;
    elseif input.is_released(BTN_UP) then
        input.lock(BTN_UP, 250)
        main_menu:focusUp()
    elseif input.is_released(BTN_DOWN) then
        input.lock(BTN_DOWN, 250)
        main_menu:focusDown()
    elseif input.is_released(BTN_A) then
        input.lock(BTN_A, 250)
        ok_handler()
    end
end

-------------------------------------------------------- Обробка натискання кнопки А(OK)

function ok_handler()
    local item_id = main_menu:getCurrItemID()
    if item_id == SIMPLE_ITEM_ID then
        showToast("Натиснуто ОК")
    elseif item_id == TOGGLE_ITEM_ID then
        if toggle_item:isOn() then
            toggle_item:off()
        else
            toggle_item:on()
        end
    end
end
