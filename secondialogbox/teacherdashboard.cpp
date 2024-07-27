#include "teacherdashboard.h"
#include "ui_teacherdashboard.h"
#include <QSqlQuery>
#include <QMessageBox>
#include <QDebug>
#include "teacherwindow1.h"
#include "QString"

teacherdashboard::teacherdashboard(QWidget *parent, const QString &username)
    : QDialog(parent)
    , ui(new Ui::teacherdashboard), t_username(username),db(new Database())
{
    ui->setupUi(this);
    this->resize(800, 600);
    setWindowTitle("TeacherDashBoard");

    assignmentOps = new AssignmentOperations(ui);
    internalOps = new InternalOperations(ui);

    connect(ui->homeButton, &QPushButton::clicked, this, &teacherdashboard::showHomePage);
    connect(ui->assignmentButton, &QPushButton::clicked, this, &teacherdashboard::showAssignmentPage);
    connect(ui->internalButton, &QPushButton::clicked, this, &teacherdashboard::showInternalPage);

    ui->stackedWidget->setCurrentWidget(ui->Home);
    ui->showusername->setText("Hello, " + t_username);

    internalOps->highlightInternalDatesOnCalender();
    assignmentOps->highlightAssignmentDatesOnCalender();

    ui->dateEdit->setDate(QDate::currentDate());
    ui->assignmentDateEdit->setDate(QDate::currentDate());

    QLabel *yellowPic = ui->yelloPic;
    int width = yellowPic->width();
    int height = yellowPic->height();

    QPixmap yellowPicture(":/images/images/yellowColour.jpeg");
    yellowPic->setPixmap(yellowPicture.scaled(width, height, Qt::KeepAspectRatio));

    QLabel *redPic = ui->redPic;
    QPixmap redPicture(":/images/images/redColour.png");
    redPic->setPixmap(redPicture.scaled(width, height, Qt::KeepAspectRatio));

}

teacherdashboard::~teacherdashboard()
{
            delete db;             // Clean up Database object
            delete assignmentOps;  // Clean up AssignmentOperations object
            delete internalOps;    // Clean up InternalOperations object
            delete ui;             // Clean up UI object
}



    void teacherdashboard::showHomePage()
    {
        ui->stackedWidget->setCurrentWidget(ui->Home);
    }

    void teacherdashboard::showAssignmentPage()
    {
        ui->stackedWidget->setCurrentWidget(ui->Assignment);
        assignmentOps->showAvailableAssignmentDates();

    }

    void teacherdashboard::showInternalPage()
    {
        ui->userLable->setText(t_username);
        ui->stackedWidget->setCurrentWidget(ui->Internal);
        internalOps->showAvailableInternalDates();

    }

    bool teacherdashboard::checkTime(QDate &selectedDate)
    {
        QDate currentDate = QDate::currentDate();
        return currentDate.addDays(7) <= selectedDate;
    }

    bool teacherdashboard::matchcode(const QString &username, const QString &code)
    {
        if (!db->connectionOpen()) {
            qDebug() << "Failed to open database";
            return false;
        }

        QSqlQuery qry;
        qry.prepare("SELECT * FROM Teacher WHERE userName = :username AND courseCode = :code");
        qry.bindValue(":username", username);
        qry.bindValue(":code", code);

        bool result = false;
        if (qry.exec()) {
            int count = 0;
            while (qry.next()) {
                count++;
            }
            result = (count == 1);
        }
        db->connectionClose();
        return result;
    }

    void teacherdashboard::on_internalUpdate_clicked()
    {
        QString block = ui->blockEdit->text();
        QString room = ui->roomEdit->text();
        QString code = ui->courseEdit->text();
        QTime selectedTime = ui->timeEdit->time();
        QString timeString = selectedTime.toString("HH:mm:ss");
        QDate selectedDate = ui->dateEdit->date();
        QString dateString = selectedDate.toString("MM/dd/yyyy");

        if (code.isEmpty()) {
            QMessageBox::information(this, "Error", "Course code is not filled");
            return;
        }
        if (block.isEmpty()) {
            QMessageBox::information(this, "Error", "Block is not filled");
            return;
        }
        if (room.isEmpty()) {
            QMessageBox::information(this, "Error", "Room is not filled");
            return;
        }

        if (!db->connectionOpen()) {
            qDebug() << "Failed to open database";
            return;
        }

        if (!checkTime(selectedDate)) {
            QMessageBox::information(this, "Error", "Minimum of 7 days required");
            return;
        }

        if (matchcode(t_username, code)) {
            if (internalOps->getAllInternalDates(dateString)) {
                db->connectionOpen();
                QSqlQuery qry;
                qry.prepare("UPDATE Exam SET Block = :block, RoomNo = :room, Time = :time, Date = :date WHERE Course_Code = :code");
                qry.bindValue(":block", block);
                qry.bindValue(":room", room);
                qry.bindValue(":time", timeString);
                qry.bindValue(":date", dateString);
                qry.bindValue(":code", code);

                if (!qry.exec()) {
                    QMessageBox::information(this, "Error", "Failed to save data.");
                    qDebug() << "Query error: " << qry.lastError().text();
                } else {
                    QMessageBox::information(this, "Saved", "Data has been saved successfully.");
                }
            } else {
                QMessageBox::critical(this, "Cannot", "The date is already taken");
            }
            db->connectionClose();
        } else {
            QMessageBox::critical(this, "Cannot", "You are not authorized to access this course ");
        }
        internalOps->highlightInternalDatesOnCalender();
        assignmentOps->highlightAssignmentDatesOnCalender();
    }

    void teacherdashboard::on_internalAdd_clicked()
    {
        QString block = ui->blockEdit->text();
        QString room = ui->roomEdit->text();
        QString code = ui->courseEdit->text();
        QTime selectedTime = ui->timeEdit->time();
        QString timeString = selectedTime.toString("HH:mm:ss");
        QDate selectedDate = ui->dateEdit->date();
        QString dateString = selectedDate.toString("MM/dd/yyyy");

        if (!db->connectionOpen()) {
            qDebug() << "Failed to open database";
            return;
        }

        if (code.isEmpty()) {
            QMessageBox::information(this, "Error", "Course code is not filled");
            return;
        }
        if (block.isEmpty()) {
            QMessageBox::information(this, "Error", "Block is not filled");
            return;
        }
        if (room.isEmpty()) {
            QMessageBox::information(this, "Error", "Room is not filled");
            return;
        }

        if (!checkTime(selectedDate)) {
            QMessageBox::information(this, "Error", "Minimum of 7 days required");
            return;
        }

        if (matchcode(t_username, code)) {
            if (internalOps->getAllInternalDates(dateString)) {
                db->connectionOpen();
                QSqlQuery qry;
                qry.prepare("INSERT INTO Exam (Course_Code, Block, RoomNo, Time, Date) VALUES (:code, :block, :room, :time, :date)");
                qry.bindValue(":code", code);
                qry.bindValue(":block", block);
                qry.bindValue(":room", room);
                qry.bindValue(":time", timeString);
                qry.bindValue(":date", dateString);

                if (!qry.exec()) {
                    QMessageBox::information(this, "Error", "Failed to save data.");
                    qDebug() << "Query error: " << qry.lastError().text();
                } else {
                    QMessageBox::information(this, "Saved", "Data has been saved successfully.");
                }
            } else {
                QMessageBox::critical(this, "Cannot", "The date is already taken");
                db->connectionClose();
            }
        } else {
            QMessageBox::critical(this, "Cannot", "You are not authorized to access this course ");
        }

        db->connectionClose();

        internalOps->highlightInternalDatesOnCalender();
        assignmentOps->highlightAssignmentDatesOnCalender();

    }

    void teacherdashboard::on_internalDelete_clicked()
    {
        QString code = ui->courseEdit->text();

        if (code.isEmpty()) {
            QMessageBox::information(this, "Error", "Please enter a Course Code to delete.");
            return;
        }

        if (!db->connectionOpen()) {
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

        db->connectionClose();
        internalOps->highlightInternalDatesOnCalender();
        assignmentOps->highlightAssignmentDatesOnCalender();
    }

    void teacherdashboard::on_logOut_clicked()
    {
        SecDialog *teacherlogin = new SecDialog;
        hide();
        teacherlogin->show();
    }

    void teacherdashboard::on_assignmentAddButton_clicked()
    {
        QString code = ui->assignmentCourseEdit->text();
        QDate selectedDate = ui->assignmentDateEdit->date();
        QDate currentdate = QDate::currentDate();
        QString currentDateString = currentdate.toString("MM/dd/yyyy");
        QString dateString = selectedDate.toString("MM/dd/yyyy");

        if (!db->connectionOpen()) {
            qDebug() << "Failed to open database";
            return;
        }

        if (code.isEmpty()) {
            QMessageBox::information(this, "Error", "Data is not filled");
            return;
        }

        if (!checkTime(selectedDate)) {
            QMessageBox::information(this, "Error", "Minimum of 7 days required");
            return;
        }

        if(matchcode(t_username, code)){
            if(assignmentOps->getAllAssignmentDates(dateString)){

                db->connectionOpen();

                QSqlQuery qry;
                qry.prepare("INSERT INTO Assignment (courseCode, deadLine, startDate) VALUES (:code, :dateString, :currentDateString)");
                qry.bindValue(":code", code);
                qry.bindValue(":dateString", dateString);
                qry.bindValue(":currentDateString", currentDateString);
                if (!qry.exec()) {
                    QMessageBox::information(this, "Error", "Failed to save data.");
                    qDebug() << "Query error: " << qry.lastError().text();
                } else {
                    QMessageBox::information(this, "Saved", "Data has been saved successfully.");
                }
            }
            else {
                QMessageBox::critical(this, "Cannot", "The date is already taken");
                db->connectionClose();
            }
        }
        else {
            QMessageBox::critical(this, "Cannot", "You are not authorized to access this course");
        }
        internalOps->highlightInternalDatesOnCalender();
        assignmentOps->highlightAssignmentDatesOnCalender();
    }

    void teacherdashboard::on_assignmentupdateButton_clicked()
    {
        QString code = ui->assignmentCourseEdit->text();
        QDate selectedDate = ui->assignmentDateEdit->date();
        QDate currentdate = QDate::currentDate();
        QString currentDateString = currentdate.toString("MM/dd/yyyy");
        QString dateString = selectedDate.toString("MM/dd/yyyy");

        if (!db->connectionOpen()) {
            qDebug() << "Failed to open database";
            return;
        }

        if (code.isEmpty()) {
            QMessageBox::information(this, "Error", "Data is not filled");
            db->connectionClose();
            return;
        }

        if (!checkTime(selectedDate)) {
            QMessageBox::information(this, "Error", "Minimum of 7 days required");
            db->connectionClose();
            return;
        }

        if(matchcode(t_username, code)){
            if(assignmentOps->getAllAssignmentDates(dateString)){

                db->connectionOpen();

                QSqlQuery qry;
                qry.prepare("UPDATE Assignment SET deadLine = :date, startDate = :currentdate WHERE courseCode = :code");
                qry.bindValue(":code", code);
                qry.bindValue(":date", dateString);
                qry.bindValue(":currentdate", currentDateString);
                if (!qry.exec()) {
                    QMessageBox::information(this, "Error", "Failed to save data.");
                    qDebug() << "Query error: " << qry.lastError().text();
                } else {
                    QMessageBox::information(this, "Saved", "Data has been saved successfully.");
                }
            }
            else {
                QMessageBox::critical(this, "Cannot", "The date is already taken");
            }
        }
        else {
            QMessageBox::critical(this, "Cannot", "You are not authorized to access this course");
        }

        db->connectionClose();
        internalOps->highlightInternalDatesOnCalender();
        assignmentOps->highlightAssignmentDatesOnCalender();
    }

    void teacherdashboard::on_assignmentDeleteButton_clicked()
    {
        QString code = ui->assignmentCourseEdit->text();

        if (code.isEmpty()) {
            QMessageBox::information(this, "Error", "Please enter a Course Code to delete.");
            return;
        }

        if (!db->connectionOpen()) {
            qDebug() << "Failed to open database";
            return;
        }

        QSqlQuery qry;
        qry.prepare("DELETE FROM Assignment WHERE Course_Code = :code");
        qry.bindValue(":code", code);

        if (qry.exec()) {
            QMessageBox::information(this, "Deleted", "Data has been deleted successfully.");
        } else {
            QMessageBox::information(this, "Error", "Failed to delete data.");
            qDebug() << "Query execution error: " << qry.lastError().text();
        }

        db->connectionClose();
        internalOps->highlightInternalDatesOnCalender();
        assignmentOps->highlightAssignmentDatesOnCalender();
    }

