#ifndef STUDENTDASHBOARD_H
#define STUDENTDASHBOARD_H

#include"list.h"

#include <QDialog>
#include <QtSql>
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
#include<QTextCharFormat>

#include"database.h"

namespace Ui {
class studentdashboard;
}

class studentdashboard : public QDialog
{
    Q_OBJECT

public:
    explicit studentdashboard(QWidget *parent = nullptr);
    ~studentdashboard();
private slots:

    void on_logOutButton_clicked();

    void on_homeButton_clicked();

    void on_assignmentButton_clicked();

    void on_internalButton_clicked();

    void getNotesAssignment();

    void showNoteForSelectedDateAssignment();

    void showNoteForSelectedDateInternal();

    void getNotesInternal();

    void highlightDatesOnCalender();


private:
    Ui::studentdashboard *ui;

    QSqlDatabase mydb;

    Database *db;

    List *list;

    QList<QMap<QDate, QString>> notesMap;

};

#endif // STUDENTDASHBOARD_H
