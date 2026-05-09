#pragma once
#include <string>
#include <iostream>


class Product {
private:
    int         m_id;
    std::string m_name;
    std::string m_category;
    double      m_price;
    int         m_quantity;
    std::string m_description;

public:
    // Constructors
    Product() = default;
    Product(int id, const std::string& name, const std::string& category,
            double price, int quantity, const std::string& description);

    // Getters
    int         getId()          const { return m_id; }
    std::string getName()        const { return m_name; }
    std::string getCategory()    const { return m_category; }
    double      getPrice()       const { return m_price; }
    int         getQuantity()    const { return m_quantity; }
    std::string getDescription() const { return m_description; }

    // Setters
    void setName(const std::string& name)           { m_name = name; }
    void setCategory(const std::string& category)   { m_category = category; }
    void setPrice(double price)                      { m_price = price; }
    void setQuantity(int quantity)                   { m_quantity = quantity; }
    void setDescription(const std::string& desc)    { m_description = desc; }

    // Serialization
    std::string toCsv()   const;
    static Product fromCsv(const std::string& line);

    // Display
    void display()    const;
    void displayRow() const;   // single table row
    static void displayHeader();
};