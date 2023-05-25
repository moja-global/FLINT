@echo off

REM Build FLINT dependencies from vcpkg manifest using Visual Studio 2022.
REM Requires CMake >= 3.26.3.
REM Run using VS2022 x64 native tools command prompt.

REM Ensure git is on the path.
set GIT_PATH="C:\Program Files\Microsoft Visual Studio\2022\Professional\Common7\IDE\CommonExtensions\Microsoft\TeamFoundation\Team Explorer\Git\cmd"
set PATH=%PATH%;%GIT_PATH%

if not exist vcpkg (
    REM Clone required vcpkg release for FLINT dependencies. This section
    REM requires an elevated prompt.
    git clone -b "2022.05.10" https://github.com/microsoft/vcpkg.git
    pushd vcpkg
    call bootstrap-vcpkg.bat

    REM Fix msys2-related errors in 2022-05-10 vcpkg
    powershell -Command "(gc scripts\cmake\vcpkg_acquire_msys.cmake) -replace 'libtool-2.4.6-9-x86_64.pkg.tar.xz', 'libtool-2.4.7-3-x86_64.pkg.tar.zst' | Out-File -encoding ASCII scripts\cmake\vcpkg_acquire_msys.cmake"
    powershell -Command "(gc scripts\cmake\vcpkg_acquire_msys.cmake) -replace 'b309799e5a9d248ef66eaf11a0bd21bf4e8b9bd5c677c627ec83fa760ce9f0b54ddf1b62cbb436e641fbbde71e3b61cb71ff541d866f8ca7717a3a0dbeb00ebf', 'a202ddaefa93d8a4b15431dc514e3a6200c47275c5a0027c09cc32b28bc079b1b9a93d5ef65adafdc9aba5f76a42f3303b1492106ddf72e67f1801ebfe6d02cc' | Out-File -encoding ASCII scripts\cmake\vcpkg_acquire_msys.cmake"
    powershell -Command "(gc scripts\cmake\vcpkg_acquire_msys.cmake) -replace 'msys2-runtime-3.2.0-8', 'msys2-runtime-3.4.6-1' | Out-File -encoding ASCII scripts\cmake\vcpkg_acquire_msys.cmake"
    powershell -Command "(gc scripts\cmake\vcpkg_acquire_msys.cmake) -replace 'fdd86f4ffa6e274d6fef1676a4987971b1f2e1ec556eee947adcb4240dc562180afc4914c2bdecba284012967d3d3cf4d1a392f798a3b32a3668d6678a86e8d3', 'fbdcf2572d242b14ef3b39f29a6119ee58705bad651c9da48ffd11e80637e8d767d20ed5d562f67d92eecd01f7fc3bc351af9d4f84fb9b321d2a9aff858b3619' | Out-File -encoding ASCII scripts\cmake\vcpkg_acquire_msys.cmake"

    REM Fix access violation errors: ensure Ninja 1.11.1
    powershell -Command "Invoke-WebRequest https://github.com/ninja-build/ninja/releases/download/v1.11.1/ninja-win.zip -OutFile ninja-win.zip"
    powershell -Command "Expand-Archive ninja-win.zip 'C:\Program Files\Microsoft Visual Studio\2022\Professional\Common7\IDE\CommonExtensions\Microsoft\CMake\Ninja' -Force"

    popd
)

REM Build all dependencies and configure CMake.
if not exist ..\source\build md ..\source\build
pushd ..\source\build

REM If OpenSSL fails to build, just keep retrying until it does.
cmake -S .. ^
    -G "Visual Studio 17 2022" ^
    -DCMAKE_INSTALL_PREFIX=bin ^
    -DVCPKG_TARGET_TRIPLET=x64-windows ^
    -DCMAKE_TOOLCHAIN_FILE=..\..\vcpkg\vcpkg\scripts\buildsystems\vcpkg.cmake ^
    -DENABLE_TESTS:BOOL=OFF ^
    -DENABLE_MOJA.MODULES.ZIPPER:BOOL=ON ^
    -DENABLE_MOJA.MODULES.GDAL:BOOL=ON ^
    -DENABLE_MOJA.MODULES.LIBPQ:BOOL=ON ^
    -DENABLE_MOJA.MODULES.POCO:BOOL=ON

popd
