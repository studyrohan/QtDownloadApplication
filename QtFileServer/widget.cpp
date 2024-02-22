#include "widget.h"

#include <QtNetwork>
#include <QDebug>
#include <QPushButton>
#include <QVBoxLayout>
#include <QTextEdit>
#include <QProgressBar>

Widget::Widget(QWidget* parent) :
    QWidget(parent)
{
    this->setWindowTitle("CSDN IT1995");
    connect(&m_tcpServer, SIGNAL(newConnection()), this, SLOT(acceptConnection()));
    startButton = new QPushButton(this);
    startButton->setText("start!");
    connect(startButton, SIGNAL(clicked(bool)), this, SLOT(startBtnClicked()));
    serverStatusLabel = new QTextEdit(this);
    serverProgressBar = new QProgressBar(this);
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(startButton);
    layout->addWidget(serverStatusLabel);
    layout->addWidget(serverProgressBar);
    this->setLayout(layout);
}

Widget::~Widget()
{;
}

void Widget::start() {
    if (!m_tcpServer.listen(QHostAddress::LocalHost, 10086)) {
        qDebug() << m_tcpServer.errorString();
        close();
        return;
    }
    startButton->setEnabled(false);
    m_totalBytes = 0;
    m_bytesReceived = 0;
    m_fileNameSize = 0;
    serverStatusLabel->setText("listen");
    serverProgressBar->reset();
}

void Widget::acceptConnection() {
    m_tcpServerConnection = m_tcpServer.nextPendingConnection();
    connect(m_tcpServerConnection, SIGNAL(readyRead()), this, SLOT(updateServerProgress()));
    connect(m_tcpServerConnection, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(displayError(QAbstractSocket::SocketError)));
    serverStatusLabel->setText("accept connection");
    //关闭服务器不再监听，直接进入数据收发
    m_tcpServer.close();
}

void Widget::updateServerProgress() {
    QDataStream in(m_tcpServerConnection);
    QString request = m_tcpServerConnection->readLine().trimmed();
    in.setVersion(QDataStream::Qt_5_7);
    // 如果接收到的数据小于16个字节，保存到来的文件头结构
    if (m_bytesReceived <= sizeof(qint64) * 2) {
        if ((m_tcpServerConnection->bytesAvailable() >= sizeof(qint64) * 2) && (m_fileNameSize == 0)) {
            // 接收数据总大小信息和文件名大小信息
            in >> m_totalBytes >> m_fileNameSize;
            m_bytesReceived += sizeof(qint64) * 2;
        }
        if ((m_tcpServerConnection->bytesAvailable() >= m_fileNameSize) && (m_fileNameSize != 0)) {
            // 接收文件名，并建立文件
            in >> m_fileName;
            serverStatusLabel->setText(tr("receive file %1 …").arg(m_fileName));
            m_bytesReceived += m_fileNameSize;
            m_localFile = new QFile(m_fileName);
            if (!m_localFile->open(QFile::WriteOnly)) {
                qDebug() << "server: open file error!";
                return;
            }
        }
        else {
            return;
        }
    }
    // 如果接收的数据小于总数据，那么写入文件
    if (m_bytesReceived < m_totalBytes) {
        m_bytesReceived += m_tcpServerConnection->bytesAvailable();
        m_inBlock = m_tcpServerConnection->readAll();
        m_localFile->write(m_inBlock);
        m_inBlock.resize(0);
    }
    serverProgressBar->setMaximum(m_totalBytes);
    serverProgressBar->setValue(m_bytesReceived);

    // 接收数据完成时
    if (m_bytesReceived == m_totalBytes) {
        m_tcpServerConnection->close();
        m_localFile->close();
        startButton->setEnabled(true);
        serverStatusLabel->setText(tr("receive file %1 succeed！").arg(m_fileName));
    }
}

void Widget::displayError(QAbstractSocket::SocketError socketError) {
    Q_UNUSED(socketError)
        qDebug() << m_tcpServerConnection->errorString();
    m_tcpServerConnection->close();
    serverProgressBar->reset();
    serverStatusLabel->setText("server is ready");
    startButton->setEnabled(true);
}

void Widget::startBtnClicked() {
    start();
}