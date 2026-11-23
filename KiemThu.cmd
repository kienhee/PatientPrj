@echo off
setlocal
pushd "%~dp0" || exit /b 1
set "testLog=%~dp0tests\ket-qua.txt"
set "tdmBin=%ProgramFiles(x86)%\Embarcadero\Dev-Cpp\TDM-GCC-64\bin"
where g++.exe >nul 2>&1
if errorlevel 1 if exist "%tdmBin%\g++.exe" set "PATH=%tdmBin%;%PATH%"

echo Building PatientPrj and running tests...
g++ --version >"%testLog%" 2>&1
if errorlevel 1 goto failed
g++ -std=c++17 -Wall -Wextra -Wshadow -static -Wl,--dynamicbase -o PatientPrj.exe PatientManager.cpp Department.cpp DepartmentList.cpp Patient.cpp PatientList.cpp InputHelper.cpp StringUtils.cpp >>"%testLog%" 2>&1
if errorlevel 1 goto failed
powershell.exe -NoLogo -NoProfile -File "%~dp0tests\Test-PatientPrj.ps1" >>"%testLog%" 2>&1
if errorlevel 1 goto failed
echo All tests passed.>>"%testLog%"
set "result=0"
goto done

:failed
echo Build or tests failed.>>"%testLog%"
set "result=1"

:done
type "%testLog%"
echo.
echo Test log: "%testLog%"
popd
pause
exit /b %result%
