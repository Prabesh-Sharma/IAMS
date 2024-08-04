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
    this->resize(800,600);


    QLabel *iams_logo = new QLabel(this);
    QPixmap picture(":/images/images/Black Illustrative Education Logo.png");
    iams_logo->setGeometry(300,0,200,200);
    int width = iams_logo->width();
    int height = iams_logo->height();
    iams_logo->setPixmap(picture.scaled(width,height,Qt::KeepAspectRatio));



    QFont font1;
    font1.setPointSize(15);
    QLabel *welcome_display = new QLabel("Welcome to IAMS",this);
    welcome_display->setGeometry(300,250,200,30);
    welcome_display->setFont(font1);
    welcome_display->setAlignment(Qt::AlignCenter);

    QFont font2;
    font2.setPointSize(15);
    QLabel *role_choose = new QLabel("Choose your role",this);
    role_choose->setGeometry(300,350,200,30);
    role_choose->setFont(font2);
    role_choose->setAlignment(Qt::AlignCenter);


    QFont font3;
    font3.setPointSize(10);
    teacher_button = new QPushButton("Teacher",this);
    teacher_button->setGeometry(QRect(QPoint(300,400),QSize(100,100)));
    teacher_button->setFont(font3);

    QFont font4;
    font4.setPointSize(10);
    student_button = new QPushButton("Student",this);
    student_button->setGeometry(QRect(QPoint(400,400),QSize(100,100)));
    student_button->setFont(font4);

    connect(teacher_button, &QPushButton::clicked, this, &MainWindow::teacherButton);
    connect(student_button, &QPushButton::clicked, this, &MainWindow::studentButton);


}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::studentButton()
{

    studentWindow1 = new studentwindow1(this);
    hide();
    studentWindow1->showMaximized();
}
void MainWindow::teacherButton()
{
    qDebug()<<"Teacher Dialog is opened";

    teacherWindow1 = new SecDialog(this);
    hide();
    teacherWindow1->showMaximized();
}
