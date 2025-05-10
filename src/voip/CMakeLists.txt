# CMakeLists.txt para sistema VoIP
if(WYDBR_ENABLE_VOIP)
    add_library(voip_lib STATIC
        VoIPSystem.cpp
        audio/AudioManager.cpp
        network/VoIPNetworkManager.cpp
        server/ChannelManager.cpp
        client/VoIPClient.cpp
    )

    target_include_directories(voip_lib PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/../../include
    )

    target_link_libraries(voip_lib
        wydbr_common
    )

    install(TARGETS voip_lib DESTINATION lib)
endif()