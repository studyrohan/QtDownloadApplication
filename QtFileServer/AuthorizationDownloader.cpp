#include "AuthorizationDownloader.h"
#include "AuthorizationLevel.h"
#include <QTcpSocket>
#include <QStringList>
#include <QCryptographicHash>


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
	connect(clientSocket, &QTcpSocket::readyRead, this, &AuthorizationDownloader::readClientData);
	connect(clientSocket, &QTcpSocket::disconnected, clientSocket, &QTcpSocket::deleteLater);
}

void AuthorizationDownloader::handleNewConnection()
{
	QTcpSocket* clientSocket = nextPendingConnection();
	if (clientSocket == nullptr)
	{
		return;
	}
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
