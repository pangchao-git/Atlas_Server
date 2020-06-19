#pragma once
#include <string>

typedef struct sqlite3 sqlite3;
typedef struct sqlite3_stmt sqlite3_stmt;

//创建表,增，删，该
bool DoSqlCommandNonReturn(sqlite3* conn, const char *sql);
bool DoSqlCommandANSIToUTF8(sqlite3* conn, std::string sql);
bool BeginTransaction(sqlite3* conn);
bool EndTransaction(sqlite3* conn, bool sqlResult);

void SAFE_CLOSE(sqlite3* conn);
void SAFE_FINALIZE(sqlite3_stmt* stmt);

#define DO_SQL_CMD(sql)  DoSqlCommandNonReturn(conn_, sql)
#define BEGIN_TRANSACTION() BeginTransaction(conn_)
#define END_TRANSACTION(sqlResult) EndTransaction(conn_, sqlResult)



