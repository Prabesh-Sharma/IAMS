#include "teacherdashboard.h"
#include "ui_teacherdashboard.h"
#include <QSqlQuery>
#include <QMessageBox>
#include <QDebug>
#include "teacherwindow1.h"
#include "QString"

teacherdashboard::teacherdashboard(QWidget *parent, const QString &username)
    : QDialog(parent)
    , ui(new Ui::teacherdashboard), t_username(username)
{
    ui->setupUi(this);
    this->resize(800, 600);
    setWindowTitle("TeacherDashBoard");

    connect(ui->homeButton, &QPushButton::clicked, this, &teacherdashboard::showHomePage);
    connect(ui->assignmentButton, &QPushButton::clicked, this, &teacherdashboard::showAssignmentPage);
    connect(ui->internalButton, &QPushButton::clicked, this, &teacherdashboard::showInternalPage);

    ui->stackedWidget->setCurrentWidget(ui->Home);
    ui->showusername->setText("Hello, " + t_username);
    highlightInternalDatesOnCalender();
    highlightAssignmentDatesOnCalender();

}

teacherdashboard::~teacherdashboard()
{

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

void teacherdashboard::showHomePage()
{
    ui->stackedWidget->setCurrentWidget(ui->Home);
}

void teacherdashboard::showAssignmentPage()
{
    ui->stackedWidget->setCurrentWidget(ui->Assignment);
    showAvailableAssignmentDates();
}

void teacherdashboard::showInternalPage()
{
    ui->userLable->setText(t_username);
    ui->stackedWidget->setCurrentWidget(ui->Internal);
    showAvailableInternalDates();

}

void teacherdashboard::highlightInternalDatesOnCalender()
{
    QStringList dateList;

    if (!connectionOpen()) {
        qDebug() << "Failed to open database";
    }

    QSqlQuery qry;
    qry.prepare("SELECT Date FROM Exam");
    if (qry.exec()) {
        while (qry.next()) {
            QString date = qry.value(0).toString();
            dateList.append(date);
        }
    } else {
        qDebug() << "Query execution error: " << qry.lastError().text();
        connectionClose();
    }

    QTextCharFormat defaultFormat;  // Updates the date, clears old ones.

    for (QDate date = ui->Calender->minimumDate(); date <= ui->Calender->maximumDate(); date = date.addDays(1)) {
        ui->Calender->setDateTextFormat(date, defaultFormat);
    }

    QTextCharFormat highlightFormat;
    highlightFormat.setBackground(Qt::red);

    for (int i = 0; i < dateList.size(); ++i) {
        QString dateString = dateList[i];
        QDate originalDate = QDate::fromString(dateString, "MM/dd/yyyy");
        ui->Calender->setDateTextFormat(originalDate, highlightFormat);
    }
}

bool teacherdashboard::checkInternalTime(QDate &selectedDate)
{
    QDate currentDate = QDate::currentDate();
    return currentDate.addDays(7) <= selectedDate;
}

bool teacherdashboard::matchcode(const QString &username, const QString &code)
{
    if (!connectionOpen()) {
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
    connectionClose();
    return result;
}

bool teacherdashboard::getAllDates(const QString &dateString)
{
    QStringList dateList;

    if (!connectionOpen()) {
        qDebug() << "Failed to open database";
        return false;
    }

    QSqlQuery qry;
    qry.prepare("SELECT Date FROM Exam");
    if (qry.exec()) {
        while (qry.next()) {
            QString date = qry.value(0).toString();
            dateList.append(date);
        }
    } else {
        qDebug() << "Query execution error: " << qry.lastError().text();
        connectionClose();
        return false;
    }
    connectionClose();

    QStringList updatedDateList = checkExamDate(dateList);

    return !updatedDateList.contains(dateString);
}

QStringList teacherdashboard::checkExamDate(QStringList &dateList)
{
    QStringList tempList;

    for (const QString &dateString : dateList) {
        QDate originalDate = QDate::fromString(dateString, "MM/dd/yyyy");

        if (!originalDate.isValid()) {
            qDebug() << "Invalid date format:" << dateString;
            continue;
        }

        QDate increasedDate = originalDate.addDays(1);
        QString increasedDateString = increasedDate.toString("MM/dd/yyyy");

        dateList.append(increasedDateString);

        QDate decreasedDate = originalDate.addDays(-1);
        QString decreasedDateString = decreasedDate.toString("MM/dd/yyyy");

        tempList.append(decreasedDateString);
    }

    dateList.append(tempList);
    return dateList;}

void teacherdashboard::on_internalUpdate_clicked()
{
    QString block = ui->blockEdit->text();
    QString room = ui->roomEdit->text();
    QString code = ui->courseEdit->text();
    QTime selectedTime = ui->timeEdit->time();
    QString timeString = selectedTime.toString("HH:mm:ss");
    QDate selectedDate = ui->dateEdit->date();
    QString dateString = selectedDate.toString("MM/dd/yyyy");

    if (code.isEmpty() && block.isEmpty() && room.isEmpty()) {
        QMessageBox::information(this, "Error", "Data is not filled ");
        return;
    }

    if (!connectionOpen()) {
        qDebug() << "Failed to open database";
        return;
    }

    if (!checkInternalTime(selectedDate)) {
        QMessageBox::information(this, "Error", "Minimum of 7 days required");
        return;
    }

    if (matchcode(t_username, code)) {
        if (getAllDates(dateString)) {
            connectionOpen();
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
        connectionClose();
    } else {
        QMessageBox::critical(this, "Cannot", "You are not authorized to access this course ");
    }
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

    if (!connectionOpen()) {
        qDebug() << "Failed to open database";
        return;
    }

    if (code.isEmpty() && block.isEmpty() && room.isEmpty()) {
        QMessageBox::information(this, "Error", "Data is not filled ");
        return;
    }

    if (!checkInternalTime(selectedDate)) {
        QMessageBox::information(this, "Error", "Minimum of 7 days required");
        return;
    }

    if (matchcode(t_username, code)) {
        if (getAllDates(dateString)) {
            connectionOpen();
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
            connectionClose();
        }
    } else {
        QMessageBox::critical(this, "Cannot", "You are not authorized to access this course ");
    }
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

void teacherdashboard::showAvailableInternalDates(){

    QStringList dateList;

    if (!connectionOpen()) {
        qDebug() << "Failed to open database";
        return;
    }

    QSqlQuery qry;
    qry.prepare("SELECT Date FROM Exam");
    if (qry.exec()) {
        while (qry.next()) {
            QString date = qry.value(0).toString();
            dateList.append(date);
        }
    } else {
        qDebug() << "Query execution error: " << qry.lastError().text();
        connectionClose();
        return;
    }
    connectionClose();

    QStringList tempList;

    for (const QString &dateString : dateList) {
        QDate originalDate = QDate::fromString(dateString, "MM/dd/yyyy");

        if (!originalDate.isValid()) {
            qDebug() << "Invalid date format:" << dateString;
            continue;
        }

        QDate increasedDate = originalDate.addDays(1);
        QString increasedDateString = increasedDate.toString("MM/dd/yyyy");

        tempList.append(increasedDateString);

        QDate decreasedDate = originalDate.addDays(-1);
        QString decreasedDateString = decreasedDate.toString("MM/dd/yyyy");

        tempList.append(decreasedDateString);
    }

    dateList.append(tempList);

    QStringList availableDateList;
    QDate currentDate = QDate::currentDate();

    int size = dateList.size() + 10;
    for (int i = 0; i < size; i++) {
        QString dateString = currentDate.addDays(i).toString("MM/dd/yyyy");
        availableDateList.append(dateString);
    }

    // Remove dates from availableDateList that are present in dateList
    for (const QString& date : dateList) {
        availableDateList.removeAll(date);
    }

    if (availableDateList.size() > 10) {
        availableDateList = availableDateList.mid(0, 10);
    }

    QTableWidget* internalTableWidget = ui->internalTableWidget;
    internalTableWidget->setRowCount(availableDateList.size());
    internalTableWidget->setColumnCount(1);
    internalTableWidget->setHorizontalHeaderLabels({"List of available dates"});
    internalTableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    internalTableWidget->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    int row = 0;
    for (const QString& date : availableDateList) {
        internalTableWidget->setItem(row, 0, new QTableWidgetItem(date));
        ++row;
    }


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

    if (!connectionOpen()) {
        qDebug() << "Failed to open database";
        return;
    }

    if (code.isEmpty()) {
        QMessageBox::information(this, "Error", "Data is not filled");
        return;
    }

    if (!checkInternalTime(selectedDate)) {
        QMessageBox::information(this, "Error", "Minimum of 7 days required");
        return;
    }

    if(matchcode(t_username, code)){
        if(getAllAssignmentDates(dateString)){

            connectionOpen();

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
            connectionClose();
        }
    }
    else {
        QMessageBox::critical(this, "Cannot", "You are not authorized to access this course");
    }
}

void teacherdashboard::on_assignmentupdateButton_clicked()
{
    QString code = ui->assignmentCourseEdit->text();
    QDate selectedDate = ui->assignmentDateEdit->date();
    QDate currentdate = QDate::currentDate();
    QString currentDateString = currentdate.toString("MM/dd/yyyy");
    QString dateString = selectedDate.toString("MM/dd/yyyy");

    if (!connectionOpen()) {
        qDebug() << "Failed to open database";
        return;
    }

    if (code.isEmpty()) {
        QMessageBox::information(this, "Error", "Data is not filled");
        connectionClose();
        return;
    }

    if (!checkInternalTime(selectedDate)) {
        QMessageBox::information(this, "Error", "Minimum of 7 days required");
        connectionClose();
        return;
    }

    if(matchcode(t_username, code)){
        if(getAllAssignmentDates(dateString)){

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

    connectionClose();}

void teacherdashboard::on_assignmentDeleteButton_clicked()
{
    QString code = ui->assignmentCourseEdit->text();

    if (code.isEmpty()) {
        QMessageBox::information(this, "Error", "Please enter a Course Code to delete.");
        return;
    }

    if (!connectionOpen()) {
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

    connectionClose();
}

void teacherdashboard::showAvailableAssignmentDates(){

    // Fetch dates from the database
    QStringList dateList;

    if (!connectionOpen()) {
        qDebug() << "Failed to open database";
        return;
    }

    QSqlQuery qry;
    qry.prepare("SELECT deadLine FROM Assignment");
    if (qry.exec()) {
        while (qry.next()) {
            QString date = qry.value(0).toString();
            dateList.append(date);
        }
    } else {
        qDebug() << "Query execution error: " << qry.lastError().text();
        connectionClose();
        return;
    }
    connectionClose();

    // Process dates: add days before and after each date
    QStringList tempList;

    for (const QString &dateString : dateList) {
        QDate originalDate = QDate::fromString(dateString, "MM/dd/yyyy");

        if (!originalDate.isValid()) {
            qDebug() << "Invalid date format:" << dateString;
            continue;
        }

        // Add one day and one day before each date
        QString increasedDateString = originalDate.addDays(1).toString("MM/dd/yyyy");
        QString decreasedDateString = originalDate.addDays(-1).toString("MM/dd/yyyy");

        tempList.append(increasedDateString);
        tempList.append(decreasedDateString);
    }

    // Append the temporary list of dates to the original list
    dateList.append(tempList);

    // Generate available dates
    QStringList availableDateList;
    QDate currentDate = QDate::currentDate();

    int size = dateList.size() + 10;
    for (int i = 0; i < size; i++) {
        QString dateString = currentDate.addDays(i).toString("MM/dd/yyyy");
        availableDateList.append(dateString);
    }

    // Remove dates from availableDateList that are present in dateList
    for (const QString& date : dateList) {
        availableDateList.removeAll(date);
    }

    // Limit to a maximum of 10 available dates
    if (availableDateList.size() > 10) {
        availableDateList = availableDateList.mid(0, 10);
    }

    // Populate QTableWidget
    QTableWidget* assignmentTableWidget = ui->assignmentTableWidget;
    assignmentTableWidget->setRowCount(availableDateList.size());
    assignmentTableWidget->setColumnCount(1);
    assignmentTableWidget->setHorizontalHeaderLabels({"List of available dates"});
    assignmentTableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    assignmentTableWidget->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    int row = 0;
    for (const QString& date : availableDateList) {
        assignmentTableWidget->setItem(row, 0, new QTableWidgetItem(date));
        ++row;
    }

}

bool teacherdashboard::getAllAssignmentDates(const QString &dateString){

    QStringList dateList;

    if (!connectionOpen()) {
        qDebug() << "Failed to open database";
        return false;
    }

    QSqlQuery qry;
    qry.prepare("SELECT deadLine FROM Assignment");
    if (qry.exec()) {
        while (qry.next()) {
            QString date = qry.value(0).toString();
            dateList.append(date);
        }
    } else {
        qDebug() << "Query execution error: " << qry.lastError().text();
        connectionClose();
        return false;
    }
    connectionClose();

    QStringList updatedDateList = checkAssignmentDate(dateList);
    return !updatedDateList.contains(dateString);
}

QStringList teacherdashboard::checkAssignmentDate(QStringList &dateList)
{
    QStringList tempList;

    for (const QString &dateString : dateList) {
        QDate originalDate = QDate::fromString(dateString, "MM/dd/yyyy");

        if (!originalDate.isValid()) {
            qDebug() << "Invalid date format:" << dateString;
            continue;
        }

        QDate increasedDate = originalDate.addDays(1);
        QString increasedDateString = increasedDate.toString("MM/dd/yyyy");

        tempList.append(increasedDateString);

        QDate decreasedDate = originalDate.addDays(-1);
        QString decreasedDateString = decreasedDate.toString("MM/dd/yyyy");

        tempList.append(decreasedDateString);
    }

    dateList.append(tempList);
    return dateList;
}

void teacherdashboard::highlightAssignmentDatesOnCalender(){

    QStringList dateList;

    if (!connectionOpen()) {
        qDebug() << "Failed to open database";
    }

    QSqlQuery qry;
    qry.prepare("SELECT deadLine FROM Assignment");
    if (qry.exec()) {
        while (qry.next()) {
            QString date = qry.value(0).toString();
            dateList.append(date);
        }
    } else {
        qDebug() << "Query execution error: " << qry.lastError().text();
        connectionClose();
    }

    QTextCharFormat defaultFormat;

    // Updates the date, clears old ones.
    // for (QDate date = ui->Calender->minimumDate(); date <= ui->Calender->maximumDate(); date = date.addDays(1)) {
    //     ui->Calender->setDateTextFormat(date, defaultFormat);
    // }

    QTextCharFormat highlightFormat;
    highlightFormat.setBackground(Qt::yellow);

    for (int i = 0; i < dateList.size(); ++i) {
        QString dateString = dateList[i];
        QDate originalDate = QDate::fromString(dateString, "MM/dd/yyyy");
        ui->Calender->setDateTextFormat(originalDate, highlightFormat);
    }
}
