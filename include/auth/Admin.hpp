#pragma once
#include "User.hpp"


class Admin : public User {
public:
    Admin() = default;
    Admin(int id, const std::string& username, const std::string& password);

    // Override all permission checks
    bool canDelete()      const override { return true; }
    bool canAdd()         const override { return true; }
    bool canEdit()        const override { return true; }
    bool canManageUsers() const override { return true; }
};