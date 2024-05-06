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
    //�رշ��������ټ�����ֱ�ӽ��������շ�
    m_tcpServer.close();
}

void Widget::updateServerProgress() {
    QDataStream in(m_tcpServerConnection);
    QString request = m_tcpServerConnection->readLine().trimmed();
    in.setVersion(QDataStream::Qt_5_7);
    // ������յ�������С��16���ֽڣ����浽�����ļ�ͷ�ṹ
    if (m_bytesReceived <= sizeof(qint64) * 2) {
        if ((m_tcpServerConnection->bytesAvailable() >= sizeof(qint64) * 2) && (m_fileNameSize == 0)) {
            // ���������ܴ�С��Ϣ���ļ�����С��Ϣ
            in >> m_totalBytes >> m_fileNameSize;
            m_bytesReceived += sizeof(qint64) * 2;
        }
        if ((m_tcpServerConnection->bytesAvailable() >= m_fileNameSize) && (m_fileNameSize != 0)) {
            // �����ļ������������ļ�
            in >> m_fileName;
            serverStatusLabel->setText(tr("receive file %1 ��").arg(m_fileName));
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
    // ������յ�����С�������ݣ���ôд���ļ�
    if (m_bytesReceived < m_totalBytes) {
        m_bytesReceived += m_tcpServerConnection->bytesAvailable();
        m_inBlock = m_tcpServerConnection->readAll();
        m_localFile->write(m_inBlock);
        m_inBlock.resize(0);
    }
    serverProgressBar->setMaximum(m_totalBytes);
    serverProgressBar->setValue(m_bytesReceived);

    // �����������ʱ
    if (m_bytesReceived == m_totalBytes) {
        m_tcpServerConnection->close();
        m_localFile->close();
        startButton->setEnabled(true);
        serverStatusLabel->setText(tr("receive file %1 succeed��").arg(m_fileName));
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