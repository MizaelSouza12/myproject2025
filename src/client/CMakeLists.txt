# CMakeLists.txt para cliente WYDBR
add_executable(WYDClient
    main.cpp
    Client.cpp
    game/GameEngine.cpp
    network/ClientNetwork.cpp
    ui/UserInterface.cpp
    graphics/GraphicsEngine.cpp
    audio/AudioEngine.cpp
)

target_include_directories(WYDClient PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../../include
)

target_link_libraries(WYDClient
    wydbr_common
)

if(WYDBR_ENABLE_VOIP)
    target_link_libraries(WYDClient voip_lib)
endif()

if(WYDBR_ENABLE_SCREEN_SHARE)
    target_link_libraries(WYDClient screenshare_lib)
endif()

install(TARGETS WYDClient DESTINATION bin)