#include "newentry.h"
#include "newtvshow.h"
#include "ui_newentry.h"

#include "newfilm.h"
#include "newtvshow.h"

newEntry::newEntry(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::newEntry)
    , newfilm(nullptr)
    , newtvshow(nullptr)
    , newVideoGame(nullptr)
{
    ui->setupUi(this);

    connect(ui->addFilm, &QPushButton::clicked, this, &::newEntry::newFilmWindow);
    connect(ui->addTVEpisode, &QPushButton::clicked, this, &::newEntry::newTVShowWindow);
    connect(ui->addVideoGame, &QPushButton::clicked, this, &::newEntry::newVideoGameWindow);
}

newEntry::~newEntry()
{
    delete ui;
}

void newEntry::newFilmWindow()
{
    if (!newfilm) {

        newfilm = new newFilm(this);
    }

    newfilm->show();
    this->hide();
}

void newEntry::newTVShowWindow()
{
    if (!newtvshow) {
        newtvshow = new class newtvshow(this);
    }

    newtvshow->show();
    this->hide();
}

void newEntry::newVideoGameWindow()
{
    if (!newVideoGame)  {
        newVideoGame = new class newVideoGame(this);
    }
    newVideoGame->show();
    this->hide();
}
