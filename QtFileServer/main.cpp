#include "FileServer.h"
#include "widget.h"
#include <QtCore/QCoreApplication>
#include <QApplication>
#include <QTcpServer>
#include <QTcpSocket>
#include <QFile>
#include <QHostAddress>
#include <QDebug>
#include <QMimeDatabase>
#include <QHttpPart>
#include <QHttpMultiPart>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDir>
#include <QDateTime>
#include <QRandomGenerator>
#include <QtNetwork/QNetworkInterface>


class FileTransferSocket : public QTcpSocket {
    Q_OBJECT
public:
    explicit FileTransferSocket(qintptr socketDescriptor, QObject* parent = nullptr)
        : QTcpSocket(parent), _socketDescriptor(socketDescriptor) {
        setSocketDescriptor(_socketDescriptor);
        //connect(this, &FileTransferSocket::readyRead, this, &FileTransferSocket::processRequest);
        connect(this, &FileTransferSocket::readyRead, this, &FileTransferSocket::processRequestBaseTcp);
        m_totalBytes = 0;
        m_bytesReceived = 0;
        m_fileNameSize = 0;
        m_localFile = nullptr;
    }

private slots:
    void processRequest() {
        QTextStream out(m_localFile);
        qint64 nums = this->bytesAvailable();
        QByteArray data = this->readAll();

        QString request = QString::fromUtf8(data);
        QStringList postDataList = request.split("\n");
        if (m_fileNameSize == 0)
        {
            for (auto& str : postDataList)
            {
                if (str.indexOf("filename=\"") != -1)
                {
                    //create folder

                    QDir dir = QDir::currentPath();
                    if (!dir.exists("peerIpAddress"))
                    {
                        if (dir.mkdir("peerIpAddress")) {
                            qDebug() << "Folder created successfully.";
                        }
                        else {
                            qDebug() << "Failed to create folder.";
                        }
                    }
                    auto fileName = str.mid(str.indexOf("filename=\"") + 10);
                    fileName.chop(1);
                    m_fileName = fileName.left(fileName.size() - 1);
                    m_fileNameSize = m_fileName.size();
                    m_localFile->setFileName("peerIpAddress/ " + m_fileName);
                    if (!m_localFile->open(QIODevice::Append | QIODevice::Text)) {
                        qDebug() << "Could not open file for writing";
                        return;
                    }
                }
                if (m_fileNameSize>0&&!str.startsWith("----boundary----"))
                {
                    // 写入数据到文件
                    out << str << endl;;
                }
                else
                {
                    break;
                }
            }
        }


        // 创建 QTextStream 对象，并绑定到 QFile


        QString peerIpAddress = this->peerAddress().toString();

        bool startWrite = false;
        for (auto &str:postDataList)
        {
            if (!startWrite)
            {
                if (str.indexOf("filename=\"")!=-1)
                {
                    //create folder
                    QDir dir = QDir::currentPath();
                    if (!dir.exists("peerIpAddress"))
                    {
                        if (dir.mkdir("peerIpAddress")) {
                            qDebug() << "Folder created successfully.";
                        }
                        else {
                            qDebug() << "Failed to create folder.";
                        }
                    }
                    auto fileName = str.mid(str.indexOf("filename=\"")+10);
                    fileName.chop(1);
                    auto filename = fileName.left(fileName.size() - 1);
                    m_localFile->setFileName("peerIpAddress/ "+ filename);
                    if (!m_localFile->open(QIODevice::Append | QIODevice::Text)) {
                        qDebug() << "Could not open file for writing";
                        return ;
                    };
                }
            }
            else
            {

                if (!str.startsWith("----boundary----"))
                {
                    // 写入数据到文件
                    out << str << endl;
                }
                else
                {
                    break;
                }
            }
        }

        // 关闭文件
        m_localFile->close();

        //QByteArray header;
        //header.append("HTTP/1.1  200 OK\r\n");
        //header.append("Content-Type: application/octet-stream\r\n");
        //header.append("Content-Length: " + QString::number(data.size()) + "\r\n");
        //header.append("\r\n");
        //write(header);
        //write("fileData is received");
        //close();
        QByteArray header;
        header.append("HTTP/1.1  200 OK\r\n");
        //header.append("Content-Type: application/octet-stream\r\n");
        //header.append("Content-Length: " + QString::number(data.size()) + "\r\n");
        header.append("\r\n");
        write(header);
        //write(data);


        //if (request.startsWith("GET")) {
        //    QString fileName = request.mid(4); // Get the filename from the request
        //    QFile file(fileName);
        //    if (file.open(QIODevice::ReadOnly)) {
        //        QByteArray fileData = file.readAll();
        //        QByteArray header;
        //        header.append("HTTP/1.1  200 OK\r\n");
        //        header.append("Content-Type: application/octet-stream\r\n");
        //        header.append("Content-Length: " + QString::number(fileData.size()) + "\r\n");
        //        header.append("\r\n");
        //        write(header);
        //        write(fileData);
        //        close();
        //    }
        //    else {
        //        qWarning() << "Failed to open file:" << fileName;
        //        QByteArray response = "HTTP/1.1  404 Not Found\r\n\r\n";
        //        write(response);
        //        close();
        //    }
        //}
        //else if (request.startsWith("POST"))
        //{
        //    QString fileName = request.mid(4); // Get the filename from the request
        //    QFile file(fileName);

        //}
        //else {
        //    QByteArray response = "HTTP/1.1  400 Bad Request\r\n\r\n";
        //    write(response);
        //    close();
        //}
    }

    void processRequestBaseTcp()
    {
        QDateTime currentDateTime = QDateTime::currentDateTime();

        //  输出当前时间
        qDebug() << "Current date and time:" << currentDateTime.toString("yyyy-MM-dd hh:mm:ss");
        QDir dir = QDir::currentPath();
        if (!dir.exists("peerIpAddress"))
        {
            if (dir.mkdir("peerIpAddress")) {
                qDebug() << "Folder created successfully.";
            }
            else {
                qDebug() << "Failed to create folder.";
            }
        }

        if (m_localFile == nullptr)
        {
            m_localFile = new QFile(this);
            auto filename = this->readLine().trimmed();
            m_fileName = QString(filename);
            m_localFile->setFileName(m_fileName);
            if (!m_localFile->open(QIODevice::WriteOnly|QIODevice::Text)) {
                qDebug() << "Could not open file for writing"<< m_localFile->errorString();
                return;
            }
        }
        QByteArray data = this->readAll();
        m_localFile->write(data);
        if (data.endsWith("---boundary---"))
        {
            //end
            m_localFile->flush();
            m_localFile->close();
            m_localFile = nullptr;
            m_fileName.clear();
        }
    }

private:
    qintptr _socketDescriptor;
    qint64 m_totalBytes;
    qint64 m_bytesReceived;
    qint64 m_fileNameSize;
    QString m_fileName;
    QFile* m_localFile;
    QByteArray m_inBlock;
};

class FileServer : public QTcpServer {
    Q_OBJECT
public:
    explicit FileServer(QObject* parent = nullptr) : QTcpServer(parent) {}

protected:
    void incomingConnection(qintptr socketDescriptor) override {
        FileTransferSocket* socket = new FileTransferSocket(socketDescriptor, this);
        connect(socket, &FileTransferSocket::disconnected, socket, &FileTransferSocket::deleteLater);
    }
};


int main(int argc, char* argv[]) {

    QCoreApplication app(argc, argv);
    FileServer server;
    if (!server.listen(QHostAddress::Any, 1234)) {
        qFatal("Unable to start the server: %s.", qPrintable(server.errorString()));
        return -1;
    }
    return app.exec();
}

#include "main.moc"