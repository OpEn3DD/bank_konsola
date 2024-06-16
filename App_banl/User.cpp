#include "User.h"
#include <windows.h>
#include <random>

User::User(DbManager& dbManager) : dbManager(dbManager) {
    const char* sql =
        "CREATE TABLE IF NOT EXISTS Users ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT, "
        "username TEXT NOT NULL, "
        "password TEXT NOT NULL, "
        "pesel TEXT NOT NULL, "
        "accNumber TEXT,"
        "balance REAL DEFAULT 0.0);";
    dbManager.executeSQL(sql);
}

void User::techniczna(int& userId) {
    const char* sql = "SELECT * FROM Users WHERE id = ?;";
    sqlite3_prepare_v2(dbManager.getDB(), sql, -1, &stmt, 0);
    sqlite3_bind_int(stmt, 1, userId);

    int rc = sqlite3_step(stmt);
    if (rc == SQLITE_ROW) {
        for (int i = 0; i < 7; i++)
        {
            const unsigned char* info = sqlite3_column_text(stmt, i);
            cerr << info << endl;
        }
        
    }
    else {
        cerr << "Nie udało sie pobraæ bilansu twojego konta" << endl;
    }

    sqlite3_finalize(stmt);
}

void User::createUser() {
    string username, password, pesel, accNumber;
    cout << "Wpisz nazwe uzytkownika: ";
    cin >> username;
    do {
        cout << "Wpisz hasło:";
        HANDLE hStdin = GetStdHandle(STD_INPUT_HANDLE);
        DWORD mode = 0;
        GetConsoleMode(hStdin, &mode);
        SetConsoleMode(hStdin, mode & (~ENABLE_ECHO_INPUT));
        cin >> password;
        SetConsoleMode(hStdin, mode);
    } while (!isPasswordRight(password));

    do {
        cout << "Wpisz swoj numer pesel ";
        cin >> pesel;
    } while (!isPeselRight(pesel));

    random_device rd;
    mt19937 eng(rd()); 
    uniform_int_distribution<> distr(0, 9); 
    for (int i = 0; i < 11; ++i)
        accNumber += std::to_string(distr(eng));


    const char* sql = "INSERT INTO Users (username, password, pesel, accNumber) VALUES (?, ?, ?, ?);";
    sqlite3_prepare_v2(dbManager.getDB(), sql, -1, &stmt, 0);
    sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, password.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, pesel.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 4, accNumber.c_str(), -1, SQLITE_STATIC);

    sqlite3_step(stmt);
    system("cls");
    cout << "Stworzono konto użytkownika" << endl;
    sqlite3_finalize(stmt);
}

bool User::login(int& userId) {
    string username, password;
    cout << "Wpisz nazwe uzytkownika: ";
    cin >> username;
    cout << "Wpisz hasło: ";
    HANDLE hStdin = GetStdHandle(STD_INPUT_HANDLE);
    DWORD mode = 0;
    GetConsoleMode(hStdin, &mode);
    SetConsoleMode(hStdin, mode & (~ENABLE_ECHO_INPUT));
    cin >> password;
    SetConsoleMode(hStdin, mode);

    const char* sql = "SELECT id FROM Users WHERE username = ? AND password = ?;";
    sqlite3_prepare_v2(dbManager.getDB(), sql, -1, &stmt, 0);
    sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, password.c_str(), -1, SQLITE_STATIC);

    int rc = sqlite3_step(stmt);
    if (rc == SQLITE_ROW) {
        userId = sqlite3_column_int(stmt, 0);
        system("cls");
        cout << "Zalogowano" << endl;
        sqlite3_finalize(stmt);
        return true;
    }
    else {
        cerr << "Nie udało sie zalogowac" << endl;
        sqlite3_finalize(stmt);
        return false;
    }
}

void User::showBalance(int userId) {
    const char* sql = "SELECT balance FROM Users WHERE id = ?;";
    sqlite3_prepare_v2(dbManager.getDB(), sql, -1, &stmt, 0);
    sqlite3_bind_int(stmt, 1, userId);

    int rc = sqlite3_step(stmt);
    if (rc == SQLITE_ROW) {
        double balance = sqlite3_column_double(stmt, 0);
        cout << "Bilans twojego konta: " << balance << endl;
    }
    else {
        cerr << "Nie uda³o sie pobraæ bilansu twojego konta" << endl;
    }

    sqlite3_finalize(stmt);
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

bool User::deleteUser() {
    return true;
}

void User::wplacSrodki(int userId, float ammount) {
    const char* sql = "UPDATE Users SET balance = balance + ? WHERE id = ?;";
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
    } else {
        cerr << "Wpłata nie przebiegła pomyślnie! Skontaktuj się z infolinią." << endl;
    }

    // Zwolnienie zasobów
    sqlite3_finalize(stmt);
}

void User::wyplacSrodki(int userId, float amount) {
    const char* sql = "UPDATE Users SET balance = balance - ? WHERE id = ? AND balance >= ?;";
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