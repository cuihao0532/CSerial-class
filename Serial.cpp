#include "StdAfx.h"
#include "Serial.h"

namespace cuish
{
    CSerial::CSerial(void)
    {
        m_hSerialPort = INVALID_HANDLE_VALUE;
        m_hIocp       = NULL;
        m_dwCompletionKey = 1;
        memset(&m_overlapped, 0, sizeof(m_overlapped));

        m_pEventHandler = NULL;
    }


    CSerial::CSerial(ISerialEventHandler* pHandler)
    {
        m_hSerialPort = INVALID_HANDLE_VALUE;
        m_hIocp       = NULL;
        m_dwCompletionKey = 1;
        memset(&m_overlapped, 0, sizeof(m_overlapped));

        m_pEventHandler = pHandler; 
    }

    CSerial::~CSerial(void)
    {
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
            m_hSerialPort = CreateFile(lpSerialPort, 0, eMode, NULL, OPEN_EXISTING, FILE_FLAG_OVERLAPPED, NULL);
            if ( INVALID_HANDLE_VALUE == m_hSerialPort )
            { 
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

        while ( true )
        {
            if ( pHandler )
            {
                pHandler->ReadFinish();
                pHandler->WriteFinish();
            }
        }
        

        return 0;
    }



} // namespace
