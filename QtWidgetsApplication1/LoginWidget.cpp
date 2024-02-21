

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

    QLabel* usernameLabel = new QLabel("Username:", this);
    m_usernameField = new QLineEdit(this);
    QLabel* passwordLabel = new QLabel("Password:", this);
    m_passwordField = new QLineEdit(this);
    m_passwordField->setEchoMode(QLineEdit::Password);

    QPushButton* loginButton = new QPushButton("Login", this);
    QPushButton* cancelButton = new QPushButton("Cancel", this);

    layout->addWidget(usernameLabel);
    layout->addWidget(m_usernameField);
    layout->addWidget(passwordLabel);
    layout->addWidget(m_passwordField);
    layout->addWidget(loginButton);
    layout->addWidget(cancelButton);

    connect(loginButton, &QPushButton::clicked, this, &LoginWidget::login);
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
        QMessageBox::information(NULL, "", "please input username");
        return;
    }
    m_client->connectToServer("127.0.0.1", 1245);
    m_client->login(m_usernameField->text().trimmed(), m_passwordField->text().trimmed());

}

void LoginWidget::cancel() {
    this->close();
    emit cancelClicked();
}
void LoginWidget::loginSucceed(int level)
{
    emit loginClicked(level);
}