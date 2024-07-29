#include"assignmentoperations.h"

QTextCharFormat assignmentHighlightFormat;

AssignmentOperations::AssignmentOperations(Ui::teacherdashboard *ui) : ui(ui), db(new Database()), list(new List()){
    assignmentHighlightFormat.setBackground(Qt::yellow);

    QFont tooltipFont("Arial", 12);
    QToolTip::setFont(tooltipFont);


    // QPalette tooltipPalette;
    // tooltipPalette.setColor(QPalette::Window, Qt::yellow);
    // tooltipPalette.setColor(QPalette::WindowText, Qt::black);
    // QToolTip::setPalette(tooltipPalette);


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

    QStringList dateList = list->assignmentDateList;


    QTextCharFormat highlightFormat;
    highlightFormat.setBackground(Qt::yellow);

    for (int i = 0; i < dateList.size(); ++i) {
        QString dateString = dateList[i];
        QDate originalDate = QDate::fromString(dateString, "MM/dd/yyyy");
        ui->Calender->setDateTextFormat(originalDate, highlightFormat);
    }

}

void AssignmentOperations::showAvailableAssignmentDates(){

    QStringList dateList = list->assignmentDateList;

    // Append the temporary list of dates to the original list
    dateList.append(list->updatedAssignmentDateList);
    dateList.append(list->internalDateList);

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

    QStringList assignmentDateList = list->assignmentDateList;

    assignmentDateList.append(list->updatedAssignmentDateList);

    assignmentDateList.append(list->internalDateList);

    return !assignmentDateList.contains(dateString);
}






