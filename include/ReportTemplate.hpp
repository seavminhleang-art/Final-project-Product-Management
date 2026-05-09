#pragma once

#include <string>
#include <vector>

class ProductManager;
class Product;

enum class ReportType {
    FULL_INVENTORY,
    SUMMARY,
    LOW_STOCK,
    CATEGORY_BREAKDOWN
};

class ReportTemplate {
public:
    explicit ReportTemplate(const ProductManager& pm,
                            const std::string& outputDir = "data/reports");

    // Preview report in terminal (tabulate table)
    void preview(ReportType type, int lowStockThreshold = 10) const;

    // Generate and save .xlsx, returns the saved file path
    std::string generate(ReportType type,
                         const std::string& generatedBy,
                         int lowStockThreshold = 10) const;

private:
    const ProductManager& m_pm;
    std::string           m_outputDir;

    // Helpers
    std::string reportTypeName(ReportType type) const;
    std::string timestampedFilename(const std::string& tag) const;

    // Row builders
    std::vector<std::vector<std::string>> buildFullInventoryRows() const;
    std::vector<std::vector<std::string>> buildSummaryRows()       const;
    std::vector<std::vector<std::string>> buildLowStockRows(int threshold) const;
    std::vector<std::vector<std::string>> buildCategoryRows()      const;
};