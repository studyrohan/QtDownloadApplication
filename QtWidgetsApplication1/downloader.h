
#ifndef DOWNLOADER_H
#define DOWNLOADER_H

#include <QObject>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>
#include <QUrl>
#include <QDateTime>
#include <QFile>
#include <QDebug>


class Downloader : public QObject
{
    Q_OBJECT
public:
    explicit Downloader(QObject* parent = 0);

    QList< QString>GetAllResource(QString url="");
    void DownloadResource(const QString& res);

    void DoDownload();

    QByteArray GetContext() const;


signals:

public slots:
    void ReplyFinished(QNetworkReply* reply);

private:
    QNetworkAccessManager* m_manager;
    QByteArray m_context;
};

#endif