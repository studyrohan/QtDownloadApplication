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

	void login(const QString& username, const QString& password);

	void Register(const QString& username, const QString& password,const QString &inviation);

	void CheckLicense(const QString &inviation);
signals:
	void loginSucceed(int);

	void RegisterResult(int);
private slots:
	void readData();

	void handleError(QAbstractSocket::SocketError error);
private:
	QTcpSocket* m_tcpClient;
	bool islogin;
};

