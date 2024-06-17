#include "Accounts.h"
#include <windows.h>
#include <random>
#include <regex>
#include <conio.h>

Accounts::Accounts(DbManager& dbManager) : dbManager(dbManager) {}

void Accounts::showBalance(int userId) {
    const char* sql = "SELECT balance FROM Accounts WHERE userId = ?;";
    sqlite3_prepare_v2(dbManager.getDB(), sql, -1, &stmt, 0);
    sqlite3_bind_int(stmt, 1, userId);

    int rc = sqlite3_step(stmt);
    if (rc == SQLITE_ROW) {
        double balance = sqlite3_column_double(stmt, 0);
        cout << "Bilans twojego konta: " << balance << endl;
    }
    else {
        cerr << "Nie udało sie pobrac bilansu twojego konta" << endl;
    }

    sqlite3_finalize(stmt);
}

void Accounts::depositMoney(int userId, float ammount) {
    const char* sql = "UPDATE Accounts SET balance = balance + ? WHERE id = ?;";
    sqlite3_stmt* stmt;
    int rc;

    // Przygotowanie zapytania
    rc = sqlite3_prepare_v2(dbManager.getDB(), sql, -1, &stmt, 0);
    if (rc != SQLITE_OK) {
        cerr << "Nie udało się przygotować zapytania: " << sqlite3_errmsg(dbManager.getDB()) << endl;
        return;
    }

    // Bindowanie parametrów
    sqlite3_bind_int(stmt, 1, ammount);
    sqlite3_bind_int(stmt, 2, userId);

    // Wykonanie zapytania
    rc = sqlite3_step(stmt);
    if (rc == SQLITE_DONE) {
        system("cls");
        cout << "Wpłacono środki. Kwota: " << ammount << " złotych." << endl;
    }
    else {
        cerr << "Wpłata nie przebiegła pomyślnie! Skontaktuj się z infolinią." << endl;
    }

    // Zwolnienie zasobów
    sqlite3_finalize(stmt);
}

void Accounts::withdrawMoney(int userId, float amount) {
    const char* sql = "UPDATE Accounts SET balance = balance - ? WHERE id = ? AND balance >= ?;";
    sqlite3_stmt* stmt;
    int rc;

    // Przygotowanie zapytania
    rc = sqlite3_prepare_v2(dbManager.getDB(), sql, -1, &stmt, 0);
    if (rc != SQLITE_OK) {
        cerr << "Nie udało się przygotować zapytania: " << sqlite3_errmsg(dbManager.getDB()) << endl;
        return;
    }

    // Bindowanie parametrów
    sqlite3_bind_int(stmt, 1, amount);
    sqlite3_bind_int(stmt, 2, userId);
    sqlite3_bind_int(stmt, 3, amount);

    // Wykonanie zapytania
    rc = sqlite3_step(stmt);
    if (rc == SQLITE_DONE) {
        if (sqlite3_changes(dbManager.getDB()) > 0) {
            system("cls");
            cout << "Wypłacono środki. Kwota: " << amount << " złotych." << endl;
        }
        else {
            system("cls");
            cerr << "Wypłata nie przebiegła pomyślnie! Upewnij się, że masz wystarczającą ilość środków." << endl;
        }
    }
    else {
        system("cls");
        cerr << "Wypłata nie przebiegła pomyślnie! Skontaktuj się z infolinią." << endl;
    }

    // Zwolnienie zasobów
    sqlite3_finalize(stmt);
}

void Accounts::createAccount(int userId, string currencyId, int& AccountId) {
    string accNumber;
    random_device rd;
    mt19937 eng(rd());
    uniform_int_distribution<> distr(0, 9);
    for (int i = 0; i < 11; ++i)
        accNumber += std::to_string(distr(eng));

    // Wykonanie polecenia PRAGMA table_info(Accounts);
    const char* sql3 = "PRAGMA table_info(Accounts);";

    int rc = sqlite3_prepare_v2(dbManager.getDB(), sql3, -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        std::cerr << "Failed to execute statement: " << sqlite3_errmsg(dbManager.getDB()) << std::endl;
        sqlite3_close(dbManager.getDB());
    }

    // Pobieranie i wyświetlanie wyników
    std::cout << "Columns in Accounts table:\n";
    std::cout << "cid | name | type | notnull | dflt_value | pk\n";
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        int cid = sqlite3_column_int(stmt, 0);
        const unsigned char* name = sqlite3_column_text(stmt, 1);
        const unsigned char* type = sqlite3_column_text(stmt, 2);
        int notnull = sqlite3_column_int(stmt, 3);
        const unsigned char* dflt_value = sqlite3_column_text(stmt, 4);
        int pk = sqlite3_column_int(stmt, 5);

        std::cout << cid << " | "
            << (name ? reinterpret_cast<const char*>(name) : "NULL") << " | "
            << (type ? reinterpret_cast<const char*>(type) : "NULL") << " | "
            << notnull << " | "
            << (dflt_value ? reinterpret_cast<const char*>(dflt_value) : "NULL") << " | "
            << pk << std::endl;
    }

    // Zwalnianie zasobów
    sqlite3_finalize(stmt);
    
    const char* sql = "INSERT INTO Accounts(userId,accountnumber,balance ,currencyId) VALUES (?,?,0.0, ?);";
     rc = sqlite3_prepare_v2(dbManager.getDB(), sql, -1, &stmt, 0);
    sqlite3_bind_int64(stmt, 1, userId);
    sqlite3_bind_text(stmt, 2, accNumber.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, currencyId.c_str(), -1, SQLITE_STATIC);
    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) {
        cout << "SQL error during step: " << sqlite3_errmsg(dbManager.getDB()) << endl;
        sqlite3_finalize(stmt);
        return;
    }
    sqlite3_finalize(stmt);
    const char* sql2 = "SELECT id,accountNumber FROM Accounts WHERE userId = ?";
    sqlite3_prepare_v2(dbManager.getDB(), sql2, -1, &stmt, 0);
    sqlite3_bind_int64(stmt, 1, userId);
    rc = sqlite3_step(stmt);
    if (rc == SQLITE_ROW) {
        cout << "x";
        AccountId = sqlite3_column_int(stmt, 0);
        cout << "Pomyslnie utworzono konto bankowe o numerze " << sqlite3_column_text(stmt, 1);
    }


}