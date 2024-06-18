#include "Currency.h"

Currency::Currency(DbManager& dbManager) : dbManager(dbManager) {
    const char* sql =
        "CREATE TABLE IF NOT EXISTS Currency ("
        "id TEXT PRIMARY KEY NOT NULL,  "
        "currencyName TEXT NOT NULL), "
        "valueToPln FLOAT NOT NULL; ";
    dbManager.executeSQL(sql);

    const char* sql2 =
        "INSERT INTO Currency (id, currencyName, valueToPln) VALUES ('PLN', 'zlotowki', 1); ";
        "INSERT INTO Currency (id, currencyName, valueToPln) VALUES ('EUR', 'euro', 4.30); "
        "INSERT INTO Currency (id, currencyName, valueToPln) VALUES ('CHF', 'zlotowki', 4.55); "
        "INSERT INTO Currency (id, currencyName, valueToPln) VALUES ('GBP', 'funt brytyjski', 5,10); ";
    dbManager.executeSQL(sql2);
};
