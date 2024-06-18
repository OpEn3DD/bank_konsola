#include "Accounts.h"
#include <windows.h>
#include <random>
#include <regex>
#include <conio.h>

Accounts::Accounts(DbManager& dbManager) : dbManager(dbManager) {
    int count{};

    const char* sql =
        "CREATE TABLE IF NOT EXISTS Currency ("
        "id INT PRIMARY KEY NOT NULL,  "
        "currencyName TEXT NOT NULL, "
        "valueToPln FLOAT NOT NULL); ";
    dbManager.executeSQL(sql);

    const char* sql4 = "SELECT COUNT(*) FROM Currency";
    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(dbManager.getDB(), sql4, -1, &stmt, 0);
    if (rc == SQLITE_OK) {
        rc = sqlite3_step(stmt);
        if (rc == SQLITE_ROW) {
            count = sqlite3_column_int(stmt, 0);
        }
        sqlite3_finalize(stmt);
    }
    else {
        std::cerr << "Failed to execute statement: " << sqlite3_errmsg(dbManager.getDB()) << std::endl;
    }

    if (count == 0)
    {
        const char* sql2 =
            "INSERT INTO Currency (id, currencyName, valueToPln) VALUES (1, 'PLN', 1); "
            "INSERT INTO Currency (id, currencyName, valueToPln) VALUES (2, 'EUR', 4.30); "
            "INSERT INTO Currency (id, currencyName, valueToPln) VALUES (3, 'CHF', 4.55); "
            "INSERT INTO Currency (id, currencyName, valueToPln) VALUES (4, 'GBP', 5.10); ";
        dbManager.executeSQL(sql2);
    }

    const char* sql3 =
        "CREATE TABLE IF NOT EXISTS Accounts ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT, "
        "userId INT NOT NULL, "
        "accountnumber TEXT NOT NULL, "
        "balance REAL DEFAULT 0.0, "
        "currencyId INT NOT NULL, "
        "FOREIGN KEY(userId) REFERENCES Users(id), "
        "FOREIGN KEY(currencyId) REFERENCES Currency(id));";
    dbManager.executeSQL(sql3);
}

void Accounts::showBalance(int loggedInAccountId) {
    const char* sql = "SELECT Accounts.balance, Currency.currencyName FROM Accounts JOIN Currency ON Accounts.currencyId = Currency.id WHERE Accounts.id = ?;";
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

bool Accounts::loginAccount(int trueUserId, int& AccountId)
{
  /*  const char* sql2 = "SELECT id, accountNumber, userId FROM Accounts WHERE userId = ?";
    sqlite3_prepare_v2(dbManager.getDB(), sql2, -1, &stmt, 0);
    sqlite3_bind_int64(stmt, 1, userId);
    int rc = sqlite3_step(stmt);
    if (rc == SQLITE_ROW) {
        if()
        cout << "Pomyslnie zalogowano na konto bankowe o numerze " << sqlite3_column_text(stmt, 1);
        return true;
    }*/
    return false;

}

void Accounts::createAccount(int userId, int currencyId) {
    string accNumber;
    random_device rd;
    mt19937 eng(rd());
    uniform_int_distribution<> distr(0, 9);
    for (int i = 0; i < 9; ++i)
        accNumber += std::to_string(distr(eng));


    const char* sql = "INSERT INTO Accounts(userId,accountnumber,balance,currencyId) VALUES (?,?,0.0,?);";
    int rc = sqlite3_prepare_v2(dbManager.getDB(), sql, -1, &stmt, 0);

    sqlite3_bind_int64(stmt, 1, userId);
    sqlite3_bind_text(stmt, 2, accNumber.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_int64(stmt, 3, currencyId);
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

void Accounts::transferMoney(int AccountId) {
    int curra, currrec;

    int receiverId;
    double amount;
    cout << "Wpisz na jakie konto chcesz wysłać przelew: ";
    cin >> receiverId;
    std::cin.clear(); // Resetuje flagi błędów
    std::cin.ignore(12343234, '\n'); // Czyszczenie strumienia wejścia
    cout << "Wpisz kwotę: ";
    cin >> amount;
    std::cin.clear(); // Resetuje flagi błędów
    std::cin.ignore(12343234, '\n'); // Czyszczenie strumienia wejścia

    sqlite3_stmt* stmt;

    // Get sender's currencyId
    const char* sql3 = "SELECT currencyId FROM Accounts WHERE id = ?";
    int rc = sqlite3_prepare_v2(dbManager.getDB(), sql3, -1, &stmt, 0);
    if (rc == SQLITE_OK) {
        sqlite3_bind_int(stmt, 1, AccountId);
        rc = sqlite3_step(stmt);
        if (rc == SQLITE_ROW) {
            curra = sqlite3_column_int(stmt, 0);
        }
        else {
            cout << "Nie udało się przeprowadzic transakcji. Upewnij się, że poprawnie wpisałeś numer konta " << sqlite3_errmsg(dbManager.getDB()) << endl;
            sqlite3_finalize(stmt);
            return;
        }
    }
    else {
        cout << "Nie udało się przeprowadzic transakcji. Błąd przygotowania zapytania: " << sqlite3_errmsg(dbManager.getDB()) << endl;
        return;
    }
    sqlite3_finalize(stmt);

    // Get receiver's currencyId
    const char* sql4 = "SELECT currencyId FROM Accounts WHERE accountnumber = ?";
    rc = sqlite3_prepare_v2(dbManager.getDB(), sql4, -1, &stmt, 0);
    if (rc == SQLITE_OK) {
        sqlite3_bind_int(stmt, 1, receiverId);
        rc = sqlite3_step(stmt);
        if (rc == SQLITE_ROW) {
            currrec = sqlite3_column_int(stmt, 0);
        }
        else {
            cout << "Nie udało się przeprowadzic transakcji. Upewnij się, że poprawnie wpisałeś numer konta " << sqlite3_errmsg(dbManager.getDB()) << endl;
            sqlite3_finalize(stmt);
            return;
        }
    }
    else {
        cout << "Nie udało się przeprowadzic transakcji. Błąd przygotowania zapytania: " << sqlite3_errmsg(dbManager.getDB()) << endl;
        return;
    }
    sqlite3_finalize(stmt);

    if (curra == currrec) {
        rc = sqlite3_exec(dbManager.getDB(), "BEGIN TRANSACTION;", 0, 0, 0);
        if (rc != SQLITE_OK) {
            cerr << "Nie udało się rozpocząć transakcji: " << sqlite3_errmsg(dbManager.getDB()) << endl;
            return;
        }

        // Update sender's balance
        const char* sqlUpdateSender = "UPDATE Accounts SET balance = balance - ? WHERE id = ? AND balance >= ?";
        rc = sqlite3_prepare_v2(dbManager.getDB(), sqlUpdateSender, -1, &stmt, 0);
        if (rc == SQLITE_OK) {
            sqlite3_bind_double(stmt, 1, amount);
            sqlite3_bind_int(stmt, 2, AccountId);
            sqlite3_bind_double(stmt, 3, amount);
            rc = sqlite3_step(stmt);
            if (rc == SQLITE_DONE && sqlite3_changes(dbManager.getDB()) > 0) {
                sqlite3_finalize(stmt);
            }
            else {
                sqlite3_finalize(stmt);
                sqlite3_exec(dbManager.getDB(), "ROLLBACK;", 0, 0, 0);
                system("cls");
                cerr << "Transakcja nie przebiegła pomyślnie! Upewnij się, że masz wystarczającą ilość środków." << endl;
                return;
            }
        }
        else {
            cerr << "Nie udało się przygotować zapytania do aktualizacji salda nadawcy: " << sqlite3_errmsg(dbManager.getDB()) << endl;
            sqlite3_exec(dbManager.getDB(), "ROLLBACK;", 0, 0, 0);
            return;
        }

        // Update receiver's balance
        const char* sqlUpdateReceiver = "UPDATE Accounts SET balance = balance + ? WHERE accountnumber = ?";
        rc = sqlite3_prepare_v2(dbManager.getDB(), sqlUpdateReceiver, -1, &stmt, 0);
        if (rc == SQLITE_OK) {
            sqlite3_bind_double(stmt, 1, amount);
            sqlite3_bind_int(stmt, 2, receiverId);
            rc = sqlite3_step(stmt);
            if (rc == SQLITE_DONE && sqlite3_changes(dbManager.getDB()) > 0) {
                sqlite3_finalize(stmt);
            }
            else {
                sqlite3_finalize(stmt);
                sqlite3_exec(dbManager.getDB(), "ROLLBACK;", 0, 0, 0);
                system("cls");
                cerr << "Transakcja nie przebiegła pomyślnie! Upewnij się, że poprawnie wpisałeś numer konta adresata." << endl;
                return;
            }
        }
        else {
            cerr << "Nie udało się przygotować zapytania do aktualizacji salda odbiorcy: " << sqlite3_errmsg(dbManager.getDB()) << endl;
            sqlite3_exec(dbManager.getDB(), "ROLLBACK;", 0, 0, 0);
            return;
        }

        rc = sqlite3_exec(dbManager.getDB(), "COMMIT;", 0, 0, 0);
        if (rc == SQLITE_OK) {
            system("cls");
            cout << "Transakcja przebiegła pomyślnie" << endl;
        }
        else {
            cerr << "Nie udało się zatwierdzić transakcji: " << sqlite3_errmsg(dbManager.getDB()) << endl;
        }
    }
    else {
        cerr << "Transakcje miedzywalutowe nie są obsługiwane" << endl;
    }
}


