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

<<<<<<< HEAD
private:
	bool shouldAcceptConnection(const QString& ip);

=======
>>>>>>> f6348b744c8929594a95d06698e6552b3d329b46
private slots:
	void handleNewConnection();

	void readClientData();

<<<<<<< HEAD
private:
	QMap<QString, int> ipRequestCounts;
=======
>>>>>>> f6348b744c8929594a95d06698e6552b3d329b46
};

