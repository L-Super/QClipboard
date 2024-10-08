configure_file(version.h.in ${CMAKE_CURRENT_SOURCE_DIR}/version.h @ONLY)

find_package(QT NAMES Qt6 Qt5 COMPONENTS Core Gui Widgets REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Core Gui Widgets REQUIRED)

if (WIN32)
    get_target_property(QmakeExecutable Qt${QT_VERSION_MAJOR}::qmake IMPORTED_LOCATION)
    get_filename_component(QtBinDir "${QmakeExecutable}" DIRECTORY)
    find_program(DEPLOYQT_EXECUTABLE windeployqt HINTS "${QtBinDir}")
endif ()

set(SRC_FILES
        main.cpp
        Clipboard.cpp Clipboard.h
        Item.cpp Item.h Item.ui
        AboutDialog.cpp AboutDialog.h AboutDialog.ui
)
set(RES_FILES Clipboard.qrc)

if (WINDOWS)
    if (CMAKE_BUILD_TYPE STREQUAL "Release")
        add_executable(${PROJECT_NAME} WIN32
                ${SRC_FILES}
                ${RES_FILES}
        )
    else ()
        add_executable(${PROJECT_NAME}
                ${SRC_FILES}
                ${RES_FILES}
        )
    endif ()
else ()
    add_executable(${PROJECT_NAME}
            ${SRC_FILES}
            ${RES_FILES}
    )
    if (UNIX AND NOT APPLE)
        set_target_properties(${PROJECT_NAME}
                PROPERTIES
                INSTALL_RPATH "$ORIGIN:$ORIGIN.:$ORIGIN/lib"
                BUILD_WITH_INSTALL_RPATH TRUE
        )
    endif ()
endif ()

#include(FetchContent)
#FetchContent_Declare(QHotkey
#        GIT_REPOSITORY https://github.com/Skycoder42/QHotkey
#        SOURCE_DIR ${CMAKE_SOURCE_DIR}/third_party/QHotkey
#        DOWNLOAD_EXTRACT_TIMESTAMP true)
#
#FetchContent_MakeAvailable(QHotkey)
#FetchContent_Declare(SingleApplication
#        URL https://github.com/itay-grudev/SingleApplication/archive/refs/heads/master.zip
#        SOURCE_DIR ${CMAKE_SOURCE_DIR}/third_party/SingleApplication
#        DOWNLOAD_EXTRACT_TIMESTAMP true)
#FetchContent_MakeAvailable(SingleApplication)

# for SingleApplication
set(QAPPLICATION_CLASS QApplication CACHE STRING "Inheritance class for SingleApplication")

add_subdirectory(third_party/QHotkey)
add_subdirectory(third_party/SingleApplication)

target_link_libraries(QClipboard
        Qt${QT_VERSION_MAJOR}::Core
        Qt${QT_VERSION_MAJOR}::Gui
        Qt${QT_VERSION_MAJOR}::Widgets
        QHotkey::QHotkey
        SingleApplication::SingleApplication
)

# Deploy the Qt Application
if (WIN32)
    add_custom_command(TARGET ${PROJECT_NAME}
            POST_BUILD
            COMMAND ${DEPLOYQT_EXECUTABLE}
            --no-translations
            # --no-compiler-runtime
            $<TARGET_FILE:${PROJECT_NAME}>
    )
endif ()