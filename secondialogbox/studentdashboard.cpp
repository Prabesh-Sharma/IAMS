#include "studentdashboard.h"
#include "ui_studentdashboard.h"
#include "studentwindow1.h"
#include <QMessageBox>
#include <QDebug>

studentdashboard::studentdashboard(QWidget *parent,  const QString &username) :
    QDialog(parent),
    ui(new Ui::studentdashboard), db(new Database), t_username(username)
{
    ui->setupUi(this);
    this->resize(800, 600);
    setWindowTitle("StudentDashBoard");
    this->showMaximized();

    connect(ui->homeButton, &QPushButton::clicked, this, &studentdashboard::on_homeButton_clicked);
    connect(ui->assignmentButton, &QPushButton::clicked, this, &studentdashboard::on_assignmentButton_clicked);
    connect(ui->internalButton, &QPushButton::clicked, this, &studentdashboard::on_internalButton_clicked);
    connect(ui->calendar, &QCalendarWidget::selectionChanged, this, &studentdashboard::showNoteForSelectedDateAssignment);
    connect(ui->calendar, &QCalendarWidget::selectionChanged, this, &studentdashboard::showNoteForSelectedDateInternal);

    QFont tooltipFont("Arial", 12);
    QToolTip::setFont(tooltipFont);

    ui->stackedWidget->setCurrentWidget(ui->Home);
    ui->userName->setText("Hello, " + t_username);

    QLabel *yellowPic = ui->yelloPic;
    int width = yellowPic->width();
    int height = yellowPic->height();

    QPixmap yellowPicture(":/images/images/1920x1080-yellow-solid-color-background.jpg");
    yellowPic->setPixmap(yellowPicture.scaled(width, height, Qt::KeepAspectRatio));

    QLabel *redPic = ui->redPic;
    QPixmap redPicture(":/images/images/1920x1080-red-solid-color-background.jpg");
    redPic->setPixmap(redPicture.scaled(width, height, Qt::KeepAspectRatio));

    ui->calendar->setVerticalHeaderFormat(QCalendarWidget::NoVerticalHeader);

    ui->calendar->setGridVisible(true);

    getNotesAssignment();

    getNotesInternal();

    highlightDatesOnCalender();
}


studentdashboard::~studentdashboard()
{
    delete ui;
    delete db;
}

void studentdashboard::on_logOutButton_clicked()
{
    studentwindow1 *studentlogin = new studentwindow1;
    hide();
    studentlogin->showMaximized();


}

void studentdashboard::on_homeButton_clicked()
{
    ui->stackedWidget->setCurrentWidget(ui->Home);
}

void studentdashboard::on_assignmentButton_clicked(){

    ui->stackedWidget->setCurrentWidget(ui->Assignment);
    QStringList assignmentDateList, codeList, startDateList;

    if (!db->connectionOpen()) {
        qDebug() << "Failed to open database";
        return;
    }

    QSqlQuery qry;
    qry.prepare("SELECT deadLine, courseCode, startDate FROM Assignment");
    if (qry.exec()) {
        while (qry.next()) {
            QString deadline = qry.value(0).toString();
            QString courseCode = qry.value(1).toString();
            QString startDate = qry.value(2).toString();

            assignmentDateList.append(deadline);
            codeList.append(courseCode);
            startDateList.append(startDate);
        }
    } else {
        qDebug() << "Query execution error: " << qry.lastError().text();
        db->connectionClose();
        return;
    }

    db->connectionClose();

    // Populate the table widget
    QTableWidget *assignmentTable = ui->AssinmentTableWidget;

    assignmentTable->setRowCount(assignmentDateList.size());
    assignmentTable->setColumnCount(3);
    QStringList title = {"Course Code", "Deadline", "Start Date"};
    assignmentTable->setHorizontalHeaderLabels(title);
    assignmentTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    assignmentTable->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    for (int row = 0; row < assignmentDateList.size(); ++row) {
        assignmentTable->setItem(row, 0, new QTableWidgetItem(codeList.at(row)));
        assignmentTable->setItem(row, 1, new QTableWidgetItem(assignmentDateList.at(row)));
        assignmentTable->setItem(row, 2, new QTableWidgetItem(startDateList.at(row)));
    }


}

void studentdashboard::on_internalButton_clicked()
{
    db->connectionOpen();
    ui->stackedWidget->setCurrentWidget(ui->Internal);
    QStringList internalDateList, codeList, timeList, blockList,RoomList;

    if (!db->connectionOpen()) {
        qDebug() << "Failed to open database";
        return;
    }

    QSqlQuery qry;
    qry.prepare("SELECT Course_Code, Date, Time, Block, RoomNo FROM Exam");
    if (qry.exec()) {
        while (qry.next()) {

            QString courseCode = qry.value(0).toString();
            QString Date = qry.value(1).toString();
            QString Time = qry.value(2).toString();
            QString Block = qry.value(3).toString();
            QString RoomNo = qry.value(4).toString();

            internalDateList.append(Date);
            codeList.append(courseCode);
            timeList.append(Time);
            blockList.append(Block);
            RoomList.append(RoomNo);

        }
    } else {
        qDebug() << "Query execution error: " << qry.lastError().text();
        db->connectionClose();
        return;
    }

    db->connectionClose();


    QTableWidget *internalTable = ui->internalTableWidget;

    internalTable->setRowCount(internalDateList.size());
    internalTable->setColumnCount(5);

    QFont headerFont = internalTable->horizontalHeader()->font();
    headerFont.setBold(true);
    headerFont.setPointSize(10);
    internalTable->horizontalHeader()->setFont(headerFont);


    QStringList headers = {"Course Code", "Date", "Time", "Block", "Room No"};
    internalTable->setHorizontalHeaderLabels(headers);

    internalTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    internalTable->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    for (int row = 0; row < internalDateList.size(); ++row) {
        internalTable->setItem(row, 0, new QTableWidgetItem(codeList.at(row)));
        internalTable->setItem(row, 1, new QTableWidgetItem(internalDateList.at(row)));
        internalTable->setItem(row, 2, new QTableWidgetItem(timeList.at(row)));
        internalTable->setItem(row, 3, new QTableWidgetItem(blockList.at(row)));
        internalTable->setItem(row, 4, new QTableWidgetItem(RoomList.at(row)));

    }

}

void studentdashboard::getNotesAssignment() {

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


            ui->calendar->setDateTextFormat(date, highlightFormat);
            QString note = courseCode ;
            notes.insert(date, note);
        }
        notesMap.append(notes);
    }
    db->connectionClose();
}

void studentdashboard::showNoteForSelectedDateAssignment() {

    QDate selectedDate = ui->calendar->selectedDate();
    for ( auto &notes : notesMap) {
        if (notes.contains(selectedDate)) {
            QString note = notes.value(selectedDate);

            // Test tooltip display
            QPoint globalPos = QCursor::pos();
            QToolTip::showText(globalPos, note, ui->calendar);

            qDebug() << "Showing tooltip for date:" << selectedDate.toString();
            return;
        }
    }
    qDebug() << "No note found for date:" << selectedDate.toString();
}

void studentdashboard::getNotesInternal() {

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

            ui->calendar->setDateTextFormat(date, highlightFormat);
            QString note = courseCode + "\n" + "Time: " + time + "\n" + "roomNo: " +roomNo + "\n" + "block:" + block;
            notes.insert(date, note);
        }
        notesMap.append(notes);
    }
    db->connectionClose();
}

void studentdashboard::showNoteForSelectedDateInternal() {

    QDate selectedDate = ui->calendar->selectedDate();
    for ( auto &notes : notesMap) {
        if (notes.contains(selectedDate)) {
            QString note = notes.value(selectedDate);

            // Test tooltip display
            QPoint globalPos = QCursor::pos();
            QToolTip::showText(globalPos, note, ui->calendar);

            qDebug() << "Showing tooltip for date:" << selectedDate.toString();
            return;
        }
    }
}

void studentdashboard::highlightDatesOnCalender() {

    QTextCharFormat defaultFormat;

    List *list = new List();
    ui->calendar->setDateTextFormat(QDate(), defaultFormat);

    QStringList internalDateList = list->internalDateList;
    QStringList assignmentDateList = list->assignmentDateList;


    QTextCharFormat internalHighlightFormat;
    internalHighlightFormat.setBackground(Qt::red);

    QTextCharFormat assignmentHighlightFormat;
    assignmentHighlightFormat.setBackground(Qt::yellow);

    // Highlight internal dates
    for (const QString& dateString : internalDateList) {
        QDate date = QDate::fromString(dateString, "MM/dd/yyyy");
        QTextCharFormat format = ui->calendar->dateTextFormat(date);
        format.setBackground(internalHighlightFormat.background());
        ui->calendar->setDateTextFormat(date, format);
    }

    // Highlight assignment dates
    for (const QString& dateString : assignmentDateList) {
        QDate date = QDate::fromString(dateString, "MM/dd/yyyy");
        QTextCharFormat format = ui->calendar->dateTextFormat(date);
        format.setBackground(assignmentHighlightFormat.background());
        ui->calendar->setDateTextFormat(date, format);
    }

}

