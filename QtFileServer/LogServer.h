#pragma once
#include "AuthorizationLevel.h"
#include <QTcpServer>

class LoginServer : public QTcpServer {
	Q_OBJECT

public:
	LoginServer(QObject* parent = nullptr);

protected:
	void incomingConnection(qintptr socketDescriptor) override;

private slots:
	void handleNewConnection();

	void readClientData();

private:
	AuthorizationLevel validateCredentials(const QString& username, const QString& password);
};

