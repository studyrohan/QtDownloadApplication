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
#include "AuthorizationLevel.h"

static std::map<AuthorizationLevel, QString> s_LeveltoTime =
{
    {AuthorizationLevel::AuthorizationLevel0, QDate(2024,8,30).toString("yyyy-MM-dd")},
    {AuthorizationLevel::AuthorizationLevel1, QDate(2024,12,30).toString("yyyy-MM-dd")},
    {AuthorizationLevel::AuthorizationLevel2, "permanent"}
};

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
    void ExtractResource(const QString& archiveFilePath, const QString& extractPath);
    void UpdatePackage(const QString& savePath);
    bool MoveDirectory(const QString& sourceDirPath, const QString& destinationDirPath);
    void VerifylicenseFile();
    QString GetLocalIP()const;
    QByteArray GetContext() const;
    void AppendResult(const QString& res){m_result .append(res);}
    void ClearResult(){m_result = "";}
    QString GetResult(){return m_result;}
private:
    QString DecryptData(const QString& data, const QString& key);
signals:
    void updateProgress(qint64 receive, qint64 total, qreal progress);
    void getdone();
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