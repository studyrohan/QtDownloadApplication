
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

static const std::vector<std::string> gs_authorizationCode = { "3VKKZ85TA8QRNSJT3HK3EC5NE82CPNH39A2DKGHC3X7UPHJ87A7KDDTRTXS7KZT3PCVUVN"
                                                "ME3LLCNJP33XMC2NPA78LUNGJQ7RADM8TWWCTD2AP7", //guochuang license
                                                "NCMD2D5EK8JDNAMCPACZGNJZ98HDEDPFJA7ZGCMLR8LZKWHAVAVDMZH57RXD2GMUHXMDPC"
                                                "TCRA8ZPAJAR3K3AZH8VRRZAZ5US8NRGJN7QH2CSS2J",
                                                "KERDFE2B3HJKZ85NQR3FWN2H7V8FFQDFA3D3A8JUVGJLDAHNN8QKEZ5FN8CNEQDQ3GNMMZ"
                                                "22PEQKVQZV9CMUFE8UR3K3AUJNRV6MWEPDRGMLUY8B" };

class Downloader : public QObject
{
    Q_OBJECT
public:
    explicit Downloader(QObject* parent = 0);

    QList< QString>GetAllResource(QString url="");
<<<<<<< HEAD
    void DownloadResource(const QString& res, const QString& path);
=======
    void DownloadResource(const QString& res,const QString& path);
>>>>>>> f6348b744c8929594a95d06698e6552b3d329b46
    void DoDownload();
    void CreateLogFolder(const QString& path);
    void UploadLog(const QString &path);
    void SendFileByTcp(const QString &path);
    void DownloadlicensFile(int level);
<<<<<<< HEAD
    void ExtractResource(const QString& archiveFilePath, const QString& extractPath);
    void UpdatePackage();
    bool moveDirectory(const QString& sourceDirPath, const QString& destinationDirPath);
    void VerifylicenseFile(int level);
=======
>>>>>>> f6348b744c8929594a95d06698e6552b3d329b46
    void SendFileByHttp(const QString &path);
    QString GetLocalIP()const;
    QByteArray GetContext() const;
    void AppendResult(const QString& res){m_result .append(res);}
    void ClearResult(){m_result = "";}
    QString GetResult(){return m_result;}
private:
    QString DecryptData(const QString& data, const QString& key);
signals:
    void updateProgress(qint64 receive, qint64 total, qreal progress);

public slots:
    void ReplyFinished(QNetworkReply* reply);
    void UpdateLog(const QString& fileName, const QString& userName);
    void DealProgress(qint64 bytes, qint64 total);
private:
    QNetworkAccessManager* m_manager;
    QByteArray m_context;
    QString latestVersion;
    QString m_result;
};

#endif