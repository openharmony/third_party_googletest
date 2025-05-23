@echo off
@rem Copyright 2024 googletest authors.
@rem
@rem Licensed under the Apache License, Version 2.0 (the "License");
@rem you may not use this file except in compliance with the License.
@rem You may obtain a copy of the License at
@rem
@rem     http://www.apache.org/licenses/LICENSE-2.0
@rem
@rem Unless required by applicable law or agreed to in writing, software
@rem distributed under the License is distributed on an "AS IS" BASIS,
@rem WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
@rem See the License for the specific language governing permissions and
@rem limitations under the License.

SETLOCAL ENABLEDELAYEDEXPANSION

SET BAZEL_EXE=%KOKORO_GFILE_DIR%\bazel-5.1.1-windows-x86_64.exe

SET PATH=C:\Python34;%PATH%
SET BAZEL_PYTHON=C:\python34\python.exe
SET BAZEL_SH=C:\tools\msys64\usr\bin\bash.exe
SET CMAKE_BIN="cmake.exe"
SET CTEST_BIN="ctest.exe"
SET CTEST_OUTPUT_ON_FAILURE=1
SET CMAKE_BUILD_PARALLEL_LEVEL=16
SET CTEST_PARALLEL_LEVEL=16

IF EXIST git\googletest (
  CD git\googletest
) ELSE IF EXIST github\googletest (
  CD github\googletest
)

IF %errorlevel% neq 0 EXIT /B 1

:: ----------------------------------------------------------------------------
:: CMake
MKDIR cmake_msvc2022
CD cmake_msvc2022

%CMAKE_BIN% .. ^
  -G "Visual Studio 17 2022" ^
  -DPYTHON_EXECUTABLE:FILEPATH=c:\python37\python.exe ^
  -DPYTHON_INCLUDE_DIR:PATH=c:\python37\include ^
  -DPYTHON_LIBRARY:FILEPATH=c:\python37\lib\site-packages\pip ^
  -Dgtest_build_samples=ON ^
  -Dgtest_build_tests=ON ^
  -Dgmock_build_tests=ON
IF %errorlevel% neq 0 EXIT /B 1

%CMAKE_BIN% --build . --target ALL_BUILD --config Debug -- -maxcpucount
IF %errorlevel% neq 0 EXIT /B 1

%CTEST_BIN% -C Debug --timeout 600
IF %errorlevel% neq 0 EXIT /B 1

CD ..
RMDIR /S /Q cmake_msvc2022

:: ----------------------------------------------------------------------------
:: Bazel

SET BAZEL_VS=C:\Program Files\Microsoft Visual Studio\2022\Community
%BAZEL_EXE% test ... ^
  --compilation_mode=dbg ^
  --copt=/std:c++14 ^
  --copt=/WX ^
  --features=external_include_paths ^
  --keep_going ^
  --test_output=errors ^
  --test_tag_filters=-no_test_msvc2017
IF %errorlevel% neq 0 EXIT /B 1
