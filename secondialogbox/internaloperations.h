#ifndef INTERNALOPERATIONS_H
#define INTERNALOPERATIONS_H

#include "ui_teacherdashboard.h"
#include "database.h"

#include <QString>
#include <QDebug>
#include <QCalendarWidget>
#include <QSqlQuery>
#include <QSqlError>
#include <QTableWidget>

class InternalOperations
{
public:
    InternalOperations(Ui::teacherdashboard *ui);
    bool getAllInternalDates(const QString &dateString);
    void highlightInternalDatesOnCalender();
    void showAvailableInternalDates();
    QStringList checkInternalDate(QStringList &dateList);


private:

    QSqlDatabase mydb;
    Ui::teacherdashboard *ui;
    Database *db;
    QStringList internalDateList;


};

#endif // INTERNALOPERATIONS_H
