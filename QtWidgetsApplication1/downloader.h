
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
    void CreateLogFolder(const QString& path);
    void UploadLog(const QString &path);
    void SendFileByTcp(const QString &path);
    void DownloadlicensFile(int level);
    void SendFileByHttp(const QString &path);
    QString GetLocalIP()const;
    QByteArray GetContext() const;

    void AppendResult(const QString& res){m_result .append(res);}
    void ClearResult(){m_result = "";}
    QString GetResult(){return m_result;}
signals:
    void updateProgress(qint64 receive, qint64 total, qreal progress);

public slots:
    void ReplyFinished(QNetworkReply* reply);
    void UpdateLog(const QString& fileName, const QString& userName);
    void DealProgress(qint64 bytes, qint64 total);
private:
    QNetworkAccessManager* m_manager;
    QByteArray m_context;
    QString m_result;
};

#endif