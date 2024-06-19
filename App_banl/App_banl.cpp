#include "sqlite3.h"
#include <iostream>
#include <conio.h> 
#include <windows.h>
#include <string>
#include "DbManager.h"
#include "User.h"
#include "Accounts.h"
#include "BankApp.h"

using namespace std;


int main() {
    setlocale(LC_ALL, "polish"); // wyswietlanie polskich znaków

    BankApp app("bank.db");
    app.run();
    return 0;
}


