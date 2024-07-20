#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include"teacherwindow1.h"
#include"studentwindow1.h"
#include <QMainWindow>
#include<QSqlQuery>
QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void studentButton();
    void teacherButton();


private:
    Ui::MainWindow *ui;
    QPushButton *teacher_button,*student_button;
    SecDialog *teacherWindow1;
    studentwindow1 *studentWindow1;

};
#endif // MAINWINDOW_H
