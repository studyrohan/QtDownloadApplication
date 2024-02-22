

#include "LoginWidget.h"
#include "LoginClient.h"
#include <QTcpSocket>
#include <QString>
#include <QDebug>
#include <QMessageBox>
#include <QWidget>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHostAddress>


LoginWidget::LoginWidget(QDialog* parent) : QDialog(parent) {
    m_client = new LoginClient(this);
    QVBoxLayout* layout = new QVBoxLayout(this);
    QHBoxLayout* userLayout = new QHBoxLayout(this);
    QHBoxLayout* passwordLayout = new QHBoxLayout(this);
    QHBoxLayout* inviationLayout = new QHBoxLayout(this);


    QLabel* usernameLabel = new QLabel("Username:", this);
    QLabel* invitationLabel = new QLabel("Invitation:", this);
	QLabel* passwordLabel = new QLabel("Password:", this);
    m_usernameField = new QLineEdit(this);
    m_passwordField = new QLineEdit(this);
	m_inviationField = new QLineEdit(this);
    m_passwordField->setEchoMode(QLineEdit::Password);

    userLayout->addWidget(usernameLabel);
    userLayout->addWidget(m_usernameField);

	passwordLayout->addWidget(passwordLabel);
	passwordLayout->addWidget(m_passwordField);

    inviationLayout->addWidget(invitationLabel);
    inviationLayout->addWidget(m_inviationField);

    QPushButton* registerButton = new QPushButton("Register", this);
    QPushButton* loginButton = new QPushButton("Login", this);
    QPushButton* checkLicenceButton = new QPushButton("check license", this);
    QPushButton* cancelButton = new QPushButton("Cancel", this);
    QHBoxLayout* registerLayout = new QHBoxLayout(this);

    registerLayout->addWidget(registerButton);
    registerLayout->addWidget(loginButton);
    registerLayout->addWidget(checkLicenceButton);
    registerLayout->addWidget(cancelButton);

    layout->addLayout(userLayout);
    layout->addLayout(passwordLayout);
    layout->addLayout(inviationLayout);
    layout->addLayout(registerLayout);
    this->setLayout(layout);
    setWindowTitle("log in");
    this->resize(500, 500);
    connect(registerButton, &QPushButton::clicked, this, &LoginWidget::Register);
    connect(loginButton, &QPushButton::clicked, this, &LoginWidget::login);
    connect(checkLicenceButton, &QPushButton::clicked, this, &LoginWidget::CheckLicense);
    connect(cancelButton, &QPushButton::clicked, this, &LoginWidget::cancel);
    connect(m_client, &LoginClient::loginSucceed, this, &LoginWidget::loginSucceed);
}

void LoginWidget::login() {
    // Handle login logic here
    if (m_usernameField->text().isEmpty())
    {
        QMessageBox::information(NULL, "", "please input username");
        return;
    }
    if (m_passwordField->text().isEmpty())
    {
        QMessageBox::information(NULL, "", "please input password");
        return;
    }
    m_client->connectToServer("127.0.0.1", 1245);
    m_client->login(m_usernameField->text().trimmed(), m_passwordField->text().trimmed());

}

void LoginWidget::Register()
{
	if (m_usernameField->text().isEmpty())
	{
		QMessageBox::information(NULL, "", "please input username");
		return;
	}
	if (m_passwordField->text().isEmpty())
	{
		QMessageBox::information(NULL, "", "please input password");
		return;
	}
	if (m_inviationField->text().isEmpty())
	{
		QMessageBox::information(NULL, "", "please input Invitation code");
		return;
	}
	m_client->connectToServer("127.0.0.1", 1245);
	m_client->Register(m_usernameField->text().trimmed(), m_passwordField->text().trimmed(),m_inviationField->text().trimmed());
}

void LoginWidget::CheckLicense()
{
	if (m_inviationField->text().isEmpty())
	{
		QMessageBox::information(NULL, "", "please input Invitation code");
		return;
	}
	m_client->connectToServer("127.0.0.1", 1245);
	m_client->CheckLicense(m_inviationField->text().trimmed());
}

void LoginWidget::cancel() {
    this->close();
    emit cancelClicked();
}
void LoginWidget::loginSucceed(int level)
{
    emit loginClicked(level);
}