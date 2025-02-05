////////////////////////////////////////////////////////////////////////////////
//
// From David J. Kruglinski (Inside Visual C++).
//
////////////////////////////////////////////////////////////////////////////////

// CBlockingSocketException, CBlockingSocket, CHttpBlockingSocket
#include "..\stdafx.h"
#include "BlockingSocket.h"


namespace vEngine {
using namespace nsSocket;

///////////////////////////////////////////////////////////////////////////////////////
//************************* Class CBlockingSocketException **************************//
///////////////////////////////////////////////////////////////////////////////////////

CBlockingSocketException::CBlockingSocketException(LPTSTR pchMessage)
{
   m_strMessage = pchMessage;
   m_nError     = WSAGetLastError();
}

BOOL CBlockingSocketException::GetErrorMessage(LPTSTR lpstrError, UINT nMaxError,
      PUINT /*pnHelpContext = NULL*/)
{
   if( m_nError == 0 )
      _sntprintf(lpstrError, nMaxError, _T("%s error"), m_strMessage.c_str());
   else
      _sntprintf(lpstrError, nMaxError, _T("%s error 0x%08x"), m_strMessage.c_str(), m_nError);
   return TRUE;
}

tstring CBlockingSocketException::GetErrorMessage(PUINT /*pnHelpContext = NULL*/)
{
   TCHAR szBuffer[512] = {0};
   GetErrorMessage(szBuffer, 512);

   return szBuffer;
}


///////////////////////////////////////////////////////////////////////////////////////
//******************************** Class CBlockingSocket ****************************//
///////////////////////////////////////////////////////////////////////////////////////

IBlockingSocket* CBlockingSocket::CreateInstance() const
{
   return new CBlockingSocket();
}

void CBlockingSocket::Cleanup()
{
   // doesn't throw an exception because it's called in a catch block
   if( m_hSocket==0 ) 
      return;

   closesocket(m_hSocket);
   m_hSocket = 0;
}

void CBlockingSocket::Create(int nType /* = SOCK_STREAM */)
{
   ASSERT( m_hSocket==0 );
   if( (m_hSocket=socket(AF_INET, nType, 0))==INVALID_SOCKET ) 
   {
      throw CBlockingSocketException(_T("Create"));
   }
}

void CBlockingSocket::Bind(LPCSOCKADDR psa) const
{
   ASSERT( m_hSocket!=0 );
   if( bind(m_hSocket, psa, sizeof(SOCKADDR))==SOCKET_ERROR )
   {
      throw CBlockingSocketException(_T("Bind"));
   }
}

void CBlockingSocket::Listen() const
{
   ASSERT( m_hSocket!=0 );
   if( listen(m_hSocket, 5)==SOCKET_ERROR )
   {
      throw CBlockingSocketException(_T("Listen"));
   }
}

bool CBlockingSocket::Accept(IBlockingSocket& sConnect, LPSOCKADDR psa) const
{
   CBlockingSocket* pConnect = static_cast<CBlockingSocket*>(&sConnect); //+#
   ASSERT( m_hSocket!=0 );
   ASSERT( pConnect->m_hSocket==0 );

   int nLengthAddr = sizeof(SOCKADDR);
   pConnect->m_hSocket = accept(m_hSocket, psa, &nLengthAddr);

   if( pConnect->operator SOCKET()==INVALID_SOCKET )
   {
      // no exception if the listen was canceled
      if( WSAGetLastError() !=WSAEINTR ) 
      {
         throw CBlockingSocketException(_T("Accept"));
      }
      return false;
   }
   return true;
}

void CBlockingSocket::Close()
{
   if( m_hSocket && closesocket(m_hSocket)==SOCKET_ERROR )
   {
      // should be OK to close if closed already
      throw CBlockingSocketException(_T("Close"));
   }
   m_hSocket = 0;
}

void CBlockingSocket::Connect(LPCSOCKADDR psa) const
{
   ASSERT( m_hSocket!=0 );
   // should timeout by itself
   if( connect(m_hSocket, psa, sizeof(SOCKADDR))==SOCKET_ERROR )
   {
      throw CBlockingSocketException(_T("Connect"));
   }
}

int CBlockingSocket::Write(const char* pch, int nSize, int nSecs) const
{
   int         nBytesSent        = 0;
   int         nBytesThisTime;
   const char* pch1              = pch;

   do
   {
      nBytesThisTime = Send(pch1, nSize - nBytesSent, nSecs);
      nBytesSent += nBytesThisTime;
      pch1 += nBytesThisTime;
   } while( nBytesSent<nSize );

   return nBytesSent;
}

int CBlockingSocket::Send(const char* pch, int nSize, int nSecs) const
{
   ASSERT( m_hSocket!=0 );
   
   // returned value will be less than nSize if client cancels the reading
   FD_SET  fd = { 1, m_hSocket };
   TIMEVAL tv = { nSecs, 0 };

   if( select(0, NULL, &fd, NULL, &tv)==0 )
   {
      throw CBlockingSocketException(_T("Send timeout"));
   }

   int nBytesSent;
   if( (nBytesSent=send(m_hSocket, pch, nSize, 0))==SOCKET_ERROR )
   {
      throw CBlockingSocketException(_T("Send"));
   }

   return nBytesSent;
}

bool CBlockingSocket::CheckReadability() const
{
   ASSERT( m_hSocket!=0 );
   
   FD_SET  fd = { 1, m_hSocket };
   TIMEVAL tv = { 0, 0 };

   const int iRet = select(0, &fd, NULL, NULL, &tv);
   
   if( iRet==SOCKET_ERROR )
   {
      throw CBlockingSocketException(_T("Socket Error"));
   }

   return iRet == 1;
}

int CBlockingSocket::Receive(char* pch, int nSize, int nSecs) const
{
   ASSERT( m_hSocket!=0 );
   
   FD_SET  fd = { 1, m_hSocket };
   TIMEVAL tv = { nSecs, 0 };

   if( select(0, &fd, NULL, NULL, &tv)==0 )
   {
      throw CBlockingSocketException(_T("Receive timeout"));
   }

   int nBytesReceived;
   if( (nBytesReceived=recv(m_hSocket, pch, nSize, 0))==SOCKET_ERROR )
   {
      throw CBlockingSocketException(_T("Receive"));
   }

   return nBytesReceived;
}

int CBlockingSocket::ReceiveDatagram(char* pch, int nSize, LPSOCKADDR psa, int nSecs) const
{
   ASSERT( m_hSocket!=0 );

   FD_SET  fd = { 1, m_hSocket };
   TIMEVAL tv = { nSecs, 0 };

   if( select(0, &fd, NULL, NULL, &tv)==0 )
   {
      throw CBlockingSocketException(_T("Receive timeout"));
   }

   // input buffer should be big enough for the entire datagram
   int nFromSize = sizeof(SOCKADDR);
   int nBytesReceived = recvfrom(m_hSocket, pch, nSize, 0, psa, &nFromSize);

   if( nBytesReceived==SOCKET_ERROR )
   {
      throw CBlockingSocketException(_T("ReceiveDatagram"));
   }

   return nBytesReceived;
}

int CBlockingSocket::SendDatagram(const char* pch, int nSize, LPCSOCKADDR psa, int nSecs) const
{
   ASSERT( m_hSocket!=0 );

   FD_SET  fd = { 1, m_hSocket };
   TIMEVAL tv = { nSecs, 0 };

   if( select(0, NULL, &fd, NULL, &tv)==0 )
   {
      throw CBlockingSocketException(_T("Send timeout"));
   }

   int nBytesSent = sendto(m_hSocket, pch, nSize, 0, psa, sizeof(SOCKADDR));
   if( nBytesSent==SOCKET_ERROR )
   {
      throw CBlockingSocketException(_T("SendDatagram"));
   }

   return nBytesSent;
}

void CBlockingSocket::GetPeerAddr(LPSOCKADDR psa) const
{
   ASSERT( m_hSocket!=0 );

   // gets the address of the socket at the other end
   int nLengthAddr = sizeof(SOCKADDR);
   if( getpeername(m_hSocket, psa, &nLengthAddr)==SOCKET_ERROR )
   {
      throw CBlockingSocketException(_T("GetPeerName"));
   }
}

void CBlockingSocket::GetSockAddr(LPSOCKADDR psa) const
{
   ASSERT( m_hSocket!=0 );

   // gets the address of the socket at this end
   int nLengthAddr = sizeof(SOCKADDR);
   if( getsockname(m_hSocket, psa, &nLengthAddr)==SOCKET_ERROR ) 
   {
      throw CBlockingSocketException(_T("GetSockName"));
   }
}

CSockAddr CBlockingSocket::GetHostByName(const char* pchName, USHORT ushPort /* = 0 */)
{
   hostent* pHostEnt = gethostbyname(pchName);
   
   if( pHostEnt==NULL)
   {
      throw CBlockingSocketException(_T("GetHostByName"));
   }

   ULONG* pulAddr = (ULONG*) pHostEnt->h_addr_list[0];
   SOCKADDR_IN sockTemp;
   sockTemp.sin_family = AF_INET;
   sockTemp.sin_port = htons(ushPort);
   sockTemp.sin_addr.s_addr = *pulAddr; // address is already in network byte order
   return sockTemp;
}

const char* CBlockingSocket::GetHostByAddr(LPCSOCKADDR psa)
{
   hostent* pHostEnt = gethostbyaddr((char*) &((LPSOCKADDR_IN) psa)
            ->sin_addr.s_addr, 4, PF_INET);
   
   if( pHostEnt==NULL )
   {
      throw CBlockingSocketException(_T("GetHostByAddr"));
   }

   return pHostEnt->h_name; // caller shouldn't delete this memory
}

///////////////////////////////////////////////////////////////////////////////////////
//**************************** Class CHttpBlockingSocket ****************************//
///////////////////////////////////////////////////////////////////////////////////////

CHttpBlockingSocket::CHttpBlockingSocket()
{
   m_pReadBuf = new char[nSizeRecv];
   m_nReadBuf = 0;
}

CHttpBlockingSocket::~CHttpBlockingSocket()
{
   delete[] m_pReadBuf;
}

int CHttpBlockingSocket::ReadHttpHeaderLine(char* pch, int nSize, int nSecs)
// reads an entire header line through CRLF (or socket close)
// inserts zero string terminator, object maintains a buffer
{
   int       nBytesThisTime = m_nReadBuf;
   ptrdiff_t nLineLength    = 0;
   char*     pch1           = m_pReadBuf;
   char*     pch2           = NULL;

   do 
   {
      // look for lf (assume preceded by cr)
      if( (pch2=(LPSTR)memchr(pch1 , '\n', nBytesThisTime))!=NULL )
      {
         ASSERT( (pch2)>m_pReadBuf );
         ASSERT( *(pch2 - 1)=='\r' );
         nLineLength = (pch2 - m_pReadBuf) + 1;
         if( nLineLength>=nSize )
            nLineLength = nSize - 1;
         memcpy(pch, m_pReadBuf, nLineLength); // copy the line to caller
         m_nReadBuf -= static_cast<unsigned int>(nLineLength);
         memmove(m_pReadBuf, pch2 + 1, m_nReadBuf); // shift remaining characters left
         break;
      }
      pch1 += nBytesThisTime;
      nBytesThisTime = Receive(m_pReadBuf + m_nReadBuf, nSizeRecv - m_nReadBuf, nSecs);
      if( nBytesThisTime<=0 )
      { // sender closed socket or line longer than buffer
         throw CBlockingSocketException(_T("ReadHeaderLine"));
      }
      m_nReadBuf += nBytesThisTime;
   } while( true );
   *(pch + nLineLength) = _T('\0');

   return static_cast<unsigned int>(nLineLength);
}

// reads remainder of a transmission through buffer full or socket close
// (assume headers have been read already)
int CHttpBlockingSocket::ReadHttpResponse(char* pch, int nSize, int nSecs)
{
   int nBytesToRead, nBytesThisTime, nBytesRead = 0;
   
   if( m_nReadBuf>0)
   { // copy anything already in the recv buffer
      memcpy(pch, m_pReadBuf, m_nReadBuf);
      pch += m_nReadBuf;
      nBytesRead = m_nReadBuf;
      m_nReadBuf = 0;
   }
   do
   { // now pass the rest of the data directly to the caller
      nBytesToRead = min(static_cast<int>(nSizeRecv), nSize - nBytesRead);
      nBytesThisTime = Receive(pch, nBytesToRead, nSecs);
      if( nBytesThisTime<=0 )
         break; // sender closed the socket
      pch += nBytesThisTime;
      nBytesRead += nBytesThisTime;
   } while( nBytesRead<=nSize );

   return nBytesRead;
}

IBlockingSocket* nsSocket::CreateDefaultBlockingSocketInstance()
{
   return new CBlockingSocket();
}

}