/**
 * @file Utils.h
 * @brief Utilitários gerais para o projeto WYDBR
 * @author WYDBR Team
 */

#ifndef WYDBR_UTILS_H
#define WYDBR_UTILS_H

#include <string>
#include <vector>
#include <random>
#include <algorithm>
#include <chrono>
#include <sstream>
#include <iomanip>
#include <filesystem>
#include <cctype>
#include <functional>
#include <codecvt>
#include <locale>
#include <regex>

namespace wydbr {
namespace core {
namespace utils {

/**
 * @brief Utilitários de string
 */
namespace string {
    /**
     * @brief Divide uma string usando um delimitador
     * @param s String a ser dividida
     * @param delimiter Delimitador
     * @return Vetor de strings
     */
    inline std::vector<std::string> split(const std::string& s, char delimiter) {
        std::vector<std::string> tokens;
        std::string token;
        std::istringstream tokenStream(s);
        while (std::getline(tokenStream, token, delimiter)) {
            tokens.push_back(token);
        }
        return tokens;
    }

    /**
     * @brief Divide uma string usando uma string como delimitador
     * @param s String a ser dividida
     * @param delimiter String delimitadora
     * @return Vetor de strings
     */
    inline std::vector<std::string> split(const std::string& s, const std::string& delimiter) {
        std::vector<std::string> tokens;
        size_t pos = 0;
        size_t prev = 0;
        
        while ((pos = s.find(delimiter, prev)) != std::string::npos) {
            tokens.push_back(s.substr(prev, pos - prev));
            prev = pos + delimiter.length();
        }
        
        tokens.push_back(s.substr(prev));
        return tokens;
    }

    /**
     * @brief Remove espaços em branco do início e fim da string
     * @param s String a ser processada
     * @return String sem espaços no início e fim
     */
    inline std::string trim(const std::string& s) {
        auto start = s.begin();
        while (start != s.end() && std::isspace(*start)) {
            start++;
        }

        auto end = s.end();
        do {
            end--;
        } while (std::distance(start, end) > 0 && std::isspace(*end));

        return std::string(start, end + 1);
    }

    /**
     * @brief Converte string para minúsculas
     * @param s String a ser convertida
     * @return String em minúsculas
     */
    inline std::string toLower(const std::string& s) {
        std::string result = s;
        std::transform(result.begin(), result.end(), result.begin(),
            [](unsigned char c) { return std::tolower(c); });
        return result;
    }

    /**
     * @brief Converte string para maiúsculas
     * @param s String a ser convertida
     * @return String em maiúsculas
     */
    inline std::string toUpper(const std::string& s) {
        std::string result = s;
        std::transform(result.begin(), result.end(), result.begin(),
            [](unsigned char c) { return std::toupper(c); });
        return result;
    }

    /**
     * @brief Verifica se uma string começa com determinado prefixo
     * @param s String a ser verificada
     * @param prefix Prefixo a ser procurado
     * @return true se a string começa com o prefixo, false caso contrário
     */
    inline bool startsWith(const std::string& s, const std::string& prefix) {
        return s.size() >= prefix.size() && s.compare(0, prefix.size(), prefix) == 0;
    }

    /**
     * @brief Verifica se uma string termina com determinado sufixo
     * @param s String a ser verificada
     * @param suffix Sufixo a ser procurado
     * @return true se a string termina com o sufixo, false caso contrário
     */
    inline bool endsWith(const std::string& s, const std::string& suffix) {
        return s.size() >= suffix.size() && 
               s.compare(s.size() - suffix.size(), suffix.size(), suffix) == 0;
    }

    /**
     * @brief Substitui todas as ocorrências de uma substring por outra
     * @param s String onde fazer a substituição
     * @param from Substring a ser substituída
     * @param to Substituição
     * @return String com substituições realizadas
     */
    inline std::string replace(std::string s, const std::string& from, const std::string& to) {
        size_t pos = 0;
        while ((pos = s.find(from, pos)) != std::string::npos) {
            s.replace(pos, from.length(), to);
            pos += to.length();
        }
        return s;
    }

    /**
     * @brief Converte um valor para string
     * @tparam T Tipo do valor
     * @param value Valor a ser convertido
     * @return String representando o valor
     */
    template<typename T>
    inline std::string toString(const T& value) {
        std::ostringstream oss;
        oss << value;
        return oss.str();
    }

    /**
     * @brief Converte string para valor numérico
     * @tparam T Tipo de destino
     * @param s String a ser convertida
     * @param defaultValue Valor padrão em caso de erro
     * @return Valor convertido ou valor padrão
     */
    template<typename T>
    inline T fromString(const std::string& s, const T& defaultValue = T()) {
        T value = defaultValue;
        std::istringstream iss(s);
        iss >> value;
        return iss.fail() ? defaultValue : value;
    }

    /**
     * @brief Converte string UTF-8 para UTF-16
     * @param utf8 String em UTF-8
     * @return String em UTF-16
     */
    inline std::wstring utf8ToUtf16(const std::string& utf8) {
        std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
        return converter.from_bytes(utf8);
    }

    /**
     * @brief Converte string UTF-16 para UTF-8
     * @param utf16 String em UTF-16
     * @return String em UTF-8
     */
    inline std::string utf16ToUtf8(const std::wstring& utf16) {
        std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
        return converter.to_bytes(utf16);
    }
} // namespace string

/**
 * @brief Utilitários de arquivos e sistema
 */
namespace file {
    /**
     * @brief Lê todo o conteúdo de um arquivo como texto
     * @param filePath Caminho do arquivo
     * @return Conteúdo do arquivo ou string vazia se falhar
     */
    inline std::string readFileAsString(const std::string& filePath) {
        std::ifstream file(filePath, std::ios::in | std::ios::binary);
        if (!file.is_open()) {
            return "";
        }
        
        file.seekg(0, std::ios::end);
        size_t size = file.tellg();
        std::string buffer(size, ' ');
        file.seekg(0);
        file.read(&buffer[0], size);
        
        return buffer;
    }

    /**
     * @brief Escreve string em arquivo
     * @param filePath Caminho do arquivo
     * @param content Conteúdo a ser escrito
     * @return true se sucesso, false se falhar
     */
    inline bool writeStringToFile(const std::string& filePath, const std::string& content) {
        std::ofstream file(filePath, std::ios::out | std::ios::binary);
        if (!file.is_open()) {
            return false;
        }
        
        file << content;
        return file.good();
    }

    /**
     * @brief Verifica se um arquivo existe
     * @param filePath Caminho do arquivo
     * @return true se existe, false caso contrário
     */
    inline bool fileExists(const std::string& filePath) {
        return std::filesystem::exists(filePath) && std::filesystem::is_regular_file(filePath);
    }

    /**
     * @brief Verifica se um diretório existe
     * @param dirPath Caminho do diretório
     * @return true se existe, false caso contrário
     */
    inline bool directoryExists(const std::string& dirPath) {
        return std::filesystem::exists(dirPath) && std::filesystem::is_directory(dirPath);
    }

    /**
     * @brief Cria diretório se não existir
     * @param dirPath Caminho do diretório
     * @return true se criado ou já existia, false se falhou
     */
    inline bool createDirectory(const std::string& dirPath) {
        if (directoryExists(dirPath)) {
            return true;
        }
        return std::filesystem::create_directories(dirPath);
    }

    /**
     * @brief Obtém extensão de um arquivo
     * @param filePath Caminho do arquivo
     * @return Extensão do arquivo (sem o ponto)
     */
    inline std::string getFileExtension(const std::string& filePath) {
        std::filesystem::path path(filePath);
        return path.extension().string().substr(1); // remove o ponto
    }

    /**
     * @brief Lista arquivos em um diretório
     * @param dirPath Caminho do diretório
     * @param extension Extensão para filtrar (opcional)
     * @return Vetor de caminhos de arquivos
     */
    inline std::vector<std::string> listFiles(const std::string& dirPath, 
                                             const std::string& extension = "") {
        std::vector<std::string> files;
        
        if (!directoryExists(dirPath)) {
            return files;
        }
        
        for (const auto& entry : std::filesystem::directory_iterator(dirPath)) {
            if (entry.is_regular_file()) {
                if (extension.empty() || getFileExtension(entry.path().string()) == extension) {
                    files.push_back(entry.path().string());
                }
            }
        }
        
        return files;
    }
} // namespace file

/**
 * @brief Utilitários de tempo e data
 */
namespace time {
    /**
     * @brief Obtém timestamp atual em milissegundos
     * @return Timestamp em milissegundos
     */
    inline uint64_t currentTimeMillis() {
        return std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch()).count();
    }

    /**
     * @brief Obtém timestamp atual em microssegundos
     * @return Timestamp em microssegundos
     */
    inline uint64_t currentTimeMicros() {
        return std::chrono::duration_cast<std::chrono::microseconds>(
            std::chrono::high_resolution_clock::now().time_since_epoch()).count();
    }

    /**
     * @brief Obtém data e hora atual formatada
     * @param format Formato da data/hora
     * @return String formatada com data/hora
     */
    inline std::string currentTimeFormatted(const std::string& format = "%Y-%m-%d %H:%M:%S") {
        auto now = std::chrono::system_clock::now();
        auto time = std::chrono::system_clock::to_time_t(now);
        std::stringstream ss;
        ss << std::put_time(std::localtime(&time), format.c_str());
        return ss.str();
    }
    
    /**
     * @brief Converte timestamp para string formatada
     * @param timestamp Timestamp em milissegundos
     * @param format Formato da data/hora
     * @return String formatada com data/hora
     */
    inline std::string timestampToString(uint64_t timestamp, 
                                        const std::string& format = "%Y-%m-%d %H:%M:%S") {
        auto timePoint = std::chrono::system_clock::time_point(
            std::chrono::milliseconds(timestamp));
        auto time = std::chrono::system_clock::to_time_t(timePoint);
        std::stringstream ss;
        ss << std::put_time(std::localtime(&time), format.c_str());
        return ss.str();
    }
} // namespace time

/**
 * @brief Utilitários de criptografia
 */
namespace crypto {
    /**
     * @brief Calcula hash MD5 de uma string
     * @param input String de entrada
     * @return String com hash MD5 em hexadecimal
     */
    inline std::string md5(const std::string& input) {
        // Implementação simplificada - em produção, use uma biblioteca criptográfica adequada
        // Esta é apenas uma função de placeholder
        std::hash<std::string> hasher;
        auto hash = hasher(input);
        std::stringstream ss;
        ss << std::hex << std::setfill('0') << std::setw(32) << hash;
        return ss.str();
    }

    /**
     * @brief Gera uma string aleatória
     * @param length Comprimento da string
     * @return String aleatória
     */
    inline std::string randomString(size_t length) {
        const std::string chars = 
            "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
        
        std::random_device rd;
        std::mt19937 generator(rd());
        std::uniform_int_distribution<> distribution(0, chars.size() - 1);
        
        std::string result;
        result.reserve(length);
        
        for (size_t i = 0; i < length; i++) {
            result += chars[distribution(generator)];
        }
        
        return result;
    }
} // namespace crypto

/**
 * @brief Utilitários de matemática
 */
namespace math {
    /**
     * @brief Limita um valor a um intervalo
     * @tparam T Tipo do valor
     * @param value Valor a ser limitado
     * @param min Valor mínimo
     * @param max Valor máximo
     * @return Valor limitado ao intervalo [min, max]
     */
    template<typename T>
    inline T clamp(const T& value, const T& min, const T& max) {
        return std::max(min, std::min(max, value));
    }

    /**
     * @brief Interpolação linear
     * @tparam T Tipo do valor
     * @param a Valor inicial
     * @param b Valor final
     * @param t Fator de interpolação (0.0 a 1.0)
     * @return Valor interpolado
     */
    template<typename T>
    inline T lerp(const T& a, const T& b, float t) {
        return a + static_cast<T>((b - a) * t);
    }

    /**
     * @brief Verifica se dois valores de ponto flutuante são aproximadamente iguais
     * @param a Primeiro valor
     * @param b Segundo valor
     * @param epsilon Precisão (valor pequeno para comparação)
     * @return true se os valores forem aproximadamente iguais
     */
    inline bool approximatelyEqual(float a, float b, float epsilon = 0.0001f) {
        return std::abs(a - b) <= epsilon;
    }
    
    /**
     * @brief Converte graus para radianos
     * @param degrees Ângulo em graus
     * @return Ângulo em radianos
     */
    inline float degreesToRadians(float degrees) {
        return degrees * 0.01745329251994329576923690768489f; // PI / 180
    }
    
    /**
     * @brief Converte radianos para graus
     * @param radians Ângulo em radianos
     * @return Ângulo em graus
     */
    inline float radiansToDegrees(float radians) {
        return radians * 57.295779513082320876798154814105f; // 180 / PI
    }
} // namespace math

} // namespace utils
} // namespace core
} // namespace wydbr

#endif // WYDBR_UTILS_H 