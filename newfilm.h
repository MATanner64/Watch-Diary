#ifndef NEWFILM_H
#define NEWFILM_H

#include <QMainWindow>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

namespace Ui {
class newFilm;
}

class newFilm : public QMainWindow
{
    Q_OBJECT

public:
    explicit newFilm(QWidget *parent = nullptr);
    ~newFilm();

private slots:
    void onSearchFilmClicked();
    void onReplyFinished(QNetworkReply *reply);

    void on_addFilm_clicked();

private:
    QString title;
    QString description;
    int runtime;
    QString releaseDate;
    QString genre;
    double rating;
    QString productionCompanies;
    QString directors;
    QString mainActors;

    void fetchMovieDetails(const QString &imdbID);
    void fetchMovieCredits(int movieID);  // Add this line
    void sendRequest(const QString &path);
    QString getDirector(const QJsonObject &credits);
    QString getMainActors(const QJsonObject &credits);
    QString getGenres(const QJsonArray &genres);
    QString getProductionCompanies(const QJsonArray &companies);

private:
    Ui::newFilm *ui;
    QNetworkAccessManager *networkManager;
};

#endif // NEWFILM_H
