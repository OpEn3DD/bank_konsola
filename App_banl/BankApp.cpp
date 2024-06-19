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
    const char* sql = "SELECT Accounts.id, Accounts.accountnumber, Currency.currencyName FROM Accounts JOIN Currency ON Accounts.currencyId = Currency.id WHERE Accounts.userId = ? ; ";
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

    const char* sql2 = "SELECT id, accountNumber, userId FROM Accounts WHERE userId = ? AND  id = ?";
    sqlite3_prepare_v2(dbManager.getDB(), sql2, -1, &stmt, 0);
    sqlite3_bind_int64(stmt, 1, userId);
    sqlite3_bind_int64(stmt, 2, accId);
    rc = sqlite3_step(stmt);
    if (rc == SQLITE_ROW) {
        system("cls");
        cout << "Pomyslnie zalogowano na konto bankowe o numerze " << sqlite3_column_text(stmt, 1) << "\n";
        loggedInAccount = true;
        return accId;
    }
    else {
        system("cls");
        cout << "Wprowadzono niew³aœciwe id konta!\n";
        return -1;
    }
   
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
                int choicec;
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
        }
        else if (loggedIn && loggedInAccount) {
            int acc{};
            cout << "1.Za³ó¿ nowe konto bankowe \n2.Poka¿ bilans konta\n3.Wp³aæ œrodki\n4.Wyplac srodki\n5.Wybierz konto bankowe\n7.Exit\nChoose an option : ";
            cin >> choice;

            switch (choice) {
            case 1:
                int choiceC;
                system("cls");
                cout << "1.Za³ó¿ konto w PLN \n2.Za³ó¿ konto w EUR\n3.Za³ó¿ konto w CHF\n4.Za³ó¿ konto w GBP \n";
                cin >> choiceC;
                switch (choiceC) {
                case 1:
                    accounts.createAccount(loggedInUserId, 1);
                    break;

                case 2:
                    accounts.createAccount(loggedInUserId, 2);
                    break;
                case 3:
                    accounts.createAccount(loggedInUserId, 3);
                    break;

                case 4:
                    accounts.createAccount(loggedInUserId, 4);
                    break;
                default:
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
                loggedInAccount = false;
                break;
            case 6:
                accounts.transferMoney(loggedInAccountId);
                break;
            case 7:
                system("cls");
                cout << "Czy napewno chcesz usunac konto? Wprowadz podana liczbe aby potwierdziæ: " << loggedInUserId;
                cin >> acc;
                if (acc == loggedInUserId)
                    user.deleteUser(loggedInUserId, loggedInAccountId, loggedInAccount, loggedIn);
                else
                    cout << "Wprowadzono niepoprawn¹ liczbe!";

                break;
            case 8:
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