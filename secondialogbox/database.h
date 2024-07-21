#ifndef DATABASE_H
#define DATABASE_H

#include <QSqlDatabase>
#include <QSqlError>
#include <QDebug>

class Database
{
public:
    Database();
    void connectionClose();
    bool connectionOpen();
    QSqlDatabase mydb;

};

#endif // DATABASE_H
