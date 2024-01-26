
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
    QString GetLocalIP()const;
    QByteArray GetContext() const;

    void AppendResult(const QString& res){m_result .append(res);}
    void ClearResult(){m_result = "";}
    QString GetResult(){return m_result;}
signals:

public slots:
    void ReplyFinished(QNetworkReply* reply);
    void UpdateLog(const QString& fileName, const QString& userName);

private:
    QNetworkAccessManager* m_manager;
    QByteArray m_context;
    QString m_result;
};

#endif