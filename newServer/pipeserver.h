#ifndef PIPESERVER_H
#define PIPESERVER_H

#include <QtNetwork/QTcpServer>
#include "clientsocket.h"

class PipeServer : public QTcpServer
{
    Q_OBJECT
public:
    explicit PipeServer(QObject *parent = 0);
    ClientSocket *clientSocket;
public slots:
    void incomingConnection(int socketId);
};

#endif // PIPESERVER_H
