#include "Admin.hpp"

Admin::Admin(int id, const std::string& username, const std::string& password)
    : User(id, username, password, UserRole::ADMIN) {}
