
#include "EventHandler.h"
#include <vector>
using std::vector;
#pragma once

namespace cuish
{
    typedef struct _tagOverlappedParam : public OVERLAPPED
    {
    public:
        _tagOverlappedParam() { memset(this, 0, sizeof(*this)); eop = Operate_MAX; }

        enum eOperate
        {
            Operate_READ  = 0,
            Operate_WRITE = 1,
            Operate_MAX  
        };

        eOperate eop;

        void* pData;

    }OverlappedParam, *POverlappedParam;

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

        bool SetState(DWORD dwBaudRate/*波特率*/, BYTE byteParity/*校验*/, BYTE byteStopBits/*停止位*/);

        bool Read(PVOID pvBuffer, DWORD dwNumBytesRead, PDWORD pdwNumBytes);

        bool Write(const PVOID pvBuffer, DWORD dwNumBytesToWrite, PDWORD pdwNumBytes);

        bool SetTimeout();

        bool SetBufferSize(DWORD dwInput, DWORD dwOutput);

        bool Start();

        bool Close();


    protected:
        HANDLE m_hSerialPort;
        HANDLE m_hIocp;
        DWORD  m_dwCompletionKey;
        OverlappedParam m_overlappedRead;
        OverlappedParam m_overlappedWrite;
        ISerialEventHandler* m_pEventHandler;
        
        std::vector<HANDLE> m_vecThreadPool;
        std::vector<HANDLE> m_vecExitEvents; 

        bool m_bExitThreads;



    protected:
        HANDLE CreateIocp();
        bool   AssociateSerialToIocp();
        friend unsigned int __stdcall WorkThread(void* p);
        bool InitThreadPool();
    };


}


