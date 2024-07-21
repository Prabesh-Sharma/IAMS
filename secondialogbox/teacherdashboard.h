#ifndef TEACHERDASHBOARD_H
#define TEACHERDASHBOARD_H

#include <QDialog>
#include <QSqlDatabase>
#include <QMessageBox>

#include "assignmentoperations.h"
#include "internaloperations.h"
#include "database.h"


QT_BEGIN_NAMESPACE
namespace Ui { class teacherdashboard; }
QT_END_NAMESPACE

class teacherdashboard : public QDialog
{
    Q_OBJECT

public:
    teacherdashboard(QWidget *parent, const QString &username);
    ~teacherdashboard();

private slots:
    void showHomePage();
    void showAssignmentPage();
    void showInternalPage();
    void on_internalUpdate_clicked();
    void on_internalAdd_clicked();
    void on_internalDelete_clicked();
    void on_logOut_clicked();
    bool checkTime(QDate &selectedDate);
    bool matchcode(const QString &username, const QString &code);
    void on_assignmentAddButton_clicked();
    void on_assignmentupdateButton_clicked();
    void on_assignmentDeleteButton_clicked();

private:
    Ui::teacherdashboard *ui;
    QString t_username;
    QSqlDatabase mydb;
    AssignmentOperations *assignmentOps;
    InternalOperations *internalOps;
    Database *db;


};

#endif // TEACHERDASHBOARD_H
