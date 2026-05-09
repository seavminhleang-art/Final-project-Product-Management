#include "ReportTemplate.hpp"
#include "ProductManager.hpp"
#include "Utils.hpp"
#include <tabulate/table.hpp>
#include <iomanip>
#include <sstream>
#include <ctime>
#include <map>
#include <iostream>

// ============================================================
//  ReportTemplate.cpp
// ============================================================

ReportTemplate::ReportTemplate(const ProductManager& pm,
                               const std::string& outputDir)
    : m_pm(pm), m_outputDir(outputDir) {
    Utils::ensureDirectoryExists(outputDir);
}

// ── Helpers ───────────────────────────────────────────────────

std::string ReportTemplate::reportTypeName(ReportType type) const {
    switch (type) {
        case ReportType::FULL_INVENTORY:    return "Full Inventory";
        case ReportType::SUMMARY:           return "Summary";
        case ReportType::LOW_STOCK:         return "Low Stock Alert";
        case ReportType::CATEGORY_BREAKDOWN:return "Category Breakdown";
    }
    return "Report";
}

std::string ReportTemplate::timestampedFilename(const std::string& tag) const {
    std::time_t t  = std::time(nullptr);
    std::tm*    tm = std::localtime(&t);
    char buf[20];
    std::strftime(buf, sizeof(buf), "%Y%m%d_%H%M%S", tm);
    return m_outputDir + "/" + tag + "_" + buf + ".xlsx";
}

// ── Row builders ──────────────────────────────────────────────

std::vector<std::vector<std::string>>
ReportTemplate::buildFullInventoryRows() const {
    std::vector<std::vector<std::string>> rows;
    rows.push_back({"ID", "Name", "Category", "Price ($)", "Qty", "Description"});
    for (const auto& p : m_pm.getAll()) {
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
    return rows;
}

std::vector<std::vector<std::string>>
ReportTemplate::buildSummaryRows() const {
    const auto& products = m_pm.getAll();
    double totalValue = 0.0;
    int    totalQty   = 0;
    double maxPrice   = 0.0;
    double minPrice   = products.empty() ? 0.0 : products[0].getPrice();
    std::map<std::string, int> catCount;

    for (const auto& p : products) {
        totalValue += p.getPrice() * p.getQuantity();
        totalQty   += p.getQuantity();
        if (p.getPrice() > maxPrice) maxPrice = p.getPrice();
        if (p.getPrice() < minPrice) minPrice = p.getPrice();
        catCount[p.getCategory()]++;
    }

    auto fmt = [](double v) {
        std::ostringstream o;
        o << "$" << std::fixed << std::setprecision(2) << v;
        return o.str();
    };

    std::vector<std::vector<std::string>> rows;
    rows.push_back({"Metric", "Value"});
    rows.push_back({"Total Products",    std::to_string(m_pm.count())});
    rows.push_back({"Total Units",       std::to_string(totalQty)});
    rows.push_back({"Total Value",       fmt(totalValue)});
    rows.push_back({"Highest Price",     fmt(maxPrice)});
    rows.push_back({"Lowest Price",      fmt(minPrice)});
    rows.push_back({"Categories",        std::to_string(catCount.size())});
    rows.push_back({"", ""});
    rows.push_back({"--- Category Breakdown ---", ""});
    for (const auto& [cat, cnt] : catCount)
        rows.push_back({cat, std::to_string(cnt) + " product(s)"});
    return rows;
}

std::vector<std::vector<std::string>>
ReportTemplate::buildLowStockRows(int threshold) const {
    std::vector<std::vector<std::string>> rows;
    rows.push_back({"ID", "Name", "Category", "Qty", "Price ($)"});
    for (const auto& p : m_pm.getAll()) {
        if (p.getQuantity() <= threshold) {
            std::ostringstream price;
            price << std::fixed << std::setprecision(2) << p.getPrice();
            rows.push_back({
                std::to_string(p.getId()),
                p.getName(),
                p.getCategory(),
                std::to_string(p.getQuantity()),
                price.str()
            });
        }
    }
    if (rows.size() == 1)
        rows.push_back({"—", "No low-stock products", "—", "—", "—"});
    return rows;
}

std::vector<std::vector<std::string>>
ReportTemplate::buildCategoryRows() const {
    std::map<std::string, std::vector<const Product*>> groups;
    for (const auto& p : m_pm.getAll())
        groups[p.getCategory()].push_back(&p);

    std::vector<std::vector<std::string>> rows;
    rows.push_back({"Category", "ID", "Name", "Qty", "Price ($)", "Subtotal ($)"});

    for (const auto& [cat, prods] : groups) {
        double subtotal = 0.0;
        bool firstRow = true;
        for (const auto* p : prods) {
            std::ostringstream price, sub;
            price << std::fixed << std::setprecision(2) << p->getPrice();
            subtotal += p->getPrice() * p->getQuantity();
            sub << std::fixed << std::setprecision(2) << (p->getPrice() * p->getQuantity());
            rows.push_back({
                firstRow ? cat : "",
                std::to_string(p->getId()),
                p->getName(),
                std::to_string(p->getQuantity()),
                price.str(),
                sub.str()
            });
            firstRow = false;
        }
        std::ostringstream tot;
        tot << std::fixed << std::setprecision(2) << subtotal;
        rows.push_back({"", "", "  Subtotal", "", "", tot.str()});
    }
    return rows;
}

// ── Public API ────────────────────────────────────────────────

void ReportTemplate::preview(ReportType type, int lowStockThreshold) const {
    auto rows = [&]() -> std::vector<std::vector<std::string>> {
        switch (type) {
            case ReportType::FULL_INVENTORY:    return buildFullInventoryRows();
            case ReportType::SUMMARY:           return buildSummaryRows();
            case ReportType::LOW_STOCK:         return buildLowStockRows(lowStockThreshold);
            case ReportType::CATEGORY_BREAKDOWN:return buildCategoryRows();
        }
        return {};
    }();

    if (rows.empty()) { Utils::printWarning("No data."); return; }

    Utils::printTitle(reportTypeName(type));

    tabulate::Table table;

    // Header row
    tabulate::Table::Row_t header;
    for (const auto& h : rows[0]) header.push_back(h);
    table.add_row(header);

    // Style header
    table[0].format()
        .font_style({tabulate::FontStyle::bold})
        .font_color(tabulate::Color::white)
        .background_color(tabulate::Color::blue);

    // Data rows
    for (size_t i = 1; i < rows.size(); ++i) {
        tabulate::Table::Row_t row;
        for (const auto& cell : rows[i]) row.push_back(cell);
        table.add_row(row);
        // Alternate row shading
        if (i % 2 == 0)
            table[i].format().background_color(tabulate::Color::grey);
    }

    std::cout << table << "\n";
}

std::string ReportTemplate::generate(ReportType type,
                                     const std::string& generatedBy,
                                     int lowStockThreshold) const {
    auto rows = [&]() -> std::vector<std::vector<std::string>> {
        switch (type) {
            case ReportType::FULL_INVENTORY:    return buildFullInventoryRows();
            case ReportType::SUMMARY:           return buildSummaryRows();
            case ReportType::LOW_STOCK:         return buildLowStockRows(lowStockThreshold);
            case ReportType::CATEGORY_BREAKDOWN:return buildCategoryRows();
        }
        return {};
    }();

    // Prepend metadata rows
    rows.insert(rows.begin(), {"Generated by", generatedBy});
    rows.insert(rows.begin(), {"Report", reportTypeName(type)});

    std::string tagMap[] = {"inventory", "summary", "lowstock", "categories"};
    std::string tag = tagMap[static_cast<int>(type)];
    std::string path = timestampedFilename(tag);

    if (Utils::xlsxWriteSheet(path, reportTypeName(type), rows)) {
        Utils::printSuccess("Report saved: " + path);
        return path;
    }
    Utils::printError("Failed to save report: " + path);
    return "";
}