#ifndef ASSIGNMENTOPERATIONS_H
#define ASSIGNMENTOPERATIONS_H

#include "ui_teacherdashboard.h"
#include "database.h"
#include "internaloperations.h"

#include<QString>
#include<QDebug>
#include<QCalendarWidget>
#include<QSqlQuery>
#include<QSqlError>
#include<QTableWidget>

class AssignmentOperations
{
public:

    AssignmentOperations(Ui::teacherdashboard *ui);

    bool getAllAssignmentDates(const QString &dateString);

    void highlightAssignmentDatesOnCalender();

    void showAvailableAssignmentDates();

    QStringList checkAssignmentDate(QStringList &dateList);

    void getInternalDateList();

private:
    QSqlDatabase mydb;

    Ui::teacherdashboard *ui;

    Database *db;

    QStringList internalDateList;



};


#endif // ASSIGNMENTOPERATIONS_H
