#include "teacherwindow1.h"
#include "ui_teacherwindow1.h"
#include "mainwindow.h"
#include "teacherdashboard.h"
#include <QLabel>
#include <QMessageBox>
#include <QSqlQuery>
#include <QSqlError>
#include <QString>

SecDialog::SecDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::SecDialog),db(new Database())
{
    ui->setupUi(this);

    if(!db->connectionOpen()) {
        ui->status->setText("Unable to open database");
        qWarning() << "Database error: " << mydb.lastError().text();
    } else {
        ui->status->setText("Connected...");
    }

    this->resize(800, 600);
    setWindowTitle("TeacherLogin");

    connect(ui->ok, &QPushButton::clicked, this, &SecDialog::okbutton);
    connect(ui->backbutton, &QPushButton::clicked, this, &SecDialog::backbutton);
    connect(ui->changePassword, &QPushButton::clicked, this, &SecDialog::changePassword);

    ui->stackedWidget->setCurrentWidget(ui->mainPage);
    ui->userEdit2->setPlaceholderText("Enter Username");
    ui->passwordEdit3->setPlaceholderText("Enter old Password");
    ui->passwordEdit2->setPlaceholderText("New Password");
}

SecDialog::~SecDialog()
{
    delete ui;
}



void SecDialog::okbutton()
{
    QString username = ui->userEdit->text();
    QString password = ui->passwordEdit->text();

    if(!db->connectionOpen()){
        qDebug() << "Failed to open database";
        return;
    }

    QSqlQuery qry;
    qry.prepare("SELECT * FROM Teacher WHERE userName = :username AND passWord = :password");
    qry.bindValue(":username", username);
    qry.bindValue(":password", password);

    if(qry.exec())
    {
        int count = 0;
        while(qry.next())
        {
            count++;
        }

        if(count == 1){
            ui->status->setText("Login successful");
            db->connectionClose();
            hide();

            TeacherDashBoard = new teacherdashboard(this, username);
            TeacherDashBoard->showMaximized();
        }
        else if(count > 1){
            ui->status->setText("Duplicate username or password");
        }
        else{
            ui->status->setText("Incorrect username or password");
            ui->passwordEdit->setText("");
        }
    }
    else
    {
        qDebug() << "Query error: " << qry.lastError().text();
    }

    db->connectionClose();
}

void SecDialog::backbutton()
{
    MainWindow *mainw = new MainWindow(this);
    hide();
    mainw->show();
}

void SecDialog::on_passwordButton_clicked()
{
    ui->stackedWidget->setCurrentWidget(ui->passwordPage);
}

bool SecDialog::containsNumber(const QString &str)
{
    for (QChar c : str) {
        if (c.isDigit()) {
            return true;
        }
    }
    return false;
}

void SecDialog::changePassword()
{
    QString username = ui->userEdit2->text();
    QString password = ui->passwordEdit2->text();
    QString oldpassword = ui->passwordEdit3->text();

    if(username.isEmpty())
    {
        QMessageBox::critical(this,"Error", "Username cannot be empty");
        return;
    }

    if (!containsNumber(password)) {
        QMessageBox::critical(this, "Error", "Password should contain a number");
        return;
    }

    if(!db->connectionOpen()){
        qDebug() << "Failed to open database";
        return;
    }

    QSqlQuery qry;
    qry.prepare("SELECT * FROM Teacher WHERE userName = :username AND passWord = :oldpassword");
    qry.bindValue(":username", username);
    qry.bindValue(":oldpassword", oldpassword);

    if(qry.exec())
    {
        int count = 0;
        while(qry.next())
        {
            count++;
        }

        if(count == 1){
            qry.prepare("UPDATE Teacher SET passWord = :password WHERE userName = :username");
            qry.bindValue(":password", password);
            qry.bindValue(":username", username);

            if (!qry.exec()) {
                QMessageBox::information(this, "Error", "Failed to change password");
                qDebug() << "Query error: " << qry.lastError().text();
            } else {
                QMessageBox::information(this, "Saved", "Password changed successfully");
            }
        }
        else{
            QMessageBox::information(this,"Error","Incorrect old password");
        }
    }
    else
    {
        qDebug() << "Query error: " << qry.lastError().text();
    }

    db->connectionClose();
}

void SecDialog::on_backButton_clicked()
{
    ui->stackedWidget->setCurrentWidget(ui->mainPage);
}
