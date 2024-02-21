#pragma once
#include <QTcpSocket>
#include <QString>


enum AuthorizationLevel
{
	AuthorizationLevelInvalid,
	AuthorizationLevel0,
	AuthorizationLevel1,
	AuthorizationLevel2,
};


class LoginClient : public QObject {
	Q_OBJECT

public:
	LoginClient(QObject* parent = nullptr);

	void connectToServer(const QString& serverAddress, int port);

	void login(const QString& username, const QString& password) ;
signals:
	void loginSucceed(int);
	
private slots:
	void readData();

	void handleError(QAbstractSocket::SocketError error);
private:
	QString m_username, m_passward;
	QTcpSocket* m_tcpClient;
	bool islogin;
};

