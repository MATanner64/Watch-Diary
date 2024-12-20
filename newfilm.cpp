#include "newfilm.h"
#include "QtSql/qsqldatabase.h"
#include "QtSql/qsqlerror.h"
#include "QtSql/qsqlquery.h"
#include "ui_newfilm.h"
#include <QDebug>
#include <QNetworkRequest>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

const QString API_KEY = "Enter API KEY HERE";
const QString BASE_URL = "https://api.themoviedb.org/3/movie/";

newFilm::newFilm(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::newFilm)
{
    ui->setupUi(this);
    this->setWindowTitle("New Film");

    // Initialize network manager
    networkManager = new QNetworkAccessManager(this);

    // Connect button click to search action
    connect(ui->SearchFilm, &QPushButton::clicked, this, &newFilm::onSearchFilmClicked);
    connect(networkManager, &QNetworkAccessManager::finished, this, &newFilm::onReplyFinished);
}

newFilm::~newFilm()
{
    delete ui;
}

void newFilm::onSearchFilmClicked() {
    QString imdbID = ui->IMDB_Number->text();
    if (!imdbID.isEmpty()) {
        fetchMovieDetails(imdbID);
    } else {
        ui->filminfo->setPlainText("Please enter a valid IMDb ID.");
    }
}

void newFilm::fetchMovieDetails(const QString &imdbID) {
    QString path = imdbID + "?api_key=" + API_KEY + "&language=en-US&external_source=imdb_id";
    sendRequest(path);
}

void newFilm::sendRequest(const QString &path) {
    QNetworkRequest request(QUrl(BASE_URL + path));
    networkManager->get(request);
}

void newFilm::onReplyFinished(QNetworkReply *reply) {
    if (reply->error() != QNetworkReply::NoError) {
        ui->filminfo->setPlainText("Error: " + reply->errorString());
        reply->deleteLater();
        return;
    }

    QJsonObject jsonObj = QJsonDocument::fromJson(reply->readAll()).object();
    QString url = reply->url().toString();

    if (url.contains("/credits")) {
        // Store credits information
        directors = getDirector(jsonObj);
        mainActors = getMainActors(jsonObj);

        // Append credits info to QPlainTextEdit
        ui->filminfo->appendPlainText("\n\nDirectors: " + directors + "\n" +
                                      "Main Actors: " + mainActors);
    } else {
        // Store movie details in member variables
        title = jsonObj["title"].toString();
        description = jsonObj["overview"].toString();
        runtime = jsonObj["runtime"].toInt();
        releaseDate = jsonObj["release_date"].toString();
        genre = getGenres(jsonObj["genres"].toArray());
        rating = jsonObj["vote_average"].toDouble();
        productionCompanies = getProductionCompanies(jsonObj["production_companies"].toArray());

        // Display movie information
        QString resultText = "Title: " + title + "\n" +
                             "Description: " + description + "\n" +
                             "Runtime: " + QString::number(runtime) + " minutes\n" +
                             "Release Date: " + releaseDate + "\n" +
                             "Genre: " + genre + "\n" +
                             "Rating: " + QString::number(rating) + "\n" +
                             "Production Companies: " + productionCompanies;
        ui->filminfo->setPlainText(resultText);

        // Fetch credits if movie ID is available
        if (jsonObj.contains("id") && jsonObj["id"].isDouble()) {
            fetchMovieCredits(jsonObj["id"].toInt());
        }
    }

    reply->deleteLater();
}

void newFilm::on_addFilm_clicked() {

    // Check if required fields are filled
    if (title.isEmpty() || description.isEmpty() || genre.isEmpty() || releaseDate.isEmpty() || ui->filmRating->text().isEmpty()) {
        ui->filminfo->appendPlainText("Please fill in all required fields before adding the film.");
        return;
    }

    // Access the existing database connection
    QSqlDatabase db = QSqlDatabase::database();

    if (!db.isOpen()) {
        ui->filminfo->appendPlainText("Failed to open database: " + db.lastError().text());
        return;
    }

    // Prepare the SQL insert statement for Media table
    QSqlQuery mediaQuery;
    mediaQuery.prepare(R"(
        INSERT INTO Media (title, type, description, genre, release_date, date_added, parental_rating, status, location)
        VALUES (:title, 'Film', :description, :genre, :release_date, :date_added, :parental_rating, :status, :location)
    )");

    // Bind values to the Media table SQL statement
    mediaQuery.bindValue(":title", title);
    mediaQuery.bindValue(":description", description);
    mediaQuery.bindValue(":genre", genre);
    mediaQuery.bindValue(":release_date", releaseDate);
    mediaQuery.bindValue(":date_added", ui->dateAdded->date().toString("yyyy-MM-dd"));
    mediaQuery.bindValue(":parental_rating", ui->filmRating->text());
    mediaQuery.bindValue(":status", ui->filmCompletion->text());
    mediaQuery.bindValue(":location", ui->filmLocation->text());

    // Execute the Media query and check for success
    if (!mediaQuery.exec()) {
        ui->filminfo->appendPlainText("\nFailed to add media to database: " + mediaQuery.lastError().text());
        db.close();
        return;
    }

    // Retrieve the last inserted ID for use in the Movies table
    int mediaId = mediaQuery.lastInsertId().toInt();

    // Prepare the SQL insert statement for Movies table
    QSqlQuery movieQuery;
    movieQuery.prepare(R"(
        INSERT INTO Movies (media_id, director, cast, studio, runtime)
        VALUES (:media_id, :director, :cast, :studio, :runtime)
    )");

    // Bind values to the Movies table SQL statement
    movieQuery.bindValue(":media_id", mediaId);
    movieQuery.bindValue(":director", directors);
    movieQuery.bindValue(":cast", mainActors);
    movieQuery.bindValue(":studio", productionCompanies);
    movieQuery.bindValue(":runtime", runtime);

    // Execute the Movies query and check for success
    if (movieQuery.exec()) {
        ui->filminfo->appendPlainText("\nMovie added to database successfully!");
    } else {
        ui->filminfo->appendPlainText("\nFailed to add movie details to database: " + movieQuery.lastError().text());
    }

    db.commit();

    db.close();
}

QString newFilm::getDirector(const QJsonObject &credits) {
    for (const QJsonValue &value : credits["crew"].toArray()) {
        QJsonObject person = value.toObject();
        if (person["job"].toString() == "Director") {
            return person["name"].toString();
        }
    }
    return "N/A";
}

QString newFilm::getMainActors(const QJsonObject &credits) {
    QStringList mainActors;
    for (int i = 0; i < qMin(3, credits["cast"].toArray().size()); ++i) {
        mainActors.append(credits["cast"].toArray()[i].toObject()["name"].toString());
    }
    return mainActors.join(", ");
}

QString newFilm::getGenres(const QJsonArray &genres) {
    QStringList genreList;
    for (const QJsonValue &value : genres) {
        genreList.append(value.toObject()["name"].toString());
    }
    return genreList.join(", ");
}

QString newFilm::getProductionCompanies(const QJsonArray &companies) {
    QStringList companyList;
    for (const QJsonValue &value : companies) {
        companyList.append(value.toObject()["name"].toString());
    }
    return companyList.join(", ");
}

void newFilm::fetchMovieCredits(int movieID) {
    QString path = QString::number(movieID) + "/credits?api_key=" + API_KEY;
    sendRequest(path);
}
