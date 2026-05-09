#pragma once
#include <string>


enum class UserRole { ADMIN, USER };

class User {
protected:
    int         m_id;
    std::string m_username;
    std::string m_password;   // plain-text for demo; hash in production
    UserRole    m_role;

public:
    User() = default;
    User(int id, const std::string& username,
         const std::string& password, UserRole role);

    virtual ~User() = default;

    // Getters
    int         getId()       const { return m_id; }
    std::string getUsername() const { return m_username; }
    std::string getPassword() const { return m_password; }
    UserRole    getRole()     const { return m_role; }
    std::string getRoleName() const;

    // Virtual — overridden by Admin
    virtual bool canDelete()   const { return false; }
    virtual bool canAdd()      const { return false; }
    virtual bool canEdit()     const { return false; }
    virtual bool canManageUsers() const { return false; }

    // Serialization
    std::string toCsv() const;
    static User* fromCsv(const std::string& line);  // factory, returns Admin* or User*

    void displayInfo() const;
};