cmake_minimum_required(VERSION 3.25)
project(QClipboard
        VERSION 1.0.7.0 # Set application version. For more details, see src/version.h.in
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

if (WIN32 AND MSVC)
    add_compile_options(/utf-8)
endif ()

add_subdirectory(src)