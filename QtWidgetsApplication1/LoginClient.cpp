#include "LoginClient.h"
#include <QMessageBox>

LoginClient::LoginClient(QObject* parent) : QObject(parent) {
	m_tcpClient = new QTcpSocket(this);
	islogin = false;
	connect(m_tcpClient, &QTcpSocket::readyRead, this, &LoginClient::readData);
	connect(m_tcpClient, static_cast<void (QTcpSocket::*)(QAbstractSocket::SocketError)>(&QTcpSocket::error), this, &LoginClient::handleError);
}

void LoginClient::connectToServer(const QString& serverAddress, int port) {
	m_tcpClient->connectToHost(serverAddress, port);
	if (!m_tcpClient->waitForConnected(3000)) {
		qDebug() << "Connection failed:" << m_tcpClient->errorString();
		QMessageBox::information(nullptr, "", m_tcpClient->errorString());
	}
}

void LoginClient::login(const QString& username, const QString& password) {
	m_username = username;
	m_passward = password;
	QString loginCommand = QString("%1:%2").arg(username).arg(password);
	m_tcpClient->write(loginCommand.toUtf8());
}

void LoginClient::readData() {
	QByteArray data = m_tcpClient->readAll();
	QString response(data);
	qDebug() << "Server response:" << response;
	QStringList list = response.split(':');
	if (list.size() == 2)
	{
		if (list.at(0) == "LOGIN_SUCCESS")
		{
			int level = list.at(1).toInt();
			if (level >= AuthorizationLevel::AuthorizationLevel0 && level <= AuthorizationLevel::AuthorizationLevel2)
			{
				QMessageBox::information(NULL, "Login", "Login Succeed",
					QMessageBox::Yes, QMessageBox::Yes);
				emit loginSucceed(level);
				return;
			}

		}
	}
	QMessageBox::information(NULL, "Login", "Login Failed",
		QMessageBox::Yes, QMessageBox::Yes);
	islogin = false;

	// Here you should handle the server's response, for example, check if the login was successful
}

void LoginClient::handleError(QAbstractSocket::SocketError error) {
	qDebug() << "Socket error:" << m_tcpClient->errorString();
	m_tcpClient->disconnectFromHost();
}