#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_QtWidgetsApplication1.h"
#include "downloader.h"
class QPushButton;
class QTextEdit;
class QTableWidget;
class QLabel;

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




private:
    QPushButton* m_button1;
    QPushButton* m_button2;
    QPushButton* m_button3;
    QPushButton* m_button4;
    QPushButton* m_button5;
    QPushButton* m_button6;
    QTextEdit* m_line;
    QLabel* m_output;
    QTableWidget* m_resTable;
    Downloader* m_download;
};
