
#ifdef _WIN32
#include <WinSock2.h>
#endif

#include "PcapReader.h"

using namespace std;

#define THREAD_READER_BUFFER_SIZE       (100*(1<<20)) //100M
#define THREAD_READER_MAX_READ_SIZE     (64*(1<<10)) //64K

class CFileReader
{
public:
    virtual bool Open(std::string sFilePath) = 0;
    virtual void Close() = 0;
    virtual bool ReadData(uint8_t*& pData, uint32_t nDataLength) = 0;
};

// Read all file content to memory before read data
class CMemFileReader : public CFileReader
{
public:
    CMemFileReader();
    virtual ~CMemFileReader();

    virtual bool Open(std::string sFilePath);
    virtual void Close();
    virtual bool ReadData(uint8_t*& pData, uint32_t nDataLength);

private:
    FILE* m_pFile;
    uint64_t m_nFileLength;

    uint8_t* m_pBuffer;
    uint8_t* m_pReadPos;
};

CMemFileReader::CMemFileReader()
: m_pFile(NULL)
, m_nFileLength(0)
, m_pBuffer(NULL)
, m_pReadPos(NULL)
{
}

CMemFileReader::~CMemFileReader()
{
    Close();
}

bool CMemFileReader::Open(string sFilePath)
{
    bool bRet = false;
    do 
    {
        // Open file
        if (fopen_s(&m_pFile, sFilePath.c_str(), "rb") != 0)
            break;

        // Get file length
        if (_fseeki64(m_pFile, 0, SEEK_END) != 0)
            break;
        m_nFileLength = _ftelli64(m_pFile);
        if (_fseeki64(m_pFile, 0, SEEK_SET) != 0)
            break;

        // Allocate buffer
        if (!m_pBuffer)
            delete [] m_pBuffer;
        m_pBuffer = new uint8_t[(size_t)m_nFileLength];
        if (!m_pBuffer)
            break;
        m_pReadPos = m_pBuffer;

        // Load data to memory
        if (fread(m_pBuffer, 1, (size_t)m_nFileLength, m_pFile) != m_nFileLength)
            break;

        bRet = true;
    } while (false);

    if (!bRet)
        Close();

    return bRet;
}

void CMemFileReader::Close()
{
    // Close file
    if (m_pFile)
    {
        fclose(m_pFile);
        m_pFile = NULL;
    }
    m_nFileLength = 0;

    // Reset buffer pointer
    if (m_pBuffer)
    {
        delete [] m_pBuffer;
        m_pBuffer = NULL;
    }
    m_pReadPos = NULL;
}

bool CMemFileReader::ReadData(uint8_t*& pData, uint32_t nDataLength)
{
    pData = m_pReadPos;
    if (m_pReadPos + nDataLength > m_pBuffer + m_nFileLength)
    {
        m_pReadPos = m_pBuffer + m_nFileLength;
        return false;
    }
    else
    {
        m_pReadPos += nDataLength;
        return true;
    }
}

// A separate thread is used to read data from file
class CThreadFileReader : public CFileReader
{
public:
    CThreadFileReader();
    virtual ~CThreadFileReader();

    virtual bool Open(std::string sFilePath);
    virtual void Close();
    virtual bool ReadData(uint8_t*& pData, uint32_t nDataLength);

private:
    FILE* m_pFile;
    uint64_t m_nFilePos;
    bool m_bIsEof;

    uint32_t m_nBufferSize;
    uint32_t m_nMaxReadSize;
    uint8_t* m_pBuffer;
    uint8_t* m_pWritePos;
    uint8_t* m_pReadPos;

    boost::thread* m_pReadFileThread;
    boost::mutex m_Mutex;
    boost::condition_variable m_ReadDataCond;
    boost::condition_variable m_ReadFileCond;

    bool AdvanceWrite(uint32_t nLength)
    {
        // Should not override existing data
        if (nLength > RestCount())
            return false;
        m_pWritePos = m_pBuffer + (((m_pWritePos - m_pBuffer) + nLength) % (m_nBufferSize + 1));
        return true;
    }
    bool AdvanceRead(uint32_t nLength)
    {
        // Should not bigger than used data or max read size
        if (nLength > UsedCount() || nLength > m_nMaxReadSize)
            return false;
        m_pReadPos = m_pBuffer + (((m_pReadPos - m_pBuffer) + nLength) % (m_nBufferSize + 1));
        return true;
    }
    inline uint32_t UsedCount() const
    {
        return (uint32_t)((m_pWritePos >= m_pReadPos) ? (m_pWritePos - m_pReadPos) : (m_pWritePos + m_nBufferSize + 1 - m_pReadPos));
    }
    inline uint32_t RestCount() const
    {
        return m_nBufferSize - UsedCount();
    }

    inline bool CanReadData(uint32_t nLength) const
    {
        return (UsedCount() >= nLength);
    }
    inline bool CanReadFile(void) const
    {
        return (RestCount() >= m_nMaxReadSize * 2);
    }

    bool ReadFileThreadFunc(void);
};


CThreadFileReader::CThreadFileReader()
: m_pFile(NULL)
, m_nFilePos(0)
, m_bIsEof(false)
, m_nBufferSize(THREAD_READER_BUFFER_SIZE)
, m_nMaxReadSize(THREAD_READER_MAX_READ_SIZE)
, m_pBuffer(NULL)
, m_pWritePos(NULL)
, m_pReadPos(NULL)
, m_pReadFileThread(NULL)
{
}

CThreadFileReader::~CThreadFileReader()
{
    Close();
    if (m_pBuffer)
    {
        delete [] m_pBuffer;
        m_pBuffer = NULL;
    }
}

bool CThreadFileReader::Open(string sFilePath)
{
    bool bRet = false;
    do 
    {
        // Open file
        if (fopen_s(&m_pFile, sFilePath.c_str(), "rb") != 0)
            break;
        m_nFilePos = 0;
        m_bIsEof = false;

        // Allocate buffer if needed
        if (!m_pBuffer)
        {
            m_pBuffer = new uint8_t[m_nBufferSize + m_nMaxReadSize];
            if (!m_pBuffer)
                break;
            m_pWritePos = m_pReadPos = m_pBuffer;
        }

        // If no reading thread, create it
        if (!m_pReadFileThread)
        {
            m_pReadFileThread = new boost::thread(boost::bind(&CThreadFileReader::ReadFileThreadFunc, this));
            if (!m_pReadFileThread)
                break;
        }

        bRet = true;
    } while (false);

    if (!bRet)
        Close();

    return bRet;
}

void CThreadFileReader::Close()
{
    FILE* pFile = m_pFile;

    // Wait reading thread
    m_pFile = NULL;
    if (m_pReadFileThread)
    {
        m_ReadFileCond.notify_one();
        m_pReadFileThread->join();
        delete m_pReadFileThread;
        m_pReadFileThread = NULL;
    }

    // Close file
    if (pFile)
        fclose(pFile);

    // Reset buffer pointer
    m_pWritePos = m_pReadPos = m_pBuffer;
}

bool CThreadFileReader::ReadData(uint8_t*& pData, uint32_t nDataLength)
{
    {
        boost::unique_lock<boost::mutex> lock(m_Mutex);

        // Check if data is ready, if not, wait
        while (!CanReadData(nDataLength))
        {
            if (m_bIsEof)
                return false;
            m_ReadDataCond.wait(lock);
        }

        // Data is ready, set pointer and length
        pData = m_pReadPos;
        if (!AdvanceRead(nDataLength))
            return false;

        // Notify read file thread if needed
        if (!m_bIsEof && CanReadFile())
            m_ReadFileCond.notify_one();
    }
    return true;
}

bool CThreadFileReader::ReadFileThreadFunc(void)
{
    FILE* pFile;
    uint32_t nReadLength;
    bool bRet = false;

    for (;;)
    {
        {
            boost::unique_lock<boost::mutex> lock(m_Mutex);

            // Check if can read file
            while (!CanReadFile())
            {
                m_ReadFileCond.wait(lock);
            }
            // Check if stop
            pFile = m_pFile;
            if (!pFile)
            {
                bRet = true;
                break;
            }

            nReadLength = RestCount() - m_nMaxReadSize;
        }

        // No need to hold lock when reading file
        // Can read file, read till rest count is n_MaxReadSize
        if ((m_pWritePos + nReadLength) > (m_pBuffer + m_nBufferSize + 1)) // wrap
        {
            uint32_t nReadLengthTemp = (uint32_t)(m_pBuffer + m_nBufferSize + 1 - m_pWritePos);
            uint32_t nReadNum;
            nReadNum = (uint32_t)fread(m_pWritePos, sizeof(uint8_t), nReadLengthTemp, pFile);
            AdvanceWrite(nReadNum);
            m_nFilePos += nReadNum;
            if (nReadNum != nReadLengthTemp)
                break;

            nReadLengthTemp = nReadLength - nReadLengthTemp;
            nReadNum = (uint32_t)fread(m_pWritePos, sizeof(uint8_t), nReadLengthTemp, pFile);
            AdvanceWrite(nReadNum);
            m_nFilePos += nReadNum;
            // Copy max read size data to tail
            memcpy(m_pBuffer + m_nBufferSize + 1, m_pBuffer, min(m_nMaxReadSize, nReadNum));
            if (nReadNum != nReadLengthTemp)
                break;
        }
        else
        {
            uint32_t nReadNum;
            nReadNum = (uint32_t)fread(m_pWritePos, sizeof(uint8_t), nReadLength, pFile);
            AdvanceWrite(nReadNum);
            m_nFilePos += nReadNum;
            if (nReadNum != nReadLength)
                break;
        }

        m_ReadDataCond.notify_one();
    }

    m_bIsEof = true;

    m_ReadDataCond.notify_one();

    return bRet;
}

CPcapReader::CPcapReader()
: m_pFileReader(NULL)
, m_bIsBe(false)
{
    memset(&m_PcapHeader, 0, sizeof(m_PcapHeader));
}

CPcapReader::~CPcapReader()
{
    if (m_pFileReader)
    {
        delete m_pFileReader;
        m_pFileReader = NULL;
    }
}

bool CPcapReader::Open(string sFilePath)
{
    bool bRet = false;
    do 
    {
        if (!m_pFileReader)
        {
            m_pFileReader = new CMemFileReader();
            if (!m_pFileReader)
                break;
        }

        if (!m_pFileReader->Open(sFilePath))
            break;

        // Read pcap header
        uint8_t* pData;
        if (!m_pFileReader->ReadData(pData, sizeof(m_PcapHeader)))
            break;
        memcpy(&m_PcapHeader, pData, sizeof(m_PcapHeader));

        // Check big or little endian
        // The magic number is 0xa1b2c3d4. If the writing
        // machine was BE, the first byte will be a1 else d4
        if (m_PcapHeader.uMagicNumber == 0xa1b2c3d4)
            m_bIsBe = true;
        else if (ntohl(m_PcapHeader.uMagicNumber) == 0xa1b2c3d4)
            m_bIsBe = false;
        else
            break;

        if (!m_bIsBe)
        {
            m_PcapHeader.uMagicNumber = ntohl(m_PcapHeader.uMagicNumber);
            m_PcapHeader.uVerMajor = ntohs(m_PcapHeader.uVerMajor);
            m_PcapHeader.uVerMinor = ntohs(m_PcapHeader.uVerMinor);
            m_PcapHeader.uThisZone = ntohl(m_PcapHeader.uThisZone);
            m_PcapHeader.uSigfigs = ntohl(m_PcapHeader.uSigfigs);
            m_PcapHeader.uSnaplen = ntohl(m_PcapHeader.uSnaplen);
            m_PcapHeader.uNetwork = ntohl(m_PcapHeader.uNetwork);
        }

        bRet = true;
    } while (false);

    if (!bRet)
        Close();

    return true;
}

void CPcapReader::Close()
{
    if (m_pFileReader)
        m_pFileReader->Close();
}

bool CPcapReader::ReadNextPktHeader(PcapPktHeader*& pPktHeader)
{
    if (m_pFileReader->ReadData((uint8_t*&)pPktHeader, sizeof(PcapPktHeader)))
    {
        if (!m_bIsBe)
        {
            pPktHeader->ts_sec = ntohl(pPktHeader->ts_sec);
            pPktHeader->ts_usec = ntohl(pPktHeader->ts_usec);
            pPktHeader->incl_len = ntohl(pPktHeader->incl_len);
            pPktHeader->orig_len = ntohl(pPktHeader->orig_len);
        }
        return true;
    }
    return false;
}

bool CPcapReader::ReadNextPkt(uint8_t*& pPkt, uint32_t nPktLength)
{
    return (m_pFileReader->ReadData(pPkt, nPktLength));
}

bool CPcapReader::ReadNext(uint8_t*& pPkt, PcapPktHeader*& pPktHeader)
{
    if (!ReadNextPktHeader(pPktHeader))
        return false;
    return (m_pFileReader->ReadData(pPkt, pPktHeader->incl_len));
}
