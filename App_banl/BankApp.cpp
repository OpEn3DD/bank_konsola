#include "BankApp.h"
#include "windows.h"
#include <string>

BankApp::BankApp(const std::string& dbName) :
    dbManager(dbName),
    user(dbManager),
    accounts(dbManager),
    loggedInUserId(-1),
    loggedInAccountId(-1)
{};

int BankApp::selectAccount(int userId) {
    int accId{};

    // SQL query to select account details based on userId
    const char* sql = "SELECT Accounts.id, Accounts.accountnumber, Currency.currencyName FROM Accounts JOIN Currency ON Accounts.currencyId = Currency.id WHERE Accounts.userId = ? ; ";
    sqlite3_stmt* stmt;

    // Prepare the SQL statement
    int rc = sqlite3_prepare_v2(dbManager.getDB(), sql, -1, &stmt, 0);
    if (rc != SQLITE_OK) {
        cerr << "Nie uda³o siê przygotowaæ zapytania: " << sqlite3_errmsg(dbManager.getDB()) << endl;
        return -1;
    }

    // Bind the userId parameter to the SQL statement
    sqlite3_bind_int64(stmt, 1, userId);

    // Execute the SQL statement and print account details
    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        cout << "ID konta: " << sqlite3_column_int(stmt, 0)
            << ", numer konta: " << sqlite3_column_text(stmt, 1)
            << ", waluta: " << sqlite3_column_text(stmt, 2) << endl;
    }

    // Check if the SQL statement execution was successful
    if (rc != SQLITE_DONE) {
        cerr << "Nie uda³o siê pobraæ listy twoich kont: " << sqlite3_errmsg(dbManager.getDB()) << endl;
        sqlite3_finalize(stmt);
        return -1;
    }

    // Finalize the SQL statement to clean up resources
    sqlite3_finalize(stmt);

    cout << "\nPodaj ID konta na które chcesz siê zalogowaæ: ";
    cin >> accId;

    // SQL query to verify the selected account based on userId and accId
    const char* sql2 = "SELECT id, accountNumber, userId FROM Accounts WHERE userId = ? AND  id = ?";

    // Prepare the second SQL statement
    sqlite3_prepare_v2(dbManager.getDB(), sql2, -1, &stmt, 0);

    // Bind the userId and accId parameters to the SQL statement
    sqlite3_bind_int64(stmt, 1, userId);
    sqlite3_bind_int64(stmt, 2, accId);

    // Execute the SQL statement to verify the account
    rc = sqlite3_step(stmt);

    // If the account exists, log in and return the account ID
    if (rc == SQLITE_ROW) {
        system("cls");
        cout << "Pomyslnie zalogowano na konto bankowe o numerze " << sqlite3_column_text(stmt, 1) << "\n";
        loggedInAccount = true;
        return accId;
    }
    else {
        // If the account does not exist, show an error message
        system("cls");
        cout << "Wprowadzono niew³aœciwe id konta!\n";
        return -1;
    }
}

void BankApp::run() {
    int choice{};
    float ammount{};

    while (true) {
        std::cout << "=========================================\n\n";
        std::cout << " /$$$$$$$                      /$$      \n";
        std::cout << "| $$__  $$                    | $$      \n";
        std::cout << "| $$  \\ $$  /$$$$$$  /$$$$$$$ | $$   /$$\n";
        std::cout << "| $$$$$$$  |____  $$| $$__  $$| $$  /$$/\n";
        std::cout << "| $$__  $$  /$$$$$$$| $$  \\ $$| $$$$$$/ \n";
        std::cout << "| $$  \\ $$ /$$__  $$| $$  | $$| $$_  $$ \n";
        std::cout << "| $$$$$$$/|  $$$$$$$| $$  | $$| $$ \\  $$\n";
        std::cout << "|_______/  \\_______/|__/  |__/|__/  \\__/\n\n";

        if (!loggedIn) {

            cout << "1. Za³ó¿ konto u¿ytkownika\n2. Zaloguj\n3. Exit\nWybierz opcje: ";
            cin >> choice;

            switch (choice) {
            case 1:
                system("cls");

                user.createUser(loggedInUserId);
                accounts.createAccount(loggedInUserId, 1);
                loggedIn = true;
               
                break;
            case 2:
                system("cls");
                loggedIn = user.login(loggedInUserId);
                loggedInAccount = false;
                break;
            case 3:
                return;
            case 68:
                user.techniczna();
                break;
            default:
                system("cls");
                cout << "B³êdna opcja. Spróbuj ponownie!" << endl;
                std::cin.clear(); // Resetuje flagi b³êdów
                std::cin.ignore(12343234, '\n');//Czysczenie strumienia wejscia
                break;
            }
        }
        else if (loggedIn && !loggedInAccount) {
            loggedInAccountId = selectAccount(loggedInUserId);
        }
        else if (loggedIn && loggedInAccount) {
            int acc{};
            cout << "1.Za³ó¿ nowe konto bankowe \n2.Poka¿ bilans konta\n3.Wp³aæ œrodki\n4.Wyp³aæ œrodki\n5.Przelew\n6.Wyloguj z konta bankkowego\n7.Usun konto\n8.Usun konto bankowe\n9.Exit\nWybierz opcjê : ";
            cin >> choice;

            switch (choice) {
            case 1:
                int choiceC;
                system("cls");
                cout << "1.Za³ó¿ konto w PLN \n2.Za³ó¿ konto w EUR\n3.Za³ó¿ konto w CHF\n4.Za³ó¿ konto w GBP \nWybierz opcjê: ";
                cin >> choiceC;
                switch (choiceC) {
                case 1:
                    system("cls");
                    accounts.createAccount(loggedInUserId, 1);
                    break;

                case 2:
                    system("cls");
                    accounts.createAccount(loggedInUserId, 2);
                    break;
                case 3:
                    system("cls");
                    accounts.createAccount(loggedInUserId, 3);
                    break;

                case 4:
                    system("cls");
                    accounts.createAccount(loggedInUserId, 4);
                    break;
                default:
                    system("cls");
                    cout << "Wybierz poprawn¹ opcjê\n";
                    break;


                }
                break;
            case 2:
                system("cls");
                accounts.showBalance(loggedInAccountId);
                break;
            case 3:
                system("cls");
                if (loggedIn) {
                    cout << "Podaj wp³acan¹ kwote: ";
                    cin >> ammount;
                    accounts.depositMoney(loggedInAccountId, ammount);
                }
                else cout << "Najpierw musisz siê zalogowac" << endl;
                break;
            case 4:
                system("cls");
                if (loggedIn) {
                    cout << "Podaj wyp³acan¹ kwote: ";
                    cin >> ammount;
                    accounts.withdrawMoney(loggedInAccountId, ammount);
                }
                else cout << "Najpierw musisz siê zalogowac" << endl;
                break;
            case 5:
                accounts.transferMoney(loggedInAccountId);
                break;
            case 6:
                loggedInAccount = false;
                system("cls");
                break;
            case 7:
                system("cls");
                cout << "Czy napewno chcesz usunac konto? Wprowadz podana liczbe aby potwierdziæ: " << loggedInUserId<<endl;
                cin >> acc;
                if (acc == loggedInUserId)
                    user.deleteUser(loggedInUserId, loggedInAccountId, loggedInAccount, loggedIn);
                else {
                    system("cls");
                    cout << "Wprowadzono niepoprawn¹ liczbe!\n";
                }
                break;
            case 8:
                accounts.deleteAccount(loggedInAccountId, loggedInAccount, loggedInUserId);
                break;
            case 9:
                return;
            case 68:
                user.techniczna();
                break;
            default:
                system("cls");
                cout << "B³êdna opcja. Spróbuj ponownie!" << endl;
                std::cin.clear(); // Resetuje flagi b³êdów
                std::cin.ignore(12343234, '\n');//Czysczenie strumienia wejscia
                break;
            }
        }
    }

}