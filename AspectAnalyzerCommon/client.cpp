#include "client.h"

static inline QByteArray IntToArray(qint32 source);

Client::Client(QObject *parent) : QObject(parent)
{
    socket = new QTcpSocket(this);
}

bool Client::connectToHost(QString host)
{
    socket->connectToHost(host, 8888);
    return socket->waitForConnected();
}

bool Client::writeData(QByteArray data)
{
    if(socket->state() == QAbstractSocket::ConnectedState)
    {
        socket->write(IntToArray(data.size())); //write size of data
        socket->write(data); //write the data itself
        return socket->waitForBytesWritten();
    }
    else
        return false;
}

bool Client::disconnectFromHost()
{
    if(socket->state() == QAbstractSocket::ConnectedState)
    {
        socket->disconnectFromHost();
        return true;
    }
    else
    {
        return false;
    }
}

QByteArray IntToArray(qint32 source) //Use qint32 to ensure that the number have 4 bytes
{
    //Avoid use of cast, this is the Qt way to serialize objects
    QByteArray temp;
    QDataStream data(&temp, QIODevice::ReadWrite);
    data << source;
    return temp;
}
