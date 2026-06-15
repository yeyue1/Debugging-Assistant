# tests/CMakeLists.txt
# 自动化测试框架配置

# 宏：添加一个测试目标
macro(add_serial_test TEST_NAME TEST_SOURCE)
    add_executable(${TEST_NAME} ${TEST_SOURCE} ${ARGN})
    target_link_libraries(${TEST_NAME} PRIVATE
        Qt${QT_VERSION_MAJOR}::Test
        Qt${QT_VERSION_MAJOR}::Core
    )
    target_include_directories(${TEST_NAME} PRIVATE ${CMAKE_SOURCE_DIR})
    add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME})
endmacro()

# ProtocolParser 测试 - 需要 ProtocolParser 和 ProtocolParserBase
add_serial_test(TestProtocolParser TestProtocolParser.cpp
    ${CMAKE_SOURCE_DIR}/ProtocolParser.cpp
    ${CMAKE_SOURCE_DIR}/ProtocolParserBase.cpp
)

# RecordStore 测试 - 只需要 RecordStore
add_serial_test(TestRecordStore TestRecordStore.cpp
    ${CMAKE_SOURCE_DIR}/RecordStore.cpp
)

# AutomationRuleEngine 测试 - 需要 AutomationRuleEngine 和 RecordStore
add_serial_test(TestAutomationRuleEngine TestAutomationRuleEngine.cpp
    ${CMAKE_SOURCE_DIR}/AutomationRuleEngine.cpp
    ${CMAKE_SOURCE_DIR}/RecordStore.cpp
)

# SendQueue 测试 - 只需要 SendQueue
add_serial_test(TestSendQueue TestSendQueue.cpp
    ${CMAKE_SOURCE_DIR}/SendQueue.cpp
)

# DeviceRegistry 测试 - 需要 DeviceRegistry 和 Qt::SerialPort
add_serial_test(TestDeviceRegistry TestDeviceRegistry.cpp
    ${CMAKE_SOURCE_DIR}/DeviceRegistry.cpp
)
target_link_libraries(TestDeviceRegistry PRIVATE Qt${QT_VERSION_MAJOR}::SerialPort)

# TemplateManager 测试 - 需要 TemplateManager、RecordStore 和 SerialPort
add_serial_test(TestTemplateManager TestTemplateManager.cpp
    ${CMAKE_SOURCE_DIR}/TemplateManager.cpp
    ${CMAKE_SOURCE_DIR}/RecordStore.cpp
)
target_link_libraries(TestTemplateManager PRIVATE Qt${QT_VERSION_MAJOR}::SerialPort)

# ParserController 测试 - 需要所有解析器和基础类
add_serial_test(TestParserController TestParserController.cpp
    ${CMAKE_SOURCE_DIR}/ParserController.cpp
    ${CMAKE_SOURCE_DIR}/ProtocolParserBase.cpp
    ${CMAKE_SOURCE_DIR}/RawDataParser.cpp
    ${CMAKE_SOURCE_DIR}/TextDataParser.cpp
    ${CMAKE_SOURCE_DIR}/ProtocolParser.cpp
)
