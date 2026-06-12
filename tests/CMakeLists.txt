# tests/CMakeLists.txt
# 自动化测试框架配置

# 宏：添加一个测试目标（禁用 AUTOMOC，因为测试文件手动 #include *.moc）
macro(add_serial_test TEST_NAME TEST_SOURCE)
    add_executable(${TEST_NAME} ${TEST_SOURCE} ${ARGN})
    set_target_properties(${TEST_NAME} PROPERTIES AUTOMOC OFF)
    target_link_libraries(${TEST_NAME} PRIVATE
        Qt${QT_VERSION_MAJOR}::Test
        Qt${QT_VERSION_MAJOR}::Core
        Qt${QT_VERSION_MAJOR}::Widgets
        Qt${QT_VERSION_MAJOR}::SerialPort
        Qt${QT_VERSION_MAJOR}::Network
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
