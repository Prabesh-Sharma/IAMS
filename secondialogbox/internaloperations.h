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
#include <QMessageBox>
#include <QMap>
#include <QAbstractButton>
#include <QToolTip>
#include <QMouseEvent>

class InternalOperations : public QObject
{
    Q_OBJECT

public:
    InternalOperations(Ui::teacherdashboard *ui);

    bool getAllInternalDates(const QString &dateString);
    void highlightInternalDatesOnCalender();
    void showAvailableInternalDates();
    QStringList checkInternalDate(QStringList &dateList);
    void getAllAssignmentDateList();
    void getNotes();
    void showNoteForSelectedDate();

    void mousePressEvent(QMouseEvent *event);

private:
    QSqlDatabase mydb;
    Ui::teacherdashboard *ui;
    Database *db;
    QStringList assignmentDateList;
    QList<QMap<QDate, QString>> notesMap;
};

#endif // INTERNALOPERATIONS_H
