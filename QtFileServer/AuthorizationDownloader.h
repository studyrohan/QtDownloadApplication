#pragma once
#pragma once
#include <QTcpServer>

#include <QDebug>

class QString;

class AuthorizationDownloader : public QTcpServer {
	Q_OBJECT

public:
	AuthorizationDownloader(QObject* parent = nullptr);

	QString encryptData(const QString& data, const QString& key);

	QString decryptData(const QString& data, const QString& key);
protected:
	void incomingConnection(qintptr socketDescriptor) override;

private slots:
	void handleNewConnection();

	void readClientData();

};

