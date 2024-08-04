#include "studentwindow1.h"
#include "ui_studentwindow1.h"
#include <QMessageBox>
#include <QDebug>
#include "mainwindow.h"

studentwindow1::studentwindow1(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::studentwindow1),db(new Database)
{
    ui->setupUi(this);

    if(!db->connectionOpen()) {
        ui->status->setText("Unable to open database");
        qWarning() << "Database error: " << mydb.lastError().text();
    } else {
        ui->status->setText("Connected...");
    }

    this->resize(800, 600);
    setWindowTitle("StudentLogin");

    connect(ui->ok, &QPushButton::clicked, this, &studentwindow1::okbutton);
    connect(ui->backbutton, &QPushButton::clicked, this, &studentwindow1::backbutton);
    connect(ui->changeButton, &QPushButton::clicked, this, &studentwindow1::changePassword);


    ui->stackedWidget->setCurrentWidget(ui->mainPage);
    ui->userEdit2->setPlaceholderText("Enter Username");
    ui->passwordEdit3->setPlaceholderText("Enter old Password");
    ui->passwordEdit2->setPlaceholderText("Enter new Password");

}

studentwindow1::~studentwindow1()
{
    delete ui;
    if (mydb.isOpen()) {
        mydb.close();
    }
}

void studentwindow1::okbutton()
{
    QString username = ui->userEdit->text();
    QString password = ui->passwordEdit->text();

    if (!db->connectionOpen()) {
        qDebug() << "Failed to open database";
        return;
    }

    QSqlQuery qry;
    qry.prepare("SELECT * FROM Student WHERE userName = :username AND password = :password");
    qry.bindValue(":username", username);
    qry.bindValue(":password", password);

    if (qry.exec()) {
        int count = 0;
        while (qry.next()) {
            count++;
        }

        qDebug() << "Count: " << count;

        if (count == 1) {
            ui->status->setText("Login successful");
            hide();
            StudentDashBoard = new studentdashboard(this);
            StudentDashBoard->show();
        } else if (count > 1) {
            ui->status->setText("Duplicate username or password");
        } else {
            ui->status->setText("Incorrect username or password");
        }
    } else {
        qDebug() << "Query execution error: " << qry.lastError().text();
    }

    db->connectionClose();
}

void studentwindow1::backbutton()
{
    MainWindow *mainw = new MainWindow(this);
    hide();
    mainw->show();
}

void studentwindow1::on_changepasswordButton_clicked()
{
    ui->stackedWidget->setCurrentWidget(ui->passwordPage);
}

bool studentwindow1::containsNumber(const QString &str)
{
    for (QChar c : str) {
        if (c.isDigit()) {
            return true;
        }
    }
    return false;
}

void studentwindow1::on_backButton_clicked()
{
    ui->stackedWidget->setCurrentWidget(ui->mainPage);
}


void studentwindow1::changePassword()
{

    QString username = ui->userEdit2->text();
    QString password = ui->passwordEdit2->text();
    QString oldpassword = ui->passwordEdit3->text();

    if (username.isEmpty()) {
        QMessageBox::critical(this, "Error", "Username cannot be empty");
        return;
    }

    if (!db->connectionOpen()) {
        qDebug() << "Failed to open database";
        return;
    }

    QSqlQuery qry;
    qry.prepare("SELECT * FROM Student WHERE userName = :username AND password = :oldpassword");
    qry.bindValue(":username", username);
    qry.bindValue(":oldpassword", oldpassword);

    if (qry.exec()) {
        int count = 0;
        while (qry.next()) {
            count++;
        }

        if (count == 1) {
            qry.prepare("UPDATE Student SET password = :password WHERE userName = :username");
            qry.bindValue(":password", password);
            qry.bindValue(":username", username);

            if (!containsNumber(password)) {
                QMessageBox::critical(this, "Error", "Password should contain a number");
                return;
            }
            if (!qry.exec()) {
                QMessageBox::information(this, "Error", "Failed to change password");
                qDebug() << "Query error: " << qry.lastError().text();
            } else {
                QMessageBox::information(this, "Saved", "Password changed successfully");
            }
        } else {
            QMessageBox::information(this, "Error", "Incorrect old password");
        }
    } else {
        qDebug() << "Query error: " << qry.lastError().text();
    }

    db->connectionClose();
}


void studentwindow1::on_showButton_clicked()
{
    if(ui->showButton->text() == "hide"){


        ui->passwordEdit->setEchoMode(QLineEdit::Password);
        ui->showButton->setText("show");

    }


    else
    {

        ui->passwordEdit->setEchoMode(QLineEdit::Normal);
        ui->showButton->setText("hide");

    }

}

