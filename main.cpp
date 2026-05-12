#include <iostream>
#include <vector>
#include "Product.hpp"
#include "ProductManager.hpp"
#include "UserManager.hpp"
#include "AuthService.hpp"
#include "ReportTemplate.hpp"
#include "Utils.hpp"

void showLoginScreen(AuthService& auth);
void showMainMenu(AuthService& auth, ProductManager& pm, UserManager& um);
void menuProducts(AuthService& auth, ProductManager& pm);
void menuUsers(AuthService& auth, UserManager& um);
void menuReports(AuthService& auth, ProductManager& pm);

static void doAddProduct(ProductManager& pm) {
    Utils::printTitle("ADD NEW PRODUCT");
    std::string name  = Utils::getStringInput("Name        : ");
    std::string cat   = Utils::getStringInput("Category    : ");
    double price      = Utils::getDoubleInput("Price ($)   : ");
    int    qty        = Utils::getIntInput("Quantity    : ", 0, 999999);
    std::string desc  = Utils::getStringInput("Description : ");
    if (pm.addProduct(name, cat, price, qty, desc))
        Utils::printSuccess("Product added successfully.");
    else
        Utils::printError("Failed to add product. Check inputs.");
}

static void doViewProducts(ProductManager& pm) {
    Utils::printTitle("ALL PRODUCTS");
    pm.displayAll();
}

static void doSearchProducts(ProductManager& pm) {
    Utils::printTitle("SEARCH / FILTER PRODUCTS");
    std::cout << "  1. Search by name\n"
              << "  2. Filter by category\n"
              << "  3. Filter by price range\n";
    int choice = Utils::getIntInput("Choice: ", 1, 3);
    std::vector<Product> results;
    if (choice == 1) {
        std::string kw = Utils::getStringInput("Keyword: ");
        results = pm.searchByName(kw);
    } else if (choice == 2) {
        std::string cat = Utils::getStringInput("Category: ");
        results = pm.filterByCategory(cat);
    } else {
        double mn = Utils::getDoubleInput("Min price: ");
        double mx = Utils::getDoubleInput("Max price: ");
        results = pm.filterByPriceRange(mn, mx);
    }
    if (results.empty()) {
        Utils::printWarning("No matching products.");
    } else {
        Product::displayHeader();
        for (const auto& p : results) p.displayRow();
        std::cout << "  Found: " << results.size() << " result(s)\n";
    }
}

static void doSortProducts(ProductManager& pm) {
    Utils::printTitle("SORT PRODUCTS");
    std::cout << "  Sort by:\n"
              << "  1. ID\n  2. Name\n  3. Category\n  4. Price\n  5. Quantity\n";
    int f = Utils::getIntInput("Field: ", 1, 5);
    std::cout << "  Order:\n  1. Ascending\n  2. Descending\n";
    int o = Utils::getIntInput("Order: ", 1, 2);
    SortField field = static_cast<SortField>(f - 1);
    SortOrder order = (o == 1) ? SortOrder::ASC : SortOrder::DESC;
    pm.sortProducts(field, order);
    Utils::printSuccess("Sorted. Displaying results:");
    pm.displayAll();
}

static void doEditProduct(ProductManager& pm) {
    Utils::printTitle("EDIT PRODUCT");
    pm.displayAll();
    int id = Utils::getIntInput("Enter Product ID to edit: ", 1, 999999);
    pm.displayProduct(id);
    std::cout << "\n  Fields: name | category | price | quantity | description\n";
    std::string field = Utils::getStringInput("Field to edit : ");
    std::string value = Utils::getStringInput("New value     : ");
    if (pm.editProduct(id, field, value))
        Utils::printSuccess("Product updated.");
    else
        Utils::printError("Update failed. Check ID and field name.");
}

static void doDeleteProduct(AuthService& auth, ProductManager& pm) {
    if (!auth.canDelete()) { Utils::printError("Permission denied."); return; }
    Utils::printTitle("DELETE PRODUCT");
    pm.displayAll();
    int id = Utils::getIntInput("Enter Product ID to delete: ", 1, 999999);
    pm.displayProduct(id);
    if (Utils::getYesNo("Confirm delete?"))
        Utils::printSuccess(pm.deleteProduct(id) ? "Product deleted." : "Delete failed.");
}

void menuProducts(AuthService& auth, ProductManager& pm) {
    bool back = false;
    while (!back) {
        Utils::clearScreen();
        Utils::printTitle("PRODUCT MANAGEMENT");
        std::cout << "  1. View all products\n"
                  << "  2. Search / Filter products\n"
                  << "  3. Sort products\n";
        if (auth.canAdd())    std::cout << "  4. Add new product\n";
        if (auth.canEdit())   std::cout << "  5. Edit product\n";
        if (auth.canDelete()) std::cout << "  6. Delete product\n";
        std::cout << "  0. Back\n";
        int ch = Utils::getIntInput("Choice: ", 0, 6);
        Utils::clearScreen();
        switch (ch) {
            case 1: doViewProducts(pm);        break;
            case 2: doSearchProducts(pm);      break;
            case 3: doSortProducts(pm);        break;
            case 4: if (auth.canAdd())  doAddProduct(pm);  break;
            case 5: if (auth.canEdit()) doEditProduct(pm); break;
            case 6: doDeleteProduct(auth, pm);             break;
            case 0: back = true;                           break;
        }
        if (!back) Utils::pauseScreen();
    }
}

void menuUsers(AuthService& auth, UserManager& um) {
    if (!auth.canManageUsers()) { Utils::printError("Admin only."); Utils::pauseScreen(); return; }
    bool back = false;
    while (!back) {
        Utils::clearScreen();
        Utils::printTitle("USER MANAGEMENT  [ADMIN]");
        std::cout << "  1. View all users\n"
                  << "  2. Add user\n"
                  << "  3. Delete user\n"
                  << "  0. Back\n";
        int ch = Utils::getIntInput("Choice: ", 0, 3);
        Utils::clearScreen();
        switch (ch) {
            case 1:
                Utils::printTitle("ALL USERS");
                um.displayAll();
                Utils::pauseScreen();
                break;
            case 2: {
                Utils::printTitle("ADD USER");
                std::string uname = Utils::getStringInput("Username : ");
                std::string pass  = Utils::getStringInput("Password : ");
                std::cout << "  Role: 1. User   2. Admin\n";
                int r = Utils::getIntInput("Role: ", 1, 2);
                UserRole role = (r == 2) ? UserRole::ADMIN : UserRole::USER;
                Utils::printSuccess(um.addUser(uname, pass, role) ? "User added." : "Failed (duplicate username?).");
                Utils::pauseScreen();
                break;
            }
            case 3: {
                um.displayAll();
                int id = Utils::getIntInput("User ID to delete: ", 1, 999999);
                if (Utils::getYesNo("Confirm delete?"))
                    Utils::printSuccess(um.deleteUser(id) ? "User deleted." : "Not found.");
                Utils::pauseScreen();
                break;
            }
            case 0: back = true; break;
        }
    }
}

void menuReports(AuthService& auth, ProductManager& pm) {
    ReportTemplate reporter(pm);
    bool back = false;
    while (!back) {
        Utils::clearScreen();
        Utils::printTitle("REPORTS");
        std::cout << "  1. Full Inventory Report\n"
                  << "  2. Summary / Statistics Report\n"
                  << "  3. Low Stock Alert Report\n"
                  << "  4. Category Breakdown Report\n"
                  << "  5. Preview report on screen\n"
                  << "  0. Back\n";
        int ch = Utils::getIntInput("Choice: ", 0, 5);
        Utils::clearScreen();
        if (ch >= 1 && ch <= 4) {
            ReportType types[] = {
                ReportType::FULL_INVENTORY,
                ReportType::SUMMARY,
                ReportType::LOW_STOCK,
                ReportType::CATEGORY_BREAKDOWN
            };
            int threshold = 10;
            if (ch == 3) threshold = Utils::getIntInput("Low stock threshold (qty): ", 1, 9999);
            reporter.generate(types[ch - 1], auth.currentUser()->getUsername(), threshold);
        } else if (ch == 5) {
            std::cout << "  1. Full Inventory\n  2. Summary\n  3. Low Stock\n  4. Category Breakdown\n";
            int sub = Utils::getIntInput("Report type: ", 1, 4);
            ReportType types[] = {
                ReportType::FULL_INVENTORY,
                ReportType::SUMMARY,
                ReportType::LOW_STOCK,
                ReportType::CATEGORY_BREAKDOWN
            };
            int threshold = 10;
            if (sub == 3) threshold = Utils::getIntInput("Low stock threshold (qty): ", 1, 9999);
            reporter.preview(types[sub - 1], threshold);
        } else if (ch == 0) {
            back = true;
        }
        if (!back) Utils::pauseScreen();
    }
}

void showMainMenu(AuthService& auth, ProductManager& pm, UserManager& um) {
    while (true) {
        Utils::clearScreen();
        Utils::printTitle("PRODUCT MANAGEMENT SYSTEM");
        std::cout << "  Logged in as: " << auth.currentUser()->getUsername()
                  << " [" << auth.currentUser()->getRoleName() << "]\n\n"
                  << "  1. Products\n"
                  << "  2. Reports\n";
        if (auth.canManageUsers()) std::cout << "  3. User Management\n";
        std::cout << "  0. Logout\n";
        int ch = Utils::getIntInput("Choice: ", 0, 3);
        switch (ch) {
            case 1: menuProducts(auth, pm); break;
            case 2: menuReports(auth, pm);  break;
            case 3: menuUsers(auth, um);    break;
            case 0: auth.logout(); return;
        }
    }
}

void showLoginScreen(AuthService& auth) {
    int attempts = 0;
    while (attempts < 3) {
        Utils::clearScreen();
        Utils::printTitle("LOGIN");
        std::string user = Utils::getStringInput("Username : ");
        std::string pass = Utils::getStringInput("Password : ");
        if (auth.login(user, pass)) return;
        Utils::printError("Invalid credentials. " + std::to_string(2 - attempts) + " attempt(s) left.");
        Utils::pauseScreen();
        ++attempts;
    }
    Utils::printError("Too many failed attempts. Exiting.");
    exit(1);
}

int main() {
    UserManager    um("data/users.dat");
    ProductManager pm("data/products.csv");
    AuthService    auth(um);
    um.loadFromFile();
    pm.loadFromFile();
    um.addUser("admin", "admin123", UserRole::ADMIN);
    while (true) {
        showLoginScreen(auth);
        showMainMenu(auth, pm, um);
    }
    return 0;
}
