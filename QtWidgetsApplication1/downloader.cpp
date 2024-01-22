#include "downloader.h"
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QNetworkInterface>
#include <QtNetwork/QHttpMultiPart>
#include <QCoreApplication>
#include <QXmlStreamReader>
#include <QRegularExpression>
#include <QEventLoop>
#include <QFileInfo>
#include <QMessageBox>


Downloader::Downloader(QObject* parent)
    :QObject(parent)
{
    m_manager = new QNetworkAccessManager(this);
}

QList< QString> Downloader::GetAllResource(QString url)
{
    QList< QString>list;
    QNetworkReply* reply = m_manager->get(QNetworkRequest(QUrl("http://192.168.8.222:8080/Overdrive/version.txt")));
	QEventLoop eventLoop;
	connect(reply, &QNetworkReply::finished, &eventLoop, &QEventLoop::quit);
	eventLoop.exec(QEventLoop::ExcludeUserInputEvents);
	if (reply->isFinished())
	{	
		if (reply->error())
		{
			result = "ERROR!\n";
			result+= reply->errorString();
		}
		else
		{

			QByteArray data = reply->readAll();
			QString res(data);
            list = res.split('\n');
		}
	}
    reply->deleteLater();
    return list;
}


void Downloader::DownloadResource(const QString& res,const QString& path)
{
    QNetworkReply* reply = m_manager->get(QNetworkRequest(QUrl(res)));
    QEventLoop eventLoop;
    connect(reply, &QNetworkReply::finished, &eventLoop, &QEventLoop::quit);
    eventLoop.exec(QEventLoop::ExcludeUserInputEvents);
    try {
        int startIndex = res.lastIndexOf("/");
        QString filename = res.mid(startIndex + 1);
        QFile file(path + '/' + filename);

        if (!file.open(QIODevice::WriteOnly)) {
            // Handle error
            result = "Failed to open file\n"; // throw exception
            if (QFileInfo(path + '/' + filename).isFile())
            {
                result += "The File is exist";
            }
        }
        else {
            file.write(reply->readAll());
            file.close();
            result= filename + QString(" :The download has finished!");
        }
    }
    catch (std::exception e)
    {
        result = "File Error"+ QString(e.what());
    }
    reply->deleteLater();
}

void Downloader::DoDownload()
{
    m_context.clear();
    connect(m_manager, SIGNAL(finished(QNetworkReply*)),
        this, SLOT(ReplyFinished(QNetworkReply*)), Qt::DirectConnection);

    m_manager->get(QNetworkRequest(QUrl("http://192.168.8.222:8080/Overdrive/version.txt")));
}

QByteArray Downloader::GetContext()const
{
    return m_context;
}

void Downloader::ReplyFinished(QNetworkReply* reply)
{
	if (reply->error())
	{
		qDebug() << "ERROR!";
		qDebug() << reply->errorString();
	}
	else
    {
        qDebug() << reply->header(QNetworkRequest::ContentTypeHeader).toString();
        qDebug() << reply->header(QNetworkRequest::LastModifiedHeader).toDateTime().toString();;
        qDebug() << reply->header(QNetworkRequest::ContentLengthHeader).toULongLong();
        qDebug() << reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
        qDebug() << reply->attribute(QNetworkRequest::HttpReasonPhraseAttribute).toString();

        QFile* file = new QFile("downloaded.txt");
        if (file->open(QFile::ReadWrite))
        {
            m_context = reply->readAll();
            file->write(m_context);
            file->flush();
            file->close();
        }
        delete file;
    }

    reply->deleteLater();
}

void Downloader::UploadLog(const QString& path)
{
    QEventLoop loop;
    QFileInfo fileInfo(path);
    QString fileName = fileInfo.fileName();
    QHttpMultiPart* multiPart = new QHttpMultiPart(QHttpMultiPart::FormDataType);
    QHttpPart filePart;
   
    filePart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"filename\"; filename=\"" + fileName + "\""));
    filePart.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("text/plain"));
    QFile* file = new QFile(path);
    if (!file->open(QIODevice::ReadOnly))
    {
        result= "File open Failed";
    }
    else {
        filePart.setBody(file->readAll());
        file->setParent(multiPart);
        file->close();
        multiPart->append(filePart);

        //request post
        QUrl url = QString("http://192.168.8.222:8080/test/");
        
        QNetworkRequest request;
        request.setUrl(url); 
        request.setRawHeader("Content-Type", "multipart/form-data");
        request.setHeader(QNetworkRequest::ContentLengthHeader, QVariant(multiPart->boundary().length() + file->size()));
        QNetworkReply* reply= m_manager->post(request, multiPart);
        multiPart->setParent(reply);
        
        connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
        loop.exec();
        if (reply->isFinished())
        {
            if (reply->error())
            {
                result = "upload error:" + reply->errorString();
            }
            else
            {
                result+= "Header:"+ reply->header(QNetworkRequest::ContentTypeHeader).toString()+"\n";
                result+= reply->header(QNetworkRequest::LastModifiedHeader).toDateTime().toString()+"\n";
                result+= reply->header(QNetworkRequest::ContentLengthHeader).toULongLong() + "\n";
                result+= "attribute:" + reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
                result += "\n";
                result+= reply->attribute(QNetworkRequest::HttpReasonPhraseAttribute).toString() + "\n";

                result += fileName + ": the Upload has finished!";
                reply->deleteLater();
            }
            UpdateLog(fileName, url.userName());
        }
    }
}

void Downloader::UpdateLog(const QString& fileName, const QString& userName)
{
    QEventLoop loop;
    QString logResult,ipAddress;
    logResult = " fileName : " + fileName + " userName : " + userName;
    //get ipList
    QList<QHostAddress> ipList = QNetworkInterface::allAddresses();
    for (QHostAddress ip : ipList)
    {
        if (ip.protocol() == QAbstractSocket::IPv4Protocol && ip != QHostAddress::LocalHost)
        {
            ipAddress = QString(" IP : ").append(ip.toString());
            
        }
    }
    logResult += ipAddress;
    QString url = "http://192.168.8.222:8080/test/UploadLogs.txt";
    QNetworkRequest request(url);
    //request.setHeader(QNetworkRequest::ContentLengthHeader, QVariant(logResult.length()));
    request.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("text/plain"));
    //add to 
    QNetworkReply* reply = m_manager->post(request,logResult.toUtf8());
    connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    loop.exec();
    if (reply->isFinished())
    {
        if (reply->error() == QNetworkReply::NoError)
        {
            result += logResult + '\n';
            result += "Logs update!";
            reply->deleteLater();
        }
        else
        {
            result += reply->errorString();
        }
    }

}