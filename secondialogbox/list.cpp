#include "list.h"


List::List() : db(new Database()){

    assignmentDates();
    internalDates();
    updatedAssignmentDates();
    updatedInternalDates();

}

void List::assignmentDates(){

    if (!db->connectionOpen()) {
        qDebug() << "Failed to open database";
        return ;
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
        return ;
    }
    db->connectionClose();
}

void List::internalDates(){


    if (!db->connectionOpen()) {
        qDebug() << "Failed to open database";
        return ;
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
        return ;
    }
    db->connectionClose();
}

void List::updatedAssignmentDates(){

    for (const QString &dateString : assignmentDateList) {
        QDate originalDate = QDate::fromString(dateString, "MM/dd/yyyy");

        if (!originalDate.isValid()) {
            qDebug() << "Invalid date format:" << dateString;
            continue;
        }

        QDate increasedDate = originalDate.addDays(1);
        QString increasedDateString = increasedDate.toString("MM/dd/yyyy");

        updatedAssignmentDateList.append(increasedDateString);

        QDate decreasedDate = originalDate.addDays(-1);
        QString decreasedDateString = decreasedDate.toString("MM/dd/yyyy");

        updatedAssignmentDateList.append(decreasedDateString);
    }

}

void List::updatedInternalDates(){

    for (const QString &dateString : internalDateList) {
        QDate originalDate = QDate::fromString(dateString, "MM/dd/yyyy");

        if (!originalDate.isValid()) {
            qDebug() << "Invalid date format:" << dateString;
            continue;
        }

        QDate increasedDate = originalDate.addDays(1);
        QString increasedDateString = increasedDate.toString("MM/dd/yyyy");

        updatedInternalDateList.append(increasedDateString);

        QDate decreasedDate = originalDate.addDays(-1);
        QString decreasedDateString = decreasedDate.toString("MM/dd/yyyy");

        updatedInternalDateList.append(decreasedDateString);
    }

}

