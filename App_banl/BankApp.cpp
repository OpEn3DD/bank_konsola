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

int BankApp::selectAccount(int& userId) {
    int accId{};
    const char* sql = "SELECT id, accountnumber, currencyId FROM Accounts WHERE userId = ?;";
    sqlite3_stmt* stmt;

    int rc = sqlite3_prepare_v2(dbManager.getDB(), sql, -1, &stmt, 0);
    if (rc != SQLITE_OK) {
        cerr << "Nie uda³o siê przygotowaæ zapytania: " << sqlite3_errmsg(dbManager.getDB()) << endl;
        return -1;
    }

    sqlite3_bind_int64(stmt, 1, userId);

    cout << "User ID: " << userId << endl;

    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        cout << "ID konta: " << sqlite3_column_int(stmt, 0)
            << ", numer konta: " << sqlite3_column_text(stmt, 1)
            << ", waluta: " << sqlite3_column_text(stmt, 2) << endl;
    }

    if (rc != SQLITE_DONE) {
        cerr << "Nie uda³o siê pobraæ listy twoich kont: " << sqlite3_errmsg(dbManager.getDB()) << endl;
        sqlite3_finalize(stmt);
        return -1;
    }

    sqlite3_finalize(stmt);

    cout << "\nPodaj ID konta na które chcesz siê zalogowaæ: ";
    cin >> accId;

    return accId;
}

void BankApp::run() {
    int choice{};
    int ammount{};

    std::cout << "$$\\      $$\\ $$$$$$\\ $$$$$$$$\\  $$$$$$\\  $$\\      $$\\ $$\\     $$\\       $$\\      $$\\       $$$$$$$\\   $$$$$$\\  $$\\   $$\\ $$\\   $$\\ $$\\   $$\\ \n";
    std::cout << "$$ | $\\  $$ |\\_$$  _|\\__$$  __|$$  __$$\\ $$$\\    $$$ |\\$$\\   $$  |      $$ | $\\  $$ |      $$  __$$\\ $$  __$$\\ $$$\\  $$ |$$ | $$  |$$ |  $$ |\n";
    std::cout << "$$ |$$$\\ $$ |  $$ |     $$ |   $$ /  $$ |$$$$\\  $$$$ | \\$$\\ $$  /       $$ |$$$\\ $$ |      $$ |  $$ |$$ /  $$ |$$$$\\ $$ |$$ |$$  / $$ |  $$ |\n";
    std::cout << "$$ $$ $$\\$$ |  $$ |     $$ |   $$$$$$$$ |$$\\$$\\$$ $$ |  \\$$$$  /        $$ $$ $$\\$$ |      $$$$$$$\\ |$$$$$$$$ |$$ $$\\$$ |$$$$$  /  $$ |  $$ |\n";
    std::cout << "$$$$  _$$$$ |  $$ |     $$ |   $$  __$$ |$$ \\$$$  $$ |   \\$$  /         $$$$  _$$$$ |      $$  __$$\\ $$  __$$ |$$ \\$$$$ |$$  $$<   $$ |  $$ |\n";
    std::cout << "$$$  / \\$$$ |  $$ |     $$ |   $$ |  $$ |$$ |\\$  /$$ |    $$ |          $$$  / \\$$$ |      $$ |  $$ |$$ |  $$ |$$ |\\$$$ |$$ |\\$$\\  $$ |  $$ |\n";
    std::cout << "$$  /   \\$$ |$$$$$$\\    $$ |   $$ |  $$ |$$ | \\_/ $$ |    $$ |          $$  /   \\$$ |      $$$$$$$  |$$ |  $$ |$$ | \\$$ |$$ | \\$$\\ \\$$$$$$  |\n";
    std::cout << "\\__/     \\__|\\______|   \\__|   \\__|  \\__|\\__|     \\__|    \\__|          \\__/     \\__|      \\_______/ \\__|  \\__|\\__|  \\__|\\__|  \\__| \\______/ \n\n";


    while (true) {
        if (!loggedIn) {

            cout << "1. Za³ó¿ konto u¿ytkownika\n2. Zaloguj\n7. Exit\nWybierz opcje: ";
            cin >> choice;

            switch (choice) {
            case 1:
                system("cls");
                if (!loggedIn) {
                    user.createUser(loggedInUserId);
                    accounts.createAccount(loggedInUserId, "PLN");
                    loggedIn = true;
                }
                else cout << "Nie mozesz utworzyæ konta uzytkownika bêd¹c zalogowanym" << endl;
                break;
            case 2:
                system("cls");
                loggedIn = user.login(loggedInUserId);
                loggedInAccount = false;
                break;
            case 7:
                return;
            case 69:
                user.techniczna();
                break;
            default:
                system("cls");
                cout << "Invalid option. Please try again." << endl;
                std::cin.clear(); // Resetuje flagi b³êdów
                std::cin.ignore(12343234, '\n');//Czysczenie strumienia wejscia
                break;
            }
        }
        else if (loggedIn && !loggedInAccount) {
            loggedInAccountId = selectAccount(loggedInUserId);
            loggedInAccount = true;
        }
        else if (loggedIn && loggedInAccount) {

            cout << "1.Za³ó¿ nowe konto bankowe \n2.Poka¿ bilans konta\n3.Wp³aæ œrodki\n4.Wyplac srodki\n5.Wybierz konto bankowe\n7.Exit\nChoose an option : ";
            cin >> choice;

            switch (choice) {
            case 1:
                system("cls");
                accounts.createAccount(loggedInUserId, "PLN");
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
                loggedInAccount = false;
                break;
            case 6:
                accounts.transferMoney(loggedInAccountId);
                break;
            case 7:
                return;
            case 69:
                user.techniczna();
                break;
            default:
                system("cls");
                cout << "Invalid option. Please try again." << endl;
                std::cin.clear(); // Resetuje flagi b³êdów
                std::cin.ignore(12343234, '\n');//Czysczenie strumienia wejscia
                break;
            }
        }
    }

}