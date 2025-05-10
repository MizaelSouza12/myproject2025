# CMakeLists.txt para componentes de integração
set(COMPONENT_NAME integration_lib)

file(GLOB_RECURSE SOURCES *.cpp)
file(GLOB_RECURSE HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/../../include/Integration/*.h)

add_library(${COMPONENT_NAME} STATIC ${SOURCES} ${HEADERS})

target_include_directories(${COMPONENT_NAME} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/../../include
)

target_link_libraries(${COMPONENT_NAME}
    security_lib
    network_lib
    storage_lib
    gamelogic_lib
    voip_lib
    screenshare_lib
    common_lib
    Threads::Threads
)

if(WYDBR_ENABLE_TESTS)
    # Tests for integration components
    add_executable(integration_tests ${CMAKE_CURRENT_SOURCE_DIR}/../../tests/integration/integration_tests.cpp)
    target_link_libraries(integration_tests ${COMPONENT_NAME} gtest gtest_main)
    add_test(NAME integration_tests COMMAND integration_tests)
endif()
