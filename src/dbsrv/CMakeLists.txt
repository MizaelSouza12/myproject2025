# CMakeLists.txt para DBSrv
add_library(dbsrv_lib STATIC
    DBSrv.cpp
    AccountManager.cpp
    ItemStorageManager.cpp
    CharacterStorageManager.cpp
    LogStorageManager.cpp
)

target_include_directories(dbsrv_lib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../../include
)

target_link_libraries(dbsrv_lib
    wydbr_common
)

install(TARGETS dbsrv_lib DESTINATION lib)