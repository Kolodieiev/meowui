-- В даному скрипті демонструється створення віджетів із Meowui та вивід їх на дисплей.

-- Піни кнопок.
BTN_EXIT = 0 
BTN_LEFT = 39
BTN_RIGHT = 40

COLOR_GREY = 0x3186 -- Змінна з 16-бітним значенням кольору.

-- Ідентифікатори віджетів повинні бути унікальними в межах одного контейнера та не повинні дорівнювати 0.
HELLO_LABEL_ID = 1
PLUS_IMG_ID = 2
TICKER_LABEL_ID = 3
TICKER_CLONE_LABEL_ID = 4

-- Завантажити зображення із карти пам'яті в PSRAM. У змінній зберігається його ID.
-- Пам'ять зображення може бути звільнена вручну за потреби або буде звільнена автоматично після завершення виходу зі скрипта.
img_id_plus = loadImg("/plus.bmp"); 

-- Ініціалізувати типи віджетів, що будуть використовуватися у скрипті.
initType("IWidgetContainer")
initType("Label") 
initType("Image")

main_layout = context.getLayout() -- Отримати вказівник на головний віджет-макет контексту.
main_layout:setBackColor(COLOR_GREY) -- Встановити фоновий колір макета.

---------------------------------------------------------- Проста текстова мітка

hello_label = Label:new(HELLO_LABEL_ID) -- Створюємо віджет текстової мітки. Якщо віджет було додано до контейнера, турбуватися про пам'ять не потрібно.
main_layout:addWidget(hello_label) -- Додаємо віджет текстової мітки до основного макету контексту.

hello_label:setText("Вітання Meowui з Lua!")
hello_label:setTextOffset(5) -- Робимо відступ зліво 5пкс
hello_label:initWidthToFit(15) -- Підганяємо ширину віджета, щоб помістився заданий текст + 15пкс для відступів.
hello_label:setHeight(40)
hello_label:setPos((gl:width() - hello_label:getWidth()) / 2, (gl:height() -  hello_label:getHeight()) / 2 - 20)
hello_label:setTextColor(TFT_ORANGE)
hello_label:setAlign(ALIGN_CENTER) -- Вирівнюємо текст по горизонталі відносно віджета.
hello_label:setGravity(GRAVITY_CENTER) -- Вирівнюємо текст по вертикалі.
hello_label:setBorder(true) -- Вмикаємо відображення рамки товщиною 1 пкс.
hello_label:setBorderColor(TFT_GREEN) 
hello_label:setCornerRadius(10) -- Задаємо скруглення кутів віджета.

---------------------------------------------------------- Зображення

if(img_id_plus > 0) then -- Якщо ресурс зображення успішно завантажено з карти пам'яті
    plus_img = Image:new(PLUS_IMG_ID)
    main_layout:addWidget(plus_img) 
    plus_img:setSrc(img_id_plus) -- Встановлюємо завантажене зображення із карти пам'яті. Розмір віджета буде дорівнювати розміру завантаженого зображенняю
    plus_img:setBackColor(COLOR_GREY) -- Встановлюємо фоновий колір, як у батьківського контейнера, щоб фон під прозорим кольором правильно накладався.
    plus_img:setPos(40, 55);
end

---------------------------------------------------------- Текстова мітка із текстом, що автоматично прокручується

ticker_label = Label:new(TICKER_LABEL_ID)
main_layout:addWidget(ticker_label)

ticker_label:setText("Приклад текстової мітки, що автоматично прокручує дуже-дуже-дуже-дуже довгий текст.")
ticker_label:setWidth(gl:width() - 40) -- Встановлюємо ширину віджета (на увесь екран - 40пкс).
ticker_label:setTextOffset(5)
ticker_label:setHeight(40)
ticker_label:setPos(20, hello_label:getYPos() + hello_label:getHeight() + 20) -- Задаємо позицію віджета зі стартовою прив'язкою до іншого віджета.
ticker_label:setTicker(true) -- Вмикаємо підтримку автоматичного прокуручування тексту, якщо той не вміщується повністю у поле віджета.
ticker_label:setGravity(GRAVITY_CENTER)
ticker_label:setBorder(true)
ticker_label:setBorderColor(TFT_RED) 

---------------------------------------------------------- Створення копії попередньої текстової мітки

ticker_clone_label = ticker_label:clone(TICKER_CLONE_LABEL_ID) -- Створюємо глибоку копію віджета з іншого віджета.
main_layout:addWidget(ticker_clone_label)

ticker_clone_label:setPos(20, ticker_clone_label:getYPos() + ticker_clone_label:getHeight() + 20) -- Змінюємо позицію скопійованого віджета.

---------------------------------------------------------- Рухаємо віджет зображення

function update() -- Функція викликається автоматично контекстом кожен кадр.
    if input.is_pressed(BTN_EXIT) then -- Обробка кнопки виходу.
        input.lock(BTN_EXIT, 1000) -- Заблокувати спрацьовування кнопки на n мс.
		context.exit() -- Завершити роботу скрипта.
        return;
    elseif input.is_holded(BTN_LEFT) then
        if(img_id_plus > 0) then
            if plus_img:getXPos() < 10 then -- Якщо поточна позиція віджета виходить за задані рамки.
                plus_img:setPos(gl.width() - plus_img:getWidth() - 10, plus_img:getYPos())  -- Зациклюємо переміщення на дисплеї.
            else
                plus_img:setPos(plus_img:getXPos() - 5, plus_img:getYPos())
            end
            main_layout:forcedDraw() -- Примусово перемальовуємо батькіський контейнер в обхід оптимізації, щоб уникнути сліду від переміщеноого віджету-зображення.
        end
    elseif input.is_holded(BTN_RIGHT) then
        if(img_id_plus > 0) then
            if plus_img:getXPos() > gl.width() - 10  then
                plus_img:setPos(10, plus_img:getYPos()) 
            else
                plus_img:setPos(plus_img:getXPos() + 5, plus_img:getYPos())
            end
            main_layout:forcedDraw()
        end
	end
end