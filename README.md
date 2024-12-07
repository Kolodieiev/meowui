Meowui це фреймоворк, що вирішує деякі із задач, які можуть виникнути, при створенні прошивки із графічним інтерфейсом на мікроконтролерах esp32 та esp32s3.
Для виводу зображення на дисплей, використовується модифікована бібліотека TFT_eSPI. З оригінальною бібліотекою, яка знаходиться за цим посиланням https://github.com/Bodmer/TFT_eSPI, фреймворк працюватиме некоректно.


Основні можливості, що пропонує Meowui:
* Базовий набір віджетів для швидкої побудови простих інтерфейсів. До цього набору входять:
  * Статичне та динамічне меню.
  * Скролбар.
  * Прогресбар.
  * Текстова мітка. 
  * Клавіатура.
  * Перемикач.
  * Спінбокс.
  * Зображення.
* Вирішення задачі створення та обробки макетів вікон, а також зміна контексту для кожного окремого вікна програми.
* Прив'язка і обробка стану пінів, незалежно від типу(сенсор/кнопка), та передача їх стану в контекст будь-якого із вікон.
* Простий ігровий рушій для створення ортогональних 2D ігор.
* Набір бібліотек для управління периферією (i2s, spi).


В планах на майбутнє:
* Розширення можливостей існуючих та створення нових віджетів.
* Допрацювання та покращення ігрового рушія.
* Покращення роботи із сенсорними дисплеями.
* Розширення списку підтримуваних периферійних пристроїв.