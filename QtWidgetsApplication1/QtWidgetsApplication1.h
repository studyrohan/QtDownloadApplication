#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_QtWidgetsApplication1.h"
#include "downloader.h"
class QPushButton;
class QTextEdit;
class QTimer;
class QProgressBar;
class QTableWidget;
class QLabel;
class LoginWidget;

class QtWidgetsApplication1 : public QMainWindow
{
    Q_OBJECT

public:
    QtWidgetsApplication1(QWidget *parent = nullptr);
    ~QtWidgetsApplication1();
    void InitSlots();

signals:

private slots:
    void ShowDownLoadResult();
    void DownLoadResult();
    void StartDownLoad();
    void ShowResource();
    //to do 
    void CheckSoftware();
    void SendLog();
    void ShowProgress(qint64 received,qint64 total,qreal progress);

    void DownloadLicense();
    void showLogin();
    void SetLogIn(int);
    bool GetIsLogIn() const;

private:
    QPushButton* m_loginButton;
    QPushButton* m_downloadLicenseButton;
    LoginWidget* m_loginWidget;
    QPushButton* m_button1;
    QPushButton* m_button2;
    QPushButton* m_button3;
    QPushButton* m_button4;
    QPushButton* m_button5;
    QPushButton* m_button6;
    QTextEdit* m_line;
    QLabel* m_output;
    QProgressBar* m_progressBar;
    QTimer* m_timer;
    QTableWidget* m_resTable;
    Downloader* m_download;

private:
    int m_isLogInlevel;
};
