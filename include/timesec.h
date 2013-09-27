#ifndef FTL_TIMESEC_H_
#define FTL_TIMESEC_H_

#ifdef WIN32
#include <WinSock2.h>
#else
#include <sys/time.h>
#endif

namespace ftl{
    // SUBSECOND factor: 1, 10, 100, ... to 1000000000.
    template<size_t SUBSECOND>
    struct timesec {
        enum {kSUBSEC=SUBSECOND};

        long          sec;   // seconds
        unsigned long nsec;  // sub-second defined as second/SUBSECOND

        timesec():sec(0), nsec(0){}
        timesec(long a) {
            sec = a/SUBSECOND;
            nsec = a-sec;
        }
        timesec(long s, unsigned long sub):sec(s), nsec(sub) {}
        timesec& operator =(long a) {
            sec = a/SUBSECOND;
            nsec = a-sec;
            return *this;
        }
        template < typename INTTYPE >
        operator INTTYPE () {
            return INTTYPE(sec + nsec * SUBSECOND);
        }
        template < size_t ANOTHERSUB >
        timesec(const timesec<ANOTHERSUB> a):sec(a.sec), nsec(ANOTHERSUB == SUBSECOND?a.nsec:(double)a.nsec*ANOTHERSUB/SUBSECOND) {}

        // convert to timeval
        operator timeval () {
            timesec<1000000>    m(*this);
            timeval v;
            v.tv_sec = m.sec;
            v.tv_usec = m.nsec;
        }
        // convert from timeval
        timesec(const timeval& v) {
            sec = v.tv_sec;
            nsec = (double)v.tv_usec*1000000/SUBSECOND;
        }


        timesec& operator+=(const timesec& a) {
            nsec += a.nsec;
            long s = nsec/SUBSECOND;
            sec += a.sec + s;
            if(s) nsec -= SUBSECOND;
            return *this;
        }
        timesec& operator-=(const timesec& y) {
            timesec& x = *this;
            x.sec -= y.sec;
            if (x.nsec < y.nsec) {
                x.nsec += (SUBSECOND - y.nsec);
                --x.sec;
            }else{
                x.nsec -= y.nsec;
            }
            return *this;
        }
        //long operator/(const timesec& x) const {
        //    //
        //}
        // return count = time*freq
        template<typename FREQ_TYPE>
        friend FREQ_TYPE count_by_freq(const timesec& a, FREQ_TYPE freq) {
            return freq*a.sec+ freq*a.nsec/kSUBSEC;
        }


        friend bool operator <(const timesec& a, const timesec& b) {
            return a.sec < b.sec || a.sec == b.sec && a.nsec < b.nsec;
        }
        friend bool operator >(const timesec& a, const timesec& b) {
            return a.sec > b.sec || a.sec == b.sec && a.nsec > b.nsec;
        }
        friend bool operator ==(const timesec& a, const timesec& b) {
            return a.sec == b.sec && a.nsec == b.nsec;
        }
        friend timesec operator+ (const timesec& a, const timesec& b) {
            timesec res=a;
            return res += b;
        }
        friend timesec operator-(const timesec& a) {
            timesec res = a;
            res.sec = - res.sec;
            return res;
        }
        friend timesec operator- (const timesec& a, const timesec& b) {
            timesec res=a;
            return res -= b;
        }
    };

    typedef timesec<1000000000> timenano;   // nano sec
    typedef timesec<1000000>    timemicro;  //same as sys/timeval
    typedef timesec<1000>       timemilli;  // milli sec
    typedef timesec<1>          timesecond; // second

} // namespace ftl
#endif // FTL_TIMESEC_H_