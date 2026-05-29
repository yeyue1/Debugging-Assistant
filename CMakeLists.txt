cmake_minimum_required(VERSION 3.16)

project(serial_prot VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets SerialPort Network)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets SerialPort Network)

set(PROJECT_SOURCES
        main.cpp
        mainwindow.cpp
        mainwindow.h
        mainwindow.ui
        SerialPanel.cpp
        SerialPanel.h
        serialpanel.ui
        SerialManager.cpp
        SerialManager.h
        SerialConfig.h
        SerialRecord.h
        SendQueue.cpp
        SendQueue.h
        DeviceRegistry.cpp
        DeviceRegistry.h
        RecordStore.cpp
        RecordStore.h
        AutomationRuleEngine.cpp
        AutomationRuleEngine.h
        TemplateManager.cpp
        TemplateManager.h
        ParserController.cpp
        ParserController.h
        NetworkConfig.h
        NetworkManager.cpp
        NetworkManager.h
        NetworkPanel.cpp
        NetworkPanel.h
        networkpanel.ui
        ProtocolParserBase.h
        ProtocolParser.cpp
        ProtocolParser.h
        RawDataParser.cpp
        RawDataParser.h
        TextDataParser.cpp
        TextDataParser.h
        ThemeManager.cpp
        ThemeManager.h
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(serial_prot
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
else()
    if(ANDROID)
        add_library(serial_prot SHARED
            ${PROJECT_SOURCES}
        )
    else()
        add_executable(serial_prot
            ${PROJECT_SOURCES}
        )
    endif()
endif()

target_link_libraries(serial_prot PRIVATE Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::SerialPort Qt${QT_VERSION_MAJOR}::Network)

if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.serial_prot)
endif()
set_target_properties(serial_prot PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS serial_prot
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(serial_prot)
endif()
