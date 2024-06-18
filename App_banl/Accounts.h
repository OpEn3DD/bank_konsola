#pragma once
#include "DbManager.h"

class Accounts
{
private:
    DbManager& dbManager;
    sqlite3_stmt* stmt;
public:
    Accounts(DbManager& dbManager);
    void showBalance(int loggedInAccountId);
    void depositMoney(int Id, float ammount);
    void withdrawMoney(int AccountId, float ammount);
    void createAccount(int userId, string currencyId);
    bool loginAccount(int userId, int& AccountId);
    int selectAccount(int userId);
    void transferMoney(int AccountId);
};

