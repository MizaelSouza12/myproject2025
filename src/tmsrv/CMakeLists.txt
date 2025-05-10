# CMakeLists.txt para TMSrv
add_library(tmsrv_lib STATIC
    TMSrv.cpp
    MobManager.cpp
    ItemManager.cpp
    PlayerManager.cpp
    MapManager.cpp
    QuestManager.cpp
    EventManager.cpp
)

target_include_directories(tmsrv_lib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../../include
)

target_link_libraries(tmsrv_lib
    wydbr_common
)

install(TARGETS tmsrv_lib DESTINATION lib)