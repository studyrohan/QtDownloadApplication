#include "AuthorizationDownloader.h"
#include "AuthorizationLevel.h"
#include <QTcpSocket>
#include <QStringList>
#include <QCryptographicHash>
<<<<<<< HEAD
#include <QDateTime>
#include <QTimer>
=======
>>>>>>> f6348b744c8929594a95d06698e6552b3d329b46


static const std::vector<std::string> gs_authorizationCode = { "3VKKZ85TA8QRNSJT3HK3EC5NE82CPNH39A2DKGHC3X7UPHJ87A7KDDTRTXS7KZT3PCVUVN"
												"ME3LLCNJP33XMC2NPA78LUNGJQ7RADM8TWWCTD2AP7", //guochuang license
												"NCMD2D5EK8JDNAMCPACZGNJZ98HDEDPFJA7ZGCMLR8LZKWHAVAVDMZH57RXD2GMUHXMDPC"
												"TCRA8ZPAJAR3K3AZH8VRRZAZ5US8NRGJN7QH2CSS2J",
												"KERDFE2B3HJKZ85NQR3FWN2H7V8FFQDFA3D3A8JUVGJLDAHNN8QKEZ5FN8CNEQDQ3GNMMZ"
												"22PEQKVQZV9CMUFE8UR3K3AUJNRV6MWEPDRGMLUY8B" };



AuthorizationDownloader::AuthorizationDownloader(QObject* parent) : QTcpServer(parent) {
	connect(this, &QTcpServer::newConnection, this, &AuthorizationDownloader::handleNewConnection);
}

QString AuthorizationDownloader::encryptData(const QString& data, const QString& key)
{
	QByteArray hash = QCryptographicHash::hash(key.toUtf8(), QCryptographicHash::Md5);
	QByteArray encryptedData = data.toUtf8();

	for (int i = 0; i < encryptedData.size(); ++i)
	{
		encryptedData[i] = encryptedData[i] ^ hash.at(i % hash.size());
	}

	return QString(encryptedData.toHex());
}

QString AuthorizationDownloader::decryptData(const QString& data, const QString& key)
{
	QByteArray hash = QCryptographicHash::hash(key.toUtf8(), QCryptographicHash::Md5);
	QByteArray decryptedData = QByteArray::fromHex(data.toUtf8());

	for (int i = 0; i < decryptedData.size(); ++i)
	{
		decryptedData[i] = decryptedData[i] ^ hash.at(i % hash.size());
	}

	return QString(decryptedData);
}

void AuthorizationDownloader::incomingConnection(qintptr socketDescriptor)
{
	QTcpSocket* clientSocket = new QTcpSocket(this);
	clientSocket->setSocketDescriptor(socketDescriptor);
<<<<<<< HEAD
	QString ip = clientSocket->peerAddress().toString();
	if (shouldAcceptConnection(ip))
	{
		connect(clientSocket, &QTcpSocket::readyRead, this, &AuthorizationDownloader::readClientData);
		connect(clientSocket, &QTcpSocket::disconnected, clientSocket, &QTcpSocket::deleteLater);
	}
	else
	{
		qWarning() << "too much connection from" << ip;
	}
}

bool AuthorizationDownloader::shouldAcceptConnection(const QString& ip)
{
	// 例如，每分钟最多10个请求
	int maxRequestsPerMinute = 3;
	QDateTime now = QDateTime::currentDateTime();
	QString key = ip + now.toString("yyyyMMddHHmm");

	if (!ipRequestCounts.contains(key)) {
		ipRequestCounts[key] = 1;
		QTimer::singleShot(60000, [this, key]() { // 1分钟后清除计数
			ipRequestCounts.remove(key);
			});
		return true;
	}
	else if (ipRequestCounts[key] < maxRequestsPerMinute) {
		ipRequestCounts[key]++;
		return true;
	}
	else {
		return false; // 超过限制
	}
=======
	connect(clientSocket, &QTcpSocket::readyRead, this, &AuthorizationDownloader::readClientData);
	connect(clientSocket, &QTcpSocket::disconnected, clientSocket, &QTcpSocket::deleteLater);
>>>>>>> f6348b744c8929594a95d06698e6552b3d329b46
}

void AuthorizationDownloader::handleNewConnection()
{
	QTcpSocket* clientSocket = nextPendingConnection();
	if (clientSocket == nullptr)
	{
		return;
	}
<<<<<<< HEAD
	
=======
>>>>>>> f6348b744c8929594a95d06698e6552b3d329b46
	qDebug() << "New connection from" << clientSocket->peerAddress().toString();
}

void AuthorizationDownloader::readClientData()
{
	QTcpSocket* clientSocket = qobject_cast<QTcpSocket*>(sender());
	if (clientSocket) {
		QByteArray data = clientSocket->readAll();
		//do not change this key,client use it decode 
		const static QString key = "ertyu6789ghjkvbnm";
		int level = data.toInt();
		QString encrypt;
		switch (AuthorizationLevel(level))
		{
		case AuthorizationLevel0:
			encrypt = encryptData(QString::fromStdString(gs_authorizationCode[0]), key);
			clientSocket->write(encrypt.toStdString().c_str());
			break;
		case AuthorizationLevel1:
			encrypt = encryptData(QString::fromStdString(gs_authorizationCode[1]), key);
			clientSocket->write(encrypt.toStdString().c_str());
			break;
		case AuthorizationLevel2:
			encrypt = encryptData(QString::fromStdString(gs_authorizationCode[2]), key);
			clientSocket->write(encrypt.toStdString().c_str());
			break;
		default:
			clientSocket->write("error");
			break;
		}
		//test
		auto str = decryptData(encrypt, key).toStdString();
	}
}
