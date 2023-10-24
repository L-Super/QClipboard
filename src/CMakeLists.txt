find_package(QT NAMES Qt6 Qt5 COMPONENTS Core Gui Widgets REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Core Gui Widgets REQUIRED)

if (WIN32)
    get_target_property(QmakeExecutable Qt${QT_VERSION_MAJOR}::qmake IMPORTED_LOCATION)
    get_filename_component(QtBinDir "${QmakeExecutable}" DIRECTORY)
    find_program(DEPLOYQT_EXECUTABLE windeployqt HINTS "${QtBinDir}")
endif ()

add_executable(QClipboard main.cpp
        Clipboard.cpp
        Clipboard.h
        Clipboard.qrc
)

include(FetchContent)
FetchContent_Declare(spdlog
        GIT_REPOSITORY https://github.com/gabime/spdlog
        GIT_TAG v1.12.0
        DOWNLOAD_EXTRACT_TIMESTAMP true)

FetchContent_Declare(QHotkey
        GIT_REPOSITORY https://github.com/Skycoder42/QHotkey
        DOWNLOAD_EXTRACT_TIMESTAMP true)

FetchContent_MakeAvailable(spdlog QHotkey)

target_link_libraries(QClipboard
        Qt${QT_VERSION_MAJOR}::Core
        Qt${QT_VERSION_MAJOR}::Gui
        Qt${QT_VERSION_MAJOR}::Widgets
        spdlog::spdlog_header_only
)

# Deploy the Qt Application
if (WIN32)
    add_custom_command(TARGET ${PROJECT_NAME}
            POST_BUILD
            COMMAND ${DEPLOYQT_EXECUTABLE}
            #        --no-translations --no-compiler-runtime
            $<TARGET_FILE:${PROJECT_NAME}>
    )
endif ()