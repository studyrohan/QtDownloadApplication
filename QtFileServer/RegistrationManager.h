#pragma once

#include "AuthorizationLevel.h"
#include <QString>
#include <QtSql/QSqlDatabase>
#include <map>


class QSqlDatabase;

class RegistrationManager
{
public:
	enum class ErrorCode
	{
		ErrorCode_username_succeed = 0,
		ErrorCode_username_sql_operation,
		ErrorCode_username_occupation,
		ErrorCode_invalid_invation,
	};

	ErrorCode RegisterUsr(const QString& username, const QString& password, const QString& invitation);

	AuthorizationLevel GetUsrLevel(const QString& username, const QString& password);

	QString GetlicenceExpirationDate(const std::string& invitation);

	bool InitDataBase();

	static RegistrationManager& Instance();

private:
	RegistrationManager();

	~RegistrationManager();

	RegistrationManager(const RegistrationManager&) = delete;

	RegistrationManager& operator =(const RegistrationManager&) = delete;

private:
	QSqlDatabase db;

	static std::map<std::string, AuthorizationLevel>s_invationToLevel;

	static std::map<AuthorizationLevel, QString>s_LevelToTime;
};