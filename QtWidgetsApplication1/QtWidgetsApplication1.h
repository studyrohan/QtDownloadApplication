#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_QtWidgetsApplication1.h"
#include "downloader.h"
class QPushButton;
class QTextEdit;
class QTableWidget;

class QtWidgetsApplication1 : public QMainWindow
{
    Q_OBJECT

public:
    QtWidgetsApplication1(QWidget *parent = nullptr);
    ~QtWidgetsApplication1();
    void InitSlots();
private slots:
    void ShowDownLoadResult();
    void DownLoadResult();
    void StartDownLoad();
    void ShowResource();
    //to do 
    void CheckSoftware();
    //bool SendLog(const QString& file);


private:
    QPushButton* m_button1;
    QPushButton* m_button2;
    QPushButton* m_button3;
    QPushButton* m_button4;
    QTextEdit* m_line;
    QTableWidget* m_resTable;
    Downloader* m_download;
};
