configure_file(version.h.in ${CMAKE_CURRENT_SOURCE_DIR}/version.h @ONLY)

find_package(QT NAMES Qt6 Qt5 COMPONENTS Core Gui Widgets REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Core Gui Widgets REQUIRED)

get_target_property(QmakeExecutable Qt${QT_VERSION_MAJOR}::qmake IMPORTED_LOCATION)
get_filename_component(QtBinDir "${QmakeExecutable}" DIRECTORY)
find_program(DEPLOY_QT_EXECUTABLE windeployqt macdeployqt HINTS "${QtBinDir}")

set(SRC_FILES
        main.cpp
        Clipboard.cpp Clipboard.h
        Item.cpp Item.h Item.ui
        AboutDialog.cpp AboutDialog.h AboutDialog.ui
        MainWindow.cpp MainWindow.h MainWindow.ui
)
set(RES_FILES Clipboard.qrc)

add_executable(${PROJECT_NAME}
        ${SRC_FILES}
        ${RES_FILES}
)

add_subdirectory(utils)

if (WIN32)
    if (CMAKE_BUILD_TYPE STREQUAL "Release")
        set_target_properties(${PROJECT_NAME} PROPERTIES WIN32_EXECUTABLE TRUE)
    endif ()
elseif (APPLE)
    set_target_properties(${PROJECT_NAME} PROPERTIES MACOSX_BUNDLE TRUE)
elseif (UNIX AND NOT APPLE)
    set_target_properties(${PROJECT_NAME}
            PROPERTIES
            INSTALL_RPATH "$ORIGIN:$ORIGIN.:$ORIGIN/lib"
            BUILD_WITH_INSTALL_RPATH TRUE
    )
endif ()

# for SingleApplication
set(QAPPLICATION_CLASS QApplication CACHE STRING "Inheritance class for SingleApplication")

include(FetchContent)
FetchContent_Declare(QHotkey
        URL https://github.com/Skycoder42/QHotkey/archive/refs/tags/1.5.0.tar.gz
)
FetchContent_Declare(SingleApplication
        URL https://github.com/itay-grudev/SingleApplication/archive/refs/tags/v3.5.2.tar.gz
)
FetchContent_Declare(magic_enum
        GIT_REPOSITORY https://github.com/Neargye/magic_enum
        GIT_TAG v0.9.7
        GIT_SHALLOW ON
        GIT_PROGRESS ON
)
FetchContent_Declare(json
        URL https://github.com/nlohmann/json/releases/download/v3.12.0/json.tar.xz)

FetchContent_MakeAvailable(QHotkey SingleApplication magic_enum json)

target_link_libraries(QClipboard
        Qt${QT_VERSION_MAJOR}::Core
        Qt${QT_VERSION_MAJOR}::Gui
        Qt${QT_VERSION_MAJOR}::Widgets
        QHotkey::QHotkey
        SingleApplication::SingleApplication
        magic_enum::magic_enum
        nlohmann_json::nlohmann_json
)

# Deploy the Qt Application
if (WIN32)
    add_custom_command(TARGET ${PROJECT_NAME}
            POST_BUILD
            COMMAND ${DEPLOY_QT_EXECUTABLE}
            --no-translations
            # --no-compiler-runtime
            $<TARGET_FILE:${PROJECT_NAME}>
    )
elseif (APPLE)
    add_custom_command(TARGET ${PROJECT_NAME}
            POST_BUILD
            COMMAND ${DEPLOY_QT_EXECUTABLE}
            "$<TARGET_BUNDLE_DIR:${PROJECT_NAME}>"
            # -dmg
    )
endif ()