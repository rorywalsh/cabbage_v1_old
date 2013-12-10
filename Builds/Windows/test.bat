ECHO OFF

IF "%1"=="rory" set test="this is a test"
IF "%1"=="" set test="not a test"

echo %test%
