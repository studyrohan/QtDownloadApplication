#include "AuthorizationDownloader.h"
#include "AuthorizationLevel.h"
#include <QTcpSocket>
#include <QStringList>

static const std::vector<std::string> gs_authorizationCode = { "wertyuiop[","asdfghjkl","asdfghjkl" };

AuthorizationDownloader::AuthorizationDownloader(QObject* parent) : QTcpServer(parent) {
	connect(this, &QTcpServer::newConnection, this, &AuthorizationDownloader::handleNewConnection);
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

		int level = data.toInt();
		switch (AuthorizationLevel(level))
		{
		case AuthorizationLevel0:
			clientSocket->write(gs_authorizationCode[0].c_str());
			break;
		case AuthorizationLevel1:
			clientSocket->write(gs_authorizationCode[1].c_str());
			break;
		case AuthorizationLevel2:
			clientSocket->write(gs_authorizationCode[2].c_str());
			break;
		default:
			clientSocket->write("error");
			break;
		}
	}
}
