
#include "EventHandler.h"
#include <vector>
using std::vector;
#pragma once

namespace cuish
{
    class CSerial
    {
    public:
        CSerial(void);
        CSerial(ISerialEventHandler* pHandler);
        virtual ~CSerial(void);

        typedef enum _eOpenMode
        {
            Serial_No_Data = 0,
            Serial_Read_Data = GENERIC_READ,
            Serial_Write_Data = GENERIC_WRITE,
            Serial_Read_Write_Data = GENERIC_READ | GENERIC_WRITE
        }OpenMode;




        bool Open(LPCTSTR lpSerialPort, OpenMode eMode);


        bool Set();


        bool Read(PVOID pvBuffer, DWORD dwNumBytesRead, PDWORD pdwNumBytes);


        bool Write(const PVOID pvBuffer, DWORD dwNumBytesToWrite, PDWORD pdwNumBytes);


    protected:
        HANDLE m_hSerialPort;
        HANDLE m_hIocp;
        DWORD  m_dwCompletionKey;
        OVERLAPPED m_overlapped;
        ISerialEventHandler* m_pEventHandler;
        
        std::vector<HANDLE> m_vecThreadPool;
        std::vector<HANDLE> m_vecExitEvents; 




    protected:
        HANDLE CreateIocp();
        bool   AssociateSerialToIocp();
        friend unsigned int __stdcall WorkThread(void* p);
        bool InitThreadPool();
    };


}


