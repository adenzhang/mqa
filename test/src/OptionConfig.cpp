#include <boost/program_options.hpp>
#include <boost/lexical_cast.hpp>
#include <iostream>
#include "Utility.h"
#include "OptionConfig.h"

using namespace std;
using namespace boost::program_options;

bool CVqtConfig::Parse(int argc, char** argv)
{
    try
    {
        options_description Desc("Program options");
        Desc.add_options()
            ("help,h", "print help message")
            ("traffic,f", value<vector<string>>(), "traffic detail, can have multiple with format <traffic_file>%<stream_number_per_thread>%<throughput_byte_per_second>")
            ("thread,t", value<uint32_t>(), "set thread number")
            ("throughput,p", value<uint32_t>(), "set throughput per stream")
            ("parse_ip_info,i", "set parse ip info")
            ("idle_run,d", "replay without feeding Telchemy library")
            ("measure_log,m", "show Telchemy measure log")
            ("packets_per_play,k", value<uint32_t>(), "debug option, number of packets per play")
            ;

        variables_map Vm;
        store(parse_command_line(argc, argv, Desc), Vm);
        notify(Vm);    

        if (Vm.count("help") || argc == 1)
        {
            cout << Desc << "\n";
            return false;
        }

        if (Vm.count("traffic"))
        {
            vector<string> vsTraffic = Vm["traffic"].as<vector<string>>();
            for (vector<string>::const_iterator It = vsTraffic.begin(); It != vsTraffic.end(); ++It)
            {
                const string& sTraffic = *It;
                VqtConfigTraffic ConfigTraffic;
                ConfigTraffic.nStreams = 1;
                ConfigTraffic.nThroughput = 0;

                size_t nPos1 = sTraffic.find('%');
                size_t nPos2 = sTraffic.find('%', nPos1 + 1);
                if (nPos1 != string::npos)
                {
                    ConfigTraffic.sFile = sTraffic.substr(0, nPos1);
                    if (nPos2 != string::npos)
                    {
                        ConfigTraffic.nStreams = boost::lexical_cast<uint32_t>(sTraffic.substr(nPos1 + 1, nPos2 - (nPos1 + 1)));
                        ConfigTraffic.nThroughput = boost::lexical_cast<uint32_t>(sTraffic.substr(nPos2 + 1));
                    }
                    else
                    {
                        ConfigTraffic.nStreams = boost::lexical_cast<uint32_t>(sTraffic.substr(nPos1 + 1));
                    }
                }
                else
                {
                    ConfigTraffic.sFile = sTraffic;
                }
                if (ConfigTraffic.sFile.empty() || ConfigTraffic.nStreams == 0)
                {
                    VqtError("Wrong traffic specification\n");
                    return false;
                }
                vTraffic.push_back(ConfigTraffic);
            }
        }
        if (Vm.count("thread"))
            nThread = Vm["thread"].as<uint32_t>();
        if (Vm.count("parse_ip_info"))
            bParseIpInfo = true;
        if (Vm.count("idle_run"))
            bIdleRun = true;
        if (Vm.count("measure_log"))
            bMeasureLog = true;
        if (Vm.count("packets_per_play"))
            nPacketsPerPlay = Vm["packets_per_play"].as<uint32_t>();
    }
    catch (exception& e)
    {
        cerr << "error: " << e.what() << "\n";
        return false;
    }
    catch (...)
    {
        cerr << "Exception of unknown type!\n";
        return false;
    }

    cout << *this;

    return true;
}

std::ostream& operator<<(std::ostream& os, const CVqtConfig& Config)
{
    for (uint32_t i = 0; i < Config.vTraffic.size(); ++i)
        os << "Traffic File (" << Config.vTraffic[i].nStreams << " streams, "
        << Config.vTraffic[i].nThroughput << " Bps): " << Config.vTraffic[i].sFile << std::endl;
    os << "Thread: " << Config.nThread << std::endl;
    os << "Parse IP Info: " << (Config.bParseIpInfo ? "true" : "false") << std::endl;
    os << "Idle Run: " << (Config.bIdleRun ? "true" : "false") << std::endl;
    os << "Measure Log: " << (Config.bMeasureLog? "true" : "false") << std::endl;
    os << "Packets Per Play: " << Config.nPacketsPerPlay << std::endl;
    return os;
}
