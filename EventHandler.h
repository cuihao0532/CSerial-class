
#pragma once

__interface ISerialEventHandler
{
    void ReadFinish(void* pBuf,  DWORD  dwRead);
    void WriteFinish(void* pBuf, DWORD  dwWritten );
};