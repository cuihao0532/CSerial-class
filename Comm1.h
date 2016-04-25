#pragma once

#include "resource.h"
#include "EventHandler.h"
#include "Serial.h"
using cuish::CSerial;

class CCom : public ISerialEventHandler
{
public:
    CCom(); 
    virtual ~CCom();
    bool Init();

    void ReadFinish();
    void WriteFinish(); 

protected:
    CSerial m_serial;
};


CCom::CCom()
{

}

CCom::~CCom()
{
}

bool CCom::Init()
{
    bool b = m_serial.Open(TEXT("com1"), CSerial::Serial_Read_Write_Data);
    return b;
}
void CCom::ReadFinish()
{

}

void CCom::WriteFinish()
{

}