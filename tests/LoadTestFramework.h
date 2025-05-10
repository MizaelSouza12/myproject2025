/**
 * @file LoadTestFramework.h
 * @brief Framework para testes de carga do WYDBR 2.0
 */

#ifndef WYDBR_LOAD_TEST_FRAMEWORK_H
#define WYDBR_LOAD_TEST_FRAMEWORK_H

#include <string>
#include <vector>
#include <functional>
#include <memory>
#include <thread>
#include <atomic>
#include <chrono>

namespace WYDBR {
namespace Test {

/**
 * @class LoadTestManager
 * @brief Gerencia testes de carga com múltiplos usuários virtuais
 */
class LoadTestManager {
public:
    explicit LoadTestManager(const std::string& testName);
    bool configure(int userCount, 
                 std::chrono::seconds rampUpTime,
                 std::chrono::seconds steadyStateTime);
    void runTest();
    void stopTest();
    
private:
    std::string m_testName;
    int m_userCount;
    std::chrono::seconds m_rampUpTime;
    std::chrono::seconds m_steadyStateTime;
    std::atomic<bool> m_running;
};

} // namespace Test
} // namespace WYDBR

#endif // WYDBR_LOAD_TEST_FRAMEWORK_H
