#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "newentry.h"
#include "newfilm.h" // Include the header of the second window
#include "newtvshow.h"
#include "newvideogame.h"
#include "viewdatabase.h"
#include "about.h"


QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void newFilmWindow();  // Slot to show the second window
    void newTVShowWindow();
    void newEntryWindow();
    void viewdatabaseWindow();
    void aboutWindow();

private:
    Ui::MainWindow *ui;

    void setupDatabase();  // Function to set up the database

    newEntry *newEntry;

    newFilm *newfilm;

    newtvshow *newTVShow;

    newVideoGame *newVideoGame;

    viewdatabase *viewdatabase;

    about *about;

};


#endif // MAINWINDOW_H
