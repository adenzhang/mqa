#ifndef MQA_MAQ_FLOWKEYS_H_
#define MQA_MAQ_FLOWKEYS_H_

#include <iostream>
#include <sstream>
#include <cassert>
#include <iomanip>      // std::setw
#include <boost/shared_ptr.hpp>

#include "mqa_global.h"

#include "ftl/ftlcollections.h"

#include "RtsdSupport.h"


namespace mqa {

    ////////////////////////////// Entry //////////////////////////////
    // Wrapper class for Ipv4/Ipv6 address
    // 64-bit alignment
    // if ipv4, the format is (IPv4-Mapped IPv6 Address): 0x00000000 00000000 0000FFFF XXXXXXXX
#define STATS_IPV4_MAPPED_IPV6  0x0000FFFF
    class StatsIpAddr
    {
    public:
        StatsIpAddr() { m_n64[0] = m_n64[1] = 0; }
        StatsIpAddr(bool bIpv4, const void* Ip)
        {
            if (bIpv4)
            {
                m_n64[0] = 0;
                m_n32[2] = STATS_IPV4_MAPPED_IPV6;
                m_n32[3] = *(UINT32*)Ip;
            }
            else
            {
                m_n64[0] = *(UINT64*)Ip;
                m_n64[1] = *((UINT64*)Ip + 1);
            }
        }
        ~StatsIpAddr() {}

        inline bool IsIpv4() const { return (m_n64[0] == 0 && m_n32[2] == STATS_IPV4_MAPPED_IPV6); }
        inline UINT32* Ipv4() { return &m_n32[3]; }
        inline UINT64* Ipv6() { return m_n64; }

        inline UINT64* Bit64() { return m_n64; }
        inline UINT32* Bit32() { return m_n32; }
        inline UINT16* Bit16() { return m_n16; }
        inline UINT8* Bit8() { return m_n8; }

        inline UINT32 Hash() const
        {
            return (IsIpv4() ? (m_n32[0] ^ m_n32[1] ^ m_n32[2] ^ m_n32[3]) : m_n32[3]);
        }

        int Compare(const StatsIpAddr& nAddr) const
        {
            for (int i = 0; i < 2; ++i)
            {
                if (m_n64[i] < nAddr.m_n64[i])
                    return -1;
                else if (m_n64[i] > nAddr.m_n64[i])
                    return 1;
            }
            return 0;
        }

        StatsIpAddr& operator=(const StatsIpAddr& nAddr)
        {
            m_n64[0] = nAddr.m_n64[0];
            m_n64[1] = nAddr.m_n64[1];
            return *this;
        }

        inline bool operator==(const StatsIpAddr& nAddr) const
        {
            return (Compare(nAddr) == 0);
        }

        inline bool operator<(const StatsIpAddr& nAddr) const
        {
            return (Compare(nAddr) < 0);
        }

        inline bool operator>(const StatsIpAddr& nAddr) const
        {
            return (Compare(nAddr) > 0);
        }

        friend std::ostream& operator<<(std::ostream& os, const StatsIpAddr& nAddr)
        {
            if (nAddr.IsIpv4())
            {
                for (int i = 0; i < 4; ++i)
                {
                    os << (UINT32)*(nAddr.m_n8 + 12 + i);
                    if (i < 3)
                        os << ".";
                }
            }
            else
            {
                for (int i = 0; i < 8; ++i)
                {
                    os << std::setw(4) << std::setfill('0') << std::hex << (UINT32)*(nAddr.m_n16 + i);
                    if (i < 7)
                        os << ":";
                }
            }
            return os;
        }

        inline void Dump(UINT8* pBuffer) const
        {
            if (IsIpv4())
            {
                *(UINT32*)pBuffer = m_n32[3];
            }
            else
            {
                *(UINT64*)pBuffer = m_n64[0];
                *(((UINT64*)pBuffer) + 1) = m_n64[1];
            }
        }

    private:
        union
        {
            UINT64 m_n64[2];
            UINT32 m_n32[4];
            UINT16 m_n16[8];
            UINT8  m_n8[16];
        };
    };

    class VQStatsConnSubEntryKey
    {
    public:
        VQStatsConnSubEntryKey() {};
        VQStatsConnSubEntryKey(UINT16 uSrcPort_, UINT16 uDestPort_, UINT8 uProtocol_, bool bSrc2Dst=true)
            : uSrcPort(bSrc2Dst?uSrcPort_:uDestPort_)
            , uDestPort(bSrc2Dst?uDestPort_:uSrcPort_)
            , uProtocol(uProtocol_)
        {
        }
        VQStatsConnSubEntryKey(const VQStatsConnSubEntryKey& k, bool bSrc2Dst=true)
            :uSrcPort(bSrc2Dst?k.uSrcPort:k.uDestPort),
            uDestPort(bSrc2Dst?k.uDestPort:k.uSrcPort),
            uProtocol(k.uProtocol)
        {};
        ~VQStatsConnSubEntryKey() {}

        // For HASH function
        bool operator==(const VQStatsConnSubEntryKey &key) const
        {
            return (key.uSrcPort == uSrcPort && key.uDestPort == uDestPort && key.uProtocol == uProtocol);
        }

        operator uintptr_t() const
        {
            return (uintptr_t) (uProtocol ^ (uSrcPort | (uDestPort << 16)));
        }

        UINT16 uSrcPort;
        UINT16 uDestPort;
        UINT8 uProtocol;
    };
    typedef VQStatsConnSubEntryKey VQStatsConnPort;


    // Bi-directional, smaller address is always src
    class VQStatsConnEntry2Key
    {
    public:
        VQStatsConnEntry2Key(){}
        VQStatsConnEntry2Key(bool bIpv4, const void* pSrcIp, const void* pDestIp)
            : ipSrc(bIpv4, pSrcIp)
            , ipDest(bIpv4, pDestIp)
        {
            if (ipSrc > ipDest)
            {
                StatsIpAddr ipTemp = ipSrc;
                ipSrc = ipDest;
                ipDest = ipTemp;
            }
        }
        ~VQStatsConnEntry2Key() {}

        // For HASH function
        bool operator==(const VQStatsConnEntry2Key &key) const
        {
            if (!(key.ipSrc == ipSrc && key.ipDest == ipDest))
                return false;
            return true;
        }
        operator uintptr_t() const
        {
            return (uintptr_t) (ipSrc.Hash() ^ ipDest.Hash());
        }

        StatsIpAddr ipSrc;
        StatsIpAddr ipDest;
    };
    typedef VQStatsConnEntry2Key VQStatsConnIp;

    // VLANMPLS Ids
    class VQStatsConnEntry1Key
    {
    public:
        VQStatsConnEntry1Key(){}
        VQStatsConnEntry1Key(UINT8 nVLANMPLSIds_, const UINT32* aVLANMPLSIds_, UINT8 nLimPort_ = 0)
            : nLimPort(nLimPort_)
            , nVLANMPLSIds(nVLANMPLSIds_)
        {
            assert(nVLANMPLSIds <= NVLANMPLS_MASK);
            for (int i = 0; i < nVLANMPLSIds_; ++i)
                aVLANMPLSIds[i] = aVLANMPLSIds_[i];
        }
        ~VQStatsConnEntry1Key() {}

        // For HASH function
        bool operator==(const VQStatsConnEntry1Key &key) const
        {
            if (!(key.nLimPort == nLimPort && key.nVLANMPLSIds == nVLANMPLSIds))
                return false;
            for (int i = 0; i < nVLANMPLSIds; ++i)
            {
                if(key.aVLANMPLSIds[i] != aVLANMPLSIds[i])
                    return false;
            }
            return true;
        }
        operator uintptr_t() const
        {
            uintptr_t nRet = ((nLimPort << 8) | nVLANMPLSIds);
            for (int i = 0; i < nVLANMPLSIds; ++i)
                nRet |= aVLANMPLSIds[i];
            return nRet;
        }

        UINT8 nLimPort;
        UINT8 nVLANMPLSIds;
        UINT32 aVLANMPLSIds[RTSD_MAX_VLANMPLS_IDS];
    };
    typedef VQStatsConnEntry1Key VQStatsConnVlan;

    struct VQStatsConnKey {
        VQStatsConnKey(VQStatsConnIp& ip, VQStatsConnPort& port,VQStatsConnVlan& vlan)
            :ip(ip), port(port),vlan(vlan) {}
        VQStatsConnIp    ip;
        VQStatsConnPort  port;
        VQStatsConnVlan  vlan;

        operator uintptr_t() const {
            return uintptr_t(ip) ^ uintptr_t(port) ^ uintptr_t(vlan);
        }
        bool operator==(VQStatsConnKey& a) const {
            return ip == a.ip && port == a.port && vlan == a.vlan;
        }
    };

    class VQStatsTunnelEntryKey
    {
    public:
        VQStatsTunnelEntryKey() {};
        VQStatsTunnelEntryKey(bool bIpv4, const void* pSrcIp, const void* pDestIp, UINT32 TEI_,
            UINT8 nVLANMPLSIds_, const UINT32* aVLANMPLSIds_, UINT8 nLimPort_= 0)
            : ipSrc(bIpv4, pSrcIp)
            , ipDest(bIpv4, pDestIp)
            , TEI(TEI_)
            , nLimPort(nLimPort_)
            , nVLANMPLSIds(nVLANMPLSIds_)
        {
            assert(nVLANMPLSIds <= NVLANMPLS_MASK);
            for (int i = 0; i < nVLANMPLSIds_; ++i)
                aVLANMPLSIds[i] = aVLANMPLSIds_[i];
        }
        ~VQStatsTunnelEntryKey() {}

        // For HASH function
        bool operator==(const VQStatsTunnelEntryKey &key) const
        {
            if (!(key.ipSrc == ipSrc && key.ipDest == ipDest && key.TEI == TEI
                && key.nLimPort == nLimPort && key.nVLANMPLSIds == nVLANMPLSIds))
                return false;
            for (int i = 0; i < nVLANMPLSIds; ++i)
            {
                if(key.aVLANMPLSIds[i] != aVLANMPLSIds[i])
                    return false;
            }
            return true;
        }
        operator uintptr_t() const
        {
            return (uintptr_t) (TEI ^ ipSrc.Hash() ^ ipDest.Hash());
        }

        StatsIpAddr ipSrc;
        StatsIpAddr ipDest;
        UINT32 TEI;
        UINT8 nLimPort;
        UINT8 nVLANMPLSIds;
        UINT32 aVLANMPLSIds[RTSD_MAX_VLANMPLS_IDS];
    };
    typedef VQStatsTunnelEntryKey VQStatsTunnelLowerAddress;

    class VQStatsTunnelSubEntryKey
    {
    public:
        VQStatsTunnelSubEntryKey() {};
        VQStatsTunnelSubEntryKey(bool bIpv4, const void* pSrcIp, const void* pDestIp,
            UINT16 uSrcPort_, UINT16 uDestPort_, UINT8 uProtocol_)
            : ipSrc(bIpv4, pSrcIp)
            , ipDest(bIpv4, pDestIp)
            , uSrcPort(uSrcPort_)
            , uDestPort(uDestPort_)
            , uProtocol(uProtocol_)
        {
        }
        ~VQStatsTunnelSubEntryKey() {}

        // For HASH function
        bool operator==(const VQStatsTunnelSubEntryKey &key) const
        {
            return (key.ipSrc == ipSrc && key.ipDest == ipDest
                && key.uSrcPort == uSrcPort && key.uDestPort == uDestPort
                && key.uProtocol == uProtocol);
        }
        operator uintptr_t() const
        {
            return (uintptr_t) (ipSrc.Hash() ^ ipDest.Hash() ^ uProtocol ^ (uSrcPort | (uDestPort << 16)));
        }

        StatsIpAddr ipSrc;
        StatsIpAddr ipDest;
        UINT16 uSrcPort;
        UINT16 uDestPort;
        UINT8 uProtocol;
    };
    typedef VQStatsTunnelSubEntryKey VQStatsTunnelUpperAddress;

    struct VQStatsTunnelKey {
        VQStatsTunnelKey(VQStatsTunnelLowerAddress& lowerAddr, VQStatsTunnelUpperAddress& upperAddr)
            :lowerAddr(lowerAddr), upperAddr(upperAddr){}

        VQStatsTunnelLowerAddress    lowerAddr;
        VQStatsTunnelUpperAddress    upperAddr;

        operator uintptr_t() const {
            return (uintptr_t)(lowerAddr) ^ (uintptr_t)(upperAddr);
        }
        bool operator==(const VQStatsTunnelKey& a) const {
            return lowerAddr == a.lowerAddr && upperAddr == a.upperAddr;
        }
    };

    typedef boost::shared_ptr<VQStatsTunnelKey> VQStatsTunnelKeyPtr;
    typedef boost::shared_ptr<VQStatsConnKey> VQStatsConnKeyPtr;

    struct FlowKey {
        VQStatsTunnelKeyPtr pTunnelKey;
        VQStatsConnKeyPtr   pConnKey;

        bool isTunnel;

    public:
        FlowKey(const VQStatsTunnelKey& k):isTunnel(true), pTunnelKey(new VQStatsTunnelKey(k)){}
        FlowKey(const VQStatsConnKey& k):isTunnel(false), pConnKey(new VQStatsConnKey(k)){}

        operator uintptr_t() {
            return isTunnel? uintptr_t(*pTunnelKey):uintptr_t(*pConnKey);
        }
        bool operator==(const FlowKey& a) {
            if( isTunnel != a.isTunnel ) return false;
            return isTunnel? *pTunnelKey == *a.pTunnelKey : *pConnKey == *a.pConnKey;
        }
    };  // FlowKey

} // namespace mqa

#endif // MQA_MAQ_FLOWKEYS_H_
