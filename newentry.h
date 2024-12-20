#ifndef NEWENTRY_H
#define NEWENTRY_H

#include "newfilm.h"
#include "newtvshow.h"
#include "newvideogame.h"
#include <QMainWindow>


namespace Ui {
class newEntry;
}

class newEntry : public QMainWindow
{
    Q_OBJECT

public:
    explicit newEntry(QWidget *parent = nullptr);
    ~newEntry();

private slots:
    void newFilmWindow();  // Slot to show the second window
    void newTVShowWindow();
    void newVideoGameWindow();

private:
    Ui::newEntry *ui;

    newFilm *newfilm;
    newtvshow *newtvshow;
    newVideoGame *newVideoGame;
};

#endif // NEWENTRY_H
