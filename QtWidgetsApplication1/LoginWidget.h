#pragma once
#include <QDialog>

class QLineEdit;
class LoginClient;
class LoginWidget : public QDialog {
    Q_OBJECT

public:
    LoginWidget(QDialog* parent = nullptr);

signals:
    void loginClicked(int);

    void cancelClicked();

private slots:
    void login();

    void cancel();

    void loginSucceed(int level);
private:
    LoginClient* m_client;

    QLineEdit* m_usernameField;
    QLineEdit* m_passwordField;
};
