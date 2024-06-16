#include "sqlite3.h"
#include <iostream>
#include <conio.h> 
#include <windows.h>
#include <string>
#include "DbManager.h"
#include "User.h"
using namespace std;

class BankApp {
public:
private:
    DbManager dbManager;
    User user;
    int loggedInUserId;
    bool loggedIn;
public:
    BankApp(const string& dbName) :
        dbManager(dbName),
        user(dbManager),

        loggedInUserId(-1) {}

    void run() {
        int choice{};
        int ammount{};

        while (true) {
            if(!loggedIn)
                cout << "1. Załóż konto\n2. Zaloguj\n7. Exit\nWybierz opcje: ";
            else {

                cout << "3.Pokaż bilans konta\n4.Wpłać środki\n5.Wyplac srodki\n7.Exit\nChoose an option : ";
            }

            cin >> choice;

            switch (choice) {
            case 1:
                system("cls");
                user.createUser();
                break;
            case 2:
                system("cls");
                loggedIn = user.login(loggedInUserId);
                break;
            case 3:
                system("cls");
                if (loggedIn) user.showBalance(loggedInUserId);
                else cout << "Najpierw musisz się zalogowac" << endl;
                break;
            case 4:
                system("cls");
                if (loggedIn) {
                    cout << "Podaj wpłacaną kwote: ";
                    cin >> ammount;
                    user.wplacSrodki(loggedInUserId, ammount);
                }
                else cout << "Najpierw musisz się zalogowac" << endl;
                break;
            case 5:
                system("cls");
                if (loggedIn) {
                    cout << "Podaj wypłacaną kwote: ";
                    cin >> ammount;
                    user.wyplacSrodki(loggedInUserId, ammount);
                }
                else cout << "Najpierw musisz się zalogowac" << endl;
                break;
            case 7:
                return;
            case 99:
                user.techniczna(loggedInUserId);
                break;

            default:
                system("cls");
                cout << "Invalid option. Please try again." << endl;
                std::cin.clear(); // Resetuje flagi błędów
                std::cin.ignore(12343234, '\n');
                break;
            }
        }
    }


};

int main() {
    setlocale(LC_ALL, "polish"); // wyswietlanie polskich znaków

    BankApp app("bank.db");
    app.run();
    return 0;
}


// Uruchomienie programu: Ctrl + F5 lub menu Debugowanie > Uruchom bez debugowania
// Debugowanie programu: F5 lub menu Debugowanie > Rozpocznij debugowanie

// Porady dotyczące rozpoczynania pracy:
//   1. Użyj okna Eksploratora rozwiązań, aby dodać pliki i zarządzać nimi
//   2. Użyj okna programu Team Explorer, aby nawiązać połączenie z kontrolą źródła
//   3. Użyj okna Dane wyjściowe, aby sprawdzić dane wyjściowe kompilacji i inne komunikaty
//   4. Użyj okna Lista błędów, aby zobaczyć błędy
//   5. Wybierz pozycję Projekt > Dodaj nowy element, aby utworzyć nowe pliki kodu, lub wybierz pozycję Projekt > Dodaj istniejący element, aby dodać istniejące pliku kodu do projektu
//   6. Aby w przyszłości ponownie otworzyć ten projekt, przejdź do pozycji Plik > Otwórz > Projekt i wybierz plik sln
