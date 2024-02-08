@ECHO OFF

SET "tempFile=%TEMP%\wsl_status.txt"
wsl.exe --status > "%tempFile%"

FIND "Default Version: 2" "%tempFile%" >nul

IF ERRORLEVEL 1 (
  ECHO Need to enable WSL 2, this will reboot your machine!
  ECHO.
  ECHO [Y]es to continue, [N]o to cancel.
  CHOICE /C YN /N >nul

  IF ERRORLEVEL 2 (
    ECHO Installation canceled. Press any key to exit.
    PAUSE >nul
    GOTO :EOF
  )

  ECHO Starting installation of wsl..
  wsl.exe --install --distribution Debian --no-launch

  IF NOT %ERRORLEVEL%==1 (
    ECHO Failed to install WSL, are you running this window as administrator? ERR=%ERRORLEVEL%
    PAUSE >nul
    GOTO :EOF
  )

  ECHO WSL 2 has been enabled. Your computer needs to reboot.

  shutdown.exe /r /t 5 /c "Enabling Windows Subsystem for Linux"
  GOTO :EOF
)

ECHO Update wsl...
wsl.exe --update

FIND "Default Distribution: Debian" "%tempFile%" >nul

IF ERRORLEVEL 1 (
  ECHO About to install Debian and make it the default distribution for WSL.
  ECHO [Y]es to continue, [N]o to cancel.
  CHOICE /C YN /N >nul

  IF ERRORLEVEL 2 (
    ECHO Installation canceled. Press any key to exit.
    PAUSE >nul
    GOTO :EOF
  )

  wsl.exe --set-default-version 2

  ECHO ********************************************************************
  ECHO * You may be prompted to enter a new username and password.        *
  ECHO * If after you've entered those you see a green prompt ending in $ *
  ECHO * then please enter the command: exit followed by ^<ENTER^>          *
  ECHO ********************************************************************
  wsl.exe --install --distribution Debian

  wsl.exe --set-default Debian
)

ECHO Setting up debian wsl...
SET WSLENV=%WSLENV%:USERNAME:USERPROFILE/p:REPO_PUBLIC_URL:REPO_PUBLIC_BRANCH:
wsl.exe --user root /bin/bash -xec "wget --version >/dev/null 2>&1 || (apt-get update -qq;apt-get install -qq -y wget)"

IF NOT %ERRORLEVEL%==0 (
  ECHO Failed to setup wsl: ERROR=%ERRORLEVEL%
  ECHO Setup failed. Press any key to exit.
  PAUSE >nul
  GOTO :EOF
)

ECHO Running bootstrap in wsl...
wsl.exe --user root /bin/bash -xec 'source /etc/os-release;wget -qO /tmp/bootstrap.sh "https://swgemu.com/getcore3/?p=2&r=wsl2&os=${ID}-${VERSION_ID}"; chmod +x /tmp/bootstrap.sh;exec /tmp/bootstrap.sh'

IF NOT %ERRORLEVEL%==0 (
  ECHO Failed to bootstrap wsl: ERROR=%ERRORLEVEL%
  ECHO Setup failed. Press any key to exit.
  PAUSE >nul
  GOTO :EOF
)

ECHO Setup complete.
ECHO You can access the environment by choosing Debian in the Start menu.
ECHO or in a terminal window type: wsl
ECHO Once in wsl you can use these commands:
ECHO   build - Builds the core3 server
ECHO   run - runs the core3 server
ECHO.

:EOF

DEL "%tempFile%"
