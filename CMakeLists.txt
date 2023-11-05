cmake_minimum_required(VERSION 3.25)
project(QClipboard)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

if (WIN32)
#    set(CMAKE_PREFIX_PATH "D:/Qt/6.5.2/mingw_64/lib/cmake")
elseif (UNIX AND NOT APPLE)
#    set(CMAKE_PREFIX_PATH "/home/leo/Qt/5.15.2/gcc_64")
endif ()
set(VERSION_MAJOR 1)
set(VERSION_MINOR 0)
set(VERSION_PATCH 3)

set(VERSION "${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}")
set(VERSION_WIN "${VERSION_MAJOR},${VERSION_MINOR},${VERSION_PATCH}")

add_subdirectory(src)