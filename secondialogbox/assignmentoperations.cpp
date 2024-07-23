#include"assignmentoperations.h"

QTextCharFormat assignmentHighlightFormat;

AssignmentOperations::AssignmentOperations(Ui::teacherdashboard *ui) : ui(ui), db(new Database()) {
    assignmentHighlightFormat.setBackground(Qt::yellow);

    QFont tooltipFont("Arial", 12);
    QToolTip::setFont(tooltipFont);


    // QPalette tooltipPalette;
    // tooltipPalette.setColor(QPalette::Window, Qt::yellow);
    // tooltipPalette.setColor(QPalette::WindowText, Qt::black);
    // QToolTip::setPalette(tooltipPalette);

    getInternalDateList();
    getNotes();

    connect(ui->Calender, &QCalendarWidget::selectionChanged, this, &AssignmentOperations::showNoteForSelectedDate);
}


void AssignmentOperations::getNotes() {

    if (!db->connectionOpen()) {
        qDebug() << "Failed to open database";
        return;
    }

    QTextCharFormat highlightFormat;
    highlightFormat.setBackground(Qt::yellow);

    QSqlQuery qry;
    qry.prepare("SELECT courseCode,deadLine from Assignment");
    if (qry.exec()) {
        QMap<QDate, QString> notes;
        while (qry.next()) {
            QString courseCode = qry.value(0).toString();
            QString dateString = qry.value(1).toString();


            QDate date = QDate::fromString(dateString, "MM/dd/yyyy");

            if (!date.isValid()) {
                qDebug() << "Invalid date:" << dateString;
                continue;
            }

            qDebug() << "Highlighting date:" << date.toString();
            ui->Calender->setDateTextFormat(date, highlightFormat);
            QString note = courseCode ;
            notes.insert(date, note);
        }
        notesMap.append(notes);
    }
    db->connectionClose();
}

void AssignmentOperations::showNoteForSelectedDate() {
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
    qDebug() << "No note found for date:" << selectedDate.toString();
}

void AssignmentOperations::highlightAssignmentDatesOnCalender(){

    QStringList dateList;

    if (!db->connectionOpen()) {
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
        db->connectionClose();
    }

    // dateList.append(internalDateList);

    QTextCharFormat highlightFormat;
    highlightFormat.setBackground(Qt::yellow);

    for (int i = 0; i < dateList.size(); ++i) {
        QString dateString = dateList[i];
        QDate originalDate = QDate::fromString(dateString, "MM/dd/yyyy");
        ui->Calender->setDateTextFormat(originalDate, highlightFormat);
    }

}

void AssignmentOperations::getInternalDateList(){

    if (!db->connectionOpen()) {
        qDebug() << "Failed to open database";
        return;
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
    }
    db->connectionClose();
}

void AssignmentOperations::showAvailableAssignmentDates(){

    // Fetch dates from the database
    QStringList dateList;

    if (!db->connectionOpen()) {
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
        db->connectionClose();
        return;
    }
    db->connectionClose();

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
    dateList.append(internalDateList);

    // Generate available dates
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

bool AssignmentOperations::getAllAssignmentDates(const QString &dateString){

    QStringList assignmentDateList;

    if (!db->connectionOpen()) {
        qDebug() << "Failed to open database";
        return false;
    }

    QSqlQuery qry;
    qry.prepare("SELECT deadLine FROM Assignment");
    if (qry.exec()) {
        while (qry.next()) {
            QString date = qry.value(0).toString();
            assignmentDateList.append(date);
        }
    } else {
        qDebug() << "Query execution error: " << qry.lastError().text();
        db->connectionClose();
        return false;
    }
    db->connectionClose();

    QStringList updatedDateList = checkAssignmentDate(assignmentDateList);

    updatedDateList.append(internalDateList);

    return !updatedDateList.contains(dateString);
}

QStringList AssignmentOperations::checkAssignmentDate(QStringList &dateList)
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




