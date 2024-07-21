#include "internaloperations.h"

QTextCharFormat internalHighlightFormat;


InternalOperations::InternalOperations(Ui::teacherdashboard *ui) : ui(ui),db(new Database()) {

    internalHighlightFormat.setBackground(Qt::red);
}



void InternalOperations::highlightInternalDatesOnCalender()
{
    QTextCharFormat defaultFormat;
    ui->Calender->setDateTextFormat(QDate(), defaultFormat);

    QStringList internalDateList;

    if (!db->connectionOpen()) {
        qDebug() << "Failed to open database";
    }

    QSqlQuery qry;
    qry.prepare("SELECT Date FROM Exam");
    if (qry.exec()) {
        while (qry.next()) {
            QString date = qry.value(0).toString();
            internalDateList.append(date);
        }
    } else {
        qDebug() << "Query execution error: " << qry.lastError().text();
        db->connectionClose();
    }


    QTextCharFormat highlightFormat;
    highlightFormat.setBackground(Qt::red);

    for (int i = 0; i < internalDateList.size(); ++i) {
        QString dateString = internalDateList[i];
        QDate originalDate = QDate::fromString(dateString, "MM/dd/yyyy");
        ui->Calender->setDateTextFormat(originalDate, highlightFormat);
    }
}

bool InternalOperations::getAllInternalDates(const QString &dateString)
{

    if (!db->connectionOpen()) {
        qDebug() << "Failed to open database";
        return false;
    }

    QSqlQuery qry;
    qry.prepare("SELECT Date FROM Exam");
    if (qry.exec()) {
        while (qry.next()) {
            QString date = qry.value(0).toString();
            internalDateList.append(date);
        }
    } else {
        qDebug() << "Query execution error: " << qry.lastError().text();
        db->connectionClose();
        return false;
    }
    db->connectionClose();

    QStringList updatedDateList = checkInternalDate(internalDateList);



    return !updatedDateList.contains(dateString);
}

QStringList InternalOperations::checkInternalDate(QStringList &dateList)
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

void InternalOperations::showAvailableInternalDates(){

    QStringList dateList;

    if (!db->connectionOpen()) {
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
        db->connectionClose();
        return;
    }
    db->connectionClose();

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
        QString dateString = currentDate.addDays(i + 7).toString("MM/dd/yyyy");
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

