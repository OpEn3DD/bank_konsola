#pragma once
#include "sqlite3.h"
#include <string>
#include <iostream>

using namespace std;
class DbManager
{
private:
    sqlite3* db;
    int rc;
public:
    DbManager(const string& dbName);
    void executeSQL(const char* sql);
    sqlite3* getDB() const;
    ~DbManager();


};

