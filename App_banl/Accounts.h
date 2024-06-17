#pragma once
#include "DbManager.h"

class Accounts
{
private:
    DbManager& dbManager;
    sqlite3_stmt* stmt;
public:
    Accounts(DbManager& dbManager);
    void showBalance(int AccountId);
    void depositMoney(int Id, float ammount);
    void withdrawMoney(int AccountId, float ammount);
    void createAccount(int userId, string currencyId, int& AccountId);
};

