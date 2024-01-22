
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
    void DownloadResource(const QString& res,const QString& path);

    void DoDownload();
    void UploadLog(const QString &path);

    QByteArray GetContext() const;

    QString result;
signals:

public slots:
    void ReplyFinished(QNetworkReply* reply);
    void UpdateLog(const QString& fileName, const QString& userName);

private:
    QNetworkAccessManager* m_manager;
    QByteArray m_context;
    
};

#endif