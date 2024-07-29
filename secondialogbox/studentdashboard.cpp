#include "studentdashboard.h"
#include "ui_studentdashboard.h"
#include "studentwindow1.h"
#include <QMessageBox>
#include <QDebug>

studentdashboard::studentdashboard(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::studentdashboard), db(new Database), list(new List)
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

    ui->calendar->setVerticalHeaderFormat(QCalendarWidget::NoVerticalHeader);

    ui->calendar->setGridVisible(true);

    getNotesAssignment();

    getNotesInternal();

    highlightDatesOnCalender();
}


studentdashboard::~studentdashboard()
{
    delete ui;
}

void studentdashboard::on_logOutButton_clicked()
{
    studentwindow1 *studentlogin = new studentwindow1;
    hide();
    studentlogin->show();

    if (mydb.isOpen()) {
        mydb.close();
    }
}

void studentdashboard::on_homeButton_clicked()
{
    ui->stackedWidget->setCurrentWidget(ui->Home);
}

void studentdashboard::on_assignmentButton_clicked()
    {
    db->connectionOpen();
        ui->stackedWidget->setCurrentWidget(ui->Assignment);
        QSqlQuery qry(mydb);
        if(qry.exec("SELECT * FROM Assignment")){
            QSqlQueryModel *modal = new QSqlQueryModel();
            modal->setQuery(std::move(qry));
            ui->AssinmentTableView->setModel(modal);

        }else{
            qDebug() << "Query execution error:" << qry.lastError().text();
            QMessageBox::critical(this,"Query Error","Failed to execute query");
        }

        db->connectionClose();

    }

void studentdashboard::on_internalButton_clicked()
{
    db->connectionOpen();
    ui->stackedWidget->setCurrentWidget(ui->Internal);
    QSqlQuery qry(mydb);
    if (qry.exec("SELECT * FROM Exam")) {
        QSqlQueryModel *modal = new QSqlQueryModel();
        modal->setQuery(std::move(qry));
        ui->internalTableView->setModel(modal);

    } else {
        qDebug() << "Query execution error:" << qry.lastError().text();
        QMessageBox::critical(this, "Query Error", "Failed to execute query");
    }

    db->connectionClose();
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

