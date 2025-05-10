BTN_EXIT = 0 -- Пін кнопки виходу

initType("IWidgetContainer") -- Ініціалізувати тип віджета, що буде використовуватися в скрипті

layout = context.getLayout() -- Отримати вказівник на шаблон контекста
layout:setBackColor(TFT_GREEN) -- Встановити фоновий колір

function update()
	if input.is_pressed(BTN_EXIT) then -- Обробка кнопки виходу
        input.lock(BTN_EXIT, 1000) 
        print("EXIT");
		context.exit()
	end
end