#ifndef STUDENTWINDOW1_H
#define STUDENTWINDOW1_H

#include <QDialog>
#include <QtSql>
#include "studentdashboard.h"

#include"database.h"


namespace Ui {
class studentwindow1;
}

class studentwindow1 : public QDialog
{
    Q_OBJECT


public:
    explicit studentwindow1(QWidget *parent = nullptr);
    ~studentwindow1();

private slots:
    void okbutton();

    void backbutton();

    void on_backButton_clicked();

    bool containsNumber(const QString &str);

    void on_changepasswordButton_clicked();

    void changePassword();

    void on_showButton_clicked();

    void on_showButton_2_clicked();

private:
    Ui::studentwindow1 *ui;
    QSqlDatabase mydb;
    Database *db;
    studentdashboard *StudentDashBoard;
};

#endif // STUDENTWINDOW1_H
