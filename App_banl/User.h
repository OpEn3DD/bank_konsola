#pragma once
#include "DbManager.h"
class User
{
private:
    DbManager& dbManager;
    sqlite3_stmt* stmt;

public:
    User(DbManager& dbManager);
    void techniczna(int& UserId);
    void createUser();
    bool login(int& UserId);
    void showBalance(int UserId);
    bool isPasswordRight(string password);
    bool isPeselRight(string pesel);
    bool isEmailRight(string email);
    bool deleteUser();
    void wplacSrodki(int Id, float ammount);
    void wyplacSrodki(int UserId, float ammount);

};

