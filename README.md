mqa
=============

Media Qos Analysis. Calculate MOS, R-Factor and Jitter.

Dependencies:
	Boost, AnalyzerCommon
	
	The paths of both libraries are set by environment vars in format $(LIBRARY)DIR or must be cmake searchable.
	
Build:
	Two buildings system provided -- build/CMakeLists.txt, msbuild/mqa.sln
	Boost library must be given by environment variable BOOSTDIR or being seachable by cmake.
	
	The output path is like lib/x86/debug, lib/x86/release, lib/x64/release, lib/x86/debug.
	
Test:
	mqatest read from pcap file and output statistical results.
	
	usage: mqatest -f filename.pcap [-t threadNum]
	
=============
version 0.3 
date: 2013-10-25
	- Define interface MQmonIf to wrap all RTP stream detection & calculations.
	- Calculate MOS, R-Factor, one-way delay, jitter for RTPCODEC_PCMU, RTPCODEC_PCMA.
	- Parse frame by StatsFrameParser.
	- Tested by mqatest.
	- Port to linux and use CMake to build