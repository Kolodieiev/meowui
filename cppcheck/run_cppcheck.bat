@echo off
chcp 65001
setlocal enabledelayedexpansion

REM Перевіряємо, чи існує файл cppcheck.exe у PATH
where cppcheck.exe >nul 2>&1
if %errorlevel% neq 0 (
    echo Помилка: cppcheck.exe не знайдено. Будь ласка, переконайтеся, що Cppcheck встановлено та додано до змінної середовища PATH.
    echo.
    pause
    exit /b 1
)

set EXCLUDE_FILE=cppcheck_excludes.txt
set EXCLUDE_OPTIONS=

REM Читаємо шляхи для виключення з файлу
if exist %EXCLUDE_FILE% (
    echo Зчитую шляхи для виключення з %EXCLUDE_FILE%...
    for /f "usebackq tokens=*" %%i in ("%EXCLUDE_FILE%") do (
        set "LINE_READ=%%i"
        REM Обрізаємо пробіли з початку і кінця рядка
        for /f "tokens=*" %%a in ("!LINE_READ!") do set "TRIMMED_LINE=%%a"

        REM Ігноруємо порожні рядки та рядки, що починаються з '#' (коментарі)
        if not "!TRIMMED_LINE!"=="" (
            if not "!TRIMMED_LINE:~0,1!"=="#" (
                REM Додаткова перевірка на обрізання пробілів
                for /f "delims=" %%x in ("!TRIMMED_LINE!") do (
                    set "CLEAN_PATH=%%x"
                )

                if not "!CLEAN_PATH!"=="" (
                    REM *** ЗМІНА В ЦЬОМУ РЯДКУ: Використовуємо -i і не додаємо ручних лапок ***
                    set "EXCLUDE_OPTIONS=!EXCLUDE_OPTIONS! -i !CLEAN_PATH!"
                )
            )
        )
    )
    echo Виключені шляхи: !EXCLUDE_OPTIONS!
    echo.
) else (
    echo Попередження: Файл виключень '%EXCLUDE_FILE%' не знайдено. Продовжую без виключень.
    echo.
)

echo Запуск Cppcheck...
set "CPPCHECK_BASE_CMD=cppcheck --enable=all --xml --suppress=missingInclude --suppress=missingIncludeSystem --suppress=unusedFunction --force"

set "FULL_COMMAND=!CPPCHECK_BASE_CMD! !EXCLUDE_OPTIONS! ../src/"

echo Команда для запуску:
echo !FULL_COMMAND! 2> cppcheck_report.xml
echo.

!FULL_COMMAND! 2> cppcheck_report.xml

if %errorlevel% equ 0 (
    echo Cppcheck завершився успішно. Звіт збережено у cppcheck_report.xml
) else (
    echo Cppcheck завершився з помилками або попередженнями (код %errorlevel%). Перевірте cppcheck_report.xml для деталей.
)

echo.
pause
endlocal