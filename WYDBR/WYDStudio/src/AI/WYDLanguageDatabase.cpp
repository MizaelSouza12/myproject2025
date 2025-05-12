#include "WYDLanguageDatabase.h"
#include <fstream>
#include <algorithm>
#include <regex>
#include <sstream>
#include <random>
#include <cctype>
#include <locale>
#include <iostream>

namespace wydstudio {
namespace ai {

class WYDLanguageDatabase::Impl {
public:
    Impl(WYDLanguageDatabase* parent) : m_parent(parent) {}
    
    // Implementações internas
    bool loadWYDTerminologyData() {
        // Esta função carrega a terminologia específica do WYD
        
        // Exemplo de implementação para carregamento de termos de equipamentos ("Sets")
        loadArmorSets();
        loadWeapons();
        loadAccessories();
        loadEnhancementTerms();
        loadClassTerms();
        loadSkillTerms();
        loadLocationTerms();
        loadMobTerms();
        loadQuestTerms();
        loadGeneralSlang();
        loadCommandTerms();
        
        // Construir índices após carregar todos os termos
        m_parent->buildIndices();
        
        return true;
    }
    
    void loadArmorSets() {
        // Sets de armadura e suas variações (gírias comuns)
        
        // Set D (Arch/Mystia)
        LanguageTerm termD;
        termD.standardForm = "Armadura Mystia";
        termD.synonyms = {"Set D", "Mystia", "D", "Armadura D", "Set Arch"};
        termD.description = "Set de armadura level 400 classe Arch/Mystia";
        termD.category = TermCategory::ITEM;
        termD.usageExamples = {
            "Preciso de um set D +11 de FM",
            "Vendo set D completo sem refino",
            "Alguém sabe quanto custa um set D de BM no Reino?",
            "Consegui dropar o set D ontem!"
        };
        m_parent->addTerm(termD);
        
        // Set A (Sephira)
        LanguageTerm termA;
        termA.standardForm = "Armadura Sephira";
        termA.synonyms = {"Set A", "Sephira", "A", "Armadura A", "Set Sephira"};
        termA.description = "Set de armadura level 200 classe Sephira";
        termA.category = TermCategory::ITEM;
        termA.usageExamples = {
            "Preciso upar para conseguir usar set A",
            "Vendo set A +6 de huntress",
            "Quanto custa set A sem refinar?"
        };
        m_parent->addTerm(termA);
        
        // Set B (Sapphira)
        LanguageTerm termB;
        termB.standardForm = "Armadura Sapphira";
        termB.synonyms = {"Set B", "Sapphira", "B", "Armadura B", "Set Sapphira"};
        termB.description = "Set de armadura level 220-250 classe Sapphira";
        termB.category = TermCategory::ITEM;
        termB.usageExamples = {
            "Dropei uma calça set B ontem",
            "Vendo set B completo +10",
            "Quem tem set B para vender?"
        };
        m_parent->addTerm(termB);
        
        // Set C (Legendaria)
        LanguageTerm termC;
        termC.standardForm = "Armadura Legendaria";
        termC.synonyms = {"Set C", "Legendaria", "C", "Armadura C", "Set Legendaria"};
        termC.description = "Set de armadura level 320-355 classe Legendaria";
        termC.category = TermCategory::ITEM;
        termC.usageExamples = {
            "Quero um set C +11 de FM",
            "Estou vendendo set C +10 completo",
            "Alguém me ajuda a dropar set C?"
        };
        m_parent->addTerm(termC);
        
        // Set Celestial (CL)
        LanguageTerm termCL;
        termCL.standardForm = "Armadura Celestial";
        termCL.synonyms = {"Set Celestial", "Celestial", "CL", "Armadura CL", "Set CL"};
        termCL.description = "Set de armadura level 380 classe Celestial";
        termCL.category = TermCategory::ITEM;
        termCL.usageExamples = {
            "Quanto custa um set CL +11?",
            "Vendo set celestial completo +10",
            "Alguém vende CL de foema?"
        };
        m_parent->addTerm(termCL);
        
        // Set Ancient (ANC)
        LanguageTerm termANC;
        termANC.standardForm = "Armadura Ancient";
        termANC.synonyms = {"Set Ancient", "Ancient", "ANC", "Armadura ANC", "Set ANC"};
        termANC.description = "Set de armadura level 400 classe Ancient";
        termANC.category = TermCategory::ITEM;
        termANC.usageExamples = {
            "Quero comprar um set ANC de trans",
            "Alguém tem set Ancient +11 para vender?",
            "Quanto custa um set ANC hoje em dia?"
        };
        m_parent->addTerm(termANC);
    }
    
    void loadWeapons() {
        // Armas e suas variações
        
        // Espada
        LanguageTerm termSword;
        termSword.standardForm = "Espada";
        termSword.synonyms = {"Sword", "Esp", "Blade"};
        termSword.description = "Arma do tipo espada, usada por guerreiros";
        termSword.category = TermCategory::ITEM;
        termSword.usageExamples = {
            "Preciso de uma espada +11 para meu guerreiro",
            "Vendo sword +13 de Foema",
            "Alguém vende espada boa para BM?"
        };
        m_parent->addTerm(termSword);
        
        // Machado
        LanguageTerm termAxe;
        termAxe.standardForm = "Machado";
        termAxe.synonyms = {"Axe", "Mach", "Hacka"};
        termAxe.description = "Arma do tipo machado, usada por guerreiros";
        termAxe.category = TermCategory::ITEM;
        termAxe.usageExamples = {
            "Procurando machado +11 ou melhor",
            "Vendo axe +15 de beast",
            "Alguém tem machado good para vender?"
        };
        m_parent->addTerm(termAxe);
        
        // Arco
        LanguageTerm termBow;
        termBow.standardForm = "Arco";
        termBow.synonyms = {"Bow", "Arc"};
        termBow.description = "Arma do tipo arco, usada por archeres";
        termBow.category = TermCategory::ITEM;
        termBow.usageExamples = {
            "Preciso de um arco +11 para minha huntress",
            "Vendo bow +13 de arch",
            "Alguém tem arco celestial para vender?"
        };
        m_parent->addTerm(termBow);
        
        // Cajado
        LanguageTerm termStaff;
        termStaff.standardForm = "Cajado";
        termStaff.synonyms = {"Staff", "Bast", "Wand", "Vara", "Bastão"};
        termStaff.description = "Arma do tipo cajado, usada por magos";
        termStaff.category = TermCategory::ITEM;
        termStaff.usageExamples = {
            "Procurando cajado +11 para meu BM",
            "Vendo staff +13 de Foema",
            "Alguém tem wand ancient para vender?"
        };
        m_parent->addTerm(termStaff);
        
        // Lança
        LanguageTerm termSpear;
        termSpear.standardForm = "Lança";
        termSpear.synonyms = {"Spear", "Lance", "Lanc"};
        termSpear.description = "Arma do tipo lança, usada por Pike";
        termSpear.category = TermCategory::ITEM;
        termSpear.usageExamples = {
            "Preciso de lança +11 para meu pike",
            "Vendo spear +13 de foema",
            "Onde dropa lança boa para lvl 300?"
        };
        m_parent->addTerm(termSpear);
    }
    
    void loadAccessories() {
        // Acessórios e suas variações
        
        // Anel
        LanguageTerm termRing;
        termRing.standardForm = "Anel";
        termRing.synonyms = {"Ring", "Anelzin", "Anelzinho"};
        termRing.description = "Acessório do tipo anel";
        termRing.category = TermCategory::ITEM;
        termRing.usageExamples = {
            "Procurando anel Ancient +9",
            "Vendo ring +11 de Foema",
            "Alguém dropou anelzin CL por aí?"
        };
        m_parent->addTerm(termRing);
        
        // Colar
        LanguageTerm termNecklace;
        termNecklace.standardForm = "Colar";
        termNecklace.synonyms = {"Necklace", "Neck", "Colarzin", "Colarzinho"};
        termNecklace.description = "Acessório do tipo colar";
        termNecklace.category = TermCategory::ITEM;
        termNecklace.usageExamples = {
            "Procurando colar Ancient +9",
            "Vendo neck +11 de Foema",
            "Alguém tem colarzin celestial para vender?"
        };
        m_parent->addTerm(termNecklace);
        
        // Brinco
        LanguageTerm termEarring;
        termEarring.standardForm = "Brinco";
        termEarring.synonyms = {"Earring", "Ear", "Brinquinho", "Brincão"};
        termEarring.description = "Acessório do tipo brinco";
        termEarring.category = TermCategory::ITEM;
        termEarring.usageExamples = {
            "Procurando brinco Ancient +9",
            "Vendo earring +11 de Foema",
            "Alguém tem brinquinho celestial para vender?"
        };
        m_parent->addTerm(termEarring);
    }
    
    void loadEnhancementTerms() {
        // Termos relacionados a refinamentos e aprimoramentos
        
        // Refino
        LanguageTerm termRefine;
        termRefine.standardForm = "Refinamento";
        termRefine.synonyms = {"Refinar", "Refino", "Refine", "Plus", "+", "Mais"};
        termRefine.description = "Processo de aprimorar equipamentos aumentando seu nível (+1, +2, etc)";
        termRefine.category = TermCategory::ENHANCEMENT;
        termRefine.usageExamples = {
            "Preciso de um set +11 de FM",
            "Vendo arma com refino +13",
            "Busco set A plus 10 ou melhor",
            "Quanto custa refinar do +10 para +11?"
        };
        m_parent->addTerm(termRefine);
        
        // Comp
        LanguageTerm termComp;
        termComp.standardForm = "Composição";
        termComp.synonyms = {"Comp", "Compor", "Combinar"};
        termComp.description = "Processo de combinar itens para melhorá-los ou criar novos";
        termComp.category = TermCategory::ENHANCEMENT;
        termComp.usageExamples = {
            "Preciso de ajuda para comp",
            "Alguém sabe como compor joias?",
            "Quero comp minha arma, mas estou sem item"
        };
        m_parent->addTerm(termComp);
        
        // Pedras
        LanguageTerm termStone;
        termStone.standardForm = "Pedra Secreta";
        termStone.synonyms = {"Pedra", "Stone", "Gema", "Joia", "Jewel"};
        termStone.description = "Item usado para compor equipamentos com bônus extras";
        termStone.category = TermCategory::ITEM;
        termStone.usageExamples = {
            "Comprando pedras de HP",
            "Vendo stones de MP",
            "Preciso de gemas para comp meu set"
        };
        m_parent->addTerm(termStone);
    }
    
    void loadClassTerms() {
        // Nomes de classes e suas variações
        
        // Guerreiro/Fighter
        LanguageTerm termFighter;
        termFighter.standardForm = "Guerreiro";
        termFighter.synonyms = {"Fighter", "War", "Warrior", "Guerra"};
        termFighter.description = "Classe base do tipo guerreiro";
        termFighter.category = TermCategory::CLASS;
        termFighter.usageExamples = {
            "Procurando guild que aceite guerreiro",
            "Meu fighter está no level 100",
            "Qual a melhor build para war?"
        };
        m_parent->addTerm(termFighter);
        
        // Mago/Mage
        LanguageTerm termMage;
        termMage.standardForm = "Mago";
        termMage.synonyms = {"Mage", "Wizard", "Wiz", "Mg", "Maguin"};
        termMage.description = "Classe base do tipo mago";
        termMage.category = TermCategory::CLASS;
        termMage.usageExamples = {
            "Procurando guild que aceite mago",
            "Meu mage está no level 100",
            "Qual a melhor build para wiz?"
        };
        m_parent->addTerm(termMage);
        
        // Arqueiro/Archer
        LanguageTerm termArcher;
        termArcher.standardForm = "Arqueiro";
        termArcher.synonyms = {"Archer", "Arq", "Arch", "Arc"};
        termArcher.description = "Classe base do tipo arqueiro";
        termArcher.category = TermCategory::CLASS;
        termArcher.usageExamples = {
            "Procurando guild que aceite arqueiro",
            "Meu archer está no level 100",
            "Qual a melhor build para arq?"
        };
        m_parent->addTerm(termArcher);
        
        // Evoluções de guerreiro
        LanguageTerm termFM;
        termFM.standardForm = "ForeMaster";
        termFM.synonyms = {"FM", "Fore", "ForeMaster", "Foema"};
        termFM.description = "Evolução avançada de guerreiro";
        termFM.category = TermCategory::CLASS;
        termFM.usageExamples = {
            "Procurando guild que aceite FM",
            "Meu foema está no level 400",
            "Qual a melhor build para FM?"
        };
        m_parent->addTerm(termFM);
        
        LanguageTerm termBM;
        termBM.standardForm = "BloodMaster";
        termBM.synonyms = {"BM", "Blood", "BloodMaster"};
        termBM.description = "Evolução avançada de mago";
        termBM.category = TermCategory::CLASS;
        termBM.usageExamples = {
            "Procurando guild que aceite BM",
            "Meu blood está no level 400",
            "Qual a melhor build para BM?"
        };
        m_parent->addTerm(termBM);
        
        LanguageTerm termHT;
        termHT.standardForm = "Huntress";
        termHT.synonyms = {"HT", "Hunt", "Huntress", "Hunter"};
        termHT.description = "Evolução avançada de arqueiro";
        termHT.category = TermCategory::CLASS;
        termHT.usageExamples = {
            "Procurando guild que aceite HT",
            "Minha hunt está no level 400",
            "Qual a melhor build para HT?"
        };
        m_parent->addTerm(termHT);
        
        // Transcendências
        LanguageTerm termDN;
        termDN.standardForm = "DivineNight";
        termDN.synonyms = {"DN", "Divine", "DivineNight"};
        termDN.description = "Transcendência de ForeMaster";
        termDN.category = TermCategory::CLASS;
        termDN.usageExamples = {
            "Procurando guild que aceite DN",
            "Meu divine está no level 400",
            "Compro set D +11 de DN"
        };
        m_parent->addTerm(termDN);
        
        LanguageTerm termAM;
        termAM.standardForm = "ArchMage";
        termAM.synonyms = {"AM", "Arch", "ArchMage", "Archi"};
        termAM.description = "Transcendência de BloodMaster";
        termAM.category = TermCategory::CLASS;
        termAM.usageExamples = {
            "Procurando guild que aceite AM",
            "Meu arch está no level 400",
            "Compro set D +11 de AM"
        };
        m_parent->addTerm(termAM);
        
        LanguageTerm termSL;
        termSL.standardForm = "SoulLinkage";
        termSL.synonyms = {"SL", "Soul", "SoulLinkage", "Soulink"};
        termSL.description = "Transcendência de Huntress";
        termSL.category = TermCategory::CLASS;
        termSL.usageExamples = {
            "Procurando guild que aceite SL",
            "Minha soul está no level 400",
            "Compro set D +11 de SL"
        };
        m_parent->addTerm(termSL);
    }
    
    void loadSkillTerms() {
        // Habilidades e suas variações
        
        // Geral
        LanguageTerm termSkill;
        termSkill.standardForm = "Habilidade";
        termSkill.synonyms = {"Skill", "Hab", "Técnica", "Poder"};
        termSkill.description = "Habilidades especiais das classes";
        termSkill.category = TermCategory::SKILL;
        termSkill.usageExamples = {
            "Qual a melhor skill para upar?",
            "Preciso de ajuda com minhas habilidades",
            "Qual a skill mais forte de BM?"
        };
        m_parent->addTerm(termSkill);
        
        // Tipos de dano
        LanguageTerm termAoe;
        termAoe.standardForm = "Área de Efeito";
        termAoe.synonyms = {"AoE", "Area", "Área", "Dano em área"};
        termAoe.description = "Habilidades que causam dano em área";
        termAoe.category = TermCategory::SKILL;
        termAoe.usageExamples = {
            "Preciso de mais skills de AoE",
            "Qual a melhor área para BM?",
            "Esse mob morre rápido com AoE"
        };
        m_parent->addTerm(termAoe);
        
        // Buffs
        LanguageTerm termBuff;
        termBuff.standardForm = "Aprimoramento";
        termBuff.synonyms = {"Buff", "Bênção", "Aumento", "Boost"};
        termBuff.description = "Habilidades que aumentam atributos temporariamente";
        termBuff.category = TermCategory::SKILL;
        termBuff.usageExamples = {
            "Preciso de buff para guerra",
            "Alguém pode me dar uns buff?",
            "FM tem o melhor buff do jogo"
        };
        m_parent->addTerm(termBuff);
        
        // Debuffs
        LanguageTerm termDebuff;
        termDebuff.standardForm = "Enfraquecimento";
        termDebuff.synonyms = {"Debuff", "Maldição", "Redução", "Nerf"};
        termDebuff.description = "Habilidades que diminuem atributos dos inimigos";
        termDebuff.category = TermCategory::SKILL;
        termDebuff.usageExamples = {
            "Coloca debuff nesse boss",
            "Precisamos de alguém com debuff forte",
            "Esse mob é imune a debuff?"
        };
        m_parent->addTerm(termDebuff);
    }
    
    void loadLocationTerms() {
        // Mapas e suas variações
        
        // Armia
        LanguageTerm termArmia;
        termArmia.standardForm = "Armia";
        termArmia.synonyms = {"Armia City", "Cidade principal", "Arm"};
        termArmia.description = "Cidade principal do jogo";
        termArmia.category = TermCategory::LOCATION;
        termArmia.usageExamples = {
            "Estou vendendo na armia leste",
            "Me encontra em armia",
            "Quem tá em arm agora?"
        };
        m_parent->addTerm(termArmia);
        
        // Dungeon
        LanguageTerm termDungeon;
        termDungeon.standardForm = "Masmorra";
        termDungeon.synonyms = {"Dungeon", "Dg", "Dunga", "Masmorra"};
        termDungeon.description = "Masmorras com monstros e chefes";
        termDungeon.category = TermCategory::LOCATION;
        termDungeon.usageExamples = {
            "Vamos fazer dungeon?",
            "Quem quer ir pra dg comigo?",
            "Essa dungeon é muito difícil"
        };
        m_parent->addTerm(termDungeon);
        
        // Noatun
        LanguageTerm termNoatun;
        termNoatun.standardForm = "Noatun";
        termNoatun.synonyms = {"Noa", "Noatum", "Cidade do porto"};
        termNoatun.description = "Cidade portuária";
        termNoatun.category = TermCategory::LOCATION;
        termNoatun.usageExamples = {
            "Vamos fazer quest em noatun?",
            "Estou em noa agora",
            "Como chego em noatun?"
        };
        m_parent->addTerm(termNoatun);
    }
    
    void loadMobTerms() {
        // Monstros e suas variações
        
        // Geral
        LanguageTerm termMob;
        termMob.standardForm = "Monstro";
        termMob.synonyms = {"Mob", "Monstrinho", "Bicho", "Criatura"};
        termMob.description = "Criaturas hostis que podem ser derrotadas";
        termMob.category = TermCategory::MOB;
        termMob.usageExamples = {
            "Onde tem bons mobs para upar?",
            "Esse mob dropa item bom?",
            "Cuidado com o mob elite ali"
        };
        m_parent->addTerm(termMob);
        
        // Boss
        LanguageTerm termBoss;
        termBoss.standardForm = "Chefe";
        termBoss.synonyms = {"Boss", "Chefão", "Elite", "Guardião"};
        termBoss.description = "Monstro poderoso que geralmente dropa itens raros";
        termBoss.category = TermCategory::MOB;
        termBoss.usageExamples = {
            "Vamos matar o boss?",
            "Quem quer enfrentar o chefão comigo?",
            "Quando vai nascer o boss?"
        };
        m_parent->addTerm(termBoss);
        
        // NPCs específicos
        LanguageTerm termTroll;
        termTroll.standardForm = "Troll";
        termTroll.synonyms = {"Troll", "Trollzinho", "Trolls"};
        termTroll.description = "Monstro do tipo troll encontrado em diversas áreas";
        termTroll.category = TermCategory::MOB;
        termTroll.usageExamples = {
            "Vamos matar trolls em armia?",
            "Esses trolls estão dropando bem",
            "Onde tem mais trolls para farm?"
        };
        m_parent->addTerm(termTroll);
    }
    
    void loadQuestTerms() {
        // Missões e suas variações
        
        // Geral
        LanguageTerm termQuest;
        termQuest.standardForm = "Missão";
        termQuest.synonyms = {"Quest", "Missão", "Tarefa", "Objetivo"};
        termQuest.description = "Tarefas atribuídas por NPCs que geralmente oferecem recompensas";
        termQuest.category = TermCategory::QUEST;
        termQuest.usageExamples = {
            "Alguém pode me ajudar com essa quest?",
            "Onde começa a missão do level 100?",
            "Essa quest dá muita exp"
        };
        m_parent->addTerm(termQuest);
        
        // Daily
        LanguageTerm termDaily;
        termDaily.standardForm = "Missão Diária";
        termDaily.synonyms = {"Daily", "Diária", "Missão do dia", "Dia", "Quest diária"};
        termDaily.description = "Missões que podem ser realizadas uma vez por dia";
        termDaily.category = TermCategory::QUEST;
        termDaily.usageExamples = {
            "Já fez suas dailys hoje?",
            "Vamos fazer as diárias juntos?",
            "Essa daily dá bons itens"
        };
        m_parent->addTerm(termDaily);
    }
    
    void loadGeneralSlang() {
        // Gírias gerais do jogo
        
        // Upar/Level up
        LanguageTerm termLevelUp;
        termLevelUp.standardForm = "Subir de Nível";
        termLevelUp.synonyms = {"Upar", "Levelar", "Ganhar level", "Subir level", "Level up", "XP"};
        termLevelUp.description = "Processo de ganhar experiência e subir de nível";
        termLevelUp.category = TermCategory::GENERAL_SLANG;
        termLevelUp.usageExamples = {
            "Vamos upar juntos?",
            "Onde é bom para levelar agora?",
            "Preciso upar rápido até o 400"
        };
        m_parent->addTerm(termLevelUp);
        
        // Drop
        LanguageTerm termDrop;
        termDrop.standardForm = "Dropar";
        termDrop.synonyms = {"Drop", "Dropou", "Caiu", "Largou", "Soltar"};
        termDrop.description = "Quando um item é obtido ao derrotar um monstro";
        termDrop.category = TermCategory::GENERAL_SLANG;
        termDrop.usageExamples = {
            "Esse mob dropa set bom?",
            "Dropei um item raro hoje!",
            "Quanto tempo até dropar um celestial?"
        };
        m_parent->addTerm(termDrop);
        
        // Farm
        LanguageTerm termFarm;
        termFarm.standardForm = "Farmar";
        termFarm.synonyms = {"Farm", "Farming", "Coletar", "Pegar", "Caçar"};
        termFarm.description = "Processo de matar monstros repetidamente para obter itens ou experiência";
        termFarm.category = TermCategory::GENERAL_SLANG;
        termFarm.usageExamples = {
            "Vamos farmar trolls?",
            "Estou farmando gold para comprar set",
            "Melhor lugar para farm de pedras?"
        };
        m_parent->addTerm(termFarm);
        
        // Gold
        LanguageTerm termGold;
        termGold.standardForm = "Gold";
        termGold.synonyms = {"Gold", "Ouro", "GP", "Dinheiro", "Grana"};
        termGold.description = "Moeda principal do jogo";
        termGold.category = TermCategory::ECONOMY;
        termGold.usageExamples = {
            "Quanto gold custa esse item?",
            "Estou farmando gold",
            "Preciso de gold para comprar set"
        };
        m_parent->addTerm(termGold);
        
        // PVP
        LanguageTerm termPvp;
        termPvp.standardForm = "PVP";
        termPvp.synonyms = {"PVP", "PK", "Player vs Player", "Duelo"};
        termPvp.description = "Combate entre jogadores";
        termPvp.category = TermCategory::PVP;
        termPvp.usageExamples = {
            "Vamos fazer PVP?",
            "Essa área é PK",
            "Quem quer duelo em armia?"
        };
        m_parent->addTerm(termPvp);
    }
    
    void loadCommandTerms() {
        // Comandos administrativos
        
        // Teleport
        LanguageTerm termTeleport;
        termTeleport.standardForm = "Teleportar";
        termTeleport.synonyms = {"TP", "Teleporte", "Teleportação", "Mover", "Teletransportar"};
        termTeleport.description = "Mover-se instantaneamente para outro local";
        termTeleport.category = TermCategory::COMMAND;
        termTeleport.usageExamples = {
            "Admin, pode me dar tp para armia?",
            "Preciso de teleporte para dungeon",
            "Como faço para teleportar para noatun?"
        };
        m_parent->addTerm(termTeleport);
        
        // Ban
        LanguageTerm termBan;
        termBan.standardForm = "Banir";
        termBan.synonyms = {"Ban", "Banimento", "Expulsar", "Suspender"};
        termBan.description = "Impedir um jogador de acessar o jogo";
        termBan.category = TermCategory::COMMAND;
        termBan.usageExamples = {
            "Esse jogador está usando hack, ban nele",
            "Fui banido sem motivo",
            "O GM pode banir quem for tóxico"
        };
        m_parent->addTerm(termBan);
        
        // Broadcast
        LanguageTerm termBroadcast;
        termBroadcast.standardForm = "Anúncio";
        termBroadcast.synonyms = {"Broadcast", "Anuncio", "Mensagem global", "Aviso", "Notificação"};
        termBroadcast.description = "Mensagem enviada para todos os jogadores";
        termBroadcast.category = TermCategory::COMMAND;
        termBroadcast.usageExamples = {
            "O GM pode dar um broadcast sobre o evento?",
            "Vi no anúncio que vai ter drop rate hoje",
            "Manda um aviso para todos sobre a guerra"
        };
        m_parent->addTerm(termBroadcast);
        
        // Criar item
        LanguageTerm termCreateItem;
        termCreateItem.standardForm = "Criar Item";
        termCreateItem.synonyms = {"Create", "Criar", "Spawn item", "Gerar", "Fazer item"};
        termCreateItem.description = "Criar um item no jogo (comando administrativo)";
        termCreateItem.category = TermCategory::COMMAND;
        termCreateItem.usageExamples = {
            "Admin, pode criar um set D +11 para mim?",
            "Como faço para gerar um item celestial?",
            "O GM tem poder para spawnar qualquer item"
        };
        m_parent->addTerm(termCreateItem);
    }
    
    bool loadLanguagePatterns() {
        // Padrões comuns de linguagem relacionados a WYD
        
        // Criar equipamento
        LanguagePattern patternCreateEquip;
        patternCreateEquip.pattern = "(cria|criar|gera|gerar|faz|fazer|quero|queria|preciso|adiciona) (um|uma|umas|uns)? (set|armadura|arma|equip|) ([A-Da-d]|cl|anc|celestial|ancient|foema|divine|arc|hunt) \\+?(\\d+)? (de|para|da|do)? ?([a-z]+)?";
        patternCreateEquip.interpretation = "Criar um equipamento de classe específica com refino";
        patternCreateEquip.intentMapping = "create_equipment";
        patternCreateEquip.examples = {
            "cria um set D +11 de FM",
            "quero uma arma celestial +10 para BM",
            "gera um set A +9 de huntress",
            "faz uma armadura ancient +7 de divine",
            "preciso de um set arc +8 de sl"
        };
        patternCreateEquip.parameterMappings = {
            {"itemType", "$3"},
            {"setType", "$4"},
            {"refineLevel", "$5"},
            {"playerClass", "$7"}
        };
        m_parent->addPattern(patternCreateEquip);
        
        // Iniciar evento de drop
        LanguagePattern patternDropEvent;
        patternDropEvent.pattern = "(inicia|iniciar|começa|começar|faz|fazer|criar|ativa|ativar) (um|uma)? (evento|drop|droprate|) (de|para|com)? ?(drop|chance|taxa|)? (de)? ?([a-z\\s]+) (em|nos|nas|para|no)? ?([a-z\\s]+)?";
        patternDropEvent.interpretation = "Iniciar um evento de drop para item específico em monstros/locais";
        patternDropEvent.intentMapping = "create_drop_event";
        patternDropEvent.examples = {
            "inicia um evento de drop de poeira de lac em trolls",
            "começa um drop rate de armadura D em noatun",
            "criar um evento para pedras em masmorras",
            "ativa drop de set celestial no deserto",
            "faz um drop de joias em todos os mobs"
        };
        patternDropEvent.parameterMappings = {
            {"eventType", "drop"},
            {"itemName", "$7"},
            {"targetLocation", "$9"}
        };
        m_parent->addPattern(patternDropEvent);
        
        // Gerenciar jogador
        LanguagePattern patternPlayerManage;
        patternPlayerManage.pattern = "(bane|ban|bani|banir|silencia|cala|silenciar|tp|teleporta|move|mover|teleportar|kick|expulsa|expulsar|kika|kikar) (o jogador|a conta|o char|o personagem|o nick|o)? ?([a-zA-Z0-9]+)";
        patternPlayerManage.interpretation = "Executar ação administrativa em um jogador";
        patternPlayerManage.intentMapping = "manage_player";
        patternPlayerManage.examples = {
            "bane o jogador Testador123",
            "silencia o nick Falador",
            "tp o personagem Hunter para armia",
            "kick o char Problemático",
            "teleporta Guerreiro123 para arena"
        };
        patternPlayerManage.parameterMappings = {
            {"action", "$1"},
            {"playerName", "$3"}
        };
        m_parent->addPattern(patternPlayerManage);
        
        // Broadcast
        LanguagePattern patternBroadcast;
        patternBroadcast.pattern = "(anuncia|broadcast|avisa|notifica|manda mensagem|envia mensagem) (que|sobre|para todos|global|no server)? ?(.+)";
        patternBroadcast.interpretation = "Enviar mensagem para todos os jogadores";
        patternBroadcast.intentMapping = "broadcast_message";
        patternBroadcast.examples = {
            "anuncia que vai ter evento em 5 minutos",
            "broadcast guerra de guildas às 20h",
            "avisa que o servidor vai reiniciar",
            "notifica sobre o drop rate ativado",
            "manda mensagem para todos sobre nova atualização"
        };
        patternBroadcast.parameterMappings = {
            {"message", "$3"}
        };
        m_parent->addPattern(patternBroadcast);
        
        return true;
    }
private:
    WYDLanguageDatabase* m_parent;
};

// Implementação dos métodos da classe principal
WYDLanguageDatabase::WYDLanguageDatabase() {
    pImpl = std::make_unique<Impl>(this);
}

WYDLanguageDatabase::~WYDLanguageDatabase() {
}

bool WYDLanguageDatabase::initialize(const std::shared_ptr<core::WYDStudioFacade>& facade) {
    m_facade = facade;
    
    // Carregar dados padrão
    bool termsLoaded = loadDefaultTerms();
    bool patternsLoaded = loadDefaultPatterns();
    
    return termsLoaded && patternsLoaded;
}

bool WYDLanguageDatabase::loadDefaultTerms() {
    return pImpl->loadWYDTerminologyData();
}

bool WYDLanguageDatabase::loadDefaultPatterns() {
    return pImpl->loadLanguagePatterns();
}

bool WYDLanguageDatabase::loadFromFile(const std::string& filePath) {
    try {
        std::ifstream file(filePath);
        if (!file.is_open()) {
            return false;
        }
        
        json data = json::parse(file);
        
        // Limpar dados existentes
        m_termsByStandardForm.clear();
        m_synonymToStandardMap.clear();
        m_patterns.clear();
        
        // Carregar termos
        if (data.contains("terms") && data["terms"].is_array()) {
            for (const auto& jsonTerm : data["terms"]) {
                LanguageTerm term;
                if (parseTermFromJson(jsonTerm, term)) {
                    addTerm(term);
                }
            }
        }
        
        // Carregar padrões
        if (data.contains("patterns") && data["patterns"].is_array()) {
            for (const auto& jsonPattern : data["patterns"]) {
                LanguagePattern pattern;
                if (parsePatternFromJson(jsonPattern, pattern)) {
                    addPattern(pattern);
                }
            }
        }
        
        // Reconstruir índices
        buildIndices();
        
        return true;
    } catch (const std::exception& e) {
        // Log do erro
        return false;
    }
}

bool WYDLanguageDatabase::saveToFile(const std::string& filePath) {
    try {
        json data;
        
        // Converter termos para JSON
        json termsArray = json::array();
        for (const auto& [_, term] : m_termsByStandardForm) {
            termsArray.push_back(termToJson(term));
        }
        data["terms"] = termsArray;
        
        // Converter padrões para JSON
        json patternsArray = json::array();
        for (const auto& pattern : m_patterns) {
            patternsArray.push_back(patternToJson(pattern));
        }
        data["patterns"] = patternsArray;
        
        // Salvar no arquivo
        std::ofstream file(filePath);
        if (!file.is_open()) {
            return false;
        }
        
        file << std::setw(2) << data;
        return true;
    } catch (const std::exception& e) {
        // Log do erro
        return false;
    }
}

bool WYDLanguageDatabase::addTerm(const LanguageTerm& term) {
    // Verificar se o termo já existe
    if (m_termsByStandardForm.find(term.standardForm) != m_termsByStandardForm.end()) {
        return false;
    }
    
    // Adicionar termo
    m_termsByStandardForm[term.standardForm] = term;
    
    // Adicionar sinônimos ao mapa de sinônimos
    for (const auto& synonym : term.synonyms) {
        m_synonymToStandardMap[synonym] = term.standardForm;
    }
    
    return true;
}

bool WYDLanguageDatabase::updateTerm(const std::string& standardForm, const LanguageTerm& term) {
    // Verificar se o termo existe
    auto it = m_termsByStandardForm.find(standardForm);
    if (it == m_termsByStandardForm.end()) {
        return false;
    }
    
    // Remover sinônimos antigos do mapa
    for (const auto& synonym : it->second.synonyms) {
        m_synonymToStandardMap.erase(synonym);
    }
    
    // Atualizar termo
    m_termsByStandardForm[standardForm] = term;
    
    // Adicionar sinônimos novos ao mapa
    for (const auto& synonym : term.synonyms) {
        m_synonymToStandardMap[synonym] = term.standardForm;
    }
    
    return true;
}

bool WYDLanguageDatabase::removeTerm(const std::string& standardForm) {
    // Verificar se o termo existe
    auto it = m_termsByStandardForm.find(standardForm);
    if (it == m_termsByStandardForm.end()) {
        return false;
    }
    
    // Remover sinônimos do mapa
    for (const auto& synonym : it->second.synonyms) {
        m_synonymToStandardMap.erase(synonym);
    }
    
    // Remover termo
    m_termsByStandardForm.erase(it);
    
    return true;
}

std::optional<LanguageTerm> WYDLanguageDatabase::getTermByStandardForm(const std::string& standardForm) const {
    auto it = m_termsByStandardForm.find(standardForm);
    if (it != m_termsByStandardForm.end()) {
        return it->second;
    }
    
    return std::nullopt;
}

std::optional<LanguageTerm> WYDLanguageDatabase::getTermBySynonym(const std::string& variation) const {
    auto it = m_synonymToStandardMap.find(variation);
    if (it != m_synonymToStandardMap.end()) {
        return getTermByStandardForm(it->second);
    }
    
    return std::nullopt;
}

std::vector<LanguageTerm> WYDLanguageDatabase::getTermsByCategory(TermCategory category) const {
    std::vector<LanguageTerm> result;
    
    for (const auto& [_, term] : m_termsByStandardForm) {
        if (term.category == category) {
            result.push_back(term);
        }
    }
    
    return result;
}

bool WYDLanguageDatabase::addPattern(const LanguagePattern& pattern) {
    m_patterns.push_back(pattern);
    return true;
}

bool WYDLanguageDatabase::updatePattern(const std::string& patternId, const LanguagePattern& pattern) {
    for (auto& existingPattern : m_patterns) {
        if (existingPattern.pattern == patternId) {
            existingPattern = pattern;
            return true;
        }
    }
    
    return false;
}

bool WYDLanguageDatabase::removePattern(const std::string& patternId) {
    auto it = std::find_if(m_patterns.begin(), m_patterns.end(), 
                           [&patternId](const LanguagePattern& pattern) {
                               return pattern.pattern == patternId;
                           });
    
    if (it != m_patterns.end()) {
        m_patterns.erase(it);
        return true;
    }
    
    return false;
}

std::vector<LanguagePattern> WYDLanguageDatabase::getAllPatterns() const {
    return m_patterns;
}

RecognizedIntent WYDLanguageDatabase::recognizeIntent(const std::string& input) const {
    RecognizedIntent result;
    result.originalInput = input;
    result.confidence = 0.0f;
    
    // Verificar se a entrada se enquadra em algum dos padrões
    std::vector<std::pair<std::string, float>> matches = matchPatterns(input);
    
    if (!matches.empty()) {
        // Ordenar matches por confiança
        std::sort(matches.begin(), matches.end(), 
                  [](const auto& a, const auto& b) {
                      return a.second > b.second;
                  });
        
        // Definir intent principal
        result.intent = matches[0].first;
        result.confidence = matches[0].second;
        
        // Extrair parâmetros para a intent principal
        for (const auto& pattern : m_patterns) {
            if (pattern.intentMapping == result.intent) {
                result.parameters = extractParameters(input, pattern);
                break;
            }
        }
        
        // Adicionar intents alternativas
        for (size_t i = 1; i < matches.size() && i < 3; ++i) {
            result.alternativeIntents.push_back(matches[i].first);
        }
    }
    
    return result;
}

std::string WYDLanguageDatabase::normalizeTerm(const std::string& term) const {
    // Verifica se o termo é um sinônimo conhecido
    auto it = m_synonymToStandardMap.find(term);
    if (it != m_synonymToStandardMap.end()) {
        return it->second;
    }
    
    // Tenta encontrar uma correspondência parcial entre sinônimos
    for (const auto& [synonym, standardForm] : m_synonymToStandardMap) {
        if (term.find(synonym) != std::string::npos || synonym.find(term) != std::string::npos) {
            return standardForm;
        }
    }
    
    // Se não encontrar, retorna o termo original
    return term;
}

std::vector<std::string> WYDLanguageDatabase::expandTerm(const std::string& term) const {
    std::vector<std::string> result;
    
    // Adiciona o termo original
    result.push_back(term);
    
    // Verifica se é um termo padrão
    auto standardTerm = getTermByStandardForm(term);
    if (standardTerm) {
        // Adiciona sinônimos
        for (const auto& synonym : standardTerm->synonyms) {
            if (std::find(result.begin(), result.end(), synonym) == result.end()) {
                result.push_back(synonym);
            }
        }
    } else {
        // Verifica se é um sinônimo
        auto synonymTerm = getTermBySynonym(term);
        if (synonymTerm) {
            // Adiciona forma padrão
            if (std::find(result.begin(), result.end(), synonymTerm->standardForm) == result.end()) {
                result.push_back(synonymTerm->standardForm);
            }
            
            // Adiciona outros sinônimos
            for (const auto& synonym : synonymTerm->synonyms) {
                if (synonym != term && std::find(result.begin(), result.end(), synonym) == result.end()) {
                    result.push_back(synonym);
                }
            }
        }
    }
    
    return result;
}

bool WYDLanguageDatabase::termExists(const std::string& term) const {
    return m_termsByStandardForm.find(term) != m_termsByStandardForm.end() || 
           m_synonymToStandardMap.find(term) != m_synonymToStandardMap.end();
}

std::string WYDLanguageDatabase::getTermDescription(const std::string& term) const {
    // Verifica se é um termo padrão
    auto standardTerm = getTermByStandardForm(term);
    if (standardTerm) {
        return standardTerm->description;
    }
    
    // Verifica se é um sinônimo
    auto synonymTerm = getTermBySynonym(term);
    if (synonymTerm) {
        return synonymTerm->description;
    }
    
    return "";
}

std::vector<std::string> WYDLanguageDatabase::tokenize(const std::string& input) const {
    return splitInput(input);
}

bool WYDLanguageDatabase::trainWithExamples(const std::map<std::string, std::string>& examples) {
    // Implementação simplificada para treinamento com exemplos
    for (const auto& [example, intent] : examples) {
        // Analisa o exemplo e extrai padrões
        
        // Esta é uma implementação muito simples que não faz nada real
        // Em um caso real, usaria-se algum algoritmo de aprendizado de máquina
    }
    
    return true;
}

bool WYDLanguageDatabase::exportForAITraining(const std::string& outputPath) const {
    try {
        json trainingData;
        
        // Adicionar termos
        json termsData = json::array();
        for (const auto& [_, term] : m_termsByStandardForm) {
            json termEntry;
            termEntry["standard"] = term.standardForm;
            termEntry["synonyms"] = term.synonyms;
            termEntry["category"] = categoryToString(term.category);
            termEntry["examples"] = term.usageExamples;
            
            termsData.push_back(termEntry);
        }
        trainingData["terms"] = termsData;
        
        // Adicionar padrões
        json patternsData = json::array();
        for (const auto& pattern : m_patterns) {
            json patternEntry;
            patternEntry["pattern"] = pattern.pattern;
            patternEntry["intent"] = pattern.intentMapping;
            patternEntry["examples"] = pattern.examples;
            
            patternsData.push_back(patternEntry);
        }
        trainingData["patterns"] = patternsData;
        
        // Salvar no arquivo
        std::ofstream file(outputPath);
        if (!file.is_open()) {
            return false;
        }
        
        file << std::setw(2) << trainingData;
        return true;
    } catch (const std::exception& e) {
        // Log do erro
        return false;
    }
}

bool WYDLanguageDatabase::loadFromClientData(const std::string& clientPath) {
    // Esta função analisaria os arquivos do cliente para extrair nomes de itens, classes, etc.
    // Aqui apenas retornamos true como placeholder
    return true;
}

std::vector<LanguageTerm> WYDLanguageDatabase::searchTerms(const std::string& query) const {
    std::vector<LanguageTerm> results;
    
    // Converter query para lowercase para busca case-insensitive
    std::string lowercaseQuery = query;
    std::transform(lowercaseQuery.begin(), lowercaseQuery.end(), lowercaseQuery.begin(),
                   [](unsigned char c){ return std::tolower(c); });
    
    for (const auto& [_, term] : m_termsByStandardForm) {
        bool found = false;
        
        // Verificar na forma padrão
        std::string lowercaseStandard = term.standardForm;
        std::transform(lowercaseStandard.begin(), lowercaseStandard.end(), lowercaseStandard.begin(),
                       [](unsigned char c){ return std::tolower(c); });
        
        if (lowercaseStandard.find(lowercaseQuery) != std::string::npos) {
            found = true;
        }
        
        // Verificar nos sinônimos
        if (!found) {
            for (const auto& synonym : term.synonyms) {
                std::string lowercaseSynonym = synonym;
                std::transform(lowercaseSynonym.begin(), lowercaseSynonym.end(), lowercaseSynonym.begin(),
                               [](unsigned char c){ return std::tolower(c); });
                
                if (lowercaseSynonym.find(lowercaseQuery) != std::string::npos) {
                    found = true;
                    break;
                }
            }
        }
        
        // Verificar na descrição
        if (!found) {
            std::string lowercaseDesc = term.description;
            std::transform(lowercaseDesc.begin(), lowercaseDesc.end(), lowercaseDesc.begin(),
                           [](unsigned char c){ return std::tolower(c); });
            
            if (lowercaseDesc.find(lowercaseQuery) != std::string::npos) {
                found = true;
            }
        }
        
        if (found) {
            results.push_back(term);
        }
    }
    
    return results;
}

std::vector<std::pair<std::string, size_t>> WYDLanguageDatabase::findWYDTerms(const std::string& text) const {
    std::vector<std::pair<std::string, size_t>> result;
    
    // Converter texto para lowercase para busca case-insensitive
    std::string lowercaseText = text;
    std::transform(lowercaseText.begin(), lowercaseText.end(), lowercaseText.begin(),
                   [](unsigned char c){ return std::tolower(c); });
    
    // Verificar cada termo padrão
    for (const auto& [standard, term] : m_termsByStandardForm) {
        std::string lowercaseStandard = standard;
        std::transform(lowercaseStandard.begin(), lowercaseStandard.end(), lowercaseStandard.begin(),
                       [](unsigned char c){ return std::tolower(c); });
        
        size_t pos = lowercaseText.find(lowercaseStandard);
        if (pos != std::string::npos) {
            result.emplace_back(standard, pos);
        }
        
        // Verificar sinônimos
        for (const auto& synonym : term.synonyms) {
            std::string lowercaseSynonym = synonym;
            std::transform(lowercaseSynonym.begin(), lowercaseSynonym.end(), lowercaseSynonym.begin(),
                           [](unsigned char c){ return std::tolower(c); });
            
            pos = lowercaseText.find(lowercaseSynonym);
            if (pos != std::string::npos) {
                result.emplace_back(synonym, pos);
            }
        }
    }
    
    // Ordenar por posição
    std::sort(result.begin(), result.end(), 
              [](const auto& a, const auto& b) {
                  return a.second < b.second;
              });
    
    return result;
}

std::vector<std::string> WYDLanguageDatabase::generateSuggestions(const std::string& partialInput, int maxSuggestions) const {
    std::vector<std::string> result;
    
    // Converter entrada para lowercase para busca case-insensitive
    std::string lowercaseInput = partialInput;
    std::transform(lowercaseInput.begin(), lowercaseInput.end(), lowercaseInput.begin(),
                   [](unsigned char c){ return std::tolower(c); });
    
    // Tokens da entrada
    std::vector<std::string> tokens = splitInput(lowercaseInput);
    
    // Verifica se a entrada se encaixa em algum padrão conhecido
    for (const auto& pattern : m_patterns) {
        for (const auto& example : pattern.examples) {
            std::string lowercaseExample = example;
            std::transform(lowercaseExample.begin(), lowercaseExample.end(), lowercaseExample.begin(),
                           [](unsigned char c){ return std::tolower(c); });
            
            if (lowercaseExample.find(lowercaseInput) == 0) {
                // O exemplo começa com a entrada parcial
                if (std::find(result.begin(), result.end(), example) == result.end()) {
                    result.push_back(example);
                    if (result.size() >= static_cast<size_t>(maxSuggestions)) {
                        return result;
                    }
                }
            }
        }
    }
    
    // Se não temos sugestões suficientes, tenta encontrar termos que correspondam aos tokens
    if (result.size() < static_cast<size_t>(maxSuggestions) && !tokens.empty()) {
        std::string lastToken = tokens.back();
        
        for (const auto& [standard, term] : m_termsByStandardForm) {
            std::string lowercaseStandard = standard;
            std::transform(lowercaseStandard.begin(), lowercaseStandard.end(), lowercaseStandard.begin(),
                           [](unsigned char c){ return std::tolower(c); });
            
            if (lowercaseStandard.find(lastToken) == 0) {
                // Gera uma sugestão substituindo o último token
                std::string suggestion = partialInput.substr(0, partialInput.size() - lastToken.size()) + standard;
                
                if (std::find(result.begin(), result.end(), suggestion) == result.end()) {
                    result.push_back(suggestion);
                    if (result.size() >= static_cast<size_t>(maxSuggestions)) {
                        return result;
                    }
                }
            }
            
            // Verifica nos sinônimos
            for (const auto& synonym : term.synonyms) {
                std::string lowercaseSynonym = synonym;
                std::transform(lowercaseSynonym.begin(), lowercaseSynonym.end(), lowercaseSynonym.begin(),
                               [](unsigned char c){ return std::tolower(c); });
                
                if (lowercaseSynonym.find(lastToken) == 0) {
                    // Gera uma sugestão substituindo o último token
                    std::string suggestion = partialInput.substr(0, partialInput.size() - lastToken.size()) + synonym;
                    
                    if (std::find(result.begin(), result.end(), suggestion) == result.end()) {
                        result.push_back(suggestion);
                        if (result.size() >= static_cast<size_t>(maxSuggestions)) {
                            return result;
                        }
                    }
                }
            }
        }
    }
    
    return result;
}

std::vector<std::string> WYDLanguageDatabase::generateCommandVariations(const std::string& intent, 
                                                                     const std::map<std::string, std::string>& parameters) const {
    std::vector<std::string> result;
    
    // Encontra padrões associados à intent
    std::vector<LanguagePattern> matchingPatterns;
    for (const auto& pattern : m_patterns) {
        if (pattern.intentMapping == intent) {
            matchingPatterns.push_back(pattern);
        }
    }
    
    // Se não encontrou padrões, retorna vazio
    if (matchingPatterns.empty()) {
        return result;
    }
    
    // Para cada padrão, gera um exemplo substituindo os parâmetros
    for (const auto& pattern : matchingPatterns) {
        // Usa os exemplos como base
        for (const auto& example : pattern.examples) {
            // Adiciona o exemplo original
            result.push_back(example);
            
            // Gera variações substituindo termos por sinônimos
            std::vector<std::pair<std::string, size_t>> terms = findWYDTerms(example);
            
            if (!terms.empty()) {
                // Para cada termo encontrado no exemplo
                for (const auto& [term, pos] : terms) {
                    // Encontra sinônimos
                    std::vector<std::string> variations = expandTerm(term);
                    
                    if (variations.size() > 1) {  // Se tem mais que o próprio termo
                        for (const auto& variation : variations) {
                            if (variation != term) {  // Pula o próprio termo
                                // Cria uma variação substituindo o termo pelo sinônimo
                                std::string newVariation = example;
                                newVariation.replace(pos, term.length(), variation);
                                
                                if (std::find(result.begin(), result.end(), newVariation) == result.end()) {
                                    result.push_back(newVariation);
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    
    return result;
}

// Métodos auxiliares
void WYDLanguageDatabase::buildIndices() {
    // Limpar índices existentes
    m_termsByCategory.clear();
    m_allVariations.clear();
    
    // Construir índice por categoria
    for (const auto& [standardForm, term] : m_termsByStandardForm) {
        m_termsByCategory[term.category].push_back(standardForm);
        
        // Adicionar todas as variações ao conjunto
        m_allVariations.insert(standardForm);
        for (const auto& synonym : term.synonyms) {
            m_allVariations.insert(synonym);
        }
    }
}

bool WYDLanguageDatabase::parseTermFromJson(const json& jsonTerm, LanguageTerm& term) {
    try {
        term.standardForm = jsonTerm.value("standardForm", "");
        
        if (jsonTerm.contains("synonyms") && jsonTerm["synonyms"].is_array()) {
            term.synonyms.clear();
            for (const auto& synonym : jsonTerm["synonyms"]) {
                term.synonyms.push_back(synonym.get<std::string>());
            }
        }
        
        term.description = jsonTerm.value("description", "");
        term.category = stringToCategory(jsonTerm.value("category", "GENERAL_SLANG"));
        
        if (jsonTerm.contains("relatedTerms") && jsonTerm["relatedTerms"].is_array()) {
            term.relatedTerms.clear();
            for (const auto& related : jsonTerm["relatedTerms"]) {
                term.relatedTerms.push_back(related.get<std::string>());
            }
        }
        
        if (jsonTerm.contains("attributes") && jsonTerm["attributes"].is_object()) {
            term.attributes.clear();
            for (auto it = jsonTerm["attributes"].begin(); it != jsonTerm["attributes"].end(); ++it) {
                term.attributes[it.key()] = it.value().get<std::string>();
            }
        }
        
        term.internalId = jsonTerm.value("internalId", "");
        
        if (jsonTerm.contains("usageExamples") && jsonTerm["usageExamples"].is_array()) {
            term.usageExamples.clear();
            for (const auto& example : jsonTerm["usageExamples"]) {
                term.usageExamples.push_back(example.get<std::string>());
            }
        }
        
        return true;
    } catch (const std::exception& e) {
        // Log do erro
        return false;
    }
}

json WYDLanguageDatabase::termToJson(const LanguageTerm& term) const {
    json result;
    
    result["standardForm"] = term.standardForm;
    result["synonyms"] = term.synonyms;
    result["description"] = term.description;
    result["category"] = categoryToString(term.category);
    result["relatedTerms"] = term.relatedTerms;
    
    json attributesJson = json::object();
    for (const auto& [key, value] : term.attributes) {
        attributesJson[key] = value;
    }
    result["attributes"] = attributesJson;
    
    result["internalId"] = term.internalId;
    result["usageExamples"] = term.usageExamples;
    
    return result;
}

bool WYDLanguageDatabase::parsePatternFromJson(const json& jsonPattern, LanguagePattern& pattern) {
    try {
        pattern.pattern = jsonPattern.value("pattern", "");
        pattern.interpretation = jsonPattern.value("interpretation", "");
        pattern.intentMapping = jsonPattern.value("intentMapping", "");
        
        if (jsonPattern.contains("examples") && jsonPattern["examples"].is_array()) {
            pattern.examples.clear();
            for (const auto& example : jsonPattern["examples"]) {
                pattern.examples.push_back(example.get<std::string>());
            }
        }
        
        if (jsonPattern.contains("parameterMappings") && jsonPattern["parameterMappings"].is_object()) {
            pattern.parameterMappings.clear();
            for (auto it = jsonPattern["parameterMappings"].begin(); it != jsonPattern["parameterMappings"].end(); ++it) {
                pattern.parameterMappings[it.key()] = it.value().get<std::string>();
            }
        }
        
        return true;
    } catch (const std::exception& e) {
        // Log do erro
        return false;
    }
}

json WYDLanguageDatabase::patternToJson(const LanguagePattern& pattern) const {
    json result;
    
    result["pattern"] = pattern.pattern;
    result["interpretation"] = pattern.interpretation;
    result["intentMapping"] = pattern.intentMapping;
    result["examples"] = pattern.examples;
    
    json paramMappingsJson = json::object();
    for (const auto& [key, value] : pattern.parameterMappings) {
        paramMappingsJson[key] = value;
    }
    result["parameterMappings"] = paramMappingsJson;
    
    return result;
}

TermCategory WYDLanguageDatabase::stringToCategory(const std::string& categoryStr) const {
    if (categoryStr == "ITEM") return TermCategory::ITEM;
    if (categoryStr == "CLASS") return TermCategory::CLASS;
    if (categoryStr == "SKILL") return TermCategory::SKILL;
    if (categoryStr == "LOCATION") return TermCategory::LOCATION;
    if (categoryStr == "MOB") return TermCategory::MOB;
    if (categoryStr == "QUEST") return TermCategory::QUEST;
    if (categoryStr == "ATTRIBUTE") return TermCategory::ATTRIBUTE;
    if (categoryStr == "ENHANCEMENT") return TermCategory::ENHANCEMENT;
    if (categoryStr == "EVENT") return TermCategory::EVENT;
    if (categoryStr == "ECONOMY") return TermCategory::ECONOMY;
    if (categoryStr == "SOCIAL") return TermCategory::SOCIAL;
    if (categoryStr == "COMMAND") return TermCategory::COMMAND;
    if (categoryStr == "PVP") return TermCategory::PVP;
    
    return TermCategory::GENERAL_SLANG;
}

std::string WYDLanguageDatabase::categoryToString(TermCategory category) const {
    switch (category) {
        case TermCategory::ITEM: return "ITEM";
        case TermCategory::CLASS: return "CLASS";
        case TermCategory::SKILL: return "SKILL";
        case TermCategory::LOCATION: return "LOCATION";
        case TermCategory::MOB: return "MOB";
        case TermCategory::QUEST: return "QUEST";
        case TermCategory::ATTRIBUTE: return "ATTRIBUTE";
        case TermCategory::ENHANCEMENT: return "ENHANCEMENT";
        case TermCategory::EVENT: return "EVENT";
        case TermCategory::ECONOMY: return "ECONOMY";
        case TermCategory::SOCIAL: return "SOCIAL";
        case TermCategory::COMMAND: return "COMMAND";
        case TermCategory::PVP: return "PVP";
        case TermCategory::GENERAL_SLANG: return "GENERAL_SLANG";
        default: return "GENERAL_SLANG";
    }
}

std::vector<std::string> WYDLanguageDatabase::splitInput(const std::string& input) const {
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream tokenStream(input);
    
    while (std::getline(tokenStream, token, ' ')) {
        if (!token.empty()) {
            tokens.push_back(token);
        }
    }
    
    return tokens;
}

std::vector<std::pair<std::string, float>> WYDLanguageDatabase::matchPatterns(const std::string& input) const {
    std::vector<std::pair<std::string, float>> matches;
    
    // Converter input para lowercase para correspondência case-insensitive
    std::string lowercaseInput = input;
    std::transform(lowercaseInput.begin(), lowercaseInput.end(), lowercaseInput.begin(),
                   [](unsigned char c){ return std::tolower(c); });
    
    for (const auto& pattern : m_patterns) {
        try {
            std::regex regex(pattern.pattern, std::regex_constants::icase);
            std::smatch match;
            
            if (std::regex_search(lowercaseInput, match, regex)) {
                // Calcula um valor de confiança baseado na correspondência
                float confidence = static_cast<float>(match[0].length()) / lowercaseInput.length();
                
                // Ajusta a confiança com base na especificidade do padrão
                confidence *= 0.7f + (0.3f * pattern.pattern.length() / 100.0f);
                
                // Limita entre 0 e 1
                confidence = std::min(1.0f, std::max(0.0f, confidence));
                
                matches.emplace_back(pattern.intentMapping, confidence);
            }
        } catch (const std::regex_error&) {
            // Ignora erros de regex inválida
            continue;
        }
    }
    
    return matches;
}

std::map<std::string, std::string> WYDLanguageDatabase::extractParameters(const std::string& input, const LanguagePattern& pattern) const {
    std::map<std::string, std::string> params;
    
    try {
        std::regex regex(pattern.pattern, std::regex_constants::icase);
        std::smatch match;
        
        if (std::regex_search(input, match, regex)) {
            // Para cada mapeamento de parâmetro
            for (const auto& [paramName, paramRegex] : pattern.parameterMappings) {
                // Extrai o número de captura do regex
                if (paramRegex.size() > 1 && paramRegex[0] == '$') {
                    int captureIndex = std::stoi(paramRegex.substr(1));
                    if (captureIndex >= 0 && captureIndex < static_cast<int>(match.size())) {
                        params[paramName] = match[captureIndex].str();
                    }
                }
            }
        }
    } catch (const std::exception&) {
        // Ignora erros
    }
    
    return params;
}

}} // namespace wydstudio::ai