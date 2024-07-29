#ifndef ASSIGNMENTOPERATIONS_H
#define ASSIGNMENTOPERATIONS_H

#include "ui_teacherdashboard.h"
#include "database.h"
#include "list.h"


#include<QString>
#include<QDebug>
#include<QCalendarWidget>
#include<QSqlQuery>
#include<QSqlError>
#include<QTableWidget>
#include<QMessageBox>
#include <QMap>
#include<QAbstractButton>
#include<QToolTip>

class AssignmentOperations: public QObject
{
     Q_OBJECT

public:

    AssignmentOperations(Ui::teacherdashboard *ui);

    bool getAllAssignmentDates(const QString &dateString);

    void highlightAssignmentDatesOnCalender();

    void showAvailableAssignmentDates();

    void getInternalDateList();

    void getNotes();

    void showNoteForSelectedDate();

    void mousePressEvent(QMouseEvent *event);

private:
    QSqlDatabase mydb;

    Ui::teacherdashboard *ui;

    Database *db;

    List *list;

    QStringList internalDateList;

    QList<QMap<QDate, QString>> notesMap;

};


#endif // ASSIGNMENTOPERATIONS_H
