# CMakeLists.txt para sistema de compartilhamento de tela
if(WYDBR_ENABLE_SCREEN_SHARE)
    add_library(screenshare_lib STATIC
        ScreenShareSystem.cpp
        capture/CaptureManager.cpp
        codec/CodecManager.cpp
        network/ScreenShareNetworkManager.cpp
        server/ShareSessionManager.cpp
        client/ScreenShareClient.cpp
    )

    target_include_directories(screenshare_lib PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/../../include
    )

    target_link_libraries(screenshare_lib
        wydbr_common
    )

    install(TARGETS screenshare_lib DESTINATION lib)
endif()