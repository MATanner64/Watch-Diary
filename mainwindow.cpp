#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QFile>
#include <QDir>
#include <QAction>
#include <QPixmap>
#include <QLabel>
#include <QIcon>

#include "newfilm.h" // Include the header of the second window
#include "newtvshow.h"
#include "newentry.h"
#include "viewdatabase.h"
#include "about.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , newEntry(nullptr)
    , newfilm(nullptr)
    , newTVShow(nullptr)
    , viewdatabase(nullptr)
    , about(nullptr)

{
    ui->setupUi(this);
    setupDatabase();

    this->setWindowIcon(QIcon(":/icon.icns"));

    ui->label->show();

    // Connect the action to the slot that will show the second window
    //connect(ui->actionFilm, &QAction::triggered, this, &MainWindow::newFilmWindow);

    //connect(ui->actionTV_Show, &QAction::triggered, this, &MainWindow::newTVShowWindow);

    //connect(ui->addFilm, &QPushButton::clicked, this, &MainWindow::newEntryWindow);

    connect(ui->viewSearchButton, &QPushButton::clicked, this, &MainWindow::viewdatabaseWindow);

    //ui->label->setPixmap(QPixmap("/Users/mtanner/Desktop/test.jpeg"));

    connect(ui->addFilm, &QPushButton::clicked, this, &MainWindow::newFilmWindow);
    connect(ui->addTVShow, &QPushButton::clicked, this, &MainWindow::newTVShowWindow);

    connect(ui->aboutButton, &QPushButton::clicked, this, &MainWindow::aboutWindow);  // Close the window



}

void MainWindow::newEntryWindow()
{
    if (!newEntry)  {
        newEntry = new class newEntry(this);
    }

    newEntry->show();
}


void MainWindow::newFilmWindow()
{
    if (!newfilm) {
        newfilm = new newFilm(this);
    }

    newfilm->show();  // Show the second window
    //this->hide();
}

void MainWindow::newTVShowWindow()
{
    if (!newTVShow) {
        newTVShow = new newtvshow(this);
    }

    newTVShow->show();
    //this->hide();
}


void MainWindow::viewdatabaseWindow()
{
    if (!viewdatabase)  {
        viewdatabase = new class viewdatabase(this);
    }

    viewdatabase->show();
    //this->hide();
}

void MainWindow::aboutWindow()
{
    if(!about)  {
        about = new class about(this);
    }

    about->show();
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::setupDatabase()
{
    QString dbPath = QDir::currentPath() + "/media_tracking.db";  // Database will be created in the project folder
    bool dbExists = QFile::exists(dbPath);

    // Set up the SQLite database
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(dbPath);

    if (!db.open()) {
        qDebug() << "Error: Unable to open the database" << db.lastError();
        return;
    }

    if (dbExists) {
        qDebug() << "Database already exists.";
        return;
    }

    QSqlQuery query;

    // Create the Media table
    QString createMediaTable = R"(
        CREATE TABLE IF NOT EXISTS Media (
            id INTEGER PRIMARY KEY,
            title TEXT NOT NULL,
            type TEXT NOT NULL,
            description TEXT,
            genre TEXT,
            release_date DATE,
            date_added DATE,
            parental_rating TEXT,
            status TEXT,
            location TEXT
        );
    )"; // Remove the extra comma here
    if (!query.exec(createMediaTable)) {
        qDebug() << "Error creating Media table:" << query.lastError();
        return;
    }

    // Create the Books table
    QString createBooksTable = R"(
        CREATE TABLE IF NOT EXISTS Books (
            media_id INTEGER PRIMARY KEY,
            author TEXT,
            publisher TEXT,
            page_count INTEGER,
            FOREIGN KEY (media_id) REFERENCES Media(id)
        );
    )";
    if (!query.exec(createBooksTable)) {
        qDebug() << "Error creating Books table:" << query.lastError();
        return;
    }

    // Create the Movies table
    QString createMoviesTable = R"(
        CREATE TABLE IF NOT EXISTS Movies (
            media_id INTEGER PRIMARY KEY,
            director TEXT,
            cast TEXT,
            studio TEXT,
            runtime INTEGER,
            FOREIGN KEY (media_id) REFERENCES Media(id)
        );
    )";
    if (!query.exec(createMoviesTable)) {
        qDebug() << "Error creating Movies table:" << query.lastError();
        return;
    }

    // Create the Games table
    QString createGamesTable = R"(
        CREATE TABLE IF NOT EXISTS Games (
            media_id INTEGER PRIMARY KEY,
            developer TEXT,
            platform TEXT,
            playtime INTEGER,
            FOREIGN KEY (media_id) REFERENCES Media(id)
        );
    )";
    if (!query.exec(createGamesTable)) {
        qDebug() << "Error creating Games table:" << query.lastError();
        return;
    }

    // Create the Episodes table
    QString createTVEpisodesTable = R"(
        CREATE TABLE IF NOT EXISTS Episodes (
            media_id INTEGER,
            episode_title TEXT,
            season_number INTEGER,
            episode_number INTEGER,
            duration INTEGER,
            rating REAL,
            episode_description TEXT,
            FOREIGN KEY (media_id) REFERENCES Media(id)
        );
    )";
    if (!query.exec(createTVEpisodesTable)) {
        qDebug() << "Error creating Episodes table:" << query.lastError();
        return;
    }

    // Create the Consumption_Log table
    QString createConsumptionLogTable = R"(
        CREATE TABLE IF NOT EXISTS Consumption_Log (
            log_id INTEGER PRIMARY KEY,
            media_id INTEGER,
            date_consumed DATE,
            time_consumed TIME,
            duration INTEGER,
            notes TEXT,
            FOREIGN KEY (media_id) REFERENCES Media(id)
        );
    )";
    if (!query.exec(createConsumptionLogTable)) {
        qDebug() << "Error creating Consumption_Log table:" << query.lastError();
        return;
    }

    db.commit();

    db.close();

    qDebug() << "Database and tables created successfully.";
}
