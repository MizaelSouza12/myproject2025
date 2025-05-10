# CMakeLists.txt para WYDStudio
add_executable(WYDStudio
    main.cpp
    WYDStudio.cpp
    ui/AdminUI.cpp
    admin/ConfigManager.cpp
    admin/LogManager.cpp
    integration/TMSrvIntegration.cpp
    integration/DBSrvIntegration.cpp
)

target_include_directories(WYDStudio PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../../include
)

target_link_libraries(WYDStudio
    wydbr_common
    tmsrv_lib
    dbsrv_lib
)

if(WYDBR_ENABLE_VOIP)
    target_link_libraries(WYDStudio voip_lib)
endif()

if(WYDBR_ENABLE_SCREEN_SHARE)
    target_link_libraries(WYDStudio screenshare_lib)
endif()

install(TARGETS WYDStudio DESTINATION bin)