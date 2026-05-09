#pragma once
#include "auth/User.hpp"
#include "UserManager.hpp"
#include <string>

// ============================================================
//  AuthService.h  —  Login / Logout / Session management
// ============================================================

class AuthService {
private:
    User*        m_currentUser;   // non-owning pointer
    UserManager& m_userManager;

public:
    explicit AuthService(UserManager& userManager);

    bool login(const std::string& username, const std::string& password);
    void logout();

    bool  isLoggedIn()   const { return m_currentUser != nullptr; }
    User* currentUser()  const { return m_currentUser; }

    // Convenience permission checks (delegates to current user)
    bool canAdd()         const;
    bool canDelete()      const;
    bool canEdit()        const;
    bool canManageUsers() const;
};