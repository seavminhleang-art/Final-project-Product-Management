#include "ProductManager.hpp"
#include "Utils.hpp"
#include <fstream>
#include <algorithm>
#include <iostream>
#include <stdexcept>
#include <sstream>
#include <iomanip>

// ============================================================
//  ProductManager.cpp
// ============================================================

ProductManager::ProductManager(const std::string& filePath)
    : m_filePath(filePath), m_nextId(1) {
    Utils::ensureDirectoryExists("data");
    loadFromFile();
}

// ── Persistence ───────────────────────────────────────────────

bool ProductManager::loadFromFile() {
    std::ifstream file(m_filePath);
    if (!file.is_open()) return false;   // first run — empty is fine

    m_products.clear();
    std::string line;
    std::getline(file, line);   // skip CSV header row

    while (std::getline(file, line)) {
        line = Utils::trim(line);
        if (line.empty()) continue;
        try {
            m_products.push_back(Product::fromCsv(line));
        } catch (...) {
            // skip malformed rows
        }
    }
    recalcNextId();
    return true;
}

bool ProductManager::saveToFile() const {
    // ── Write CSV (internal format) ───────────────────────────
    std::ofstream file(m_filePath);
    if (!file.is_open()) return false;
    file << "ID,Name,Category,Price,Quantity,Description\n";
    for (const auto& p : m_products)
        file << p.toCsv() << "\n";
    file.close();

    // ── Sync to Excel (.xlsx) via xlnt ────────────────────────
    std::vector<std::vector<std::string>> rows;
    rows.push_back({"ID", "Name", "Category", "Price ($)", "Quantity", "Description"});
    for (const auto& p : m_products) {
        std::ostringstream price;
        price << std::fixed << std::setprecision(2) << p.getPrice();
        rows.push_back({
            std::to_string(p.getId()),
            p.getName(),
            p.getCategory(),
            price.str(),
            std::to_string(p.getQuantity()),
            p.getDescription()
        });
    }
    return true;
}

// ── Helpers ───────────────────────────────────────────────────

void ProductManager::recalcNextId() {
    m_nextId = 1;
    for (const auto& p : m_products)
        if (p.getId() >= m_nextId)
            m_nextId = p.getId() + 1;
}

bool ProductManager::idExists(int id) const {
    for (const auto& p : m_products)
        if (p.getId() == id) return true;
    return false;
}

// ── Create ────────────────────────────────────────────────────

bool ProductManager::addProduct(const std::string& name,
                                 const std::string& category,
                                 double price, int quantity,
                                 const std::string& description) {
    if (!Utils::isNonEmpty(name) || !Utils::isNonEmpty(category)) return false;
    if (!Utils::isValidPrice(price) || !Utils::isValidQuantity(quantity))  return false;

    m_products.emplace_back(m_nextId++, name, category, price, quantity, description);
    return saveToFile();
}

// ── Read / Display ────────────────────────────────────────────

void ProductManager::displayAll() const {
    if (m_products.empty()) {
        Utils::printWarning("No products found.");
        return;
    }
    Product::displayHeader();
    for (const auto& p : m_products) p.displayRow();
    Utils::printSeparator('-', 80);
    std::cout << " Total: " << m_products.size() << " product(s)\n";
}

void ProductManager::displayProduct(int id) const {
    for (const auto& p : m_products)
        if (p.getId() == id) { p.display(); return; }
    Utils::printError("Product with ID " + std::to_string(id) + " not found.");
}

// ── Update ────────────────────────────────────────────────────

bool ProductManager::editProduct(int id, const std::string& field,
                                  const std::string& value) {
    for (auto& p : m_products) {
        if (p.getId() != id) continue;
        std::string f = Utils::toLower(field);
        if      (f == "name")        p.setName(value);
        else if (f == "category")    p.setCategory(value);
        else if (f == "price")       p.setPrice(std::stod(value));
        else if (f == "quantity")    p.setQuantity(std::stoi(value));
        else if (f == "description") p.setDescription(value);
        else return false;
        return saveToFile();
    }
    return false;
}

// ── Delete ────────────────────────────────────────────────────

bool ProductManager::deleteProduct(int id) {
    auto it = std::remove_if(m_products.begin(), m_products.end(),
                              [id](const Product& p){ return p.getId() == id; });
    if (it == m_products.end()) return false;
    m_products.erase(it, m_products.end());
    return saveToFile();
}

// ── Search / Filter ───────────────────────────────────────────

std::vector<Product> ProductManager::searchByName(const std::string& keyword) const {
    std::string kw = Utils::toLower(keyword);
    std::vector<Product> results;
    for (const auto& p : m_products)
        if (Utils::toLower(p.getName()).find(kw) != std::string::npos)
            results.push_back(p);
    return results;
}

std::vector<Product> ProductManager::filterByCategory(const std::string& category) const {
    std::string cat = Utils::toLower(category);
    std::vector<Product> results;
    for (const auto& p : m_products)
        if (Utils::toLower(p.getCategory()) == cat)
            results.push_back(p);
    return results;
}

std::vector<Product> ProductManager::filterByPriceRange(double minP, double maxP) const {
    std::vector<Product> results;
    for (const auto& p : m_products)
        if (p.getPrice() >= minP && p.getPrice() <= maxP)
            results.push_back(p);
    return results;
}

// ── Sort ──────────────────────────────────────────────────────

void ProductManager::sortProducts(SortField field, SortOrder order) {
    auto cmp = [&](const Product& a, const Product& b) -> bool {
        bool less_than = false;
        switch (field) {
            case SortField::ID:       less_than = a.getId()       < b.getId();       break;
            case SortField::NAME:     less_than = a.getName()     < b.getName();     break;
            case SortField::CATEGORY: less_than = a.getCategory() < b.getCategory(); break;
            case SortField::PRICE:    less_than = a.getPrice()    < b.getPrice();    break;
            case SortField::QUANTITY: less_than = a.getQuantity() < b.getQuantity(); break;
        }
        return (order == SortOrder::ASC) ? less_than : !less_than;
    };
    std::sort(m_products.begin(), m_products.end(), cmp);
}