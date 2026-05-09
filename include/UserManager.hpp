#pragma once
#include <string>
#include <vector>
#include <memory>
#include "auth/User.hpp"

class UserManager {
private:
    std::string m_filePath;
    int m_nextId;
    std::vector<std::unique_ptr<User>> m_users;

public:
    
    explicit UserManager(const std::string& filePath);

    // Persistence
    bool loadFromFile();
    bool saveToFile() const;

    // Authentication
    User* findByCredentials(const std::string& username,
                            const std::string& password) const;

    // CRUD
    bool addUser(const std::string& username,
                 const std::string& password,
                 UserRole role);

    bool deleteUser(int id);

    void displayAll() const;
};