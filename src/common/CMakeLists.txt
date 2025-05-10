# CMakeLists.txt para componentes comuns
add_library(wydbr_common STATIC
    network/NetworkManager.cpp
    security/SecurityManager.cpp
    utils/Logger.cpp
    utils/Config.cpp
    database/DatabaseManager.cpp
)

target_include_directories(wydbr_common PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../../include
)

target_link_libraries(wydbr_common
    Threads::Threads
)

install(TARGETS wydbr_common DESTINATION lib)