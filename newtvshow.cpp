#include "newtvshow.h"
#include "ui_newtvshow.h"  // Include the generated header file for the UI
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QNetworkRequest>
#include <QUrl>
#include <QDebug>

#include "QtSql/qsqldatabase.h"
#include "QtSql/qsqlerror.h"
#include "QtSql/qsqlquery.h"

const QString API_KEY = "Enter API KEY HERE";  // Replace with your TMDb API key
const QString BASE_URL = "https://api.themoviedb.org/3";

// Main window class for TV show entry
newtvshow::newtvshow(QWidget *parent) :
    QMainWindow(parent)
    , ui(new Ui::newtvshow)  // Initialize the ui object
{
    ui->setupUi(this);  // Set up the UI from the .ui file
    networkManager = new QNetworkAccessManager(this);

    // Connect the search button to the slot
    connect(ui->SearchTV, &QPushButton::clicked, this, &newtvshow::on_SearchTV_clicked);
    connect(ui->addEpisode, &QPushButton::clicked, this, &newtvshow::on_addTvShow_clicked);
}

newtvshow::~newtvshow()
{
    delete ui;
}

void newtvshow::setTvInfoText(const QString &text)
{
    ui->tvinfo->appendPlainText(text);
}

bool seriesInfoFetched = false;
bool episodeInfoFetched = false;

void newtvshow::on_SearchTV_clicked() {

    ui->tvinfo->clear();

    QString imdbID = ui->IMDB_Number->text();
    int seasonNumber = ui->season_Number->text().toInt();
    int episodeNumber = ui->episode_Number->text().toInt();

    // Fetch TV Series info if not already fetched
    if (!seriesInfoFetched) {
        TvSeriesInfoFetcher *seriesFetcher = new TvSeriesInfoFetcher(this);
        seriesFetcher->fetchTvSeriesInfoByImdbID(imdbID);
        seriesInfoFetched = true;  // Set the flag after fetching
    }

    if (seriesInfoFetched == true ) {
        // Fetch Episode info if not already fetched
        if (!episodeInfoFetched) {
            EpisodeInfoFetcher *episodeFetcher = new EpisodeInfoFetcher(this);
            episodeFetcher->fetchEpisodeInfoByImdbID(imdbID, seasonNumber, episodeNumber);
            episodeInfoFetched = true;  // Set the flag after fetching
        }
    }

}

// Class for fetching TV series information based on IMDb ID
TvSeriesInfoFetcher::TvSeriesInfoFetcher(newtvshow *mainWindow, QObject *parent)
    : QObject(parent), mainWindow(mainWindow), networkManager(new QNetworkAccessManager(this)) {
    connect(networkManager, &QNetworkAccessManager::finished, this, &TvSeriesInfoFetcher::onReplyFinished);
}

void TvSeriesInfoFetcher::fetchTvSeriesInfoByImdbID(const QString &imdbID) {
    QString url = BASE_URL + "/find/" + imdbID + "?api_key=" + API_KEY + "&language=en-US&external_source=imdb_id";
    networkManager->get(QNetworkRequest(QUrl(url)));
}

void TvSeriesInfoFetcher::onReplyFinished(QNetworkReply *reply) {
    if (reply->error() != QNetworkReply::NoError) {
        qWarning() << "Error:" << reply->errorString();
        reply->deleteLater();
        return;
    }

    QJsonObject jsonObj = QJsonDocument::fromJson(reply->readAll()).object();
    QString url = reply->url().toString();

    if (url.contains("/find")) {
        // Extract TV ID from IMDb lookup response
        QJsonArray tvResults = jsonObj["tv_results"].toArray();
        if (!tvResults.isEmpty()) {
            int tvID = tvResults[0].toObject()["id"].toInt();
            // Fetch TV series details
            fetchTvSeriesInfo(tvID);
        } else {
            qWarning() << "No TV series found for IMDb ID.";
        }
    } else if (url.contains("/tv")) {
        // Process TV series details (Media table)
        QJsonObject tvInfo = jsonObj;
        QString title = tvInfo["name"].toString();
        QString genre = getGenres(tvInfo["genres"].toArray());
        QString releaseDate = tvInfo["first_air_date"].toString();
        QString showDescription = tvInfo["overview"].toString();
        QString studios = getStudios(tvInfo["production_companies"].toArray());

        // Format the show details to display in tvinfo
        QString displayText = QString("Show Title: %1\nGenre: %2\nRelease Date: %3\nDescription: %4\nStudios: %5")
                                  .arg(title.isEmpty() ? "N/A" : title)
                                  .arg(genre.isEmpty() ? "N/A" : genre)
                                  .arg(releaseDate.isEmpty() ? "N/A" : releaseDate)
                                  .arg(showDescription.isEmpty() ? "N/A" : showDescription)
                                  .arg(studios.isEmpty() ? "N/A" : studios);

        mainWindow->setTvInfoText(displayText);
        // Display the text in tvinfo

        seriesInfoFetched = false;
    }

    reply->deleteLater();
}

void TvSeriesInfoFetcher::fetchTvSeriesInfo(int tvID) {
    // Fetch TV series details using the TMDb ID
    QString url = BASE_URL + "/tv/" + QString::number(tvID) + "?api_key=" + API_KEY + "&language=en-US";
    networkManager->get(QNetworkRequest(QUrl(url)));
}

QString TvSeriesInfoFetcher::getGenres(const QJsonArray &genres) {
    QStringList genreList;
    for (const QJsonValue &value : genres) {
        QJsonObject genre = value.toObject();
        genreList.append(genre["name"].toString());
    }
    return genreList.join(", ");
}

QString TvSeriesInfoFetcher::getStudios(const QJsonArray &productionCompanies) {
    QStringList studioList;
    for (const QJsonValue &value : productionCompanies) {
        QJsonObject studio = value.toObject();
        studioList.append(studio["name"].toString());
    }
    return studioList.join(", ");
}

// Class for fetching episode information based on IMDb ID, season, and episode number
EpisodeInfoFetcher::EpisodeInfoFetcher(newtvshow *mainWindow, QObject *parent)
    : QObject(parent), mainWindow(mainWindow), networkManager(new QNetworkAccessManager(this)) {
    connect(networkManager, &QNetworkAccessManager::finished, this, &EpisodeInfoFetcher::onReplyFinished);
}

void EpisodeInfoFetcher::fetchEpisodeInfoByImdbID(const QString &imdbID, int seasonNumber, int episodeNumber) {
    QString url = BASE_URL + "/find/" + imdbID + "?api_key=" + API_KEY + "&language=en-US&external_source=imdb_id";
    networkManager->get(QNetworkRequest(QUrl(url)));

    this->seasonNumber = seasonNumber;
    this->episodeNumber = episodeNumber;
}

void EpisodeInfoFetcher::onReplyFinished(QNetworkReply *reply) {
    if (reply->error() != QNetworkReply::NoError) {
        qWarning() << "Error:" << reply->errorString();
        reply->deleteLater();
        return;
    }

    QJsonObject jsonObj = QJsonDocument::fromJson(reply->readAll()).object();
    QString url = reply->url().toString();

    if (url.contains("/find")) {
        // Extract TV ID from IMDb lookup response
        QJsonArray tvResults = jsonObj["tv_results"].toArray();
        if (!tvResults.isEmpty()) {
            int tvID = tvResults[0].toObject()["id"].toInt();
            // Fetch episode details
            fetchEpisodeDetails(tvID, seasonNumber, episodeNumber);
        } else {
            qWarning() << "No TV series found for IMDb ID.";
        }
    } else if (url.contains("/episode")) {
        // Process episode details
        QJsonObject episodeInfo = jsonObj;
        QString episodeTitle = episodeInfo["name"].toString();
        int season = episodeInfo["season_number"].toInt();
        int episode = episodeInfo["episode_number"].toInt();
        int duration = episodeInfo["runtime"].toInt();
        double episodeRating = episodeInfo["vote_average"].toDouble();
        QString episodeDescription = episodeInfo["overview"].toString();
        QString airDate = episodeInfo["air_date"].toString();

        // Format the episode details to display in tvinfo
        QString displayText = QString("Episode Title: %1\nSeason: %2\nEpisode: %3\nAir Date: %4\nDuration: %5\nRating: %6\nDescription: %7")
                                  .arg(episodeTitle.isEmpty() ? "N/A" : episodeTitle)
                                  .arg(season)
                                  .arg(episode)
                                  .arg(airDate.isEmpty() ? "N/A" : airDate)
                                  .arg(duration > 0 ? QString::number(duration) + " minutes" : "N/A")
                                  .arg(episodeRating > 0 ? QString::number(episodeRating) : "N/A")
                                  .arg(episodeDescription.isEmpty() ? "N/A" : episodeDescription);

        mainWindow->setTvInfoText(displayText);
        // Display the text in tvinfo


        episodeInfoFetched = false;
    }

    reply->deleteLater();
}

void EpisodeInfoFetcher::fetchEpisodeDetails(int tvID, int seasonNumber, int episodeNumber) {
    // Fetch episode details using the TMDb ID, season number, and episode number
    QString url = BASE_URL + "/tv/" + QString::number(tvID) + "/season/" + QString::number(seasonNumber) + "/episode/" + QString::number(episodeNumber) + "?api_key=" + API_KEY + "&language=en-US";
    networkManager->get(QNetworkRequest(QUrl(url)));
}

void newtvshow::on_addTvShow_clicked() {
    // Check if required fields are filled
    if (ui->IMDB_Number->text().isEmpty() || ui->season_Number->text().isEmpty() || ui->episode_Number->text().isEmpty()) {
        ui->tvinfo->appendPlainText("Please fill in all required fields before adding the TV show.");
        return;
    }

    // Access the existing database connection
    QSqlDatabase db = QSqlDatabase::database();
    if (!db.isOpen()) {
        ui->tvinfo->appendPlainText("Failed to open database: " + db.lastError().text());
        return;
    }

    // Start a transaction
    db.transaction();

    // Prepare the SQL insert statement for Media table
    QSqlQuery mediaQuery;
    mediaQuery.prepare(R"(
        INSERT INTO Media (title, type, description, genre, release_date, date_added, parental_rating, status, location)
        VALUES (:title, 'TV Show', :description, :genre, :release_date, :date_added, :parental_rating, :status, :location)
    )");

    // Extract data from the UI
    QString title = ui->showTitle->text();
    QString description = ui->showDescpription->text();
    QString genre = ui->genre->text();
    QString releaseDate = ui->airDate->date().toString("yyyy-MM-dd");
    QString status = ui->tvCompletion->text(); // Get status from tvCompletion QLineEdit

    mediaQuery.bindValue(":title", title);
    mediaQuery.bindValue(":description", description);
    mediaQuery.bindValue(":genre", genre);
    mediaQuery.bindValue(":release_date", releaseDate);
    mediaQuery.bindValue(":date_added", ui->dateAdded->date().toString("yyyy-MM-dd"));
    mediaQuery.bindValue(":parental_rating", ui->tvRating->text());
    mediaQuery.bindValue(":status", status); // Bind the status
    mediaQuery.bindValue(":location", ui->tvLocation->text());

    // Execute the Media query and check for success
    if (!mediaQuery.exec()) {
        ui->tvinfo->appendPlainText("\nFailed to add media to database: " + mediaQuery.lastError().text());
        db.rollback();
        db.close();
        return;
    }

    // Get the ID of the last inserted Media row
    int mediaId = mediaQuery.lastInsertId().toInt();

    // Prepare the SQL insert statement for Episodes table
    QSqlQuery episodeQuery;
    episodeQuery.prepare(R"(
        INSERT INTO Episodes (media_id, episode_title, season_number, episode_number, duration, rating, episode_description)
        VALUES (:media_id, :episode_title, :season_number, :episode_number, :duration, :rating, :episode_description)
    )");

    // Extract episode data from the UI
    QString episodeTitle = ui->episodeTitle->text();
    int seasonNumber = ui->season_Number->text().toInt();
    int episodeNumber = ui->episode_Number->text().toInt();
    QString duration = ui->episodeDuration->text();
    float episodeRating = ui->episodeRating->text().toFloat();
    QString episodeDescription = ui->episodeDescription->text();

    episodeQuery.bindValue(":media_id", mediaId);
    episodeQuery.bindValue(":episode_title", episodeTitle);
    episodeQuery.bindValue(":season_number", seasonNumber);
    episodeQuery.bindValue(":episode_number", episodeNumber);
    episodeQuery.bindValue(":duration", duration);
    episodeQuery.bindValue(":rating", episodeRating);
    episodeQuery.bindValue(":episode_description", episodeDescription);

    // Execute the Episodes query and check for success
    if (!episodeQuery.exec()) {
        ui->tvinfo->appendPlainText("\nFailed to add episode to database: " + episodeQuery.lastError().text());
        db.rollback();
        db.close();
        return;
    }

    // Commit the transaction if both queries were successful
    db.commit();
    ui->tvinfo->appendPlainText("TV Show and episode added successfully!");
    db.close();
}
