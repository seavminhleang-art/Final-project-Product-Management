#include "auth/AuthService.hpp"
#include "Utils.hpp"
#include <iostream>

// ============================================================
//  AuthService.cpp
// ============================================================

AuthService::AuthService(UserManager& userManager)
    : m_currentUser(nullptr), m_userManager(userManager) {}

bool AuthService::login(const std::string& username, const std::string& password) {
    m_currentUser = m_userManager.findByCredentials(username, password);
    if (m_currentUser) {
        Utils::printSuccess("Welcome, " + m_currentUser->getUsername() +
                            "! [" + m_currentUser->getRoleName() + "]");
    }
    return m_currentUser != nullptr;
}

void AuthService::logout() {
    if (m_currentUser)
        std::cout << "\n  Goodbye, " << m_currentUser->getUsername() << "!\n";
    m_currentUser = nullptr;
}

bool AuthService::canAdd()         const { return m_currentUser && m_currentUser->canAdd(); }
bool AuthService::canDelete()      const { return m_currentUser && m_currentUser->canDelete(); }
bool AuthService::canEdit()        const { return m_currentUser && m_currentUser->canEdit(); }
bool AuthService::canManageUsers() const { return m_currentUser && m_currentUser->canManageUsers(); }