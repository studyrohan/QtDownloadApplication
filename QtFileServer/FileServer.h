//
//
//#pragma once
//
//#include <QtNetwork/QTcpServer>
//#include <QtNetwork/QTcpSocket>
//#include <QMimeDatabase>
//#include <QFile>
//#include <QFileInfo>
//#include <QDir>
//#include <QTextStream>
//#include <QDateTime>
//
//class FileServer : public QTcpServer
//{
//    Q_OBJECT
//
//public:
//    explicit FileServer(QObject* parent = nullptr)
//        : QTcpServer(parent)
//    {
//        if (!listen(QHostAddress::Any, 8080)) {
//            qFatal("无法监听端口：%s", errorString().toLocal8Bit().constData());
//        }
//    }
//
//protected:
//    void incomingConnection(qintptr socketDescriptor) override
//    {
//        QTcpSocket* socket = new QTcpSocket(this);
//        socket->setSocketDescriptor(socketDescriptor);
//
//        connect(socket, &QTcpSocket::readyRead, this, [this, socket]() {
//            QByteArray data = socket->readAll();
//        QString request = QString::fromUtf8(data);
//        if (request.startsWith("GET")) {
//            handleGetRequest(socket, request);
//        }
//        else if (request.startsWith("POST")) {
//            handlePostRequest(socket, request);
//        }
//        else {
//            socket->write("HTTP/1.0  400 Bad Request\r\n");
//            socket->write("Content-Type: text/html\r\n");
//            socket->write("\r\n");
//            socket->write("<html><body><h1>400 Bad Request</h1></body></html>");
//            socket->close();
//        }
//            });
//
//        connect(socket, &QTcpSocket::disconnected, socket, &QTcpSocket::deleteLater);
//    }
//
//private:
//    void handleGetRequest(QTcpSocket* socket, const QString& request)
//    {
//        QString fileName = request.section(' ', 1, 1, QString::SectionSkipEmpty).section('/', 1, 1, QString::SectionSkipEmpty);
//        QFile file(fileName);
//        if (file.open(QIODevice::ReadOnly)) {
//            QTextStream response(socket);
//            response << "HTTP/1.0  200 OK\r\n";
//            response << "Content-Type: " << QMimeDatabase().mimeTypeForFile(fileName).name().toUtf8() << "\r\n";
//            response << "\r\n";
//            response << file.readAll();
//            file.close();
//        }
//        else {
//            QTextStream response(socket);
//            response << "HTTP/1.0  404 Not Found\r\n";
//            response << "Content-Type: text/html\r\n";
//            response << "\r\n";
//            response << "<html><body><h1>404 Not Found</h1></body></html>";
//        }
//        socket->close();
//    }
//
//    void handlePostRequest(QTcpSocket* socket, const QString& request)
//    {
//        //  这里你需要解析POST请求，并将数据写入到文件中。
//        //  这通常需要使用QHttpParser来解析请求，并从请求体中读取数据。
//        //  这里只是一个简单的示例，没有实现POST请求的处理。
//        QTextStream response(socket);
//        response << "HTTP/1.0  200 OK\r\n";
//        response << "Content-Type: text/html\r\n";
//        response << "\r\n";
//        response << "<html><body><h1>POST请求已接收</h1></body></html>";
//        socket->close();
//    }
//};