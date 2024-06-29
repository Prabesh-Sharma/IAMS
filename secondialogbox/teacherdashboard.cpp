#include "teacherdashboard.h"
#include "ui_teacherdashboard.h"
#include <QSqlQuery>
#include <QMessageBox>
#include <QDebug>
#include"teacherwindow1.h"
teacherdashboard::teacherdashboard(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::teacherdashboard)
{
    ui->setupUi(this);
    this->resize(800, 600);
    setWindowTitle("TeacherDashBoard");

    connect(ui->homeButton, &QPushButton::clicked, this, &teacherdashboard::showHomePage);
    connect(ui->assignmentButton, &QPushButton::clicked, this, &teacherdashboard::showAssignmentPage);
    connect(ui->internalButton, &QPushButton::clicked, this, &teacherdashboard::showInternalPage);

    if (!connectionOpen()) {
        ui->status->setText("Unable to open database");
    } else {
        ui->status->setText("Connected...");
    }
}

teacherdashboard::~teacherdashboard()
{
    connectionClose();
    delete ui;
}

bool teacherdashboard::connectionOpen()
{
    if (QSqlDatabase::contains("qt_sql_default_connection")) {
        mydb = QSqlDatabase::database("qt_sql_default_connection");
    } else {
        mydb = QSqlDatabase::addDatabase("QSQLITE");
        mydb.setDatabaseName("C:/Users/A S U S/Desktop/IAMS/database/iamsdata.db");
    }

    if (!mydb.open()) {
        qDebug() << "Database error: " << mydb.lastError().text();
        return false;
    } else {
        return true;
    }
}

void teacherdashboard::connectionClose()
{
    if (mydb.isOpen()) {
        mydb.close();
    }
}

void teacherdashboard::showHomePage() {
    ui->stackedWidget->setCurrentWidget(ui->Home);
}

void teacherdashboard::showAssignmentPage() {
    ui->stackedWidget->setCurrentWidget(ui->Assignment);
}

void teacherdashboard::showInternalPage() {
    ui->stackedWidget->setCurrentWidget(ui->Internal);
}

void teacherdashboard::on_internalUpdate_clicked()
{


        QString block = ui->blockEdit->text();
        QString room = ui->roomEdit->text();
        QString code = ui->courseEdit->text();
        QTime selectedTime = ui->timeEdit->time();
        QString timeString = selectedTime.toString("HH:mm:ss");
        QDate selectedDate = ui->dateEdit->date();
        QString dateString = selectedDate.toString("dd-MM-yyyy");

        if (code.isEmpty()) {
            QMessageBox::information(this, "Error", "Course Code is required to update the record.");
            return;
        }

        if (!connectionOpen()) {
            qDebug() << "Failed to open database";
            return;
        }

        QSqlQuery qry;
        qry.prepare("UPDATE Exam SET Block = :block, RoomNo = :room, Time = :time, Date = :date WHERE Course_Code = :code");
        qry.bindValue(":block", block);
        qry.bindValue(":room", room);
        qry.bindValue(":time", timeString);
        qry.bindValue(":date", dateString);
        qry.bindValue(":code", code);

        if (qry.exec()) {
            QMessageBox::information(this, "Updated", "Data has been updated successfully.");
        } else {
            QMessageBox::information(this, "Error", "Failed to update data.");
            qDebug() << "Query execution error: " << qry.lastError().text();
        }

        connectionClose();

}

void teacherdashboard::on_internalAdd_clicked()
{
    QString block = ui->blockEdit->text();
    QString room = ui->roomEdit->text();
    QString code = ui->courseEdit->text();
    QTime selectedTime = ui->timeEdit->time();
    QString timeString = selectedTime.toString("HH:mm:ss");
    QDate selectedDate = ui->dateEdit->date();
    QString dateString = selectedDate.toString("dd-MM-yyyy");
    if (!connectionOpen()) {
        qDebug() << "Failed to open database";
        return;
    }

    QSqlQuery qry;
    qry.prepare("INSERT INTO Exam (Course_Code, Block, RoomNo, Time, Date) VALUES (:code, :block, :room, :time, :date)");
    qry.bindValue(":code", code);
    qry.bindValue(":block", block);
    qry.bindValue(":room", room);
    qry.bindValue(":time", timeString);
    qry.bindValue(":date", dateString);

    if (qry.exec()) {
        QMessageBox::information(this, "Saved", "Data has been saved successfully.");
    } else {
        QMessageBox::information(this, "Error", "Failed to save data.");
        qDebug() << "Query execution error: " << qry.lastError().text();
    }

    connectionClose();
}

void teacherdashboard::logOut()
{
    SecDialog *teacherlogin = new SecDialog;
    hide();
    teacherlogin->show();
}

void teacherdashboard::on_internalDelete_clicked()
{
    QString code = ui->courseEdit->text();

    if (code.isEmpty()) {
        QMessageBox::information(this, "Error", "Please enter a Course Code to delete.");
        return;
    }

    if (!connectionOpen()) {
        qDebug() << "Failed to open database";
        return;
    }

    QSqlQuery qry;
    qry.prepare("DELETE FROM Exam WHERE Course_Code = :code");
    qry.bindValue(":code", code);

    if (qry.exec()) {
        QMessageBox::information(this, "Deleted", "Data has been deleted successfully.");
    } else {
        QMessageBox::information(this, "Error", "Failed to delete data.");
        qDebug() << "Query execution error: " << qry.lastError().text();
    }

    connectionClose();

}

