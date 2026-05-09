#include "UserManager.hpp"
#include "auth/Admin.hpp"
#include "Utils.hpp"
#include <fstream>
#include <algorithm>
#include <iostream>
#include <iomanip>

// ============================================================
//  UserManager.cpp
// ============================================================

UserManager::UserManager(const std::string& filePath)
    : m_filePath(filePath), m_nextId(1) {
    Utils::ensureDirectoryExists("data");
    loadFromFile();

    // Seed default admin if no users exist
    if (m_users.empty()) {
        m_users.push_back(std::make_unique<Admin>(m_nextId++, "admin", "admin123"));
        saveToFile();
    }
}

// ── Persistence ───────────────────────────────────────────────

bool UserManager::loadFromFile() {
    std::ifstream file(m_filePath);
    if (!file.is_open()) return false;

    m_users.clear();
    std::string line;
    std::getline(file, line);   // skip header

    while (std::getline(file, line)) {
        line = Utils::trim(line);
        if (line.empty()) continue;
        User* u = User::fromCsv(line);
        if (u) m_users.push_back(std::unique_ptr<User>(u));
    }

    // Recalc next ID
    m_nextId = 1;
    for (const auto& u : m_users)
        if (u->getId() >= m_nextId)
            m_nextId = u->getId() + 1;

    return true;
}

bool UserManager::saveToFile() const {
    std::ofstream file(m_filePath);
    if (!file.is_open()) return false;
    file << "ID,Username,Password,Role\n";
    for (const auto& u : m_users)
        file << u->toCsv() << "\n";
    return true;
}

// ── Auth ──────────────────────────────────────────────────────

User* UserManager::findByCredentials(const std::string& username,
                                      const std::string& password) const {
    for (const auto& u : m_users)
        if (u->getUsername() == username && u->getPassword() == password)
            return u.get();
    return nullptr;
}

// ── CRUD ──────────────────────────────────────────────────────

bool UserManager::addUser(const std::string& username,
                           const std::string& password, UserRole role) {
    if (!Utils::isNonEmpty(username) || !Utils::isNonEmpty(password)) return false;
    // Prevent duplicate usernames
    for (const auto& u : m_users)
        if (u->getUsername() == username) return false;

    if (role == UserRole::ADMIN)
        m_users.push_back(std::make_unique<Admin>(m_nextId++, username, password));
    else
        m_users.push_back(std::make_unique<User>(m_nextId++, username, password, UserRole::USER));

    return saveToFile();
}

bool UserManager::deleteUser(int id) {
    auto it = std::remove_if(m_users.begin(), m_users.end(),
                              [id](const std::unique_ptr<User>& u){ return u->getId() == id; });
    if (it == m_users.end()) return false;
    m_users.erase(it, m_users.end());
    return saveToFile();
}

void UserManager::displayAll() const {
    std::cout << std::left
              << std::setw(5) << "ID"
              << std::setw(20) << "Username"
              << std::setw(10) << "Role" << "\n";
    Utils::printSeparator('-', 35);
    for (const auto& u : m_users) u->displayInfo();
    Utils::printSeparator('-', 35);
}