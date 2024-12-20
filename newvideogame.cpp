#include "newvideogame.h"
#include "ui_newvideogame.h"
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QMessageBox>
#include <QDateTime>
#include <QByteArray>

newVideoGame::newVideoGame(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::newVideoGame)
{
    ui->setupUi(this);

    // Connect the "Search" button to the search slot
    connect(ui->searchGame, &QPushButton::clicked, this, &newVideoGame::onSearchButtonClicked);
}

newVideoGame::~newVideoGame()
{
    delete ui;
}

// Slot for handling the search button click
void newVideoGame::onSearchButtonClicked() {
    QString gameTitle = ui->gameTitle->text();  // Get the game title from the input
    QString platform = ui->gamePlatform->text();  // Get the platform from the input

    if (!gameTitle.isEmpty() && !platform.isEmpty()) {
        searchGame(gameTitle, platform);
    } else {
        QMessageBox::warning(this, "Input Error", "Please provide both game title and platform.");
    }
}

// Function to send the request to the IGDB API and fetch game data
void newVideoGame::searchGame(const QString& gameTitle, const QString& platform) {
    QString accessToken = " ";  // Replace with your actual API token
    QString clientId = " ";  // Replace with your IGDB Client ID

    QNetworkAccessManager *manager = new QNetworkAccessManager(this);
    QNetworkRequest request(QUrl("https://api.igdb.com/v4/games"));

    // Set request headers
    request.setRawHeader("Client-ID", clientId.toUtf8());
    request.setRawHeader("Authorization", QString("Bearer %1").arg(accessToken).toUtf8());
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");

    QByteArray body;
    body.append(QString("fields name,platform,release_date; search \"%1\"; limit 1;")
                    .arg(gameTitle)  // gameTitle is the QString you're searching for
                    .toUtf8());

    // Send POST request
    QNetworkReply *reply = manager->post(request, body);

    // Connect reply finished signal to handler
    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        if (reply->error() == QNetworkReply::NoError) {
            // Successfully received a response
            QByteArray responseData = reply->readAll();

            // Parse JSON response
            QJsonDocument doc = QJsonDocument::fromJson(responseData);
            if (doc.isArray()) {
                QJsonArray games = doc.array();
                if (!games.isEmpty()) {
                    QJsonObject gameObj = games[0].toObject();

                    // Extract game details
                    QString name = gameObj["name"].toString();
                    QString summary = gameObj["summary"].toString();
                    QString genres = getGenres(gameObj);
                    QString releaseDate = QDateTime::fromSecsSinceEpoch(gameObj["first_release_date"].toInt()).toString("yyyy-MM-dd");
                    QString platform = getPlatform(gameObj);

                    // Display in the UI
                    QString gameInfo = QString("Title: %1\nSummary: %2\nGenres: %3\nRelease Date: %4\nPlatform: %5")
                                           .arg(name).arg(summary).arg(genres).arg(releaseDate).arg(platform);

                    ui->gameInfo->setPlainText(gameInfo);
                } else {
                    ui->gameInfo->setPlainText("No results found.");
                }
            }
        } else {
            // Handle network error
            ui->gameInfo->setPlainText("Error fetching game details.");
            qDebug() << "Error: " << reply->errorString();
        }
        reply->deleteLater();
    });
}

// Helper function to extract genres from the game object
QString newVideoGame::getGenres(const QJsonObject& gameObj) {
    QString genres;
    QJsonArray genresArray = gameObj["genres"].toArray();
    for (int i = 0; i < genresArray.size(); ++i) {
        QJsonObject genreObj = genresArray[i].toObject();
        if (i > 0) genres.append(", ");
        genres.append(genreObj["name"].toString());
    }
    return genres;
}

// Helper function to extract platform names from the game object
QString newVideoGame::getPlatform(const QJsonObject& gameObj) {
    QString platforms;
    QJsonArray platformsArray = gameObj["platforms"].toArray();
    for (int i = 0; i < platformsArray.size(); ++i) {
        QJsonObject platformObj = platformsArray[i].toObject();
        if (i > 0) platforms.append(", ");
        platforms.append(platformObj["name"].toString());
    }
    return platforms;
}
