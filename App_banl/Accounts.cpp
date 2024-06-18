#include "Accounts.h"
#include <windows.h>
#include <random>
#include <regex>
#include <conio.h>

Accounts::Accounts(DbManager& dbManager) : dbManager(dbManager) {
    const char* sql3 =
        "CREATE TABLE IF NOT EXISTS Accounts ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT, "
        "userId INT NOT NULL, "
        "accountnumber TEXT NOT NULL, "
        "balance REAL DEFAULT 0.0, "
        "currencyId TEXT NOT NULL, "
        "FOREIGN KEY(userId) REFERENCES Users(id), "
        "FOREIGN KEY(currencyId) REFERENCES Currency(id));";

    dbManager.executeSQL(sql3);
}

void Accounts::showBalance(int loggedInAccountId) {
    const char* sql = "SELECT balance,currencyId FROM Accounts WHERE id = ?;";
    int rc = sqlite3_prepare_v2(dbManager.getDB(), sql, -1, &stmt, 0);
    sqlite3_bind_int64(stmt, 1, loggedInAccountId);
    rc = sqlite3_step(stmt);
    if (rc == SQLITE_ROW) {
        double balance = sqlite3_column_double(stmt, 0);
        cout << "Bilans twojego konta: " << balance << " "<< sqlite3_column_text(stmt, 1)<< endl;
    }
    else {
        cerr << "Nie udało sie pobrac bilansu twojego konta " <<sqlite3_errmsg(dbManager.getDB())<<endl;
    }

    sqlite3_finalize(stmt);
}

void Accounts::depositMoney(int accountId, float ammount) {
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
    sqlite3_bind_int(stmt, 2, accountId);

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

void Accounts::withdrawMoney(int accountId, float amount) {
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
    sqlite3_bind_int(stmt, 2, accountId);
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

bool Accounts::loginAccount(int userId, int& AccountId)
{
    const char* sql2 = "SELECT id,accountNumber FROM Accounts WHERE userId = ?";
    sqlite3_prepare_v2(dbManager.getDB(), sql2, -1, &stmt, 0);
    sqlite3_bind_int64(stmt, 1, userId);
    int rc = sqlite3_step(stmt);
    if (rc == SQLITE_ROW) {
        AccountId = sqlite3_column_int(stmt, 0);
        cout << "Pomyslnie utworzono konto bankowe o numerze " << sqlite3_column_text(stmt, 1);
        return true;
    }

}

void Accounts::createAccount(int userId, string currencyId) {
    string accNumber;
    random_device rd;
    mt19937 eng(rd());
    uniform_int_distribution<> distr(0, 9);
    for (int i = 0; i < 11; ++i)
        accNumber += std::to_string(distr(eng));


    const char* sql = "INSERT INTO Accounts(userId,accountnumber,balance,currencyId) VALUES (?,?,0.0,?);";
    int rc = sqlite3_prepare_v2(dbManager.getDB(), sql, -1, &stmt, 0);

    sqlite3_bind_int64(stmt, 1, userId);
    sqlite3_bind_text(stmt, 2, accNumber.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, currencyId.c_str(), -1, SQLITE_STATIC);
    rc = sqlite3_step(stmt);

    if (rc != SQLITE_DONE) {
        cout << "SQL error: " << sqlite3_errmsg(dbManager.getDB()) << endl;
        sqlite3_finalize(stmt);
        return;
    }
    sqlite3_finalize(stmt); 

    const char* sql2 = "SELECT last_insert_rowid()";
    rc = sqlite3_prepare_v2(dbManager.getDB(), sql2, -1, &stmt, 0);


    rc = sqlite3_step(stmt);
    int last_id = 0;
    if (rc == SQLITE_ROW) {
        last_id = sqlite3_column_int(stmt, 0);
    }
    else {
        cout << "SQl error: " << sqlite3_errmsg(dbManager.getDB()) << endl;
        sqlite3_finalize(stmt);
        return;
    }
    sqlite3_finalize(stmt); 

    const char* sql3 = "SELECT accountNumber FROM Accounts WHERE id = ?";
    rc = sqlite3_prepare_v2(dbManager.getDB(), sql3, -1, &stmt, 0);


    sqlite3_bind_int64(stmt, 1, last_id);
    rc = sqlite3_step(stmt);
    if (rc == SQLITE_ROW) {
        cout << "Pomyslnie utworzono konto bankowe o numerze " << sqlite3_column_text(stmt, 0) << endl;
    }
    else {
        cout << "Nie udało się utworzyć konta: " << sqlite3_errmsg(dbManager.getDB()) << endl;
    }
    sqlite3_finalize(stmt);
}

