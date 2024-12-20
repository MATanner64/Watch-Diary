#include "about.h"
#include "ui_about.h"
#include <QSqlQuery>
#include <QMessageBox>
#include <QtSql/qsqlerror.h>

about::about(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::about)
{
    ui->setupUi(this);

    // Connect the reset button to the resetDatabase slot
    connect(ui->resetButton, &QPushButton::clicked, this, &about::resetDatabase);
}

about::~about()
{
    delete ui;
}

bool about::connectToDatabase()
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("media_tracking.db");

    if (!db.open()) {
        qDebug() << "Error: Unable to connect to database:" << db.lastError().text();
        return false;
    }
    return true;
}

void about::resetDatabase()
{
    // Prompt user with a confirmation dialog
    QMessageBox::StandardButton reply = QMessageBox::question(
        this, "Reset Database", "Are you sure you want to reset the database? This action cannot be undone.",
        QMessageBox::Yes | QMessageBox::No);

    if (reply != QMessageBox::Yes) {
        return;
    }

    // Connect to the database
    if (!connectToDatabase()) {
        return;
    }

    // Start a transaction to ensure consistency
    QSqlDatabase db = QSqlDatabase::database();
    if (!db.transaction()) {
        qDebug() << "Failed to start transaction:" << db.lastError().text();
        return;
    }

    // SQL queries to delete data from all tables
    QStringList tables = {"Media", "Movies", "Episodes"};

    for (const QString &table : tables) {
        QSqlQuery query;
        query.prepare(QString("DELETE FROM %1").arg(table));

        if (!query.exec()) {
            qDebug() << "Failed to reset the database:" << query.lastError().text();
            db.rollback(); // Rollback in case of an error
            return;
        }
    }

    // Commit the transaction if all queries are successful
    if (!db.commit()) {
        qDebug() << "Failed to commit transaction:" << db.lastError().text();
        return;
    }

    QSqlQuery query;
    query.prepare("UPDATE sqlite_sequence SET seq = 0 WHERE name = 'Media'");
    if (!query.exec()) {
        qDebug() << "Failed to reset auto-increment sequence:" << query.lastError().text();
    }

    qDebug() << "Database reset successfully.";
}
