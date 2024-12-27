#ifndef NEWVIDEOGAME_H
#define NEWVIDEOGAME_H

#include <QMainWindow>
#include <QNetworkReply>

namespace Ui {
class newVideoGame;
}

class newVideoGame : public QMainWindow
{
    Q_OBJECT

public:
    explicit newVideoGame(QWidget *parent = nullptr);
    ~newVideoGame();

private slots:
    // Slot for the search button clicked event
    void onSearchButtonClicked();

private:
    // Function to search for a game using the IGDB API
    void searchGame(const QString& gameTitle, const QString& platform);

    // Helper functions for extracting and formatting game details
    QString getGenres(const QJsonObject& gameObj);
    QString getPlatform(const QJsonObject& gameObj);

    Ui::newVideoGame *ui;  // Pointer to the UI elements

};

#endif // NEWVIDEOGAME_H
