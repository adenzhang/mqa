#ifndef CLASSIFYCONSTS_H_
#define  CLASSIFYCONSTS_H_
enum eLayerType
{
    LAYER_NONE = 0,
    LAYER_IPV4 = 1,
    LAYER_IPV6 = 2,
    LAYER_GRE = 3,
    LAYER_GTP = 4
};

enum eFragType
{
    FRAG_NONE = 0,
    FRAG_FIRST = 1,
    FRAG_NORMAL = 2,
    FRAG_LAST = 3
};

#define ETHER_PROTO_VLAN                0x8100
#define ETHER_PROTO_VLAN_SWAP           0x0081
#define ETHER_PROTO_MPLS_UNICAST        0x8847
#define ETHER_PROTO_MPLS_UNICAST_SWAP   0x4788
#define ETHER_PROTO_MPLS_MULTICAST      0x8848
#define ETHER_PROTO_MPLS_MULTICAST_SWAP 0x4888
#define ETHER_PROTO_IPV4                0x0800
#define ETHER_PROTO_IPV4_SWAP           0x0008
#define ETHER_PROTO_IPV6                0x86DD
#define ETHER_PROTO_IPV6_SWAP           0xDD86

#define ETHER_PROTO_IPV4_HEADER_VER     0x40
#define ETHER_PROTO_IPV6_HEADER_VER     0x60

#define UDP_LEN 8
#define UDP_PORT_GTPU           0x0868
#define UDP_PORT_GTPU_SWAP      0x6808
#define UDP_PORT_GTPC           0x084B  // 2123
#define UDP_PORT_GTPC_SWAP      0x4B08
#define UDP_PORT_GTPC_PRIME     0x0D28  // 3368
#define UDP_PORT_GTPC_PRIME_SWAP    0x280D  

#define TCP_PORT_DIAMETER       0x0F1C  // 3868
#define TCP_PORT_DIAMETER_SWAP  0x1C0F

#define TCP_UDP_PORT_SIP1       0x13C4 // 5060
#define TCP_UDP_PORT_SIP2       0x13C5 // 5061
#define TCP_UDP_PORT_SIP3       0x13C6 // 5062
#define TCP_UDP_PORT_SIP1_SWAP  0xC413
#define TCP_UDP_PORT_SIP2_SWAP  0xC513
#define TCP_UDP_PORT_SIP3_SWAP  0xC613

#define IS_SIP_PORT(nPort) \
    ((nPort == TCP_UDP_PORT_SIP1_SWAP) || (nPort == TCP_UDP_PORT_SIP2_SWAP) || (nPort == TCP_UDP_PORT_SIP3_SWAP))

#define TCP_RADIUS_PORT1        0x066D  //1645
#define TCP_RADIUS_PORT2        0x066E  //1646
#define TCP_RADIUS_PORT3        0x0714  //1812
#define TCP_RADIUS_PORT4        0x0715  //1813
#define TCP_RADIUS_PORT5        0x0ED7  //3799
#define TCP_RADIUS_PORT1_SWAP   0x6D06  //1645
#define TCP_RADIUS_PORT2_SWAP   0x6E06  //1646
#define TCP_RADIUS_PORT3_SWAP   0x1407  //1812
#define TCP_RADIUS_PORT4_SWAP   0x1507  //1813
#define TCP_RADIUS_PORT5_SWAP   0xD70E  //3799

#define IS_RADIUS_PORT(nPort) \
    ((nPort == TCP_RADIUS_PORT1_SWAP) || (nPort == TCP_RADIUS_PORT2_SWAP) || \
    (nPort == TCP_RADIUS_PORT3_SWAP) || (nPort == TCP_RADIUS_PORT4_SWAP) || \
    (nPort == TCP_RADIUS_PORT5_SWAP)) 

#define DHCP_PORT1              0x0043  // 67
#define DHCP_PORT2              0x0044  // 68
#define DHCP_PORT3              0x0222  // 546
#define DHCP_PORT4              0x0223  // 547
#define DHCP_PORT1_SWAP         0x4300  // 67
#define DHCP_PORT2_SWAP         0x4400  // 68
#define DHCP_PORT3_SWAP         0x2202  // 546
#define DHCP_PORT4_SWAP         0x2302  // 547

#define IS_DHCP_PORT(nPort) \
    ((nPort == DHCP_PORT1_SWAP) || (nPort == DHCP_PORT2_SWAP) || \
    (nPort == DHCP_PORT3_SWAP) || (nPort == DHCP_PORT4_SWAP) )

#define FTPC_PORT               0x0015  // 21
#define FTPC_PORT_SWAP          0x1500  // 21

// DNS (53,1870,2164,5352,5353)
#define DNS_PORT1               0x0035  // 53
#define DNS_PORT2               0x0743  // 1870
#define DNS_PORT3               0x0874  // 2164
#define DNS_PORT4               0x14E8  // 5352
#define DNS_PORT5               0x14E9  // 5353
#define DNS_PORT1_SWAP          0x3500  // 53
#define DNS_PORT2_SWAP          0x4307  // 1870
#define DNS_PORT3_SWAP          0x7408  // 2164
#define DNS_PORT4_SWAP          0xE814  // 5352
#define DNS_PORT5_SWAP          0xE914  // 5353

#define IS_DNS_PORT(nPort) \
    ((nPort == DNS_PORT1_SWAP) || (nPort == DNS_PORT2_SWAP) || \
    (nPort == DNS_PORT3_SWAP) || (nPort == DNS_PORT4_SWAP) )

#define VOLGA_PORT              0x36B1  // 14001
#define VOLGA_PORT_SWAP         0xB136  // 14001

#define S102AP_PORT             0x5AE8 // 23272
#define S102AP_PORT_SWAP        0xE85A // 23272

#define ALU_ENB_TRACE_PORT      0x7531  // 30001
#define ALU_ENB_TRACE_PORT_SWAP 0x3175  // 30001

#define HUAWEI_ENB_TRACE_PORT      0x5BCC  // 23500
#define HUAWEI_ENB_TRACE_PORT_SWAP 0xCC5B  // 23500

#define IS_ENB_TRACE_PORT(nPort) \
    ((nPort == ALU_ENB_TRACE_PORT_SWAP) || (nPort == HUAWEI_ENB_TRACE_PORT_SWAP))

#define MIP_PORT                0x01B2  // 434
#define MIP_PORT_SWAP           0xB201

#define DSMIPV6_NAT_PORT        0x105F  // 4191
#define DSMIPV6_NAT_PORT_SWAP   0x5F10  // 4191

#define RTSP_PORT1   0x022A // 554
#define RTSP_PORT2   0x216A // 8554
#define RTSP_PORT1_SWAP   0x2A02 // 554
#define RTSP_PORT2_SWAP   0x6A21 // 8554

#define IS_RTSP_PORT(nPort) \
    ((nPort == RTSP_PORT1_SWAP) || (nPort == RTSP_PORT2_SWAP))

#define TR069_CWMP_PORT     0x1D7B  // 7547
#define TR069_CWMP_PORT_SWAP    0x7B1D  // 7547

#define IPSEC_NAT_PORT      0x1194  // 4500
#define IPSEC_NAT_PORT_SWAP 0x9411  // 4500

#define STUN_PORT       0x0D96  // 3478
#define STUN_PORT_SWAP  0x960D  // 3478

#define MSRP_PORT       0x0B27  // 2855
#define MSRP_PORT_SWAP  0x270B  // 2855

#define H323_PORT1  0x06B6  //  1718	h323gatedisc
#define H323_PORT2  0x06B7  //  1719	h323gatestat
#define H323_PORT3  0x06B8  //  1720	h323hostcall
#define H323_PORT1_SWAP  0xB606
#define H323_PORT2_SWAP  0xB706
#define H323_PORT3_SWAP  0xB806

#define IS_H323_PORT(nPort) \
    ((nPort == H323_PORT1_SWAP) || (nPort == H323_PORT2_SWAP) || (nPort == H323_PORT3_SWAP))

#define MEGACO_PORT1  0x097B  // 2427 megaco gateway
#define MEGACO_PORT2  0x0B80  // 2944 megaco h248
#define MEGACO_PORT3  0x0B81  // 2945 megaco h248 binary
#define MEGACO_PORT1_SWAP  0x7B09
#define MEGACO_PORT2_SWAP  0x800B
#define MEGACO_PORT3_SWAP  0x810B

#define IS_MEGACO_PORT(nPort) \
    ((nPort == MEGACO_PORT1_SWAP) || (nPort == MEGACO_PORT2_SWAP) || (nPort == MEGACO_PORT3_SWAP))

#define COPS_PORT   0x0CD8  // 3288
#define COPS_PORT_SWAP   0xD80C  // 3288

#define SABP_PORT        0x0D7C   // 3452
#define SABP_PORT_SWAP   0x7C0D

#define SMPP_PORT   0x0AD7  // 2775
#define SMPP_PORT_SWAP   0xD70A  // 2775

#define SOIP_PORT1  0x0BEC  // 3052
#define SOIP_PORT2  0x0C00  // 3072
#define SOIP_PORT1_SWAP  0xEC0B  // 3052
#define SOIP_PORT2_SWAP  0x000C  // 3072

#define IS_SOIP_PORT(nPort) \
    ((nPort == SOIP_PORT1_SWAP) || (nPort == SOIP_PORT2_SWAP))

#define ISAKMP_PORT     0x01F4  // 500
#define ISAKMP_PORT_SWAP     0xF401  // 500

// Non-Tunneled
#define OMA_SUPL_PORT1        0x1C69  //7273
#define OMA_SUPL_PORT2        0x1C6A  //7274
#define OMA_SUPL_PORT3        0x1C6C  //7276
#define OMA_SUPL_PORT4        0x1C6D  //7277
#define OMA_SUPL_PORT1_SWAP        0x691C
#define OMA_SUPL_PORT2_SWAP        0x6A1C
#define OMA_SUPL_PORT3_SWAP        0x6C1C
#define OMA_SUPL_PORT4_SWAP        0x6D1C
// Tunneled/Non-Tunneled
#define OMA_SUPL_PORT5        0x1C6B  //7275
#define OMA_SUPL_PORT6        0x23FA  //9210
#define OMA_SUPL_PORT7        0x23FB  //9211
#define OMA_SUPL_PORT5_SWAP        0x6B1C
#define OMA_SUPL_PORT6_SWAP        0xFA23
#define OMA_SUPL_PORT7_SWAP        0xFB23

#define IS_OMA_SUPL_PORT(nPort) \
    ((nPort == OMA_SUPL_PORT1_SWAP) || (nPort == OMA_SUPL_PORT2_SWAP) || \
    (nPort == OMA_SUPL_PORT3_SWAP) || (nPort == OMA_SUPL_PORT4_SWAP) || \
    (nPort == OMA_SUPL_PORT5_SWAP) || (nPort == OMA_SUPL_PORT6_SWAP) || \
    (nPort == OMA_SUPL_PORT7_SWAP)) 


#define IPV4_PROTO_ICMP 0x01
#define IPV4_PROTO_IGMP 0x02
#define IPV4_PROTO_IPV6 0x29
#define IPV4_PROTO_SCTP 0x84
#define IPV4_PROTO_UDP  0x11
#define IPV4_PROTO_TCP  0x06
#define IPV4_PROTO_GRE  0x2f

#define IPV6_HDR_LEN      40

#define IPv6_HDR_HOP_BY_HOP     0x0
#define IPV6_HDR_ROUTING        0x2b
#define IPV6_HDR_FRAGMENT       0x2c
#define IPV6_HDR_GRE            0x2f
#define IPV6_HDR_ESP            0x32 // 50
#define IPV6_HDR_AH             0x33 // 51
#define IPV6_HDR_ICMP           0x3a // 58
#define IPV6_HDR_DESTINATION    0x3c // 60
#define IPV6_HDR_MLD            IPV6_HDR_ICMP // Based on ICMPv6
#define IPV6_HDR_MOBILITY       0x87 // used for PMIP

#define FRAGMENT_OFFSET_BITMASK   0x1FFF
#define FRAGMENT_MF_BITMASK       0x2000
#define IPV4_HDR_ID_OFFSET        4      // 4 bytes from beginning of IP header 

#define IPV6_NEXT_HDR_OFFSET      6      // 6 bytes from beginning of IP header

#define GTPU_MT_GPDU       0xff
#define GTPU_MT_ENDMARKER  0xfe
#define GTPU_MT_ERROR_INDICATION  0x1A

#define LDAP_PORT       0x0185 // 389
#define LDAP_PORT_SWAP  0x8501 // 389

enum eFrameType 
{
    FRAME_TYPE_NONE = 0,
    FRAME_TYPE_CP_FIRST = 1,
    FRAME_TYPE_CP = 1,
    FRAME_TYPE_SCTP = 2,    // Ethernet/IP/SCTP
    FRAME_TYPE_GTPC = 3,    // Ethernet/IP/UDP Src/Dst Port=2123
    FRAME_TYPE_SIP = 4,     // Ethernet/IP/(UDP|TCP) Src/Dst Port=5060-5061
    FRAME_TYPE_RADIUS = 5,  // Ethernet/IP/TCP Src/Dst Port = 1645-1646, 1812-1813, 3799
    FRAME_TYPE_DIAMETER = 6,// Ethernet/IP/TCP Port = 3868
    FRAME_TYPE_PMIPV6 = 7,  // Ethernet/(opt IPv4)/IPv6 Next Header = Mobility Header (87 (hex))
    FRAME_TYPE_S102AP = 8,  // Ethernet\IPv4 or IPv6\UDP port = 23272
    FRAME_TYPE_DHCP = 9,
    FRAME_TYPE_FTPC = 10,
    FRAME_TYPE_DNS = 11,
    FRAME_TYPE_VOLGA = 12,
    FRAME_TYPE_ICMP = 13,   // ICMPv4 = 1, ICMPv6 = 58 (0x3A)
    FRAME_TYPE_DSMIPV6_NAT = 14,
    FRAME_TYPE_RTSP = 15,
    FRAME_TYPE_H323 = 16,
    FRAME_TYPE_MEGACO = 17,
    FRAME_TYPE_STUN = 18,
    FRAME_TYPE_ESP = 19,
    FRAME_TYPE_AH  = 20,
    FRAME_TYPE_MIP = 21,
    FRAME_TYPE_ENB_TRACE = 22,
    FRAME_TYPE_IGMP = 23,
    FRAME_TYPE_LDAP = 24,
    FRAME_TYPE_CP_LAST = 99, //
    FRAME_TYPE_UP = 100,
    FRAME_TYPE_GTPU = 101,  // Ethernet/IPv4or6/UDP Port = 2152 (GTPv2)
    FRAME_TYPE_GRE = 102,   // Ethernet/(opt IPv4)/IPv6 (hdr = GRE 0x2f)/GRE/
    FRAME_TYPE_IPV4_OTHER = 110,
    FRAME_TYPE_IPV6_OTHER = 111,
    FRAME_TYPE_NON_IP =     120,
    FRAME_TYPE_FRAGMENT_NOT_FIRST = 130,
};

enum eCILimType
{
    CI_ETHERNET,
    CI_ATM,
    CI_OTHER
};

#endif // CLASSIFYCONSTS_H_