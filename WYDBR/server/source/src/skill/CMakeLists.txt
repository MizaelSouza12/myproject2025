# CMakeLists.txt para o componente Skill

# Adiciona a biblioteca principal de habilidades
add_library(wyd_skill
    SkillLoader.cpp
)

# Inclui diretórios
target_include_directories(wyd_skill PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/../../include
    ${CMAKE_CURRENT_SOURCE_DIR}/../../include/skill
)

# Vincula bibliotecas necessárias
target_link_libraries(wyd_skill
    wyd_core
)

# Adiciona teste de compatibilidade binária
add_executable(test_skill_binary_compatibility
    test_skill_binary_compatibility.cpp
)

# Inclui diretórios para o teste
target_include_directories(test_skill_binary_compatibility PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/../../include
    ${CMAKE_CURRENT_SOURCE_DIR}/../../include/skill
)

# Vincula bibliotecas para o teste
target_link_libraries(test_skill_binary_compatibility
    wyd_skill
    wyd_core
)