cmake_minimum_required(VERSION 3.16)
set(CMAKE_PREFIX_PATH "C:/Qt/6.9.1/msvc2022_64/")


project(log VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets Charts) 
qt_standard_project_setup()

qt_add_executable(log display.cpp main.cpp datalog.cpp)

target_include_directories(log PRIVATE Qt6 WF_SDK)
target_link_libraries(log PRIVATE Qt6::Widgets Qt6::Charts "C:/Program Files (x86)/Digilent/WaveFormsSDK/lib/x64/dwf.lib")
set_target_properties (log PROPERTIES WIN32_EXECUTABLE ON)
target_compile_options(log PUBLIC "/permissive-" "/Zc:__cplusplus" )