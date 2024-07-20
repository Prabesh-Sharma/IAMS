#ifndef TEACHERDASHBOARD_H
#define TEACHERDASHBOARD_H

#include <QDialog>
#include <QSqlDatabase>
#include <QMessageBox>

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
    void highlightInternalDatesOnCalender();
    void highlightAssignmentDatesOnCalender();
    void on_internalUpdate_clicked();
    void on_internalAdd_clicked();
    void on_internalDelete_clicked();
    void on_logOut_clicked();
    bool checkInternalTime(QDate &selectedDate);
    bool matchcode(const QString &username, const QString &code);
    bool getAllDates(const QString &dateString);
    bool getAllAssignmentDates(const QString &dateString);
    QStringList checkExamDate(QStringList &dateList);
    QStringList checkAssignmentDate(QStringList &dateList);
    void connectionClose();
    bool connectionOpen();
    void showAvailableInternalDates();
    void showAvailableAssignmentDates();
    void on_assignmentAddButton_clicked();
    void on_assignmentupdateButton_clicked();
    void on_assignmentDeleteButton_clicked();

private:
    Ui::teacherdashboard *ui;
    QString t_username;
    QSqlDatabase mydb;



};

#endif // TEACHERDASHBOARD_H
