@echo off
REM Build script for WYDBR 2.0 on Windows
setlocal

REM Set up environment
if not exist build mkdir build

REM Parse arguments
set BUILD_TYPE=Release
set BUILD_TESTS=ON
set BUILD_BENCHMARKS=ON
set ENABLE_OPTIMIZATION=ON
set ENABLE_ASAN=OFF

:parse_args
if "%1"=="" goto end_parse_args
if "%1"=="--debug" set BUILD_TYPE=Debug
if "%1"=="--no-tests" set BUILD_TESTS=OFF
if "%1"=="--no-benchmarks" set BUILD_BENCHMARKS=OFF
if "%1"=="--no-optimization" set ENABLE_OPTIMIZATION=OFF
if "%1"=="--asan" set ENABLE_ASAN=ON
shift
goto parse_args
:end_parse_args

echo Building WYDBR 2.0 (%BUILD_TYPE%)
echo Tests: %BUILD_TESTS%
echo Benchmarks: %BUILD_BENCHMARKS%
echo Optimization: %ENABLE_OPTIMIZATION%
echo AddressSanitizer: %ENABLE_ASAN%

REM Configure CMake
cd build
cmake .. -DCMAKE_BUILD_TYPE=%BUILD_TYPE% ^
         -DWYDBR_BUILD_TESTS=%BUILD_TESTS% ^
         -DWYDBR_BUILD_BENCHMARKS=%BUILD_BENCHMARKS% ^
         -DWYDBR_ENABLE_OPTIMIZATION=%ENABLE_OPTIMIZATION% ^
         -DWYDBR_ENABLE_ASAN=%ENABLE_ASAN%

REM Build
cmake --build . --config %BUILD_TYPE% --parallel %NUMBER_OF_PROCESSORS%

REM Run tests if enabled
if "%BUILD_TESTS%"=="ON" (
    echo Running tests...
    ctest -C %BUILD_TYPE% --output-on-failure
)

echo Build completed successfully.
cd ..
