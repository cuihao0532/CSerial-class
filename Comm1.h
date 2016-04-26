#pragma once

#include "resource.h"
#include "EventHandler.h"
#include "Serial.h"
using cuish::CSerial;

#include <iostream>
using std::cout;
using std::endl;

class CCom : public ISerialEventHandler
{
public:
    CCom(); 
    virtual ~CCom(); 
    bool Init(LPCTSTR lpSerialName, CSerial::_eOpenMode eMode);
    bool SetState(DWORD dwBaudRate/*波特率*/, BYTE byteParity/*校验*/, BYTE byteStopBits/*停止位*/);
    bool SetBufferSize(DWORD dwInput, DWORD dwOutput);
    bool Write(void* pBuf, DWORD dwNumBytsToWirte);
    bool Start();

    void ReadFinish(void* pBuf,  DWORD  dwRead);
    void WriteFinish(void* pBuf, DWORD  dwWritten );

protected:
    CSerial m_serial;
};


CCom::CCom() : m_serial(this)
{

}

CCom::~CCom()
{
}

 
bool CCom::Init(LPCTSTR lpSerialName, CSerial::_eOpenMode eMode)
{
    bool b = m_serial.Open(lpSerialName, eMode);
    return b;
}

  

void CCom::ReadFinish(void* pBuf,  DWORD  dwRead)
{
    std::cout << (char*)( pBuf ) << std::endl;
    int a = 0;
}

void CCom::WriteFinish(void* pBuf, DWORD  dwWritten )
{
    std::cout << (char*)( pBuf ) << ",   Written Bytes=" << dwWritten << std::endl;
    int b = 0;
}

bool CCom::SetState(DWORD dwBaudRate/*波特率*/, BYTE byteParity/*校验*/, BYTE byteStopBits/*停止位*/)
{
    return m_serial.SetState(dwBaudRate, byteParity, byteStopBits);
}
bool CCom::SetBufferSize(DWORD dwInput, DWORD dwOutput)
{
    return m_serial.SetBufferSize(dwInput, dwOutput);
}

bool CCom::Start()
{
    return m_serial.Start();
}
bool CCom::Write(void* pBuf, DWORD dwNumBytsToWirte)
{
    bool b = m_serial.Write(pBuf, dwNumBytsToWirte, NULL);
    return b;
}
