#include "Utils.hpp"
#include <xlnt/xlnt.hpp>
#include <algorithm>
#include <cctype>
#include <iostream>
#include <limits>
#include <sstream>
#include <fstream>
#include <sys/stat.h>

#ifdef _WIN32
  #include <direct.h>
  #define MKDIR(p) _mkdir(p)
  #define CLEAR "cls"
#else
  #include <unistd.h>
  #define MKDIR(p) mkdir(p, 0755)
  #define CLEAR "clear"
#endif

namespace Utils {

// ── String ────────────────────────────────────────────────────

std::string trim(const std::string& s) {
    auto start = s.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    auto end = s.find_last_not_of(" \t\r\n");
    return s.substr(start, end - start + 1);
}

std::string toLower(const std::string& s) {
    std::string r = s;
    std::transform(r.begin(), r.end(), r.begin(), ::tolower);
    return r;
}

std::vector<std::string> splitCsv(const std::string& line) {
    std::vector<std::string> fields;
    std::string field;
    bool inQuotes = false;

    for (size_t i = 0; i < line.size(); ++i) {
        char c = line[i];
        if (c == '"') {
            if (inQuotes && i + 1 < line.size() && line[i+1] == '"') {
                field += '"'; ++i;
            } else {
                inQuotes = !inQuotes;
            }
        } else if (c == ',' && !inQuotes) {
            fields.push_back(trim(field));
            field.clear();
        } else {
            field += c;
        }
    }
    fields.push_back(trim(field));
    return fields;
}

std::string escapeCsv(const std::string& f) {
    if (f.find_first_of(",\"\n") == std::string::npos) return f;
    std::string out = "\"";
    for (char c : f) { if (c == '"') out += '"'; out += c; }
    out += '"';
    return out;
}

// ── Console UI ────────────────────────────────────────────────

void clearScreen()  { system(CLEAR); }
void pauseScreen()  {
    std::cout << "\n  Press ENTER to continue...";
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

void printSeparator(char ch, int width) {
    std::cout << std::string(width, ch) << "\n";
}

void printTitle(const std::string& title) {
    printSeparator('=', 60);
    int pad = static_cast<int>((60 - static_cast<int>(title.size())) / 2);
    if (pad < 0) pad = 0;
    std::cout << std::string(pad, ' ') << title << "\n";
    printSeparator('=', 60);
}

void printSuccess(const std::string& msg) { std::cout << "\n  [OK]  " << msg << "\n"; }
void printError  (const std::string& msg) { std::cout << "\n  [ERR] " << msg << "\n"; }
void printWarning(const std::string& msg) { std::cout << "\n  [!]   " << msg << "\n"; }

// ── Input helpers ─────────────────────────────────────────────

int getIntInput(const std::string& prompt, int min, int max) {
    int val;
    while (true) {
        std::cout << "  " << prompt;
        if (std::cin >> val && val >= min && val <= max) {
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            return val;
        }
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        printError("Enter a number between " + std::to_string(min) +
                   " and " + std::to_string(max));
    }
}

double getDoubleInput(const std::string& prompt) {
    double val;
    while (true) {
        std::cout << "  " << prompt;
        if (std::cin >> val && val >= 0) {
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            return val;
        }
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        printError("Enter a valid non-negative number.");
    }
}

std::string getStringInput(const std::string& prompt) {
    std::string val;
    std::cout << "  " << prompt;
    std::getline(std::cin, val);
    return trim(val);
}

bool getYesNo(const std::string& prompt) {
    std::string ans;
    std::cout << "  " << prompt << " (y/n): ";
    std::getline(std::cin, ans);
    return (!ans.empty() && (ans[0] == 'y' || ans[0] == 'Y'));
}

// ── Validation ────────────────────────────────────────────────

bool isValidPrice   (double price)        { return price >= 0.0; }
bool isValidQuantity(int qty)             { return qty >= 0; }
bool isNonEmpty     (const std::string& s){ return !trim(s).empty(); }

// ── File helpers ──────────────────────────────────────────────

bool fileExists(const std::string& path) {
    std::ifstream f(path);
    return f.good();
}

bool ensureDirectoryExists(const std::string& path) {
    struct stat info;
    if (stat(path.c_str(), &info) == 0) return true;
    return MKDIR(path.c_str()) == 0;
}

// ── XLSX helpers ──────────────────────────────────────────────

bool xlsxWriteSheet(const std::string& path,
                    const std::string& sheetName,
                    const std::vector<std::vector<std::string>>& rows) {
    try {
        xlnt::workbook wb;
        xlnt::worksheet ws = wb.active_sheet();
        ws.title(sheetName);

        for (size_t r = 0; r < rows.size(); ++r) {
            for (size_t c = 0; c < rows[r].size(); ++c) {
                ws.cell(static_cast<xlnt::column_t::index_t>(c + 1),
                        static_cast<xlnt::row_t>(r + 1))
                  .value(rows[r][c]);
            }
        }

        wb.save(path);
        return true;
    } catch (const std::exception& e) {
        printError(std::string("xlsxWriteSheet: ") + e.what());
        return false;
    }
}

} // namespace Utils