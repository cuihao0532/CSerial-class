#include "StdAfx.h"
#include "Serial.h"

namespace cuish
{
    CSerial::CSerial(void)
    {
        m_hSerialPort = INVALID_HANDLE_VALUE;
        m_hIocp       = NULL;
        m_dwCompletionKey = 1;
        m_overlappedRead.eop = OverlappedParam::Operate_READ;
        m_overlappedWrite.eop = OverlappedParam::Operate_WRITE; 
        m_pEventHandler = NULL;
        m_bExitThreads = false;
    }


    CSerial::CSerial(ISerialEventHandler* pHandler)
    {
        m_hSerialPort = INVALID_HANDLE_VALUE;
        m_hIocp       = NULL;
        m_dwCompletionKey = 1;
        m_overlappedRead.eop = OverlappedParam::Operate_READ;
        m_overlappedWrite.eop = OverlappedParam::Operate_WRITE; 
        m_bExitThreads = false;

        m_pEventHandler = pHandler; 
    }

    CSerial::~CSerial(void)
    {
        Close();
    }


    bool CSerial::SetState(
        DWORD dwBaudRate     /*波特率*/, 
        BYTE byteParity      /*校验*/,
        BYTE byteStopBits    /*停止位*/)
    {
        if ( !m_hSerialPort ) return false;

        DCB dcb;
        BOOL bRet = GetCommState(m_hSerialPort, &dcb);
        if ( !bRet ) return false;

        dcb.BaudRate = dwBaudRate;
        dcb.Parity = byteParity;
        dcb.StopBits = byteStopBits; 
        dcb.ByteSize = 8;

        bRet = SetCommState(m_hSerialPort, &dcb); 
        return bRet;
    }

    bool CSerial::Read(PVOID pvBuffer, DWORD dwNumBytesRead, PDWORD pdwNumBytes)
    {
        if ( !m_hSerialPort ) return false;

        m_overlappedRead.pData = pvBuffer;
        ReadFile(m_hSerialPort, pvBuffer, dwNumBytesRead, pdwNumBytes, &m_overlappedRead); 
        return true;
    }

    bool CSerial::Write(const PVOID pvBuffer, DWORD dwNumBytesToWrite, PDWORD pdwNumBytes)
    {
        if ( !m_hSerialPort ) return false;

        m_overlappedWrite.pData = pvBuffer;
        WriteFile(m_hSerialPort, pvBuffer, dwNumBytesToWrite, pdwNumBytes, &m_overlappedWrite);
        return true;
    }

    bool CSerial::SetBufferSize(DWORD dwInput, DWORD dwOutput)
    {
        if ( !m_hSerialPort ) return false;

        BOOL bRet = SetupComm(m_hSerialPort, dwInput, dwOutput); 
        return ( bRet ? true : false );
    }

    bool CSerial::Start()
    {
        if ( !m_hSerialPort ) return false;

        PurgeComm(m_hSerialPort, PURGE_TXCLEAR | PURGE_RXCLEAR);
        return InitThreadPool(); 
    }

    bool CSerial::Close()
    {
        m_bExitThreads = true;

        // 设置退出线程事件
        for (int i = 0; i < m_vecExitEvents.size(); ++ i )
        {
            SetEvent( m_vecExitEvents[ i ] ); 
        }
        m_vecExitEvents.clear();

        // 关闭线程句柄
        for ( int i = 0; i < m_vecThreadPool.size(); ++ i )
        {
            CloseHandle( m_vecThreadPool[ i ] );
        }
        m_vecThreadPool.clear();


        if ( INVALID_HANDLE_VALUE != m_hSerialPort )
        {
            CloseHandle(m_hSerialPort);
            m_hSerialPort = INVALID_HANDLE_VALUE;
        } 

        if ( m_hIocp )
        {
            CloseHandle(m_hIocp);
            m_hIocp = NULL;
        } 

        return true;
    }

    HANDLE CSerial::CreateIocp()
    {
        if ( !m_hIocp )
        {
            m_hIocp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0); 
        }

        return m_hIocp;
    }


    bool CSerial::AssociateSerialToIocp()
    {
        HANDLE h = CreateIoCompletionPort(m_hSerialPort, m_hIocp, m_dwCompletionKey, 0);

        return ( h == m_hIocp );
    }

  

    bool CSerial::InitThreadPool()
    {
        // 获取CPU个数
        SYSTEM_INFO sysInfo;
        GetSystemInfo(&sysInfo);

        for ( int i = 0; i < sysInfo.dwNumberOfProcessors * 2 + 1; ++ i )
        {
            HANDLE hThread = (HANDLE)_beginthreadex(
                NULL,
                0,
                WorkThread,
                this,
                0,
                NULL);

            if ( INVALID_HANDLE_VALUE != hThread && NULL != hThread )
            {
                m_vecThreadPool.push_back(hThread);

                HANDLE hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

                if ( NULL != hEvent )
                    m_vecExitEvents.push_back(hEvent); 
                else
                    return false;
            }
            else
            {
                return false;
            } 

        } // for

        return true;
    }



    bool cuish::CSerial::Open(LPCTSTR lpSerialPort, OpenMode eMode)
    {
        bool bRet = false;
        int  nErrorCode = 0;

        if ( INVALID_HANDLE_VALUE == m_hSerialPort )
        {
            m_hSerialPort = CreateFile(lpSerialPort, eMode, 0, NULL, OPEN_EXISTING, FILE_FLAG_OVERLAPPED, NULL);
            if ( INVALID_HANDLE_VALUE == m_hSerialPort )
            { 
                DWORD dwErr = GetLastError();
                return false;
            }

            HANDLE hIocp = CreateIocp();
            if ( hIocp )
            {
                bRet = AssociateSerialToIocp();
            }
        } // if 

        return bRet;
    }

     



    //////////////////////////////////////////////////////////////////////////
    unsigned int __stdcall WorkThread(void* p)
    {
        CSerial* pSerial = (CSerial*)( p );
        if ( !pSerial ) return 0;

        ISerialEventHandler* pHandler = pSerial->m_pEventHandler; 
        DWORD dwBytes = 0;
        DWORD dwCompleteKey = 0;
        POverlappedParam lpOverlapped = NULL;

        while ( ! pSerial->m_bExitThreads )
        {
            BOOL bRet = GetQueuedCompletionStatus(
                pSerial->m_hIocp, &dwBytes, &dwCompleteKey, (LPOVERLAPPED*)( &lpOverlapped ), INFINITE);

            if ( !bRet)  continue;

            // 读完成
            if ( lpOverlapped && OverlappedParam::Operate_READ == lpOverlapped->eop )
            {
                if ( pHandler )
                    pHandler->ReadFinish(lpOverlapped->pData, lpOverlapped->InternalHigh);
            }
            

            // 写完成
            if ( lpOverlapped && OverlappedParam::Operate_WRITE == lpOverlapped->eop )
            {
                if ( pHandler )
                    pHandler->WriteFinish(lpOverlapped->pData, lpOverlapped->InternalHigh); 
            } 
            
        }
        

        return 0;
    }



} // namespace
