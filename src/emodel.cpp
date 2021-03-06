#define NOMINMAX

#include "mqa/RtpTypes.h"
#include <math.h>
#include <algorithm>

namespace mqa {

static RtpEModelFactor c_factors[] = {
		{RTPCODEC_PCMU,      0,     0,     8.6},
		{RTPCODEC_PCMA,      0,     0,     8.6},
		{RTPCODEC_G711_PLC,  0,     0,     50.2},
		{RTPCODEC_G723,      7.5,   15.0,  16.1},
		{RTPCODEC_G729,      10,    11.0,  19.0},

		{RTPCODEC_G726_A_16, 0.125, 50,    8.6},
		{RTPCODEC_G726_U_16, 0.125, 50,    8.6},
		{RTPCODEC_G726_A_24, 0.125, 25,    8.6},
		{RTPCODEC_G726_U_24, 0.125, 25,    8.6},
		{RTPCODEC_G726_A_32, 0.125, 7,     8.6},
		{RTPCODEC_G726_U_32, 0.125, 7,     8.6},
		{RTPCODEC_G726_A_40, 0.125, 2,     8.6},
		{RTPCODEC_G726_U_40, 0.125, 2,     8.6},

		{RTPCODEC_AMRNB_0475,5,     18,    7},
		{RTPCODEC_AMRNB_0515,5,     26,    8},
		{RTPCODEC_AMRNB_0590,5,     14,    8},
		{RTPCODEC_AMRNB_0670,5,     12,    9},
		{RTPCODEC_AMRNB_0740,5,     10,    9},
		{RTPCODEC_AMRNB_0795,5,     8,     9},
		{RTPCODEC_AMRNB_1020,5,     6,     10},
		{RTPCODEC_AMRNB_1220,0,     5,     10},

		{RTPCODEC_AMRWB_0660,5,     41,    4},
		{RTPCODEC_AMRWB_0885,5,     26,    4.1},
		{RTPCODEC_AMRWB_1265,5,     13,    4.3},
		{RTPCODEC_AMRWB_1425,5,     10,    4.35},
		{RTPCODEC_AMRWB_1585,5,     7,     4.4},
		{RTPCODEC_AMRWB_1825,5,     5,     4.5},
		{RTPCODEC_AMRWB_1985,5,     3,     4.55},
		{RTPCODEC_AMRWB_2385,5,     8,     4.9},

        {RTPCODEC_RTAUDIO_NB,10,    8,     4.9},
        {RTPCODEC_RTAUDIO_WB,10,    8,     4.9}
};
RtpEModelFactor *GetEModelFactor(RTPCodec codec)
{
    int n = sizeof(c_factors)/sizeof(RtpEModelFactor);
	for(int i = 0; i<n; ++i) {
		if(c_factors[i].codec == codec) return &c_factors[i];
	}
	return NULL;
}


    bool CalculateRFactor(RTPCodec nCodecType,
        double    dCodecFrameSize,
        double    dRTPjitter,
        double    dRTCPDelay,
        double    dAfactor,
        double    dPacketLossRate,
        double*   Rfactor,
        double*   MOS)
    {
        ////////// Initialization /////////////////////////////////////////

        double De, Dj;  // encoding delay,
        double T, Tr, Ta;
        double TERV, Re, Roe, Xdt, Idte, Idd, Id, Rle, X, Xdl, Idle, R;
        double Dr = 0.0; // receive side's access delay, default to 0.
        double Ds = 0.0; // send side's access delay, default to 0.

        double Ie, Bpl, Ie_eff, MOS_CQE;

        double Is;

        bool isWideBand = false;

        float R0;
        RtpEModelFactor *factors = GetEModelFactor(nCodecType);

        ///////////////////////////////////////////////////////////////////
        // Delay impairment factor: Id
        //
        // Id = Idte + Idle + Idd
        //
        //	Note: refer to G.107 and Psytechnics technical report 
        //
        ///////////////////////////////////////////////////////////////////

        if( factors ) {
        	De = 1.2 * (dCodecFrameSize + factors->fLookahead);
        	switch(RTPCODEC_MASK & nCodecType) {
        	case RTPCODEC_AMRWB:
        	case RTPCODEC_RTAUDIO_WB:
        		isWideBand = true;
        	}
        }else{
			// Estimation of De
			// choosing best-case of (packet size + lookahead) ms + 20% will be a reasonable estimate of encoding delay
			switch(nCodecType)
			{
			//case G711A_LAW:
			//case G711U_LAW:
			//case G711_PLC: // G.711 PLC (Packet Loss Concealment)
			case RTPCODEC_PCMU:
			case RTPCODEC_PCMA:
			case RTPCODEC_G711_PLC:
				De = 1.2 * dCodecFrameSize;
				break;

			//case G7231:
			case RTPCODEC_G723:
				De = 1.2 * (dCodecFrameSize + 7.5);
				break;

			//case G729A:
			//case G729AB:
			case RTPCODEC_G729:
				De = 1.2 * (dCodecFrameSize + 10);
				break;

			//case G726_ULAW_16:
			//case G726_ALAW_16:
			//case G726_ULAW_24:
			//case G726_ALAW_24:
			//case G726_ULAW_32:
			//case G726_ALAW_32:
			//case G726_ULAW_40:
			//case G726_ALAW_40:
			//    De = 1.2 * (dCodecFrameSize + 0.125);
			//    break;
			case RTPCODEC_G726_U:
			case RTPCODEC_G726_A:
					De = 1.2 * (dCodecFrameSize + 0.125);
					break;

				// 2010 AMR
			case RTPCODEC_AMRNB_0475:
			case RTPCODEC_AMRNB_0515:
			case RTPCODEC_AMRNB_0590:
			case RTPCODEC_AMRNB_0670:
			case RTPCODEC_AMRNB_0740:
			case RTPCODEC_AMRNB_0795:
			case RTPCODEC_AMRNB_1020:
				De = 1.2 * (dCodecFrameSize + 5);	// 5ms look ahead...see wiki
				break;
			case RTPCODEC_AMRNB_1220:
				De = 1.2 * (dCodecFrameSize + 0);	// 12.2K is the only one with zero look ahead...see wiki
				break;

			//case RTPCODEC_AMRWB_0660:
			//case RTPCODEC_AMRWB_0885:
			//case RTPCODEC_AMRWB_1265:
			//case RTPCODEC_AMRWB_1425:
			//case RTPCODEC_AMRWB_1585:
			//case RTPCODEC_AMRWB_1825:
			//case RTPCODEC_AMRWB_1985:
			//case RTPCODEC_AMRWB_2305:
			//case RTPCODEC_AMRWB_2385:
			case RTPCODEC_AMRWB:
				De = 1.2 * (dCodecFrameSize + 5);	// 5ms look ahead...see wiki
				isWideBand = true;
				break;

			default:
				De = 1.2 * dCodecFrameSize;
				//return false;
				break;
			} // switch nCodecType
        }

        Dj = std::min((double)dCodecFrameSize + 0.9 * dRTPjitter, (double)300);

        //mean one way delay
        T = dRTCPDelay + Dj + De +Dr;	// network delay + jitter~decoding delay + encoding delay + receiving side delay
        // absolute delay
        Ta = dRTCPDelay + Dj + De + Ds + Dr;	// network delay + jitter~decoding delay + encoding delay + send-side delay + receiving side delay
        // round trip delay
        Tr = 2.0 * dRTCPDelay + Dj + De;	// network delay + jitter~decoding delay + encoding delay 

        // Id calculation 
        TERV = 65.0 + (6.0 * exp(-0.3 * pow(T, 2.0))) 
            - 40.0 * log10( (1.0 + T / 10.0) / (1.0 + T / 150.0) );
        if (isWideBand)	// G.107.1-201112
        {
            if (T < 100)
                TERV = TERV + 0.08*T + 10;
            else	// delay >=100ms
                TERV = TERV + 18;
        }

        // Idte, talker echo 
        if (isWideBand)
            Re = 80.0 + 3 * (TERV - 14.0);
        else
            Re = 80.0 + 2.5 * (TERV - 14.0);
        Roe = -1.5 * (-61.18 - 2.0);
        Xdt = (Roe - Re) / 2.0;
        Idte = Xdt + sqrt(pow(Xdt, 2.0) + 100.0);
        Idte = (Idte - 1.0) * (1.0 - exp(-T));

        // Idle, listener echo
        Rle = 1228.5 * pow((Tr + 1.0), -0.25);
        Xdl = (94.77 - Rle) / 2.0;
        Idle = Xdl + sqrt(pow(Xdl, 2.0) + 169.0);

        // Idd, too long absolute delay 
        if (Ta > 100.0) 
        {
            X = (log(Ta / 100.0)) / log(2.0);

            Idd = 25.0 * (pow((1.0 + pow(X, 6.0)),(1.0 / 6.0)) 
                - 3.0 * pow((1.0 + pow((X / 3.0), 6.0)), (1.0 / 6.0)) + 2.0);
        }
        else
        {
            Idd = 0.0; 
        }

        Id = Idte + Idle + Idd;  // Id value

        ///////////////////////////////////////////////////////////////////
        // Equipment impairment factor: Ie
        //
        //	Note: refer G.107 and G.113 
        //
        ///////////////////////////////////////////////////////////////////

        if( factors ) {
        	Ie = factors->Ie;
        	Bpl = factors->Bpl;
        }else{
        switch(nCodecType)
        {
        //case G711A_LAW:
        //case G711U_LAW:
        case RTPCODEC_PCMU:
        case RTPCODEC_PCMA:
            Ie = 0.0;
            Bpl = 8.6;
            break;

        case RTPCODEC_G711_PLC: // G.711 PLC (Packet Loss Concealment)
            Ie = 0.0;
            Bpl = 50.2;
            break;

        //case G7231:
        case RTPCODEC_G723:
            Ie = 15.0;
            Bpl = 16.1;
            break;

        //case G729A:
        //case G729AB:
        case RTPCODEC_G729:
            Ie = 11.0;
            Bpl = 19.0;
            break;

        case RTPCODEC_G726_U_16:
        case RTPCODEC_G726_A_16:
            Ie = 50.0;
            Bpl = 8.6;
            break;

        case RTPCODEC_G726_U_24:
        case RTPCODEC_G726_A_24:
            Ie = 25.0;
            Bpl = 8.6;
            break;

        case RTPCODEC_G726_U_32:
        case RTPCODEC_G726_A_32:
            Ie = 7.0;
            Bpl = 8.6;
            break;

        case RTPCODEC_G726_U_40:
        case RTPCODEC_G726_A_40:
            Ie = 2.0;
            Bpl = 8.6;
            break;

            // 2010 AMR.....from E-model Bpl does not have much effect on the actual result
        case RTPCODEC_AMRNB_0475:
            Ie = 18.0;	// guess from using lower bitrate version of AMR 12.2
            Bpl = 7.0;	// guess
            break;
        case RTPCODEC_AMRNB_0515:
            Ie = 16.0;	// guess from using lower bitrate version of AMR 12.2
            Bpl = 8.0;	// guess
            break;
        case RTPCODEC_AMRNB_0590:
            Ie = 14.0;	// guess from using lower bitrate version of AMR 12.2
            Bpl = 8.0;	// guess
            break;
        case RTPCODEC_AMRNB_0670:
            Ie = 12.0;	// guess from using lower bitrate version of AMR 12.2
            Bpl = 9.0;	// guess
            break;
        case RTPCODEC_AMRNB_0740:
            Ie = 10.0;	// IS-641 G.113 provisional value
            Bpl = 9.0;	// guess
            break;
        case RTPCODEC_AMRNB_0795:
            Ie = 8.0;	// guess from using lower bitrate version of AMR 12.2
            Bpl = 9.0;	// guess
            break;
        case RTPCODEC_AMRNB_1020:
            Ie = 6.0;	// guess from using lower bitrate version of AMR 12.2
            Bpl = 10.0;	// guess
            break;
        case RTPCODEC_AMRNB_1220:
            Ie = 5.0;	// GSM-EFR G.113 provisional value
            Bpl = 10.0;	// GSM-EFR G.113 provisional value
            break;

            // the following wideband values are from G.113 Amendment 1 (03/2009)
            // the MOS for bitrate difference aligns with VoiceAge published MOS quite well....used to guess Bpl

        case RTPCODEC_AMRWB_0660:
            Ie = 41.0;
            Bpl = 4.0;	// guess
            break;
        case RTPCODEC_AMRWB_0885:
            Ie = 26.0;
            Bpl = 4.1;	// guess
            break;
        case RTPCODEC_AMRWB_1265:
            Ie = 13.0;
            Bpl = 4.3;	// diotic sound presentation assumed
            break;
        case RTPCODEC_AMRWB_1425:
            Ie = 10.0;
            Bpl = 4.35;	// guess
            break;
        case RTPCODEC_AMRWB_1585:
            Ie = 7.0;
            Bpl = 4.4;	// guess
            break;
        case RTPCODEC_AMRWB_1825:
            Ie = 5.0;
            Bpl = 4.5;	// guess
            break;
        case RTPCODEC_AMRWB_1985:
            Ie = 3.0;
            Bpl = 4.55;	// guess
            break;
        case RTPCODEC_AMRWB_2305:
            Ie = 1.0;
            Bpl = 4.6;	// diotic sound presentation assumed
            break;
        case RTPCODEC_AMRWB_2385:
            Ie = 8.0;
            Bpl = 4.9;	// diotic sound presentation assumed
            break;

        default:
            Ie = 0.0;
            Bpl = 8.6;
            //return false;
            break;
        }
        }

        //	Ie_eff = Ie + (95.0 - Ie) * (dPacketLossRate / 100.0) / ((dPacketLossRate / 100.0) + Bpl);
        Ie_eff = Ie + (95.0 - Ie) * dPacketLossRate / (dPacketLossRate + Bpl);

        ///////////////////////////////////////////////////////////////////
        // R-factor Value: Rfactor
        //
        ///////////////////////////////////////////////////////////////////

        if (isWideBand)
        {
            R0 = 110.1394653F;
            Is = 0.0f;
        }
        else
        {
            R0 = 94.76882158F;
            Is = 1.415485f;
        }

        R = R0 - Is - Id - Ie_eff + dAfactor; 	

        if( R < 0.0 )	// this should not be necessary, but it will be extremely rare and could be a bug if zero/negative
            R = 0.0;

        // note that wideband R factor should not exceed 129.5; narrowband R factor should not exceed 100
        // otherwise likely to be a bug in calculation or wrong input value

        *Rfactor = R;

        ///////////////////////////////////////////////////////////////////
        // MOS score: MOS
        //
        //	Note: the calculation of MOS based on G.107
        //
        ///////////////////////////////////////////////////////////////////

        if (isWideBand)
            R = R/1.29;		// 20130625 G.107.1-201112 annex A

        if(R > 100 ) 
            MOS_CQE = 4.5;
        else
            MOS_CQE = 1.0 + 0.035 * R + R * (R - 60.0) * (100.0 - R) * 7e-6;

        if( MOS_CQE < 1.0 )
            MOS_CQE = 1.0;

        *MOS = MOS_CQE;

        return true;
    }
}
