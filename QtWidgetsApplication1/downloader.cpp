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
            ClearResult();
            AppendResult("ERROR!\n" + reply->errorString());
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
    ClearResult();
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
             // throw exception
            if (QFileInfo(path + '/' + filename).isFile())
            {
                AppendResult("Failed to open file!\n The File is exist\n");
            }
        }
        else {
            file.write(reply->readAll());
            file.close();
            AppendResult(filename + QString(" :The download has finished!\n"));
        }
    }
    catch (std::exception e)
    {
        AppendResult( "File Error"+ QString(e.what())+'\n');
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

void Downloader::CreateLogFolder(const QString& path)
{
    //create ip folder
    QEventLoop loop;
    QString folderName = GetLocalIP();
    QString folderPath = "C:/Users/Administrator/Desktop/test/myfolder";
    QString url = QString("http://192.168.8.222:8080/test/upload");
    QNetworkRequest folderRequest(url);
    folderRequest.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("application/json"));
    QString jsonData = QString("{\"folderName\":\"%1\",\"parentDirectory\":\"%2\"}")
        .arg(folderName)
        .arg(url);
    //need to set http head
    QNetworkReply* folderReply = m_manager->put(folderRequest, jsonData.toUtf8());
    connect(folderReply, SIGNAL(finished()), &loop, SLOT(quit()));
    loop.exec();
    Q_UNUSED(folderReply);
    QMessageBox::information(nullptr,"error",folderReply->readAll());
    if (folderReply->isFinished())
    {
        if (folderReply->error())
        {
            AppendResult("new folder build error:" + folderReply->errorString());
            
        }
        else {
            AppendResult(folderName + ": the folder has bulid!\n");
            //UploadLog(path);
        }
        folderReply->deleteLater();
    }
}

void Downloader::UploadLog(const QString& path)
{
    QEventLoop loop;
    QFileInfo fileInfo(path);
    QString fileName = fileInfo.fileName();
    QHttpMultiPart* multiPart = new QHttpMultiPart(QHttpMultiPart::FormDataType);
    QHttpPart filePart;
    ClearResult();
    
    if (fileInfo.suffix() == "txt")
    {  
        filePart.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("text/plain"));
    }
    else
    {
        filePart.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("application/octet-stream"));
    }
    filePart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"filename\"; filename=\"" + fileName + "\""));
    QFile* file = new QFile(path);
    if (!file->open(QIODevice::ReadOnly))
    {
        AppendResult("File didn't open!");
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
        request.setRawHeader("Connection", "keep-alive");
        request.setRawHeader("Content-Length",QByteArray::number(multiPart->boundary().length()+file->size()));
        //request.setRawHeader("Transfer-Encoding", "chunked");
        QNetworkReply* reply= m_manager->post(request, multiPart);
        multiPart->setParent(reply);

        connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
        loop.exec();
        if (reply->isFinished())
        {
            if (reply->error())
            {
                AppendResult("upload error:" + reply->errorString());
            }
            else
            {
                AppendResult("Header:" + reply->header(QNetworkRequest::ContentTypeHeader).toString() + "\n" +
                    reply->header(QNetworkRequest::LastModifiedHeader).toDateTime().toString() + "\n" +
                    reply->header(QNetworkRequest::ContentLengthHeader).toULongLong() + "\n" +
                    "attribute:" + reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt() +
                    "\n" + reply->attribute(QNetworkRequest::HttpReasonPhraseAttribute).toString() + "\n" +
                    fileName + ": the Upload has finished!\n");             
            }
            reply->deleteLater();
            UpdateLog(fileName, url.userName());
        }
    }
}

QString Downloader::GetLocalIP()const
{
    QString ipAddress;
    QList<QHostAddress> ipList = QNetworkInterface::allAddresses();
    for (QHostAddress ip : ipList)
    {
        if (ip.protocol() == QAbstractSocket::IPv4Protocol && ip != QHostAddress::LocalHost)
        {
            ipAddress = ip.toString();
        }
    }
    return ipAddress;
}

void Downloader::UpdateLog(const QString& fileName, const QString& userName)
{
    QEventLoop loop;
    QString url = "http://192.168.8.222:8080/test/logs.txt";
    QNetworkRequest request(url);
    //request.setHeader(QNetworkRequest::ContentLengthHeader, QVariant(logResult.length()));
    //add to
    QNetworkReply* reply = m_manager->get(request);
    connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    loop.exec();
    QByteArray previous = reply->readAll();
    if (reply->isFinished())
    {
        if (reply->error() == QNetworkReply::NoError)
        {
            QString later = "ip: " + GetLocalIP() + " filename: " + fileName + " username: " + userName;
            QString url = "http://192.168.8.222:8080/test/";
            QNetworkRequest request(url);
            QHttpMultiPart* multiPart = new QHttpMultiPart(QHttpMultiPart::FormDataType);
            QHttpPart newPart;
            QFile* file=new QFile;
            newPart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"filename\"; filename=\"logs.txt\""));
            newPart.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("text/plain"));
            QString s =file->readAll();
            QMessageBox::information(nullptr, "", s);
            if (file->open(QIODevice::ReadWrite))
            {
                file->write(previous);
                newPart.setBody(file->readAll());
                file->setParent(multiPart);
                file->flush();
                file->close();
            }
            else
            {
                AppendResult("\nFile open failed!");
            }
            delete file;
            multiPart->append(newPart);
            request.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("multipart/form-data"));
            QNetworkReply* postReply = m_manager->post(request, multiPart);
            multiPart->setParent(postReply);
            connect(postReply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
            loop.exec();
            if (postReply->isFinished())
            {
                if (postReply->error() == QNetworkReply::NoError)
                {
                    AppendResult("\nLogs update!");
                }
                else
                {
                    AppendResult("post update logs:" + postReply->errorString());
                }
                postReply->deleteLater();
            }
        }
        else
        {
            AppendResult("get previous logs:"+reply->errorString());
        }
        reply->deleteLater();
    }
}