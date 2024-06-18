#pragma once
#include "DbManager.h"
#include "Accounts.h"
#include "Currency.h"
#include "User.h"
class BankApp
{
private:
    DbManager dbManager;
    User user;
    Accounts accounts;
    int loggedInAccountId;
    bool loggedIn;
    bool loggedInAccount;
public:
    int loggedInUserId;
    BankApp(const std::string& dbName);
    void run();
    int selectAccount(int& userId);
};


