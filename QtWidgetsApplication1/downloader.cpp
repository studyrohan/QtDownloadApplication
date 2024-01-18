#include "downloader.h"
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>
#include <QXmlStreamReader>
#include <QRegularExpression>
#include <QEventLoop>


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
			qDebug() << "ERROR!";
			qDebug() << reply->errorString();
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

void Downloader::DownloadResource(const QString& res)
{
    QNetworkReply* reply = m_manager->get(QNetworkRequest(QUrl(res)));
    QEventLoop eventLoop;
    connect(reply, &QNetworkReply::finished, &eventLoop, &QEventLoop::quit);
    eventLoop.exec(QEventLoop::ExcludeUserInputEvents);
    QFile file("file.zip");
    if (!file.open(QIODevice::WriteOnly)) {
        // Handle error
    }
    else {
        file.write(reply->readAll());
        file.close();
    }
    reply->deleteLater();
}

void Downloader::DoDownload()
{
    m_context.clear();
    

    connect(m_manager, SIGNAL(finished(QNetworkReply*)),
        this, SLOT(ReplyFinished(QNetworkReply*)), Qt::DirectConnection);

    m_manager->get(QNetworkRequest(QUrl("http://192.168.8.222:8080/test/hello.txt")));
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