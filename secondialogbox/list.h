#ifndef LIST_H
#define LIST_H

#include "database.h"
#include <QSqlQuery>
#include <QDate>


class List
{
public:
    List();

    Database *db;

    QStringList assignmentDateList, internalDateList;

    QStringList updatedAssignmentDateList, updatedInternalDateList;

    void assignmentDates();

    void internalDates();

    void updatedAssignmentDates();

    void updatedInternalDates();


};

#endif // LIST_H
