//sooScreenShare by Simon Wezstein (B-LechCode), 2019
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#include "transportservertcp.h"

transportServerTCP::transportServerTCP()
{
    m_defaultParameters[PORT] = parameter("The TCP port to listen on","int16","58000");
    m_defaultParameters[HOST_ADDRESS] = parameter("The address/network interface to listen on","string","any");
    m_defaultParameters[MAX_BUFFER_UTILIZATION] = parameter("Maximum Buffer utilization","int","99999999");
    connect(&m_srvr,SIGNAL(newConnection()),this,SLOT(on_newConnection()));
    setParameters(m_defaultParameters);

}

transportServerTCP::~transportServerTCP()
{
   end();
}

void transportServerTCP::init()
{
    notifyMessage("start listening on "+m_parameters[HOST_ADDRESS].value()+":"+m_parameters[PORT].value());

    m_srvr.listen(m_interface,m_port);
}

int64_t transportServerTCP::send(const char *dat, int64_t len)
{        
    int64_t retVal = -1;
    if(m_ptrSock)
    {       
        if(!m_ptrSock->isOpen())
            return retVal;
        if (m_ptrSock->bytesToWrite() > m_maxBuffUtil)
        {
            notifyMessage("Send buffer too full!, not sent!");
            return retVal;
        }
        retVal =  m_ptrSock->write(dat,len);
    }

    return retVal;
}

void transportServerTCP::on_newConnection()
{
    notifyMessage("new connection!");
    QTcpSocket* ptrSock = m_srvr.nextPendingConnection();
    m_ptrSock = ptrSock;
    connect(m_ptrSock,SIGNAL(disconnected()),this,SLOT(on_socketDisconnected()));
    m_ptrSock->setSocketOption(QAbstractSocket::LowDelayOption,1);
    m_srvr.close();
}

void transportServerTCP::on_socketDisconnected()
{
    notifyMessage("connection closed!");

    init();
}

void transportServerTCP::end()
{   
    if(m_ptrSock)
    {
        m_ptrSock->close();
    }
    m_ptrSock = nullptr;
    m_srvr.close();
}

/// @todo conversion with std::stoi could fail, make this safe
void transportServerTCP::initParameters()
{
    //port
    bool ok;
    int prt = m_parameters[PORT].valueInt(ok);
    if(ok)
        m_port = static_cast<uint16_t>(prt);


    //network interface
    QHostAddress addr;
    std::string  par = m_parameters[HOST_ADDRESS].value();
    if(par == "any")
        addr = QHostAddress::Any;
    else if(par == "anyV4")
        addr = QHostAddress::AnyIPv4;
    else if(par == "anyV6")
        addr = QHostAddress::AnyIPv6;
    else
    {
        addr.setAddress(QString(par.c_str()));
    }

    //Buff
    int buffUtil = m_parameters[MAX_BUFFER_UTILIZATION].valueInt(ok);
    if(ok)
        m_maxBuffUtil = buffUtil;


}

void transportServerTCP::notifyMessage(const char* str)
{
    notifyMessage(std::string(str));
}

void transportServerTCP::notifyMessage(const std::string &str)
{
    if(m_observer)
        m_observer->transportNewMessage(str);
}

void transportServerTCP::parameterMapChangedEvent()
{
   end();
   initParameters();
   init();
}

void transportServerTCP::parameterChangedEvent(const std::string &key)
{
    end();
    initParameters();
    init();
}
