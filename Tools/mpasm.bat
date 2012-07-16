@echo off
mpasmwin /empasm.err %1 %2 %3 %4 %5 %6 %7 %8 %9
if %ERRORLEVEL% == 0 goto success
type mpasm.err
del mpasm.err
exit /B 1
:success