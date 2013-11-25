#ifndef FTL_TIMESEC_H_
#define FTL_TIMESEC_H_
#include <stddef.h>

#ifdef WIN32
#include <WinSock2.h>
#else
#include <sys/time.h>
#endif

namespace ftl{
    // SUBSECOND factor: 1, 10, 100, ... to 1000000000.
    template<size_t SUBSECOND_T>
    struct timesec {
        enum {kSUBSEC=SUBSECOND_T};

        long          sec;   // seconds.
        long          nsec;  // sub-second defined as second/kSUBSEC. has the same sign of sec.

        timesec():sec(0), nsec(0){}
        timesec(long a) {
            sec = a/kSUBSEC;
            nsec = a%kSUBSEC;
        }
        timesec(long s, long sub):sec(s), nsec(sub) {}
        timesec& operator =(long a) {
            sec = a/kSUBSEC;
            nsec = a%kSUBSEC;
            return *this;
        }
        // convert to long in kSUBSEC
        template <typename INTTYPE>
        INTTYPE as() const {
            return ((INTTYPE)sec)*kSUBSEC+nsec;
        }
        template < size_t ANOTHERSUB_T >
        timesec(const timesec<ANOTHERSUB_T> a):sec(a.sec), nsec(ANOTHERSUB_T == kSUBSEC?a.nsec:(double)a.nsec*ANOTHERSUB_T/kSUBSEC) {}

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
            nsec = ((double)v.tv_usec)*1000000/kSUBSEC;
        }
        timesec& operator+=(const timesec& a) {
            nsec += a.nsec;
            long s = nsec/kSUBSEC;
            sec += a.sec + s;
            if(s) nsec = nsec%kSUBSEC;
            return *this;
        }
        // x and ay must be positive
        friend int diff(timesec& x, const timesec& ay)
        {
            timesec y = ay;
            /* Perform the carry for the later subtraction by updating y. */
            if (x.nsec < y.nsec) {
                int nsec = (y.nsec - x.nsec) / kSUBSEC + 1;  // always nsec == 1?
                y.nsec -= kSUBSEC * nsec;
                y.sec += nsec;
            }
            if (x.nsec - y.nsec > kSUBSEC) {  // possible?
                int nsec = (x.nsec - y.nsec) / kSUBSEC;
                y.nsec += kSUBSEC * nsec;
                y.sec -= nsec;
            }
            int negative = x.sec < y.sec?1:0;

            /* Compute the time remaining to wait.
            tv_usec is certainly positive. */
            x.sec = x.sec - y.sec;     // may negative
            x.nsec = x.nsec - y.nsec;  // positive

            if( x.sec < 0 ){  // adjust to positive
                 x.sec = - x.sec - 1;
                 x.nsec = kSUBSEC - x.nsec;
            }
            /* Return 1 if result is negative. */
            return negative;
        }

        inline timesec& neg() {
            sec = - sec;
            nsec = - nsec;
            return *this;
        }
        timesec& operator-=(const timesec& y) {
            timesec& x = *this;
            int negX = x<0?1:0;
            int negY = y<0?1:0;
            if( negX ^ negY ) {
                *this += -y;
            }else{ // x and y have the same sign
                if( negX ) {
                    timesec aby = -y;
                    x.neg();
                    if( !diff(x, aby) )
                        neg();
                }else {
                    if( diff(x, y) )
                        neg();
                }
            }
            return *this;
        }
        //template< typename T>
        //timesec& operator*=(const T& y) {
        //    double r = y*sec + y*nsec/kSUBSEC;
        //    sec = r/kSUBSEC;
        //    nsec = r-sec*kSUBSEC;
        //    return *this;
        //}
        friend timesec operator*(const timesec& a, double y) {
            timesec x(a);
            double r = y*x.sec*kSUBSEC+ y*x.nsec;
            x.sec = (long)r/kSUBSEC;
            x.nsec = r-x.sec*kSUBSEC;
            return x;
        }
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
            res.neg();
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
