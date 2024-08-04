#ifndef TEACHERWINDOW1_H
#define TEACHERWINDOW1_H

#include "database.h"

#include <QDialog>
#include <QSqlDatabase>
#include <QSqlError>
#include <QDebug>

class teacherdashboard;

namespace Ui {
class SecDialog;
}

class SecDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SecDialog(QWidget *parent = nullptr);
    ~SecDialog();

private slots:
    void okbutton();

    void backbutton();

    void on_passwordButton_clicked();

    void changePassword();

    bool containsNumber(const QString &str);

    void on_backButton_clicked();


    void on_showButton_clicked();

private:
    Ui::SecDialog *ui;
    teacherdashboard *TeacherDashBoard;
    QSqlDatabase mydb;
    Database *db;

};

#endif // TEACHERWINDOW1_H
