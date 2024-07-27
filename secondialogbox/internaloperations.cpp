#include "internaloperations.h"

QTextCharFormat internalHighlightFormat;


InternalOperations::InternalOperations(Ui::teacherdashboard *ui) : ui(ui),db(new Database()) {

    internalHighlightFormat.setBackground(Qt::red);
    getAllAssignmentDateList();
    QFont tooltipFont("Arial", 12);
    QToolTip::setFont(tooltipFont);
    getNotes();
    getAllAssignmentDateList();
    connect(ui->Calender, &QCalendarWidget::selectionChanged, this, &InternalOperations::showNoteForSelectedDate);
}

void InternalOperations::getNotes() {

    if (!db->connectionOpen()) {
        qDebug() << "Failed to open database";
        return;
    }

    QTextCharFormat highlightFormat;
    highlightFormat.setBackground(Qt::yellow);

    QSqlQuery qry;
    qry.prepare("SELECT Course_Code, Date, Time, Block, RoomNo FROM Exam");
    if (qry.exec()) {
        QMap<QDate, QString> notes;
        while (qry.next()) {
            QString courseCode = qry.value(0).toString();
            QString dateString = qry.value(1).toString();
            QString time = qry.value(2).toString();
            QString block = qry.value(3).toString();
            QString roomNo = qry.value(4).toString();

            QDate date = QDate::fromString(dateString, "MM/dd/yyyy");

            if (!date.isValid()) {
                qDebug() << "Invalid date:" << dateString;
                continue;
            }

            qDebug() << "Highlighting date:" << date.toString();
            ui->Calender->setDateTextFormat(date, highlightFormat);
            QString note = courseCode + "\n" + "Time: " + time + "\n" + "roomNo: " +roomNo + "\n" + "block:" + block;
            notes.insert(date, note);
        }
        notesMap.append(notes);
    }
    db->connectionClose();
}

void InternalOperations::showNoteForSelectedDate() {
    QDate selectedDate = ui->Calender->selectedDate();
    for ( auto &notes : notesMap) {
        if (notes.contains(selectedDate)) {
            QString note = notes.value(selectedDate);

            // Test tooltip display
            QPoint globalPos = QCursor::pos();
            QToolTip::showText(globalPos, note, ui->Calender);

            qDebug() << "Showing tooltip for date:" << selectedDate.toString();
            return;
        }
    }
}

void InternalOperations:: getAllAssignmentDateList(){

    if (!db->connectionOpen()) {
        qDebug() << "Failed to open database";
        return;
    }

    QSqlQuery qry;
    qry.prepare("SELECT deadline FROM Assignment");
    if (qry.exec()) {
        while (qry.next()) {
            QString date = qry.value(0).toString();
            assignmentDateList.append(date);
        }
    } else {
        qDebug() << "Query execution error: " << qry.lastError().text();
        db->connectionClose();
    }

    db->connectionClose();

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
    QStringList internalDateList;

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

    updatedDateList.append(assignmentDateList);

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

    dateList.append(assignmentDateList);

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

