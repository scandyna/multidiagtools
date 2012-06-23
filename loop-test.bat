:loop
tests\mdtutils\mdtutilstest.exe -maxwarnings 0
if %errorlevel% NEQ 0 (goto end)
goto loop
:end
