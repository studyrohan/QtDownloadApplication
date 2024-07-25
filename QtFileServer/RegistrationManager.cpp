#include "RegistrationManager.h"
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>
#include <QDebug>
#include <QDateTime>

RegistrationManager::ErrorCode RegistrationManager::RegisterUsr(const QString& username, const QString& password, const QString& invitation)
{
	//   查询用户名是否存在
	QSqlQuery query;

	QString checkUsernameQuery = QString("SELECT COUNT(*) FROM Users WHERE username = '%1'").arg(username);

	if (!query.exec(checkUsernameQuery)) {
		qDebug() << "Error: Unable to execute query." << query.lastError();
		return ErrorCode::ErrorCode_username_sql_operation;
	}

	//  检查结果
	if (query.next()) {
		int count = query.value(0).toInt();
		if (count > 0) {
			return ErrorCode::ErrorCode_username_occupation;
		}
		else {
			qDebug() << "Username does not exist in the database.";
		}
	}
	else {
		qDebug() << "No results returned from query.";
	}

	if (s_invationToLevel.count(invitation.toStdString())==0)
	{
		return ErrorCode::ErrorCode_invalid_invation;
	}
	QString level = QString::number(s_invationToLevel[invitation.toStdString()]);
	QString insertUserQuery = QString("INSERT INTO Users (username, password, level) VALUES ('%1', '%2', '%3')").arg(username, password, level);

	if (!query.exec(insertUserQuery)) {
		qDebug() << "Error: Unable to insert user." << query.lastError();
		return ErrorCode::ErrorCode_username_sql_operation;
	}
	return ErrorCode::ErrorCode_username_succeed;
}

AuthorizationLevel RegistrationManager::GetUsrLevel(const QString& username, const QString& password)
{
	//Query the password for a given username

	QString queryPassword = QString("SELECT level FROM Users WHERE username = '%1'").arg(username);
	QSqlQuery query;
	if (!query.exec(queryPassword)) {
		qDebug() << "Error: Unable to execute query." << query.lastError();
		return AuthorizationLevel::AuthorizationLevelInvalid;
	}

	//   Check the result
	if (query.next()) {
		AuthorizationLevel level = (AuthorizationLevel)query.value(0).toInt();
		qDebug() << QString("level for the {%1} is:").arg( username) << level;
		return level;
	}
	else {
		qDebug() << "No results returned from query.";
	}

	//   查询用户名和密码是否存在
	QString checkCredentialsQuery = QString("SELECT COUNT(*) FROM Users WHERE username = '%1' AND password = '%2'").arg(username, password);

	if (!query.exec(checkCredentialsQuery)) {
		qDebug() << "Error: Unable to execute query." << query.lastError();
		return AuthorizationLevel::AuthorizationLevelInvalid;
	}

	//  检查结果
	if (query.next()) {
		int count = query.value(0).toInt();
		if (count > 0) {
			qDebug() << "Username and password match an entry in the database.";
			QString getLevel = QString("SELECT level FROM Users WHERE username = '%1'").arg(username);

			if (!query.exec(getLevel)) {
				qDebug() << "Error: Unable to execute query." << query.lastError();
				return AuthorizationLevel::AuthorizationLevelInvalid;
			}

			//   Check the result
			if (query.next()) {
				int level = query.value(0).toInt();
				qDebug() << "level for the username is:" << password;
				return AuthorizationLevel(level);
			}
			else {
				qDebug() << "No results returned from query.";
			}
		}
		else {
			qDebug() << "Username and password do not match any entry in the database.";
		}
	}
	else {
		qDebug() << "No results returned from query.";
	}
	return AuthorizationLevel::AuthorizationLevelInvalid;
}

QString RegistrationManager::GetlicenceExpirationDate(const std::string& invitation)
{
	if (s_invationToLevel.count(invitation)==0)
	{
		return "invalid";
	}
	AuthorizationLevel level = s_invationToLevel[invitation];
	return s_LevelToTime[level];
}

bool RegistrationManager::InitDataBase()
{
	QStringList drivers = QSqlDatabase::drivers();

	// 打印所有驱动名称
	qDebug() << "Available database drivers:";
	foreach(const QString & driver, drivers) {
		qDebug() << driver;
	}
	db = QSqlDatabase::addDatabase("QSQLITE");
	db.setDatabaseName("UserDB.db");

	//  打开数据库连接
	if (!db.open()) {
		qDebug() << "Error: Unable to open database.";
		return false;
	}

	//  创建查询对象
	QSqlQuery query;

	//  创建用户表
	QString createTableQuery = "CREATE TABLE IF NOT EXISTS Users ("
		"id INTEGER PRIMARY KEY AUTOINCREMENT,"
		"username TEXT NOT NULL,"
		"password TEXT NOT NULL,"
		"level INTEGER);";

	if (!query.exec(createTableQuery)) {
		qDebug() << "Error: Unable to create table." << query.lastError();
		return false;
	}
	return true;

}

RegistrationManager& RegistrationManager::Instance()
{
	static RegistrationManager instance;
	return instance;
}

RegistrationManager::RegistrationManager()
{
	InitDataBase();
}

std::map<std::string, AuthorizationLevel> RegistrationManager::s_invationToLevel =
{
	{"Qsdyouijsdf73",AuthorizationLevel::AuthorizationLevel0},
	{"QsJSDKLFJpoif",AuthorizationLevel::AuthorizationLevel1},
	{"SDLFJLKSDJfkl",AuthorizationLevel::AuthorizationLevel2},
};

std::map<AuthorizationLevel, QString> RegistrationManager::s_LevelToTime =
{

	{AuthorizationLevel::AuthorizationLevel0, QDate(2024, 8,30).toString()},
	{AuthorizationLevel::AuthorizationLevel1, QDate(2024, 12,30).toString()},
	{AuthorizationLevel::AuthorizationLevel2, "permanent"}
};

RegistrationManager::~RegistrationManager()
{
	db.close();
}

