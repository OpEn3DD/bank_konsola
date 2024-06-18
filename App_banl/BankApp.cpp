#include "BankApp.h"
#include <string>
BankApp::BankApp(const std::string& dbName) :
    dbManager(dbName),
    user(dbManager),
    accounts(dbManager),
    loggedInUserId(-1),
    loggedInAccountId(-1)
{};
void BankApp::run() {
    int choice{};
    int ammount{};

    while (true) {
        if (!loggedIn) {

            cout << "1. Za³ó¿ konto u¿ytkownika\n2. Zaloguj\n7. Exit\nWybierz opcje: ";
            cin >> choice;

            switch (choice) {
            case 1:
                system("cls");
                if (!loggedIn) user.createUser();
                else cout << "Nie mozesz utworzyæ konta uzytkownika bêd¹c zalogowanym" << endl;
                break;
            case 2:
                system("cls");
                loggedIn = user.login(loggedInUserId);
                break;
            case 7:
                return;
            default:
                system("cls");
                cout << "Invalid option. Please try again." << endl;
                std::cin.clear(); // Resetuje flagi b³êdów
                std::cin.ignore(12343234, '\n');//Czysczenie strumienia wejscia
                break;
            }
        }
        else if (loggedIn) {

            cout << "1.Za³ó¿ nowe konto bankowe \n2.Poka¿ bilans konta\n3.Wp³aæ œrodki\n4.Wyplac srodki\n7.Exit\nChoose an option : ";
            cin >> choice;

            switch (choice) {
            case 1:
                system("cls");
                if (loggedIn) accounts.createAccount(loggedInUserId, "PLN", loggedInAccountId);
                else cout << "Najpierw musisz siê zalogowac" << endl;
                break;
            case 2:
                system("cls");
                if (loggedIn) accounts.showBalance(loggedInAccountId);
                else cout << "Najpierw musisz siê zalogowac" << endl;
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
            case 7:
                return;
            case 99:
                user.techniczna(loggedInUserId);
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