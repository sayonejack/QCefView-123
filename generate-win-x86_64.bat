setlocal

set QTDIR=D:\Qt\5.15.2\msvc2019_64

set http_proxy=127.0.0.1:10809
set https_proxy=127.0.0.1:10809

set CEF_SDK_VERSION=119.4.7+g55e15c8+chromium-119.0.6045.199

set CEF_VERSION_MAJOR=%CEF_SDK_VERSION:~0,3%
echo %CEF_VERSION_MAJOR%

set buildDir=build/%CEF_VERSION_MAJOR%

cmake -S . -B %buildDir% -A x64 -DPROJECT_ARCH=x86_64 -DBUILD_DEMO=ON -DCMAKE_INSTALL_PREFIX:PATH="%cd%/out/windows.x86_64" -DCEF_SDK_VERSION=%CEF_SDK_VERSION% %*

endlocal
pause
