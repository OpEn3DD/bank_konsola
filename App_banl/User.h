#pragma once
#include "DbManager.h"
class User
{
private:
    DbManager& dbManager;
    sqlite3_stmt* stmt;

public:
    User(DbManager& dbManager);
    void techniczna();
    void createUser(int& loggedInAccountId);
    bool login(int& UserId);
    bool isPasswordRight(string password);
    bool isPeselRight(string pesel);
    bool isEmailRight(string email);
    void deleteUser(int& userId, int& loggedInAccountId, bool& loggedInAccount, bool& loggedIn);
};

