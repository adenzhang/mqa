#include <vector>
#include <iostream>
#include <boost/chrono.hpp>
#include "LogUtility.h"
#include "OptionConfig.h"
#include "PcapReader.h"
#ifdef USE_ManualParser
//#include "ManualParser.h"
#else
#include "StatsFrameParser.h"
#endif

#include "MQmonIf.h"

using namespace std;
using namespace boost::chrono;
//using namespace mqa;

#define VQT_STREAM_BUFFER_LENGTH    (2*(1<<10)) // 2K

typedef mqa::StatsFrameParser CMQmonFrameInfo;
typedef std::list<mqa::StatsFrameParser*> CMQmonFrameList;

class CVqtWorkerThread;
class CVqtStream
{
public:
    CVqtStream(CVqtWorkerThread& _Thread, uint32_t _Id, CMQmonFrameList& _FrameList)
        : Thread(_Thread)
        , nId(_Id)
        , FrameList(_FrameList)
        , nThroughput(0)
        , nPlayedLength(0)
        , pBuffer(NULL)
    {}

    ~CVqtStream()
    {
        if (pBuffer)
        {
            delete [] pBuffer;
            pBuffer = NULL;
        }
    }

    CVqtWorkerThread& Thread;
    uint32_t nId;
    CMQmonFrameList& FrameList;
    CMQmonFrameList::const_iterator ItNext;
    uint32_t nThroughput;
    uint64_t nPlayedLength;
    boost::chrono::steady_clock::time_point nStartTime;
    uint8_t* pBuffer;
};
typedef std::vector<CVqtStream*> CVqtStreamList;

typedef std::vector<CMQmonFrameList*> CVqtFrameListSet;
class CVqtInstance;
class CVqtWorkerThread
{
public:
    CVqtWorkerThread(CVqtInstance& Instance, uint32_t nId)
        : m_Instance(Instance)
        , m_nId(nId)
        , m_pMQmonInterface(NULL)
        , m_nSleepMSec(0)
        , m_nTotalLength(0)
        , m_nTotalPacket(0)
        , m_nDetectedStream(0)
    {
        m_pThread = new boost::thread(&CVqtWorkerThread::WorkerThreadFunc, this);
        m_pMQmonInterface = mqa::MQmon::Instance()->CreateInterface();
    }
    ~CVqtWorkerThread()
    {
        if (m_pThread)
        {
            m_pThread->interrupt();
            m_pThread->join();
            delete m_pThread;
            m_pThread = NULL;
        }

        for (CVqtStreamList::const_iterator It = m_StreamList.begin(); It != m_StreamList.end(); ++It)
            delete *It;
        m_StreamList.clear();

        if (m_pMQmonInterface)
        {
            delete m_pMQmonInterface;
            m_pMQmonInterface = NULL;
        }
    }

    inline uint32_t Id(void) const { return m_nId; }

    boost::thread* Thread() { return m_pThread; }

private:
    CVqtInstance& m_Instance;
    uint32_t m_nId;
    boost::thread* m_pThread;
    CVqtStreamList m_StreamList;
    steady_clock::time_point m_nStartLogTime;
    steady_clock::time_point m_nLastLogTime;
    mqa::CMQmonInterface* m_pMQmonInterface;
    mqa::CMQmonMetrics m_MQmonMetrics;

    uint64_t m_nSleepMSec;
    uint64_t m_nTotalLength;
    uint64_t m_nTotalPacket;
    UINT32   m_nDetectedStream;

    void Report(steady_clock::time_point* pNow = NULL, bool bForce = false);
    bool CreateStreams(void);
    void SetIpAddress(uint8_t* pAddr, bool bIpv6, uint32_t nThreadId, uint32_t nStreamId);
#ifdef USE_ManualParser
    void SetAddress(CVqtStream& Stream, CMQmonFrameInfo* pInfo, MQmonFrameInfoIp* pIpInfo = NULL);
    bool IndicatePacket(CVqtStream& Stream, CMQmonFrameInfo* pInfo, const MQmonFrameInfoIp* pIpInfo);
#else
    void SetAddress(CVqtStream& Stream, CMQmonFrameInfo* parser);
    bool IndicatePacket(CVqtStream& Stream, CMQmonFrameInfo* parser);
#endif
    bool PlayOneStream(CVqtStream& Stream, const steady_clock::time_point& nNow, uint64_t& nSleepMSecs, bool& bFinished);
    void WorkerThreadFunc(void);
};

class CVqtWorkerThreadList
{
public:
    typedef std::vector<CVqtWorkerThread*> VqtWorkerThreadList;

    CVqtWorkerThreadList() {}
    ~CVqtWorkerThreadList()
    {
        JoinAll();
        for (VqtWorkerThreadList::const_iterator It = m_ThreadList.begin(); It != m_ThreadList.end(); ++It)
            delete *It;
        m_ThreadList.clear();
    }

    void JoinAll()
    {
        for (VqtWorkerThreadList::iterator It = m_ThreadList.begin(); It != m_ThreadList.end(); ++It)
            (*It)->Thread()->join();
    }

    VqtWorkerThreadList& ThreadList() { return m_ThreadList; }

private:
    std::vector<CVqtWorkerThread*> m_ThreadList;
};

class CVqtInstance
{
public:
    CVqtInstance(CVqtConfig& Config)
        : m_Config(Config)
        , m_bStop(false)
    {};
    ~CVqtInstance()
    {
        m_bStop = true;
        Fini();
    }

    bool Init(void);
    void Fini(void);
    bool LoadTraffic(void);
    bool LaunchWorkers(void);

private:
    const CVqtConfig& m_Config;
    CVqtFrameListSet m_FrameListSet;
    CVqtWorkerThreadList m_ThreadList;
    bool m_bStop;

    bool LoadPcap(const std::string& sTrafficFile, CMQmonFrameList& FrameList);

    friend class CVqtWorkerThread;
};

bool CVqtInstance::Init(void)
{
    m_bStop = false;
    return true;
}

void CVqtInstance::Fini(void)
{
    m_ThreadList.JoinAll();
    for (CVqtFrameListSet::const_iterator It = m_FrameListSet.begin(); It != m_FrameListSet.end(); ++It)
    {
        for (CMQmonFrameList::const_iterator It1 = (*It)->begin(); It1 != (*It)->end(); ++It1)
            delete *It1;
        (*It)->clear();
        delete *It;
    }
    m_FrameListSet.clear();
}

bool CVqtInstance::LoadPcap(const std::string& sTrafficFile, CMQmonFrameList& FrameList)
{
    CPcapReader PcapReader;

    if (!PcapReader.Open(sTrafficFile))
        return false;

    PcapFileHeader& PcapHeader = PcapReader.PcapHeader();
    for (;;)
    {
        PcapPktHeader* pPktHeader;
        uint8_t* pPkt;

        if (!PcapReader.ReadNext(pPkt, pPktHeader))
            break;

        // Allocate frame info and parse each packet
        CMQmonFrameInfo* pFrameInfo = new CMQmonFrameInfo();
        if (!pFrameInfo)
            return false;
#ifdef USE_ManualParser
        if (!pFrameInfo->Extract(pPkt, pPktHeader->incl_len, m_Config.bParseIpInfo))
        {
            delete pFrameInfo;
            continue;
        }
        pFrameInfo->nTimestamp.tv_sec = pPktHeader->ts_sec;
        pFrameInfo->nTimestamp.tv_usec = pPktHeader->ts_usec;
#else
        if(!pFrameInfo->ParseFrame(pPkt, pPktHeader->incl_len, 0)) 
        {
            delete pFrameInfo;
            continue;
        }
        pFrameInfo->nTimestamp.tv_sec = pPktHeader->ts_sec;
        pFrameInfo->nTimestamp.tv_usec = pPktHeader->ts_usec;
#endif

        // Insert into queue
        FrameList.push_back(pFrameInfo);
    }

    cout << "Frames in " << sTrafficFile << ": " << FrameList.size() << endl;

    return true;
}

bool CVqtInstance::LoadTraffic(void)
{
    for (uint32_t i = 0; i < m_Config.vTraffic.size(); ++i)
    {
        CMQmonFrameList* pFrameList = new CMQmonFrameList;
        if (!pFrameList)
            return false;
        if (!LoadPcap(m_Config.vTraffic[i].sFile, *pFrameList))
        {
            delete pFrameList;
            return false;
        }
        m_FrameListSet.push_back(pFrameList);
    }
    return true;
}

bool CVqtInstance::LaunchWorkers(void)
{
    for (uint32_t i = 0; i < m_Config.nThread; ++i)
    {
        CVqtWorkerThread* pThread = new CVqtWorkerThread(*this, i);
        if (!pThread)
        {
            VqtError("Create thread %d error.\n", i);
            return false;
        }
        m_ThreadList.ThreadList().push_back(pThread);
    }
    return true;
}

bool CVqtWorkerThread::CreateStreams(void)
{
    uint32_t nId = 0;
    for (uint32_t i = 0; i < m_Instance.m_Config.vTraffic.size(); ++i)
    {
        CMQmonFrameList& FrameList = *m_Instance.m_FrameListSet[i];
        for (uint32_t j = 0; j < m_Instance.m_Config.vTraffic[i].nStreams; ++j)
        {
            CVqtStream* pStream = new CVqtStream(*this, nId++, FrameList);
            if (!pStream)
            {
                VqtError("Allocate stream %d error.\n", i);
                return false;
            }
            pStream->nThroughput = m_Instance.m_Config.vTraffic[i].nThroughput;
            pStream->ItNext = FrameList.begin();
            if (!m_Instance.m_Config.bParseIpInfo)
            {
                pStream->pBuffer = new uint8_t[VQT_STREAM_BUFFER_LENGTH];
                if (!pStream->pBuffer)
                {
                    VqtError("Allocate stream buffer %d error.\n", i);
                    return false;
                }
            }

            m_StreamList.push_back(pStream);
        }
    }

    return true;
}

void CVqtWorkerThread::Report(steady_clock::time_point* pNow, bool bForce)
{
    // Print info every 1 second
    steady_clock::time_point nNow = (pNow ? *pNow : steady_clock::now());
    if (!bForce && (nNow - m_nLastLogTime) < milliseconds(1000))
        return;

    if (!m_pMQmonInterface->GetMetrics(m_MQmonMetrics))
        VqtError("GetMetrics error in %s.\n", __FUNCTION__);

    milliseconds nElapsed = duration_cast<milliseconds>(nNow - m_nStartLogTime);
    if (milliseconds(0) == nElapsed)
        nElapsed = milliseconds(1);
    VqtInfo("Thread %d: length %I64u, packet %I64u, elapsed %I64u, MBps %I64u, Pps %I64u, sleep %I64u\n",
        m_nId, m_nTotalLength, m_nTotalPacket, nElapsed.count(),
        m_nTotalLength * 1000 / nElapsed.count(),
        m_nTotalPacket * 1000 / nElapsed.count(),
        m_nSleepMSec);
    m_nSleepMSec = 0;

    m_nLastLogTime = nNow;
}

void CVqtWorkerThread::SetIpAddress(uint8_t* pAddr, bool bIpv6, uint32_t nThreadId, uint32_t nStreamId)
{
    // IP address is set according to stream id and thread id
    // First highest 24 bit is added by StreamId, next 8 bit is added by ThreadId
    uint32_t nAddr = *(uint32_t*)pAddr;
    nAddr = (((nAddr >> 8) + nStreamId) << 8) | (((nAddr & 0x000000ff) + nThreadId) & 0x000000ff);

    *(uint32_t*)pAddr = nAddr;
}

#ifdef USE_ManualParser
void CVqtWorkerThread::SetAddress(CVqtStream& Stream, CMQmonFrameInfo* pInfo, MQmonFrameInfoIp* pIpInfo)
{
    if (!pInfo)
    {
        VqtError("Wrong argument in %s.\n", __FUNCTION__);
        return;
    }

    uint8_t* pSrcAddr;
    uint8_t* pDstAddr;
    if (m_Instance.m_Config.bParseIpInfo && pIpInfo)
    {
        pSrcAddr = pIpInfo->aSrcAddr.Ipv4;
        pDstAddr = pIpInfo->aDstAddr.Ipv4;
    }
    else
    {
        pSrcAddr = pInfo->SrcAddrInnerPtr(Stream.pBuffer);
        pDstAddr = pInfo->DstAddrInnerPtr(Stream.pBuffer);
    }

    bool bIpv6 = (pInfo->IsTunnel() ? pInfo->IsIpUpperIpv6() : pInfo->IsIpLowerIpv6());
    SetIpAddress(pSrcAddr, bIpv6, m_nId, Stream.nId);
    SetIpAddress(pDstAddr, bIpv6, m_nId, Stream.nId);

    return;
}
bool CVqtWorkerThread::IndicatePacket(CVqtStream& Stream, CMQmonFrameInfo* pInfo, const MQmonFrameInfoIp* pIpInfo)
{
    if (!pInfo)
    {
        VqtError("Wrong argument in %s.\n", __FUNCTION__);
        return false;
    }
    uint8_t* pBuffer = (m_Instance.m_Config.bParseIpInfo ? pInfo->Frame() : Stream.pBuffer);
    if (m_Instance.m_Config.nThread > 0)
    {
        //static boost::mutex Mutex;
        //boost::unique_lock<boost::mutex> lock(Mutex);
        //if (!m_pMQmonInterface->IndicatePacket(pBuffer, pInfo->FrameLength(), *pInfo, pIpInfo))  // @az todo
            return false;
    }
    else
    {
        //if (!m_pMQmonInterface->IndicatePacket(pBuffer, pInfo->FrameLength(), *pInfo, pIpInfo))  // @az todo
            return false;
    }

    return true;
}
#else
void CVqtWorkerThread::SetAddress(CVqtStream& Stream, CMQmonFrameInfo* parser)
{
    uint8_t* pSrcAddr = (uint8_t*)parser->SrcIp();
    uint8_t* pDstAddr = (uint8_t*)parser->DestIp();
    SetIpAddress(pSrcAddr, !parser->IsIpv4(), m_nId, Stream.nId);
    SetIpAddress(pDstAddr, !parser->IsIpv4(), m_nId, Stream.nId);
}
bool CVqtWorkerThread::IndicatePacket(CVqtStream& Stream, CMQmonFrameInfo* parser)
{
    UINT32       len;
    const UINT8 *p = parser->GetTransPayload(len);
    
    if( mqa::MQMON_NOTIFY_ACTIVATING == m_pMQmonInterface->IndicateRtpPacket(
        m_nDetectedStream, p, len, parser->nTimestamp.tv_sec, parser->nTimestamp.tv_usec*1000, NULL) )
    {
        m_nDetectedStream++;
    }

    return false;
}
#endif


bool CVqtWorkerThread::PlayOneStream(CVqtStream& Stream, const steady_clock::time_point& nNow, uint64_t& nSleepMSecs, bool& bFinished)
{
    if (Stream.ItNext == Stream.FrameList.end())
    {
        bFinished = true;
        return true;
    }

    uint64_t nMSecs = duration_cast<milliseconds>(nNow - Stream.nStartTime).count();
    uint64_t nCanPlay = nMSecs * Stream.nThroughput / 1000;
    if (Stream.nThroughput != 0) // Need time throttle
    {
        if (Stream.nPlayedLength > nCanPlay)
        {
            nSleepMSecs = (Stream.nPlayedLength - nCanPlay) * 1000 / Stream.nThroughput;
            return true;
        }
    }

    // Play some packets
    for (uint32_t i = 0; i < m_Instance.m_Config.nPacketsPerPlay; ++i)
    {
        if (Stream.ItNext == Stream.FrameList.end())
        {
            bFinished = true;
            break;
        }

        CMQmonFrameInfo* pInfo = *Stream.ItNext;
#ifdef USE_ManualParser
        MQmonFrameInfoIp IpInfo;
        if (!m_Instance.m_Config.bParseIpInfo) // Copy frame
            memcpy(Stream.pBuffer, pInfo->Frame(), pInfo->FrameLength());
        else
            memcpy(&IpInfo, &pInfo->tIpInfo, sizeof(IpInfo));

        // Change IP address
        SetAddress(Stream, pInfo, &IpInfo);

        // Replay the packet
        if (!m_Instance.m_Config.bIdleRun && !IndicatePacket(Stream, pInfo, (m_Instance.m_Config.bParseIpInfo ? &IpInfo : NULL)))
            VqtError("Error IndicatePacket in %s.\n", __FUNCTION__);
        //else if (m_Instance.m_Config.bIdleRun)
        //{
        //    int j = 2;
        //    for (int i = 0; i < 100000; ++i)
        //        j *= 2;
        //}

        Stream.nPlayedLength += pInfo->FrameLength();
        m_nTotalLength += pInfo->FrameLength();

#else
        SetAddress(Stream, pInfo);
        // Replay the packet
        if (!m_Instance.m_Config.bIdleRun && !IndicatePacket(Stream, pInfo))
            VqtError("Error IndicatePacket in %s.\n", __FUNCTION__);

        Stream.nPlayedLength += pInfo->nDataLength;
        m_nTotalLength += pInfo->nDataLength;
#endif
        ++m_nTotalPacket;

        ++Stream.ItNext;
    }

    if (Stream.nThroughput != 0) // Need time throttle
    {
        // Check if need sleep
        if (Stream.nPlayedLength > nCanPlay)
            nSleepMSecs = (Stream.nPlayedLength - nCanPlay) * 1000 / Stream.nThroughput;
        else
            nSleepMSecs = 0;
    }

    return true;
}

void CVqtWorkerThread::WorkerThreadFunc(void)
{
    VqtInfo("Worker thread %d starts.\n", m_nId);

    // Create streams.
    if (!CreateStreams())
        return;

    steady_clock::time_point nNow = steady_clock::now();
    for (CVqtStreamList::const_iterator It = m_StreamList.begin(); It != m_StreamList.end(); ++It)
        (*It)->nStartTime = nNow;
    m_nStartLogTime = m_nLastLogTime = nNow;
    for (uint32_t nLoops = 0;;++nLoops)
    {
        if (m_Instance.m_bStop)
            break;

        nNow = steady_clock::now();
        uint32_t nFinishStream = 0;
        uint64_t nLeastSleepMSecs = UINT64_MAX;
        for (CVqtStreamList::const_iterator It = m_StreamList.begin(); It != m_StreamList.end(); ++It)
        {
            bool bFinished = false;
            uint64_t nSleepMSecs = UINT64_MAX;
            if (!PlayOneStream(**It, nNow, nSleepMSecs, bFinished))
                return;
            if (bFinished)
                ++nFinishStream;
            if (nSleepMSecs < nLeastSleepMSecs)
                nLeastSleepMSecs = nSleepMSecs;

            Report(NULL);
        }

        // If all streams finished, stop
        if (nFinishStream == m_StreamList.size())
            break;

        // Time throttle
        if (nLeastSleepMSecs > 0 && nLeastSleepMSecs != UINT64_MAX)
        {
            boost::this_thread::sleep_for(milliseconds(nLeastSleepMSecs));
            m_nSleepMSec += nLeastSleepMSecs;
        }
        else if (m_Instance.m_Config.nThread > 1)
        {
            boost::this_thread::yield();
        }
    }

    Report(NULL, true);
    VqtInfo("Worker thread %d ends.\n", m_nId);

    return;
}

int main00(int argc, char** argv)
{
    getchar();
    CVqtConfig Config;
    if (!Config.Parse(argc, argv))
        return 1;

    //if (!MQmonInit(MQMON_TELCHEMY, Config.bMeasureLog))  // @az todo
    {
        VqtError("MQmonInit error.\n");
        return 1;
    }

    VqtSetLogLevel(VQT_LOG_DEBUG);

    {
        CVqtInstance Instance(Config);
        // Parse command line option
        if (!Instance.Init())
            return 1;

        // Load pcap file
        if (!Instance.LoadTraffic())
            return 1;

        // Launch worker threads
        if (!Instance.LaunchWorkers())
            return 1;

        Instance.Fini();
    }

    //MQmonFini();  // @az todo
    return 0;
}