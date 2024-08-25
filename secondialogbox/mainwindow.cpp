#include "mainwindow.h"
#include "ui_mainwindow.h"
#include<QPixmap>
#include<Qlabel>
#include<QPushButton>
#include<QFont>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent) , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle("IAMS");
    this->showMaximized();

}


void MainWindow::on_student_button_clicked()
{
    studentWindow1 = new studentwindow1(this);
    hide();
    studentWindow1->showMaximized();

}


void MainWindow::on_teacher_button_clicked()
{
    teacherWindow1 = new SecDialog(this);
    hide();
    teacherWindow1->showMaximized();
}

MainWindow::~MainWindow()
{
    delete ui;
    delete studentWindow1;
    delete teacherWindow1;
}
