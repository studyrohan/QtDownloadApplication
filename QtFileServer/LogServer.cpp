

#pragma once
#include "LogServer.h"
#include <QTcpServer>
#include <QTcpSocket>
#include <QStringList>
#include <QDebug>

LoginServer::LoginServer(QObject* parent) : QTcpServer(parent) {
	connect(this, &QTcpServer::newConnection, this, &LoginServer::handleNewConnection);
}

void LoginServer::incomingConnection(qintptr socketDescriptor) {
	QTcpSocket* clientSocket = new QTcpSocket(this);
	clientSocket->setSocketDescriptor(socketDescriptor);
	connect(clientSocket, &QTcpSocket::readyRead, this, &LoginServer::readClientData);
	connect(clientSocket, &QTcpSocket::disconnected, clientSocket, &QTcpSocket::deleteLater);
}

void LoginServer::handleNewConnection() {
	QTcpSocket* clientSocket = nextPendingConnection();
	if (clientSocket == nullptr)
	{
		return;
	}
	qDebug() << "New connection from" << clientSocket->peerAddress().toString();
}

void LoginServer::readClientData() {
	QTcpSocket* clientSocket = qobject_cast<QTcpSocket*>(sender());
	if (clientSocket) {
		QByteArray data = clientSocket->readAll();
		QStringList credentials = QString::fromUtf8(data).split(':');
		if (credentials.size() == 2) {
			QString username = credentials.at(0);
			QString password = credentials.at(1);
			// Validate the credentials against your database or other storage
			AuthorizationLevel level = validateCredentials(username, password);

			QByteArray response =  "LOGIN_SUCCESS:" + QString::number((int)level).toUtf8();
			clientSocket->write(response);
		}
		else
		{
			clientSocket->write("LOGIN_FAILED");
		}
	}
}

AuthorizationLevel LoginServer::validateCredentials(const QString& username, const QString& password) {
	// Implement your logic to check the username and password
	// For example, you could check against a database or a predefined list of credentials
	// Return true if the credentials are valid, false otherwise
	// This is just a placeholder for the actual validation logic
	if (username == "test" && password == "123456")
	{
		return AuthorizationLevel::AuthorizationLevel0;
	}
	else if (username == "eddy" && password == "Zwsoft2024")
	{
		return AuthorizationLevel::AuthorizationLevel1;
	}
	else if (username == "Zwsoft.developer" && password == "Zwsoft,.")
	{
		return AuthorizationLevel::AuthorizationLevel2;
	}
	return AuthorizationLevel::AuthorizationLevelInvalid;
}
