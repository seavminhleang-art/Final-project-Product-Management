#include "auth/User.hpp"
#include "auth/Admin.hpp"
#include "Utils.hpp"
#include <sstream>
#include <iostream>
#include <iomanip>

// ============================================================
//  User.cpp
// ============================================================

User::User(int id, const std::string& username,
           const std::string& password, UserRole role)
    : m_id(id), m_username(username), m_password(password), m_role(role) {}

std::string User::getRoleName() const {
    return (m_role == UserRole::ADMIN) ? "Admin" : "User";
}

// ── Serialization ─────────────────────────────────────────────

std::string User::toCsv() const {
    return std::to_string(m_id) + "," +
           Utils::escapeCsv(m_username) + "," +
           Utils::escapeCsv(m_password) + "," +
           (m_role == UserRole::ADMIN ? "ADMIN" : "USER");
}

User* User::fromCsv(const std::string& line) {
    auto f = Utils::splitCsv(line);
    if (f.size() < 4) return nullptr;

    int         id   = std::stoi(f[0]);
    std::string user = f[1];
    std::string pass = f[2];
    UserRole    role = (f[3] == "ADMIN") ? UserRole::ADMIN : UserRole::USER;

    if (role == UserRole::ADMIN)
        return new Admin(id, user, pass);
    else
        return new User(id, user, pass, UserRole::USER);
}

// ── Display ───────────────────────────────────────────────────

void User::displayInfo() const {
    std::cout << std::left
              << std::setw(5)  << m_id
              << std::setw(20) << m_username
              << std::setw(10) << getRoleName() << "\n";
}