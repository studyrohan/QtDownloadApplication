#pragma once
#pragma once
#include <QTcpServer>

#include <QDebug>



class AuthorizationDownloader : public QTcpServer {
	Q_OBJECT

public:
	AuthorizationDownloader(QObject* parent = nullptr);

protected:
	void incomingConnection(qintptr socketDescriptor) override;

private slots:
	void handleNewConnection();
	void readClientData();

};

