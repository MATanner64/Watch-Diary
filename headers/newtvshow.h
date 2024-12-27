#ifndef NEWTVSHOW_H
#define NEWTVSHOW_H

#include "QtCore/qjsonobject.h"
#include <QMainWindow>
#include <QNetworkAccessManager>
#include <QNetworkReply>

namespace Ui {
class newtvshow;
}

// Main window class for TV show entry
class newtvshow : public QMainWindow
{
    Q_OBJECT

public:
    explicit newtvshow(QWidget *parent = nullptr);
    ~newtvshow();

    void setTvInfoText(const QString &text);  // New setter function to update tvinfo

private slots:
    void on_SearchTV_clicked();
    void on_addTvShow_clicked();


private:
    Ui::newtvshow *ui;
    QNetworkAccessManager *networkManager;

};

// Class for fetching TV series information based on IMDb ID
class TvSeriesInfoFetcher : public QObject
{
    Q_OBJECT

public:
    explicit TvSeriesInfoFetcher(newtvshow *mainWindow, QObject *parent = nullptr);
    void fetchTvSeriesInfoByImdbID(const QString &imdbID);



private slots:
    void onReplyFinished(QNetworkReply *reply);


private:
    newtvshow *mainWindow;
    QNetworkAccessManager *networkManager;
    void fetchTvSeriesInfo(int tvID);
    QString getGenres(const QJsonArray &genres);
    QString getStudios(const QJsonArray &productionCompanies);
};

// Class for fetching episode information based on IMDb ID, season, and episode number
class EpisodeInfoFetcher : public QObject
{
    Q_OBJECT

public:
    explicit EpisodeInfoFetcher(newtvshow *mainWindow, QObject *parent = nullptr);
    void fetchEpisodeInfoByImdbID(const QString &imdbID, int seasonNumber, int episodeNumber);

private slots:
    void onReplyFinished(QNetworkReply *reply);

private:
    newtvshow *mainWindow;
    QNetworkAccessManager *networkManager;
    int seasonNumber;
    int episodeNumber;
    void fetchEpisodeDetails(int tvID, int seasonNumber, int episodeNumber);
};

#endif // NEWTVSHOW_H
