#include "DbManager.h"
using namespace std;

DbManager::DbManager(const string& dbName) {
    rc = sqlite3_open(dbName.c_str(), &db);
    if (rc) {
        cout << "Blad polaczenia z baza danych: " << sqlite3_errmsg(db) << endl;
        exit(1);
    }
}

DbManager::~DbManager() {
    sqlite3_close(db);
}

void DbManager::executeSQL(const char* sql) {
    rc = sqlite3_exec(db, sql, 0, 0, 0);
    if (rc) {
        cerr << "SQL error: " << sqlite3_errmsg(db) << endl;
        exit(1);
    }
}

sqlite3* DbManager::getDB() const {
    return db;
}
