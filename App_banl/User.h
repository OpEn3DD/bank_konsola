#pragma once
#include "DbManager.h"
class Account
{
private:
    DbManager& dbManager;
    sqlite3_stmt* stmt;

public:
    Account(DbManager& dbManager);
    void techniczna(int& accountId);
    void createAccount();
    bool login(int& accountId);
    void showBalance(int accountId);
    bool isPasswordRight(string password);
    bool isPeselRight(string pesel);
    bool deleteAccount();
    void wplacSrodki(int accountId, float ammount);
    void wyplacSrodki(int accountId, float ammount);

};

