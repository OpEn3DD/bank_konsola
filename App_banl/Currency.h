#pragma once
#include "DbManager.h"
class Currency
{
private:
    DbManager& dbManager;
    sqlite3_stmt* stmt;
public:
    Currency(DbManager& dbManager);
};