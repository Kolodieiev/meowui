BTN_EXIT = 0 

COLOR_GREY = 0x3186

initType("IWidgetContainer")
initType("ProgressBar")

main_layout = context.getLayout() 
main_layout:setBackColor(COLOR_GREY) 

progress_bar = ProgressBar:new(1)
main_layout:addWidget(progress_bar)
progress_bar:setWidth(gl:width() - 80)
progress_bar:setHeight(20)
progress_bar:setPos((gl.width() - progress_bar:getWidth()) * 0.5, (gl.height() - progress_bar:getHeight()) * 0.5) 
progress_bar:setMax(100)
progress_bar:setBackColor(TFT_BLACK)
progress_bar:setProgressColor(TFT_GREEN)
progress_bar:setBorderColor(TFT_ORANGE)
progress_bar:setBorder(true)

function update()
    if input.is_pressed(BTN_EXIT) then
        input.lock(BTN_EXIT, 1000)
		context.exit()
        return;
	end

    local progress = progress_bar:getProgress()
    if progress < 100 then
        progress_bar:setProgress(progress + 1)
    else 
        progress_bar:reset()
    end
end