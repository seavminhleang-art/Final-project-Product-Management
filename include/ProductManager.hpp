#pragma once
#include "Product.hpp"
#include <vector>
#include <string>
#include <functional>

// ============================================================
//  ProductManager.h  —  All product operations + CSV persistence
// ============================================================

enum class SortField  { ID, NAME, CATEGORY, PRICE, QUANTITY };
enum class SortOrder  { ASC, DESC };

class ProductManager {
private:
    std::vector<Product> m_products;
    std::string          m_filePath;
    int                  m_nextId;

    // Internal helpers
    void   recalcNextId();
    bool   idExists(int id) const;

public:
    explicit ProductManager(const std::string& filePath = "data/products.csv");

    // ── Persistence ──────────────────────────────────────────
    bool loadFromFile();
    bool saveToFile()  const;

    // ── Create ───────────────────────────────────────────────
    bool addProduct(const std::string& name, const std::string& category,
                    double price, int quantity, const std::string& description);

    // ── Read / Display ────────────────────────────────────────
    void displayAll()                           const;
    void displayProduct(int id)                 const;

    // ── Update ───────────────────────────────────────────────
    bool editProduct(int id, const std::string& field, const std::string& value);

    // ── Delete ───────────────────────────────────────────────
    bool deleteProduct(int id);

    // ── Search / Filter ───────────────────────────────────────
    std::vector<Product> searchByName(const std::string& keyword)     const;
    std::vector<Product> filterByCategory(const std::string& category) const;
    std::vector<Product> filterByPriceRange(double minP, double maxP)  const;

    // ── Sort ─────────────────────────────────────────────────
    void sortProducts(SortField field, SortOrder order = SortOrder::ASC);

    // ── Utilities ─────────────────────────────────────────────
    int  count()    const { return static_cast<int>(m_products.size()); }
    bool isEmpty()  const { return m_products.empty(); }
    const std::vector<Product>& getAll() const { return m_products; }
};