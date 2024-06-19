#include "User.h"
#include <windows.h>
#include <random>
#include <regex>
#include <conio.h>

User::User(DbManager& dbManager) : dbManager(dbManager) {
    const char* sql =
        "CREATE TABLE IF NOT EXISTS Users ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT, "
        "username TEXT NOT NULL, "
        "password TEXT NOT NULL, "
        "pesel TEXT NOT NULL, "
        "email TEXT NOT NULL);";
    dbManager.executeSQL(sql);
}

void User::techniczna() {
    const char* sql = "SELECT id, username FROM Users;";
    sqlite3_stmt* stmt;

    int rc = sqlite3_prepare_v2(dbManager.getDB(), sql, -1, &stmt, 0);
    //rc = sqlite3_step(stmt);

    if (rc == SQLITE_OK) {
        while (sqlite3_step(stmt) == SQLITE_ROW)
            cout << "ID: " << sqlite3_column_int(stmt, 0) << ", username:  " << sqlite3_column_text(stmt, 1) << endl;
    }
    else {
        cerr << "Nie udało sie pobrac listy. " << sqlite3_errmsg(dbManager.getDB()) << endl;
    }

    sqlite3_finalize(stmt);
}

void User::createUser(int& loggedInUserId) {
    string username, password, pesel, email;
    cout << "Wpisz nazwe uzytkownika: ";
    cin >> username;
    char ch;

    do {
        password.clear(); // Czyszczenie hasła przed ponownym wprowadzeniem
        cout << "Wpisz hasło: ";
        while ((ch = _getch()) != 13) { // 13 to ASCII dla Enter
            if (ch == 8) { // 8 jest ASCII dla Backspace
                if (!password.empty()) {
                    cout << "\b \b"; // Usuwanie ostatniego znaku
                    password.pop_back();
                }
            }
            else {
                password.push_back(ch);
                cout << '*';
            }
        }
        cout << endl; // Przechodzenie do nowej linii po wprowadzeniu hasła
    } while (!isPasswordRight(password));

    do {
        cout << "Wpisz swoj numer pesel: ";
        cin >> pesel;
    } while (!isPeselRight(pesel));

    do {
        cout << "Wpisz swoj numer email: ";
        cin >> email;
    } while (!isEmailRight(email));

    const char* sql = "INSERT INTO Users (username, password, pesel, email) VALUES (?, ?, ?, ?);";
    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(dbManager.getDB(), sql, -1, &stmt, 0);
    if (rc != SQLITE_OK) {
        cout << "SQL error: " << sqlite3_errmsg(dbManager.getDB()) << endl;
        return;
    }

    sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, password.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, pesel.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 4, email.c_str(), -1, SQLITE_STATIC);

    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) {
        cout << "SQL error: " << sqlite3_errmsg(dbManager.getDB()) << endl;
        sqlite3_finalize(stmt);
        return;
    }
    sqlite3_finalize(stmt);

    const char* sql2 = "SELECT last_insert_rowid()";
    rc = sqlite3_prepare_v2(dbManager.getDB(), sql2, -1, &stmt, 0);
    if (rc != SQLITE_OK) {
        cout << "SQL error: " << sqlite3_errmsg(dbManager.getDB()) << endl;
        return;
    }

    rc = sqlite3_step(stmt);
    if (rc == SQLITE_ROW) {
        loggedInUserId = sqlite3_column_int(stmt, 0);
        cout << "Logged in Account ID: " << loggedInUserId << endl;
    }
    else {
        cout << "SQL error: " << sqlite3_errmsg(dbManager.getDB()) << endl;
        sqlite3_finalize(stmt);
        return;
    }
    sqlite3_finalize(stmt);

    cout << "Stworzono konto użytkownika.\n" << endl;
}

bool User::login(int& userId) {
    string username, password;
    char ch;
    cout << "Wpisz nazwe uzytkownika: ";
    cin >> username;
    cout << "Wpisz hasło: ";
    while ((ch = _getch()) != 13) { // 13 is ASCII for Enter key
        if (ch == 8) { // 8 is ASCII for Backspace key
            if (!password.empty()) {
                cout << "\b \b"; // Erase the last character
                password.pop_back();
            }
        }
        else {
            password.push_back(ch);
            cout << '*';
        }
    }
    cout << password;

    const char* sql = "SELECT id, username FROM Users WHERE username = ? AND password = ?;";
    sqlite3_prepare_v2(dbManager.getDB(), sql, -1, &stmt, 0);
    sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, password.c_str(), -1, SQLITE_STATIC);

    int rc = sqlite3_step(stmt);

    if (rc == SQLITE_ROW) {
        userId = sqlite3_column_int(stmt, 0);
        system("cls");
        cout << "Zalogowano, witamy " << sqlite3_column_text(stmt, 1) << ".\n" << endl;
        sqlite3_finalize(stmt);
        return true;
    }
    else {
        cerr << "Nie udało sie zalogowac" << endl;
        sqlite3_finalize(stmt);
        return false;
    }
}

bool User::isPasswordRight(string password) {
    bool hasUpperCase = false;
    bool hasDigit = false;
    bool hasSpecialChar = false;

    //Sprawdzenie czy w hasle wystepują wszystkie typy znaków
    for (char ch : password) {
        if (isupper(ch)) {
            hasUpperCase = true;
        }
        else if (isdigit(ch)) {
            hasDigit = true;
        }
        else if (!isalnum(ch)) {
            hasSpecialChar = true;
        }
    }
    //Sprawdzanie długosci hasla
    if(password.length() >= 8 && (hasUpperCase + hasDigit + hasSpecialChar) >= 3)
        return true;

    cout << "Haslo nie spelnia minimalnych wymagan\n";
    return false;
}

bool User::isPeselRight(string pesel) {
    // Sprawdzenie długości PESEL
    if (pesel.length() != 11) {
        return false;
    }

    // Sprawdzenie czy wszystkie znaki są cyframi
    for (char ch : pesel) {
        if (!isdigit(ch)) {
            return false;
        }
    }

    // Obliczenie sumy kontrolnej
    int weights[] = { 1, 3, 7, 9, 1, 3, 7, 9, 1, 3 };
    int sum = 0;
    for (int i = 0; i < 10; ++i) {
        sum += (pesel[i] - '0') * weights[i];
    }

    // Sprawdzenie ostatniej cyfry jako sumy kontrolnej
    int checksum = (10 - (sum % 10)) % 10;
    return checksum == (pesel[10] - '0');

}

bool User::isEmailRight(string email) {
    // Definiowanie wzorca dla poprawnego adresu email
    const std::regex pattern(R"((\w+)(\.\w+)*@(\w+)(\.\w+)+)");

    // Sprawdzanie, czy wprowadzony email pasuje do wzorca
    return std::regex_match(email, pattern);

}

void User::deleteUser(int& userId, int& loggedInAccountId, bool& loggedInAccount, bool& loggedIn) {
    const char* sql =
        "DELETE FROM Users WHERE id = ?; "
        "DELETE FROM Accounts WHERE userId = ?; ";

    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(dbManager.getDB(), sql, -1, &stmt, 0);
    if (rc != SQLITE_OK) {
        cout << "SQL error: " << sqlite3_errmsg(dbManager.getDB()) << endl;
        return;
    }

    sqlite3_bind_int64(stmt, 1, userId);
    sqlite3_bind_int64(stmt, 2, userId);

    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) {
        cout << "SQL error: " << sqlite3_errmsg(dbManager.getDB()) << endl;
        sqlite3_finalize(stmt);
        return;
    }

    sqlite3_finalize(stmt);

    system("cls");
    cout << "Pomyślnie usunięto konto użytkownika." << endl;
    userId = -1;
    loggedInAccountId = -1;
    loggedInAccount = false;
    loggedIn = false;

}


