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
	QString loginCommand = QString("%1:%2").arg(username).arg(password);
	m_tcpClient->write(loginCommand.toUtf8());
}

void LoginClient::Register(const QString& username, const QString& password, const QString& inviation)
{
	QString loginCommand = QString("%1:%2:%3").arg(username).arg(password).arg(inviation);
	m_tcpClient->write(loginCommand.toUtf8());
}

void LoginClient::CheckLicense(const QString& inviation)
{
	m_tcpClient->write(inviation.toUtf8());
}

void LoginClient::readData() {
	QByteArray data = m_tcpClient->readAll();
	QString response(data);
	qDebug() << "Server response:" << response;
	QStringList list = response.split(':');
	if (list.size() == 2)
	{
		if (list.at(0) == "CHECK_LICENSE")
		{
			QMessageBox::information(NULL, "Check license", "License validity is : "+ list.at(1),
				QMessageBox::Yes, QMessageBox::Yes);
		}
		else if (list.at(0) == "LOGIN_SUCCESS")
		{
			int level = list.at(1).toInt();
			if (level >= AuthorizationLevel::AuthorizationLevel0 && level <= AuthorizationLevel::AuthorizationLevel2)
			{
				QMessageBox::information(NULL, "Login", "Login Succeed",
					QMessageBox::Yes, QMessageBox::Yes);
				emit loginSucceed(level);
				return;
			}
			QMessageBox::information(NULL, "Login", "Login Failed",
				QMessageBox::Yes, QMessageBox::Yes);
		}
		else if (list.at(0) == "REGISTER")
		{
			QString logInfomation;
			int level = list.at(1).toInt();
			switch (level)
			{
			case 0:
			{
				logInfomation = "register succeed";
				emit RegisterResult(0);
				break;
			}
			case 1:
				logInfomation = "unknown error";
				break;
			case 2:
				logInfomation = "user name already exists";
				break;
			case 3:
				logInfomation = "invalid invitation";
				break;
			default:
				logInfomation = "unknown error";
				break;
			}
			QMessageBox::information(NULL, "Register", logInfomation,
				QMessageBox::Yes, QMessageBox::Yes);
			return;
		}
	}

	islogin = false;

	// Here you should handle the server's response, for example, check if the login was successful
}

void LoginClient::handleError(QAbstractSocket::SocketError error) {
	qDebug() << "Socket error:" << m_tcpClient->errorString();
	m_tcpClient->disconnectFromHost();
}