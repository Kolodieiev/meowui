-- Номера кнопок, як вказано в прошивці Meowui
BTN_UP = 38
BTN_DOWN = 41
BTN_LEFT = 39
BTN_RIGHT = 40
BTN_EXIT = 0

function update()
	if input.is_pressed(BTN_EXIT) then
        input.lock(BTN_EXIT, 1000) -- Заблокувати спрацьовування на n мс
        print("EXIT");
		context.exit()
    elseif input.is_holded(BTN_UP) then
        input.lock(BTN_UP, 100)
        print("UP");
    elseif input.is_holded(BTN_DOWN) then
        input.lock(BTN_DOWN, 100)
        print("DOWN");
	elseif input.is_holded(BTN_LEFT) then
        input.lock(BTN_LEFT, 100)
        print("LEFT");
	elseif input.is_holded(BTN_RIGHT) then
        input.lock(BTN_RIGHT, 100)
        print("RIGHT");
    end
end