make debug
if %errorlevel% neq 0 exit /b %errorlevel%
cd bin
main.exe
cd ..
