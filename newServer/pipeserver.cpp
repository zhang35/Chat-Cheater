#include "pipeserver.h"

PipeServer::PipeServer(QObject *parent) :
    QTcpServer(parent)
{

}

void PipeServer::incomingConnection(int socketId)
{ 
    clientSocket = new ClientSocket(this);
    clientSocket->setSocketDescriptor(socketId);
    connect(clientSocket, SIGNAL(disconnected()), clientSocket, SLOT(deleteLater()));
}
