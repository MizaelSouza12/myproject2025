# CMakeLists.txt para sistema VoIP
if(WYDBR_ENABLE_VOIP)
    add_library(voip_lib STATIC
        VoIPSystem.cpp
        webrtc/WebRTCConnection.cpp
        audio/AudioManager.cpp
        codec/CodecManager.cpp
        network/VoIPNetworkManager.cpp
        channels/ChannelManager.cpp
        security/VoIPSecurityManager.cpp
    )

    target_include_directories(voip_lib PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/../../include
        ${WEBRTC_INCLUDE_DIRS}
    )

    target_link_libraries(voip_lib
        wydbr_common
        ${WEBRTC_LIBRARIES}
        opus
    )

    # Configurar flags de compilação para WebRTC
    target_compile_definitions(voip_lib PRIVATE
        WEBRTC_POSIX
        WEBRTC_LINUX
        WEBRTC_USE_PIPEWIRE
    )

    # Encontrar OpenSSL
    find_package(OpenSSL REQUIRED)

    # Adicionar biblioteca de segurança
    add_library(voip_security_lib STATIC
        security/VoIPSecurityManager.cpp
        security/SecurityConfigLoader.cpp
        security/SecurityLogger.cpp
        security/SecurityMonitor.cpp
        security/AuthenticationManager.cpp
    )

    target_include_directories(voip_security_lib PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${OPENSSL_INCLUDE_DIR}
    )

    target_link_libraries(voip_security_lib
        PUBLIC
            ${OPENSSL_LIBRARIES}
    )

    # Vincular biblioteca de segurança à biblioteca principal
    target_link_libraries(voip_lib PRIVATE voip_security_lib)

    install(TARGETS voip_lib DESTINATION lib)
endif()