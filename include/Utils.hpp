
#pragma once
#include <string>
#include <vector>

// ============================================================
//  Utils.hpp  —  Shared utility functions
// ============================================================

namespace Utils {

    // String helpers
    std::string  trim(const std::string& s);
    std::string  toLower(const std::string& s);
    std::vector<std::string> splitCsv(const std::string& line);
    std::string  escapeCsv(const std::string& field);

    // Console UI helpers
    void  clearScreen();
    void  pauseScreen();
    void  printSeparator(char ch = '-', int width = 72);
    void  printTitle(const std::string& title);
    void  printSuccess(const std::string& msg);
    void  printError(const std::string& msg);
    void  printWarning(const std::string& msg);

    // Input helpers
    int    getIntInput(const std::string& prompt, int min, int max);
    double getDoubleInput(const std::string& prompt);
    std::string getStringInput(const std::string& prompt);
    bool   getYesNo(const std::string& prompt);

    // Validation
    bool isValidPrice(double price);
    bool isValidQuantity(int qty);
    bool isNonEmpty(const std::string& s);

    // File helpers
    bool fileExists(const std::string& path);
    bool ensureDirectoryExists(const std::string& path);

    // XLSX helpers
    bool xlsxWriteSheet(const std::string& path,
                        const std::string& sheetName,
                        const std::vector<std::vector<std::string>>& rows);

} // namespace Utils