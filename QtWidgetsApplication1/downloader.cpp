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
#include <QFileDialog>
#include <QFileDialog>
#include <qprocess.h>
#include <QDirIterator>

QString g_DownloadPath = "http://110.41.63.246:8080";
QString g_ServerPath = "110.41.63.246";

Downloader::Downloader(QObject* parent)
    :QObject(parent)
{
    m_manager = new QNetworkAccessManager(this);
}

QList< QString> Downloader::GetAllResource(QString url)
{
    QList< QString>list;
    QNetworkReply* reply = m_manager->get(QNetworkRequest(QUrl(g_DownloadPath +"/version.txt")));
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

void Downloader::DownloadResource(const QString& res, const QString& path)
{
    ClearResult();
    QNetworkReply* reply = m_manager->get(QNetworkRequest(QUrl(res)));
    QEventLoop eventLoop;
    connect(reply, &QNetworkReply::finished, &eventLoop, &QEventLoop::quit);
    connect(reply, &QNetworkReply::downloadProgress, this, &Downloader::DealProgress);
    eventLoop.exec(QEventLoop::ExcludeUserInputEvents);

    if (reply->error())
    {
        qDebug() << "ERROR!";
        QString str = reply->errorString();
    }
    else
    {
        qDebug() << reply->header(QNetworkRequest::ContentTypeHeader).toString();
        qDebug() << reply->header(QNetworkRequest::LastModifiedHeader).toDateTime().toString();;
        qDebug() << reply->header(QNetworkRequest::ContentLengthHeader).toULongLong();
        qDebug() << reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
        qDebug() << reply->attribute(QNetworkRequest::HttpReasonPhraseAttribute).toString();

    }

    qint64 fileSize = reply->size();
    AppendResult( "File size:" +QString::number(fileSize/(2048*2048))+" MB\n");

    int startIndex = res.lastIndexOf("/");
    QString filename = res.mid(startIndex + 1);
    QFile file(path + '/' + filename);
    try {
        if (!file.open(QIODevice::WriteOnly)) {
            // Handle error
             // throw exception
            AppendResult("Failed to open file!");
            if (QFileInfo(path + '/' + filename).isFile())
            {
                AppendResult("The File is exist\n");
            }
        }
        else {
            file.write(reply->readAll());
            file.close();
            AppendResult(filename + QString(" :The download has finished!\n"));
            reply->deleteLater();
        }
    }
    catch (std::exception e)
    {
        AppendResult( "File Error"+ QString(e.what())+'\n');
    }
    //extract
    QString archiveFilePath = path + "/" + filename;
    QString extractpath = path;
    ExtractResource(archiveFilePath, extractpath);
}

void Downloader::ExtractResource(const QString& archiveFilePath, const QString& extractPath)
{
    QString sevenZipPath = QDir::currentPath() + "/7z.exe";
    QProcess process;
    QStringList arguments;
    arguments << "x" << "-o" + extractPath << archiveFilePath;
    process.start(sevenZipPath, arguments);
    process.waitForFinished();

    if (process.exitCode() == 0)
    {
        AppendResult("Extract successfully");
    }
        
    else
    {
        AppendResult("Extract failed!");
    }
}

void Downloader::DoDownload()
{
    m_context.clear();
    connect(m_manager, SIGNAL(finished(QNetworkReply*)),
        this, SLOT(ReplyFinished(QNetworkReply*)), Qt::UniqueConnection);
    m_manager->get(QNetworkRequest(QUrl(g_DownloadPath+ "/latestVersion.txt")));
}

QByteArray Downloader::GetContext()const
{
    return m_context;
}

QString Downloader::DecryptData(const QString& data, const QString& key)
{
	QByteArray hash = QCryptographicHash::hash(key.toUtf8(), QCryptographicHash::Md5);
	QByteArray decryptedData = QByteArray::fromHex(data.toUtf8());

	for (int i = 0; i < decryptedData.size(); ++i)
	{
		decryptedData[i] = decryptedData[i] ^ hash.at(i % hash.size());
	}

	return QString(decryptedData);
}

void Downloader::ReplyFinished(QNetworkReply* reply)
{
    disconnect(m_manager, SIGNAL(finished(QNetworkReply*)),
        this, SLOT(ReplyFinished(QNetworkReply*)));
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

       m_context = reply->readAll();
    }
    reply->deleteLater();
    emit getdone();
}

void Downloader::UpdatePackage(const QString& savePath)
{
    QString localPackage(savePath + "/OverdriveSDK");
    QString latest(savePath + "/temp/OverdriveSDK");
    if (!QFile::exists(localPackage))
    {
        if (!MoveDirectory(latest, savePath))
        {
            qDebug() << "move failed" << endl;
        }
        QString tempPath = savePath + "/temp";
        QDir dir(tempPath);
        dir.removeRecursively();
        QMessageBox::information(nullptr, "update", "update successfully, please check the package");
    }
    
    else
    {
        QString backupPackage(savePath + "/backup");
        QDir saveDir(savePath);
        QDir backupDir(backupPackage);
        if (saveDir.exists("backup"))
        {
            backupDir.removeRecursively();
        }
        saveDir.mkdir("backup");

        if (!MoveDirectory(localPackage, backupPackage))
        {
            qDebug() << "backup failed!" << endl;
            backupDir.removeRecursively();
        }

        QDir tempdir(savePath + "/temp");
        if (!MoveDirectory(latest, savePath))
        {
            qDebug() << "move failed" <<endl;
            if (!MoveDirectory(backupPackage, savePath))
            {
                qDebug() << "restore failed, original version is in backup" << endl;
            }

            tempdir.removeRecursively();
            QMessageBox::critical(nullptr, "Error", "move error, Update failed.");
            return;
        }
        backupDir.removeRecursively();
        tempdir.removeRecursively();
        QMessageBox::information(nullptr, "update", "Update successfully!");
    }
}

bool Downloader::MoveDirectory(const QString& sourceDirPath, const QString& destinationDirPath)
{
    QDir sourceDir(sourceDirPath);
    QDir destinationDir(destinationDirPath);

    if (!sourceDir.exists() || !destinationDir.exists()) {
        return false;
    }

    QString destinationPath = destinationDir.filePath(sourceDir.dirName());

    if (QFile::exists(destinationPath)) {
        return false;
    }

    if (!sourceDir.rename(sourceDirPath, destinationPath)) {
        return false;
    }

    return true;
}

void Downloader::CreateLogFolder(const QString& path)
{
    //create ip folder
    QEventLoop loop;
    QString folderName = GetLocalIP();
    QString folderPath = "C:/Users/Administrator/Desktop/test/myfolder";
    QString url = QString(g_DownloadPath+"/test/upload");
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

void Downloader::SendFileByTcp(const QString& path)
{
    ClearResult();

	QFile file(path);
	if (!file.open(QIODevice::ReadOnly)) {
		qDebug() << "Failed to open file for reading: " << file.errorString();
		AppendResult("Failed to open file for reading: " + file.errorString());
		return;
	}

    QTcpSocket socket;

    socket.connectToHost(g_ServerPath, 1234);
    if (!socket.waitForConnected()) {
        AppendResult("Failed to connect to server: " + socket.errorString());
        return;
    }

    // Read the file data into a QByteArray
    QByteArray fileData = file.readAll();
	QFileInfo fileInfo(file);

	//  获取文件名，不包含目录路径
	QString fileName = fileInfo.fileName();
    file.close();
    socket.write(fileName.toLocal8Bit());
    socket.write("\n");
    // Write the file data to the socket
    qint64 bytesWritten = socket.write(fileData);
    if (bytesWritten == -1) {
        qDebug() << "Failed to write file data to socket: " << socket.errorString();
        AppendResult("Failed to write file data to socket: " + socket.errorString());
        return;
    }
    socket.write("---boundary---");
    // Flush the socket to ensure all data is sent
    socket.flush();
    // to deal with
    //if (!socket.waitForBytesWritten()) {
    //    qDebug() << "Failed to flush socket: " << socket.errorString();
    //    AppendResult("Failed to flush socket: " + socket.errorString());
    //    return;
    //}
    AppendResult("succeed!");
}

void Downloader::DownloadlicensFile(int level)
{
	ClearResult();
	QString dir = QFileDialog::getExistingDirectory(nullptr, tr("Open Directory"), "/home", QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    if (dir.isEmpty())
    {
        return;
    }
	QTcpSocket socket;

	socket.connectToHost(g_ServerPath, 1246);
	if (!socket.waitForConnected()) {
		AppendResult("Failed to connect to server: " + socket.errorString());
		return;
	}
	// Send data
	socket.write(std::to_string(level).c_str());
	socket.flush();

	// Wait for the reply
	if (!socket.waitForReadyRead(5000)) {
		qDebug() << "No reply from the server: " << socket.errorString();
		return ;
	}

	// Read the reply
	QByteArray reply = socket.readAll();
    //do not change this key
    const static QString key = "ertyu6789ghjkvbnm";
    QString decrytReply = DecryptData(QString::fromUtf8(reply), key);
	qDebug() << "Received reply: " << reply;
    if (reply.size() == -1) {
        qDebug() << "Failed to download license:" << socket.errorString();
        AppendResult("Failed to download license: " + socket.errorString());
        return;
    }
	QFile file(dir.append("/overdrivelicense.h"));
	if (!file.open(QIODevice::WriteOnly)) {
		qDebug() << "Failed to open file for writing: " << file.errorString();
		AppendResult("Failed to open file for writing: " + file.errorString());
		return;
	}
    QString str = "/* (C) Copyright 2023, ZWSOFT CO., LTD.(Guangzhou)All Rights Reserved.*/\n\
#ifndef OVERDRIVE_LICENSE_DEFINED\n\
#define OVERDRIVE_LICENSE_DEFINED\n\
#define OVERDRIVE_LICENSE     \"code\"\n\
#endif\n";
    str.replace("code", decrytReply.toUtf8());
    file.write(str.toUtf8());
    file.flush();
    file.close();
    QMessageBox::information(NULL, "", "download license succeed,please check the overdrivelicense.h file");
}

void Downloader::VerifylicenseFile()
{
    QString filePath = QFileDialog::getOpenFileName(nullptr, QObject::tr("Open License File"), "", QObject::tr("License Files (*.h)"));
    if (filePath.isEmpty())
    {
        return;
    }
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qDebug() << "Failed to open file " << file.errorString();
        return;
    }

    QTextStream in(&file);
    QString licenseContent = in.readAll();
    file.close();

    QString code;
    QRegularExpression re("\"([^\"]*)\"");
    QRegularExpressionMatch match = re.match(licenseContent);

    if (match.hasMatch()) {
        code = match.captured(1);
    }
    
    static const std::vector<std::string> gs_authorizationCode = { "3VKKZ85TA8QRNSJT3HK3EC5NE82CPNH39A2DKGHC3X7UPHJ87A7KDDTRTXS7KZT3PCVUVN"
                                                    "ME3LLCNJP33XMC2NPA78LUNGJQ7RADM8TWWCTD2AP7", //guochuang license
                                                    "NCMD2D5EK8JDNAMCPACZGNJZ98HDEDPFJA7ZGCMLR8LZKWHAVAVDMZH57RXD2GMUHXMDPC"
                                                    "TCRA8ZPAJAR3K3AZH8VRRZAZ5US8NRGJN7QH2CSS2J",
                                                    "KERDFE2B3HJKZ85NQR3FWN2H7V8FFQDFA3D3A8JUVGJLDAHNN8QKEZ5FN8CNEQDQ3GNMMZ"
                                                    "22PEQKVQZV9CMUFE8UR3K3AUJNRV6MWEPDRGMLUY8B" };

    QString licenseDate;
    QString title = "verify license";
    QString message = "license validity date:";
    for (int level = 0; level < gs_authorizationCode.size(); ++ level)
    {
        if (gs_authorizationCode[level].c_str() == code)
        {
            switch (level)
            {
            case 0:
                licenseDate = s_LeveltoTime[AuthorizationLevel0];
                break;
            case 1:
                licenseDate = s_LeveltoTime[AuthorizationLevel1];
                break;
            case 2:
                licenseDate = s_LeveltoTime[AuthorizationLevel2];
                break;
            default:
                QMessageBox::critical(nullptr, "warning", "unknown error");
                break;
            }

            if (licenseDate == "permanent")
            {
                message += "permanent\nverify successfully";
            }
            else 
            {
                QString currentDate = QDate::currentDate().toString("yyyy-MM-dd");
                if (currentDate < licenseDate) message = message + licenseDate + "\nverify successfully";
                else message = message + licenseDate + "\nLicense has expired! verify failed!";
            }

            QMessageBox::information(NULL, title, message,
                QMessageBox::Yes, QMessageBox::Yes);
            return;
        }
    }
    QMessageBox::critical(NULL, title, "invalid license!",
        QMessageBox::Yes, QMessageBox::Yes);
}

void Downloader::SendFileByHttp(const QString& path)
{
	QEventLoop loop;
	QFileInfo fileInfo(path);
	QString fileName = fileInfo.fileName();
	QHttpMultiPart* multiPart = new QHttpMultiPart(QHttpMultiPart::FormDataType);
	{
		QHttpPart textPart;
		textPart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"text\""));
		textPart.setBody("Hello, this is a text field!");
		//multiPart->append(textPart);
	}
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
		//filePart.setBodyDevice(file);
		file->setParent(multiPart); // The file will be deleted with the multiPart
		QByteArray fileContext = file->readAll();
		int contextSize = fileContext.size();

		QString str = QString::fromLocal8Bit(fileContext);
		filePart.setBody(fileContext);
		//file->close();
		multiPart->append(filePart);
		//multiPart->setBoundary("--boundary--");

		//request post
		QUrl url = QString("http://localhost:1234/");

		QNetworkRequest request;
		request.setUrl(url);
		request.setRawHeader("Content-Type", "multipart/form-data");
		//qDebug()<< QByteArray::number(multiPart->boundary().length() + file->size());
		//request.setRawHeader("Content-Length",QByteArray::number(file->size()));
		//request.setRawHeader("Content-Length",QByteArray::number(multiPart->boundary().length()+file->size()));

		QNetworkReply* reply = m_manager->post(request, multiPart);
		//QNetworkReply* reply = m_manager->post(request, file);
		multiPart->setParent(reply); //  设置reply为multiPart的父对象，这样当reply被删除时，multiPart也会被删除
		qint64 contentLength = reply->header(QNetworkRequest::ContentLengthHeader).toLongLong();
		connect(reply, &QNetworkReply::uploadProgress, this, &Downloader::DealProgress);
		connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
		loop.exec();
		if (reply->isFinished())
		{
			QNetworkReply::NetworkError err = reply->error();
			if (err != QNetworkReply::NoError)
			{
				AppendResult("upload error:" + reply->errorString());
			}
			else
			{
				AppendResult(fileName + ": the Upload has finished!\n");
			}
			reply->deleteLater();
			//UpdateLog(fileName, url.userName());
		}
	}
}

void Downloader::UploadLog(const QString& path)
{
    return SendFileByTcp(path);
    // we dont know how to deal with http request ,use tcp is much easier but my lack of information
    //return SendFileByHttp(path);
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
    QString url = g_DownloadPath+"/test/logs.txt";
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
            QString url = g_DownloadPath+"/test/";
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

void Downloader::DealProgress(qint64 bytes, qint64 total)
{
    qreal progress = bytes * 100 / (total+0.1) ;
    emit updateProgress(bytes, total, progress);
}