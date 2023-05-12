cd /d %~dp0

REM Edit this with the right path to vcvarsall.bat.
call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvarsall.bat" x64
meson setup build64 --buildtype=release
meson configure build64
ninja -C build64

mkdir bin
copy build64\src\cardio.dll bin\cardio.dll
pause