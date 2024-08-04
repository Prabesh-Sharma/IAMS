#include "database.h"

Database::Database()
{

}

bool Database::connectionOpen()
{
    if (QSqlDatabase::contains("qt_sql_default_connection")) {
        mydb = QSqlDatabase::database("qt_sql_default_connection");
    } else {
        mydb = QSqlDatabase::addDatabase("QSQLITE");
        mydb.setDatabaseName("C:\\Users\\A S U S\\Desktop\\IAMS\\database\\iamsdata.db");
    }

    if (!mydb.open()) {
        qDebug() << "Database error: " << mydb.lastError().text();
        return false;
    } else {
        return true;
    }
}

void Database::connectionClose()
{
    if (mydb.isOpen()) {
        mydb.close();
    }
}
