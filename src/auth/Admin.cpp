#include "Product.hpp"
#include "Utils.hpp"
#include <sstream>
#include <iomanip>
#include <iostream>

// ============================================================
//  Product.cpp
// ============================================================

Product::Product(int id, const std::string& name, const std::string& category,
                 double price, int quantity, const std::string& description)
    : m_id(id), m_name(name), m_category(category),
      m_price(price), m_quantity(quantity), m_description(description) {}

// ── Serialization ────────────────────────────────────────────

std::string Product::toCsv() const {
    std::ostringstream oss;
    oss << m_id << ","
        << Utils::escapeCsv(m_name)        << ","
        << Utils::escapeCsv(m_category)    << ","
        << std::fixed << std::setprecision(2) << m_price << ","
        << m_quantity                       << ","
        << Utils::escapeCsv(m_description);
    return oss.str();
}

Product Product::fromCsv(const std::string& line) {
    auto fields = Utils::splitCsv(line);
    if (fields.size() < 6) return Product{};
    Product p;
    p.m_id          = std::stoi(fields[0]);
    p.m_name        = fields[1];
    p.m_category    = fields[2];
    p.m_price       = std::stod(fields[3]);
    p.m_quantity    = std::stoi(fields[4]);
    p.m_description = fields[5];
    return p;
}

// ── Display ───────────────────────────────────────────────────

void Product::displayHeader() {
    std::cout << std::left
              << std::setw(5)  << "ID"
              << std::setw(22) << "Name"
              << std::setw(16) << "Category"
              << std::setw(12) << "Price ($)"
              << std::setw(10) << "Qty"
              << "Description\n";
    Utils::printSeparator('-', 80);
}

void Product::displayRow() const {
    std::cout << std::left
              << std::setw(5)  << m_id
              << std::setw(22) << m_name.substr(0, 20)
              << std::setw(16) << m_category.substr(0, 14)
              << std::setw(12) << std::fixed << std::setprecision(2) << m_price
              << std::setw(10) << m_quantity
              << m_description.substr(0, 28) << "\n";
}

void Product::display() const {
    Utils::printSeparator('=', 50);
    std::cout << " Product Details\n";
    Utils::printSeparator('=', 50);
    std::cout << " ID          : " << m_id          << "\n"
              << " Name        : " << m_name        << "\n"
              << " Category    : " << m_category    << "\n"
              << " Price       : $" << std::fixed << std::setprecision(2) << m_price << "\n"
              << " Quantity    : " << m_quantity    << "\n"
              << " Description : " << m_description << "\n";
    Utils::printSeparator('=', 50);
}