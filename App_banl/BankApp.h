#pragma once
#include "DbManager.h"
#include "Accounts.h"
#include "User.h"
class BankApp
{
private:
    DbManager dbManager;
    User user;
    Accounts accounts;
    int loggedInUserId;
    int loggedInAccountId;
    bool loggedIn;
    bool loggedInAccount;
public:
    BankApp(const std::string& dbName);
    void run();
};


