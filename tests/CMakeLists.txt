# CMakeLists.txt para testes do WYDBR 2.0

# Encontrar Google Test
find_package(GTest REQUIRED)

# Arquivos de teste
set(TEST_SOURCES
    combat/CombatSystemTest.cpp
    character/CharacterSystemTest.cpp
    inventory/InventorySystemTest.cpp
    skills/SkillSystemTest.cpp
    quests/QuestSystemTest.cpp
    guilds/GuildSystemTest.cpp
    crafting/CraftingSystemTest.cpp
    events/EventSystemTest.cpp
    chat/ChatSystemTest.cpp
    matches/MatchSystemTest.cpp
    ranking/RankingSystemTest.cpp
    ban/BanSystemTest.cpp
    log/LogSystemTest.cpp
)

# Criar executável de testes
add_executable(wydbr_tests ${TEST_SOURCES})

# Vincular dependências
target_link_libraries(wydbr_tests
    PRIVATE
    wydbr_game_logic
    GTest::GTest
    GTest::Main
)

# Adicionar testes
add_test(NAME wydbr_tests COMMAND wydbr_tests)

# Configurar propriedades dos testes
set_tests_properties(wydbr_tests PROPERTIES
    ENVIRONMENT "GTEST_COLOR=1"
    TIMEOUT 300
)

# Mensagem de configuração
message(STATUS "WYDBR 2.0 Tests")
message(STATUS "Test Sources: ${TEST_SOURCES}")
