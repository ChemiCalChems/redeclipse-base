#pragma once

// generic useful stuff for any C++ program

#ifndef _TOOLS_H
#define _TOOLS_H

#include <vector>

#ifdef NULL
#undef NULL
#endif
#define NULL 0

typedef signed char schar;
typedef unsigned char uchar;
typedef unsigned short ushort;
typedef unsigned int uint;
typedef unsigned long ulong;
typedef signed long long int llong;
typedef unsigned long long int ullong;

#ifdef _DEBUG
#define ASSERT(c) assert(c)
#else
#define ASSERT(c) if(c) {}
#endif

#if defined(__GNUC__) || (defined(_MSC_VER) && _MSC_VER >= 1400)
#define RESTRICT __restrict
#else
#define RESTRICT
#endif

#ifdef __GNUC__
#define UNUSED __attribute__((unused))
#else
#define UNUSED
#endif

#ifdef _MSC_VER
#define unlink _unlink
#endif

#include <algorithm>
template <typename T1, typename T2> T1 clamp(const T1& x, const T2& lo, const T2& hi) {
	//return std::clamp(x, static_cast<T1>(lo), static_cast<T1>(hi));
	// For some reason, the commented out version causes a segfault (???)
	return std::max(static_cast<T1>(lo), std::min(x, static_cast<T1>(hi)));
}

#ifdef __GNUC__
#define bitscan(mask) (__builtin_ffs(mask)-1)
#else
#ifdef WIN32
#pragma intrinsic(_BitScanForward)
static inline int bitscan(uint mask)
{
    ulong i;
    return _BitScanForward(&i, mask) ? i : -1;
}
#else
static inline int bitscan(uint mask)
{
    if(!mask) return -1;
    int i = 1;
    if(!(mask&0xFFFF)) { i += 16; mask >>= 16; }
    if(!(mask&0xFF)) { i += 8; mask >>= 8; }
    if(!(mask&0xF)) { i += 4; mask >>= 4; }
    if(!(mask&3)) { i += 2; mask >>= 2; }
    return i - (mask&1);
}
#endif
#endif

#define BIT(n) (1 << (n))

#define listdinit(array, length, begin, end, prev, next) do \
{ \
    begin = &array[0]; \
    end = &array[(length)-1]; \
    loopi(length) \
    { \
        array[i].prev = !i ? NULL : &array[i-1]; \
        array[i].next = (i == (length)-1) ? NULL : &array[i+1]; \
    } \
} while(0)

#define listinit(array, length, begin, prev, next) do \
{ \
    begin = &array[0]; \
    loopi(length) \
    { \
        array[i].prev = !i ? NULL : &array[i-1]; \
        array[i].next = (i == (length)-1) ? NULL : &array[i+1]; \
    } \
} while(0)

#define listdremove(elem, begin, end, prev, next) do \
{ \
    if(elem->prev) elem->prev->next = elem->next; \
    else begin = elem->next; \
    if(elem->next) elem->next->prev = elem->prev; \
    else end = elem->prev; \
} while(0)

#define listremove(elem, begin, prev, next) do \
{ \
    if(elem->prev) elem->prev->next = elem->next; \
    else begin = elem->next; \
    if(elem->next) elem->next->prev = elem->prev; \
} while(0)

#define listpopfront(result, begin, prev, next) do \
{ \
    result = begin; \
    if(begin) begin = begin->next; \
    if(begin) begin->prev = NULL; \
    if(result) result->prev = result->next = NULL; \
} while(0)

#define listpopback(result, end, prev, next) do \
{ \
    result = end; \
    if(end) end = end->prev; \
    if(end) end->next = NULL; \
    if(result) result->prev = result->next = NULL; \
} while(0)

#define listpushfront(elem, begin, prev, next) do \
{ \
    if(begin) begin->prev = elem; \
    elem->next = begin; \
    elem->prev = NULL; \
    begin = elem; \
} while(0)

#define listdpushfront(elem, begin, end, prev, next) do \
{ \
    if(begin) begin->prev = elem; \
    elem->next = begin; \
    elem->prev = NULL; \
    begin = elem; \
    if(!end) end = elem; \
} while(0)

#define listdpushback(elem, begin, end, prev, next) do \
{ \
    if(end) end->next = elem; \
    elem->next = NULL; \
    elem->prev = end; \
    end = elem; \
    if(!begin) begin = elem; \
} while(0)

#define rnd(x) ((int)(randomMT()&0x7FFFFFFF)%(x))
#define rndscale(x) (float((randomMT()&0x7FFFFFFF)*double(x)/double(0x7FFFFFFF)))
#define detrnd(s, x) ((int)(((((uint)(s))*1103515245+12345)>>16)%(x)))
#define isnumeric(c) (isdigit(c) || c == '+' || c == '-')

#define loop(v,u) for(int v = 0; v < int(u); ++v)
#define loopi(u) loop(i,u)
#define loopj(u) loop(j,u)
#define loopk(u) loop(k,u)
#define loopl(u) loop(l,u)
#define loopm(u) loop(m,u)
#define loopn(u) loop(n,u)

#define looprev(v,u) for(int v = int(u); --v >= 0;)
#define loopirev(u) looprev(i,u)
#define loopjrev(u) looprev(j,u)
#define loopkrev(u) looprev(k,u)
#define looplrev(u) looprev(l,u)
#define loopmrev(u) looprev(m,u)
#define loopnrev(u) looprev(n,u)

#define loopcs(v,m,c,s) \
    int lcs##v##start = 0; \
    int lcs##v##end = 0; \
    if(c > 0) \
    { \
        lcs##v##start = clamp(s, 0, m-1); \
        lcs##v##end = clamp(lcs##v##start+c-1, 0, m-1); \
    } \
    else if(c < 0) \
    { \
        lcs##v##start = clamp(m-1-std::max(s, 0)+c+1, 0, m-1); \
        lcs##v##end = clamp(m-1-std::max(s, 0), 0, m-1); \
    } \
    else \
    { \
        lcs##v##start = clamp(s, 0, m-1); \
        lcs##v##end = std::max(m-1, 0); \
    } \
    for(int v = lcs##v##start; v <= lcs##v##end; v++)

#define loopcsi(m,c,s) loopcs(i,m,c,s)
#define loopcsj(m,c,s) loopcs(j,m,c,s)
#define loopcsk(m,c,s) loopcs(k,m,c,s)
#define loopcsl(m,c,s) loopcs(l,m,c,s)

#define loopcsrev(v,m,c,s) \
    int lcs##v##start = 0; \
    int lcs##v##end = 0; \
    if(c > 0) \
    { \
        lcs##v##start = m-1-clamp(s, 0, m-1); \
        lcs##v##end = clamp(lcs##v##start-c+1, 0, m-1); \
    } \
    else if(c < 0) \
    { \
        lcs##v##start = clamp(std::max(s, 0)-c-1, 0, m-1); \
        lcs##v##end = clamp(s, 0, m-1); \
    } \
    else \
    { \
        lcs##v##start = clamp(m-1-s, 0, m-1); \
        lcs##v##end = 0; \
    } \
    for(int v = lcs##v##start; v >= lcs##v##end; v--)

#define loopcsirev(m,c,s) loopcsrev(i,m,c,s)
#define loopcsjrev(m,c,s) loopcsrev(j,m,c,s)
#define loopcskrev(m,c,s) loopcsrev(k,m,c,s)
#define loopcslrev(m,c,s) loopcsrev(l,m,c,s)

#define DELETEP(p) if(p) { delete   p; p = 0; }
#define DELETEA(p) if(p) { delete[] p; p = 0; }

#define PI (3.14159265358979f)
#define PI2 (2*PI)
#define SQRT2 (1.4142135623731f)
#define SQRT3 (1.73205080756888f)
#define SQRT5 (2.23606797749979f)
#define RAD (PI / 180.0f)

#ifdef WIN32
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif
#ifndef M_LN2
#define M_LN2 0.693147180559945309417
#endif

#ifdef _MSC_VER
#pragma warning (3: 4189)       // local variable is initialized but not referenced
#pragma warning (disable: 4244) // conversion from 'int' to 'float', possible loss of data
#pragma warning (disable: 4267) // conversion from 'size_t' to 'int', possible loss of data
#pragma warning (disable: 4355) // 'this' : used in base member initializer list
#pragma warning (disable: 4800) // forcing value to bool 'true' or 'false' (performance warning)
#pragma warning (disable: 4996) // 'strncpy' was declared deprecated
#include <direct.h>
#define chdir _chdir
#define strcasecmp _stricmp
#define strncasecmp _strnicmp
#define getcwd _getcwd
#define PATHDIV '\\'
#else
#define PATHDIV '/'
#endif
#else
#define __cdecl
#define _vsnprintf vsnprintf
#define PATHDIV '/'
#endif

#ifdef __GNUC__
#define PRINTFARGS(fmt, args) __attribute__((format(printf, fmt, args)))
#else
#define PRINTFARGS(fmt, args)
#endif

// easy safe strings

#define MAXSTRLEN 512 // must be at least 512 bytes to comply with rfc1459
typedef char string[MAXSTRLEN];

#define BIGSTRLEN 4096
typedef char bigstring[BIGSTRLEN];

#define HUGESTRLEN 8192
typedef char hugestring[HUGESTRLEN];

inline void vformatstring(char *d, const char *fmt, va_list v, int len) { _vsnprintf(d, len, fmt, v); d[len-1] = 0; }
template<size_t N> inline void vformatstring(char (&d)[N], const char *fmt, va_list v) { vformatstring(d, fmt, v, N); }

inline char *copystring(char *d, const char *s, size_t len)
{
    size_t slen = std::min(strlen(s), len-1);
    memcpy(d, s, slen);
    d[slen] = 0;
    return d;
}
template<size_t N> inline char *copystring(char (&d)[N], const char *s) { return copystring(d, s, N); }

inline char *concatstring(char *d, const char *s, size_t len) { size_t used = strlen(d); return used < len ? copystring(d+used, s, len-used) : d; }
template<size_t N> inline char *concatstring(char (&d)[N], const char *s) { return concatstring(d, s, N); }

inline char *prependstring(char *d, const char *s, size_t len)
{
    size_t slen = std::min(strlen(s), len);
    memmove(&d[slen], d, std::min(len - slen, strlen(d) + 1));
    memcpy(d, s, slen);
    d[len-1] = 0;
    return d;
}
template<size_t N> inline char *prependstring(char (&d)[N], const char *s) { return prependstring(d, s, N); }

inline void nformatstring(char *d, int len, const char *fmt, ...) PRINTFARGS(3, 4);
inline void nformatstring(char *d, int len, const char *fmt, ...)
{
    va_list v;
    va_start(v, fmt);
    vformatstring(d, fmt, v, len);
    va_end(v);
}

template<size_t N> inline void formatstring(char (&d)[N], const char *fmt, ...) PRINTFARGS(2, 3);
template<size_t N> inline void formatstring(char (&d)[N], const char *fmt, ...)
{
    va_list v;
    va_start(v, fmt);
    vformatstring(d, fmt, v, int(N));
    va_end(v);
}

template<size_t N> inline void concformatstring(char (&d)[N], const char *fmt, ...) PRINTFARGS(2, 3);
template<size_t N> inline void concformatstring(char (&d)[N], const char *fmt, ...)
{
    va_list v;
    va_start(v, fmt);
    int len = strlen(d);
    vformatstring(d + len, fmt, v, int(N) - len);
    va_end(v);
}

extern char *tempformatstring(const char *fmt, ...) PRINTFARGS(1, 2);

#define defformatstring(d,...) string d; formatstring(d, __VA_ARGS__)
#define defvformatstring(d,last,fmt) string d; { va_list ap; va_start(ap, last); vformatstring(d, fmt, ap); va_end(ap); }
#define stringz(d) string d; d[0] = 0;

#define defformatbigstring(d,...) bigstring d; formatstring(d, __VA_ARGS__)
#define defvformatbigstring(d,last,fmt) bigstring d; { va_list ap; va_start(ap, last); vformatstring(d, fmt, ap); va_end(ap); }
#define bigstringz(d) bigstring d; d[0] = 0;

#define defformathugestring(d,...) hugestring d; formatstring(d, __VA_ARGS__)
#define defvformathugestring(d,last,fmt) hugestring d; { va_list ap; va_start(ap, last); vformatstring(d, fmt, ap); va_end(ap); }
#define hugestringz(d) hugestring d; d[0] = 0;

template<size_t N> inline bool matchstring(const char *s, size_t len, const char (&d)[N])
{
    return len == N-1 && !memcmp(s, d, N-1);
}

inline char *newstring(size_t l)                { return new char[l+1]; }
inline char *newstring(const char *s, size_t l) { return copystring(newstring(l), s, l+1); }
inline char *newstring(const char *s)           { if(!s) s = ""; size_t l = strlen(s); char *d = newstring(l); memcpy(d, s, l+1); return d; }

inline char *newconcatstring(const char *s, const char *t)
{
    size_t slen = strlen(s), tlen = strlen(t);
    char *r = newstring(slen + tlen);
    memcpy(r, s, slen);
    memcpy(&r[slen], t, tlen);
    r[slen+tlen] = '\0';
    return r;
}

#define loopv(v)    for(int i = 0; i<(v).length(); i++)
#define loopvj(v)   for(int j = 0; j<(v).length(); j++)
#define loopvk(v)   for(int k = 0; k<(v).length(); k++)
#define loopvl(v)   for(int l = 0; l<(v).length(); l++)
#define loopvm(v)   for(int m = 0; m<(v).length(); m++)
#define loopvn(v)   for(int n = 0; n<(v).length(); n++)
#define loopvrev(v) for(int i = (v).length()-1; i>=0; i--)
#define loopvjrev(v) for(int j = (v).length()-1; j>=0; j--)
#define loopvkrev(v) for(int k = (v).length()-1; k>=0; k--)
#define loopvlrev(v) for(int l = (v).length()-1; l>=0; l--)
#define loopvmrev(v) for(int m = (v).length()-1; m>=0; m--)
#define loopvnrev(v) for(int n = (v).length()-1; n>=0; n--)
#define loopcsv(u,c,s) loopcs(i,(u).length(),c,s)
#define loopcsvj(u,c,s) loopcs(j,(u).length(),c,s)
#define loopcsvk(u,c,s) loopcs(k,(u).length(),c,s)
#define loopcsvl(u,c,s) loopcs(l,(u).length(),c,s)
#define loopcsvm(u,c,s) loopcs(m,(u).length(),c,s)
#define loopcsvn(u,c,s) loopcs(n,(u).length(),c,s)
#define loopcsvrev(u,c,s) loopcsrev(i,(u).length(),c,s)
#define loopcsvjrev(u,c,s) loopcsrev(j,(u).length(),c,s)
#define loopcsvkrev(u,c,s) loopcsrev(k,(u).length(),c,s)
#define loopcsvlrev(u,c,s) loopcsrev(l,(u).length(),c,s)
#define loopcsvmrev(u,c,s) loopcsrev(m,(u).length(),c,s)
#define loopcsvnrev(u,c,s) loopcsrev(n,(u).length(),c,s)

template<class T> inline void memclear(T *p, size_t n) { memset((void *)p, 0, n * sizeof(T)); }
template<class T> inline void memclear(T &p) { memset((void *)&p, 0, sizeof(T)); }
template<class T, size_t N> inline void memclear(T (&p)[N]) { memset((void *)p, 0, N * sizeof(T)); }

template <class T>
struct databuf
{
    enum
    {
        OVERREAD  = 1<<0,
        OVERWROTE = 1<<1
    };

    T *buf;
    int len, maxlen;
    uchar flags;

    databuf() : buf(NULL), len(0), maxlen(0), flags(0) {}

    template<class U>
    databuf(T *buf, U maxlen) : buf(buf), len(0), maxlen((int)maxlen), flags(0) {}

    void reset()
    {
        len = 0;
        flags = 0;
    }

    void reset(T *buf_, int maxlen_)
    {
        reset();
        buf = buf_;
        maxlen = maxlen_;
    }

    const T &get()
    {
        static const T overreadval = 0;
        if(len<maxlen) return buf[len++];
        flags |= OVERREAD;
        return overreadval;
    }

    databuf subbuf(int sz)
    {
        sz = clamp(sz, 0, maxlen-len);
        len += sz;
        return databuf(&buf[len-sz], sz);
    }

    T *pad(int numvals)
    {
        T *vals = &buf[len];
        len += std::min(numvals, maxlen-len);
        return vals;
    }

    void put(const T &val)
    {
        if(len<maxlen) buf[len++] = val;
        else flags |= OVERWROTE;
    }

    void put(const T *vals, int numvals)
    {
        if(maxlen - len < numvals)
        {
            numvals = maxlen - len;
            flags |= OVERWROTE;
        }
        memcpy(&buf[len], (const void *)vals, numvals*sizeof(T));
        len += numvals;
    }

    int get(T *vals, int numvals)
    {
        if(maxlen - len < numvals)
        {
            numvals = maxlen - len;
            flags |= OVERREAD;
        }
        memcpy(vals, (void *)&buf[len], numvals*sizeof(T));
        len += numvals;
        return numvals;
    }

    void offset(int n)
    {
        n = std::min(n, maxlen);
        buf += n;
        maxlen -= n;
        len = std::max(len-n, 0);
    }

    T *getbuf() const { return buf; }
    bool empty() const { return len==0; }
    int length() const { return len; }
    int remaining() const { return maxlen-len; }
    bool overread() const { return (flags&OVERREAD)!=0; }
    bool overwrote() const { return (flags&OVERWROTE)!=0; }

    bool check(int n) { return remaining() >= n; }

    void forceoverread()
    {
        len = maxlen;
        flags |= OVERREAD;
    }
};

typedef databuf<char> charbuf;
typedef databuf<uchar> ucharbuf;

struct packetbuf : ucharbuf
{
    ENetPacket *packet;
    int growth;

    packetbuf(ENetPacket *packet) : ucharbuf(packet->data, packet->dataLength), packet(packet), growth(0) {}
    packetbuf(int growth, int pflags = 0) : growth(growth)
    {
        packet = enet_packet_create(NULL, growth, pflags);
        buf = (uchar *)packet->data;
        maxlen = packet->dataLength;
    }
    ~packetbuf() { cleanup(); }

    void reliable() { packet->flags |= ENET_PACKET_FLAG_RELIABLE; }

    void resize(int n)
    {
        enet_packet_resize(packet, n);
        buf = (uchar *)packet->data;
        maxlen = packet->dataLength;
    }

    void checkspace(int n)
    {
        if(len + n > maxlen && packet && growth > 0) resize(std::max(len + n, maxlen + growth));
    }

    ucharbuf subbuf(int sz)
    {
        checkspace(sz);
        return ucharbuf::subbuf(sz);
    }

    void put(const uchar &val)
    {
        checkspace(1);
        ucharbuf::put(val);
    }

    void put(const uchar *vals, int numvals)
    {
        checkspace(numvals);
        ucharbuf::put(vals, numvals);
    }

    ENetPacket *finalize()
    {
        resize(len);
        return packet;
    }

    void cleanup()
    {
        if(growth > 0 && packet && !packet->referenceCount) { enet_packet_destroy(packet); packet = NULL; buf = NULL; len = maxlen = 0; }
    }
};

template<class T>
static inline float heapscore(const T &n) { return n; }

struct sortless
{
    template<class T> bool operator()(const T &x, const T &y) const { return x < y; }
    bool operator()(char *x, char *y) const { return strcmp(x, y) < 0; }
    bool operator()(const char *x, const char *y) const { return strcmp(x, y) < 0; }
};

struct sortnameless
{
    template<class T> bool operator()(const T &x, const T &y) const { return sortless()(x.name, y.name); }
    template<class T> bool operator()(T *x, T *y) const { return sortless()(x->name, y->name); }
    template<class T> bool operator()(const T *x, const T *y) const { return sortless()(x->name, y->name); }
};

template<class T> struct isclass
{
    template<class C> static char test(void (C::*)(void));
    template<class C> static int test(...);
    enum { yes = sizeof(test<T>(0)) == 1 ? 1 : 0, no = yes^1 };
};

static inline uint hthash(const char *key)
{
    uint h = 5381;
    for(int i = 0, k; (k = key[i]); i++) h = ((h<<5)+h)^k;    // bernstein k=33 xor
    return h;
}

static inline bool htcmp(const char *x, const char *y)
{
    return !strcmp(x, y);
}

struct stringslice
{
    const char *str;
    int len;
    stringslice() {}
    stringslice(const char *str, int len) : str(str), len(len) {}
    stringslice(const char *str, const char *end) : str(str), len(int(end-str)) {}

    const char *end() const { return &str[len]; }
};

inline char *newstring(const stringslice &s) { return newstring(s.str, s.len); }
inline const char *stringptr(const char *s) { return s; }
inline const char *stringptr(const stringslice &s) { return s.str; }
inline int stringlen(const char *s) { return int(strlen(s)); }
inline int stringlen(const stringslice &s) { return s.len; }

inline char *copystring(char *d, const stringslice &s, size_t len)
{
    size_t slen = std::min(size_t(s.len), len-1);
    memcpy(d, s.str, slen);
    d[slen] = 0;
    return d;
}
template<size_t N> inline char *copystring(char (&d)[N], const stringslice &s) { return copystring(d, s, N); }

static inline uint memhash(const void *ptr, int len)
{
    const uchar *data = (const uchar *)ptr;
    uint h = 5381;
    loopi(len) h = ((h<<5)+h)^data[i];
    return h;
}

static inline uint hthash(const stringslice &s) { return memhash(s.str, s.len); }

static inline bool htcmp(const stringslice &x, const char *y)
{
    return x.len == (int)strlen(y) && !memcmp(x.str, y, x.len);
}

static inline uint hthash(int key)
{
    return key;
}

static inline bool htcmp(int x, int y)
{
    return x==y;
}

#ifndef STANDALONE
static inline uint hthash(GLuint key)
{
    return key;
}

static inline bool htcmp(GLuint x, GLuint y)
{
    return x==y;
}
#endif

template <class T> struct vector
{
    /* 
    static const int MINSIZE = 8;

    T *buf;
    int alen, ulen;
    */

    std::vector<T> _v;

    vector() : _v{} {}

    vector(const vector &v)
    {
        this->_v = v._v;
    }

    vector<T> &operator=(const vector<T> &v)
    {
        this->_v = v._v;
        return *this;
    }

    T &add(const T &x)
    {
        _v.push_back(x);
        return _v.back();
    }

    T &add()
    {
        _v.push_back(T{});
        return _v.back();
    }

    void add(const T &x, size_t n)
    {
        for (size_t i=0; i<n; i++) _v.push_back(x);
    }

    T &dup()
    {
        _v.push_back(_v.back()); 
        return _v.back();
    }

    
    void move(vector<T> &v)
    {
        _v = std::move(v._v);
    }
    
    bool inrange(size_t i) const { return i<_v.size(); }

    T pop() { auto result = _v.back(); _v.pop_back(); return result;}
    T &last() { return _v.back(); }
//    T &first() { return _v.front(); }
    void drop() { _v.erase(_v.end() - 1); }
    bool empty() const { return _v.empty(); }

    int capacity() const { return _v.capacity(); }
    int length() const { return _v.size(); }
    T &operator[](int i) { return _v.at(i); }
    const T &operator[](int i) const { return _v.at(i); }

    T *disown() { 
        auto buffer = new T[_v.size()];
        std::copy(_v.begin(), _v.end(), buffer);
        this->_v = std::vector<T>{};
        return buffer;
    }

    void shrink(const int i) { setsize(i); }

    void setsize(const int i)  { 
        if (i <= length()) while(i < length()) _v.pop_back();
        else throw std::logic_error("wrong setsize");
    }

    void setsize(int i, const T &x)
    {
        if(i >= length()) add(x, i - length());
        else shrink(i);
    }


    void deletecontents(int n = 0) { 
        for (auto it = _v.begin() + n; it != _v.end(); it++) {
            delete *it;
        }
        _v.erase(_v.begin() + n, _v.end());
    }
    
    void deletearrays(int n = 0) { 
        for (auto it = _v.begin() + n; it != _v.end(); it++) {
            delete [] *it;
        }
        _v.erase(_v.begin() + n, _v.end());
    }

    T *getbuf() { return _v.data(); }
    const T *getbuf() const { return _v.data(); }
    bool inbuf(const T *e) const { return e >= _v.begin() && e < _v.end(); }

    template<class F>
    void sort(F fun, int i = 0, int n = -1)
    {
        std::sort(_v.begin() + i, n < 0 ? _v.end() : _v.begin() + n + i, fun);
    }

    void sort() { sort(sortless()); }
    void sortname() { sort(sortnameless()); }

    void growbuf(int sz)
    {
        _v.reserve(sz);
    }

    databuf<T> reserve(int sz)
    {
        _v.reserve(_v.size() + sz);
        return databuf<T>(&_v[_v.size()], sz);
    }

    void advance(int sz)
    {
        _v.insert(_v.end(), &_v[_v.size()], &_v[_v.size()+sz]);
    }

    void addbuf(const databuf<T> &p)
    {
        advance(p.length());
    }

    T *pad(int n)
    {
        T *buf = reserve(n).buf;
        advance(n);
        return buf;
    }

    void put(const T &v) { _v.push_back(v); }

    void put(const T *v, int n)
    {
        _v.insert(_v.end(), v, v + n);
    }

    void remove(int i, int n)
    {
        _v.erase(_v.begin() + i, _v.begin() + i + n);
    }

    T remove(int i)
    {
        T result = _v.at(i);
        _v.erase(_v.begin() + i);
        return result;
    }

    T removeunordered(int i)
    {
        return remove(i);
    }

    template<class U>
    int find(const U &o)
    {
        auto it = std::find(_v.begin(), _v.end(), o);
        return it != _v.end() ? std::distance(_v.begin(), it) : -1;
    }

    void addunique(const T &o)
    {
        if (std::find(_v.begin(), _v.end(), o) == _v.end()) _v.push_back(o);
    }
    
    void removeobj(const T &o)
    {
        for (auto it = _v.begin(); it != _v.end(); it++) {
            if (*it == o) {
                _v.erase(it);
                break;
            }
        }
    }

    //void replacewithlast(const T &o)
    //{
    //    if(!ulen) return;
    //    loopi(ulen-1) if(buf[i]==o)
    //    {
    //        buf[i] = buf[ulen-1];
    //        break;
    //    }
    //    ulen--;
    //}

    T &insert(int i, const T &e)
    {
        return std::ref(*_v.insert(_v.begin() + i, e));
    }

    T *insert(int i, const T *e, int n)
    {
        return &*_v.insert(_v.begin() + i, e, e + n);
    }

    void reverse()
    {
        std::reverse(_v.begin(), _v.end());
    }
    
    template<class K>
    int htfind(const K &key)
    {
        for (size_t i = 0; i < _v.size(); i++) if(htcmp(key, _v.at(i))) return i;
        return -1;
    }

    void unique() // contents must be initially sorted
    {
        _v.erase(std::unique(_v.begin(), _v.end()), _v.end()); 
    }
    void uniquedeletecontents()
    {
        for (auto it = std::unique(_v.begin(), _v.end()); it != _v.end(); it++) {
            delete *it;
        }
        unique();
    }
    void uniquedeletearrays()
    {
        for (auto it = std::unique(_v.begin(), _v.end()); it != _v.end(); it++) {
            delete [] *it;
        }
        unique();
    }

    void write(int i, const T *v, int n)
    {
        if(i < 0)
        {
            n += i;
            v -= i;
            i = 0;
        }
        if(n <= 0 || i > length()) return;
        if(i + n > length()) growbuf(i + n);
        std::copy(v, v + n, _v.begin() + i);
    }

    void write(int i, const vector<T>& v)
    {
        write(i, v.getbuf(), v.length());
    }

    void write(int i, const vector<T>& v, int n)
    {
        write(i, v.getbuf(), std::min(v.length(), n));
    }
};

template <class T> struct smallvector
{
    T *buf;
    int len;

    smallvector() : buf(NULL), len(0)
    {
    }

    smallvector(const smallvector &v) : buf(NULL), len(0)
    {
        *this = v;
    }

    ~smallvector() { shrink(0); }

    smallvector<T> &operator=(const smallvector<T> &v)
    {
        shrink(0);
        growbuf(v.length());
        loopv(v) buf[i] = v[i];
        return *this;
    }

    void growbuf(int sz)
    {
        len = std::max(sz, 0);
        if(len)
        {
            buf = (T *)realloc(buf, len*sizeof(T));
            if(!buf) abort();
        }
        else if(buf) { free(buf); buf = NULL; }
    }

    T &add(const T &x)
    {
        growbuf(len+1);
        new (&buf[len-1]) T(x);
        return buf[len-1];
    }

    T &add()
    {
        growbuf(len+1);
        new (&buf[len-1]) T;
        return buf[len-1];
    }

    void add(const T &x, int n)
    {
        if(n <= 0) return;
        growbuf(len+n);
        while(n > 0) new (&buf[len-n--]) T(x);
    }

    void put(const T &v) { add(v); }

    void put(const T *v, int n)
    {
        if(n <= 0) return;
        growbuf(len + n);
        memcpy(&buf[len-n], v, n*sizeof(T));
    }

    void write(int i, const T *v, int n)
    {
        if(i < 0)
        {
            n += i;
            v -= i;
            i = 0;
        }
        if(n <= 0 || i > len) return;
        if(i + n > len) growbuf(i + n);
        memcpy(&buf[i], v, n*sizeof(T));
    }

    void write(int i, const smallvector<T>& v)
    {
        write(i, v.getbuf(), v.length());
    }

    void write(int i, const smallvector<T>& v, int n)
    {
        write(i, v.getbuf(), std::min(v.length(), n));
    }

    void shrink(int i)
    {
        ASSERT(i<=len);
        if(i >= len) return;
        if(isclass<T>::yes) for(int j = i; j < len; j++) buf[j].~T();
        growbuf(i);
    }

    void setsize(int i)
    {
        ASSERT(i<=len);
        if(i >= len) return;
        growbuf(i);
    }

    void setsize(int i, const T &x)
    {
        if(i >= len) add(x, i - len);
        else growbuf(i);
    }

    void deletecontents()
    {
        for(int i = 0; i < len; i++) delete buf[i];
        setsize(0);
    }

    void deletearrays()
    {
        for(int i = 0; i < len; i++) delete[] buf[i];
        setsize(0);
    }

    T remove(int i)
    {
        T e = buf[i];
        for(int p = i+1; p<len; p++) buf[p-1] = buf[p];
        growbuf(len-1);
        return e;
    }

    T removeunordered(int i)
    {
        T e = buf[i];
        if(len>1) buf[i] = buf[len-1];
        growbuf(len-1);
        return e;
    }

    void drop() { buf[len-1].~T(); growbuf(len-1); }

    T &insert(int i, const T &e)
    {
        add(T());
        for(int p = len-1; p>i; p--) buf[p] = buf[p-1];
        buf[i] = e;
        return buf[i];
    }

    T *insert(int i, const T *e, int n)
    {
        growbuf(len+n);
        loopj(n) add(T());
        for(int p = len-1; p>=i+n; p--) buf[p] = buf[p-n];
        loopj(n) buf[i+j] = e[j];
        return &buf[i];
    }

    bool inrange(size_t i) const { return i<size_t(len); }
    bool inrange(int i) const { return i>=0 && i<len; }

    T &last() { return buf[len-1]; }
    T &first() { return buf[0]; }
    bool empty() const { return len==0; }
    int length() const { return len; }
    T &operator[](int i) { ASSERT(i>=0 && i<len); return buf[i]; }
    const T &operator[](int i) const { ASSERT(i >= 0 && i<len); return buf[i]; }
    T *getbuf() { return buf; }
    const T *getbuf() const { return buf; }
    bool inbuf(const T *e) const { return e >= buf && e < &buf[len]; }

    template<class U>
    int find(const U &o)
    {
        loopi(len) if(buf[i]==o) return i;
        return -1;
    }

    template<class K>
    int htfind(const K &key)
    {
        loopi(len) if(htcmp(key, buf[i])) return i;
        return -1;
    }
};

template<class H, class E, class K, class T> struct hashbase
{
    typedef E elemtype;
    typedef K keytype;
    typedef T datatype;

    enum { CHUNKSIZE = 64 };

    struct chain { E elem; chain *next; };
    struct chainchunk { chain chains[CHUNKSIZE]; chainchunk *next; };

    int size;
    int numelems;
    chain **chains;

    chainchunk *chunks;
    chain *unused;

    enum { DEFAULTSIZE = 1<<10 };

    hashbase(int size = DEFAULTSIZE)
      : size(size)
    {
        numelems = 0;
        chunks = NULL;
        unused = NULL;
        chains = new chain *[size];
        memset(chains, 0, size*sizeof(chain *));
    }

    ~hashbase()
    {
        DELETEA(chains);
        deletechunks();
    }

    chain *insert(uint h)
    {
        if(!unused)
        {
            chainchunk *chunk = new chainchunk;
            chunk->next = chunks;
            chunks = chunk;
            loopi(CHUNKSIZE-1) chunk->chains[i].next = &chunk->chains[i+1];
            chunk->chains[CHUNKSIZE-1].next = unused;
            unused = chunk->chains;
        }
        chain *c = unused;
        unused = unused->next;
        c->next = chains[h];
        chains[h] = c;
        numelems++;
        return c;
    }

    template<class U>
    T &insert(uint h, const U &key)
    {
        chain *c = insert(h);
        H::setkey(c->elem, key);
        return H::getdata(c->elem);
    }

    #define HTFIND(success, fail) \
        uint h = hthash(key)&(this->size-1); \
        for(chain *c = this->chains[h]; c; c = c->next) \
        { \
            if(htcmp(key, H::getkey(c->elem))) return success H::getdata(c->elem); \
        } \
        return (fail);

    template<class U>
    T *access(const U &key)
    {
        HTFIND(&, NULL);
    }

    template<class U, class V>
    T &access(const U &key, const V &elem)
    {
        HTFIND( , insert(h, key) = elem);
    }

    template<class U>
    T &operator[](const U &key)
    {
        HTFIND( , insert(h, key));
    }

    template<class U>
    T &find(const U &key, T &notfound)
    {
        HTFIND( , notfound);
    }

    template<class U>
    const T &find(const U &key, const T &notfound)
    {
        HTFIND( , notfound);
    }

    template<class U>
    bool remove(const U &key)
    {
        uint h = hthash(key)&(size-1);
        for(chain **p = &chains[h], *c = chains[h]; c; p = &c->next, c = c->next)
        {
            if(htcmp(key, H::getkey(c->elem)))
            {
                *p = c->next;
                c->elem.~E();
                new (&c->elem) E;
                c->next = unused;
                unused = c;
                numelems--;
                return true;
            }
        }
        return false;
    }

    void recycle()
    {
        if(!numelems) return;
        loopi(size)
        {
            chain *c = chains[i];
            if(!c) continue;
            for(;;)
            {
                htrecycle(c->elem);
                if(!c->next) break;
                c = c->next;
            }
            c->next = unused;
            unused = chains[i];
            chains[i] = NULL;
        }
        numelems = 0;
    }

    void deletechunks()
    {
        for(chainchunk *nextchunk; chunks; chunks = nextchunk)
        {
            nextchunk = chunks->next;
            delete chunks;
        }
    }

    void clear()
    {
        if(!numelems) return;
        memset(chains, 0, size*sizeof(chain *));
        numelems = 0;
        unused = NULL;
        deletechunks();
    }

    static inline chain *enumnext(void *i) { return ((chain *)i)->next; }
    static inline K &enumkey(void *i) { return H::getkey(((chain *)i)->elem); }
    static inline T &enumdata(void *i) { return H::getdata(((chain *)i)->elem); }
};

template<class T> static inline void htrecycle(const T &) {}

template<class T> struct hashset : hashbase<hashset<T>, T, T, T>
{
    typedef hashbase<hashset<T>, T, T, T> basetype;

    hashset(int size = basetype::DEFAULTSIZE) : basetype(size) {}

    static inline const T &getkey(const T &elem) { return elem; }
    static inline T &getdata(T &elem) { return elem; }
    template<class K> static inline void setkey(T &elem, const K &key) {}

    template<class V>
    T &add(const V &elem)
    {
        return basetype::access(elem, elem);
    }
};

template<class T> struct hashnameset : hashbase<hashnameset<T>, T, const char *, T>
{
    typedef hashbase<hashnameset<T>, T, const char *, T> basetype;

    hashnameset(int size = basetype::DEFAULTSIZE) : basetype(size) {}

    template<class U> static inline const char *getkey(const U &elem) { return elem.name; }
    template<class U> static inline const char *getkey(const U &elem) requires requires {elem.name.c_str();}{ return elem.name.c_str(); }
    template<class U> static inline const char *getkey(U *elem) { return elem->name; }
    template<class U> static inline const char *getkey(U *elem) requires requires {elem->name.c_str();}{ return elem->name.c_str(); }
    static inline T &getdata(T &elem) { return elem; }
    template<class K> static inline void setkey(T &elem, const K &key) {}

    template<class V>
    T &add(const V &elem)
    {
        return basetype::access(getkey(elem), elem);
    }
};

template<class K, class T> struct hashtableentry
{
    K key;
    T data;
};

template<class K, class T>
static inline void htrecycle(hashtableentry<K, T> &entry)
{
    htrecycle(entry.key);
    htrecycle(entry.data);
}

template<class K, class T> struct hashtable : hashbase<hashtable<K, T>, hashtableentry<K, T>, K, T>
{
    typedef hashbase<hashtable<K, T>, hashtableentry<K, T>, K, T> basetype;
    typedef typename basetype::elemtype elemtype;

    hashtable(int size = basetype::DEFAULTSIZE) : basetype(size) {}

    static inline K &getkey(elemtype &elem) { return elem.key; }
    static inline T &getdata(elemtype &elem) { return elem.data; }
    template<class U> static inline void setkey(elemtype &elem, const U &key) { elem.key = key; }
};

#define enumeratekt(ht,k,e,t,f,b) loopi((ht).size) for(void *ec = (ht).chains[i]; ec;) { k &e = (ht).enumkey(ec); t &f = (ht).enumdata(ec); ec = (ht).enumnext(ec); b; }
#define enumerate(ht,t,e,b)       loopi((ht).size) for(void *ec = (ht).chains[i]; ec;) { t &e = (ht).enumdata(ec); ec = (ht).enumnext(ec); b; }

struct unionfind
{
    struct ufval
    {
        int rank, next;

        ufval() : rank(0), next(-1) {}
    };

    vector<ufval> ufvals;

    int find(int k)
    {
        if(k>=ufvals.length()) return k;
        while(ufvals[k].next>=0) k = ufvals[k].next;
        return k;
    }

    int compressfind(int k)
    {
        if(ufvals[k].next<0) return k;
        return ufvals[k].next = compressfind(ufvals[k].next);
    }

    void unite (int x, int y)
    {
        while(ufvals.length() <= std::max(x, y)) ufvals.add();
        x = compressfind(x);
        y = compressfind(y);
        if(x==y) return;
        ufval &xval = ufvals[x], &yval = ufvals[y];
        if(xval.rank < yval.rank) xval.next = y;
        else
        {
            yval.next = x;
            if(xval.rank==yval.rank) yval.rank++;
        }
    }
};

template <class T, int SIZE> struct queue
{
    int head, tail, len;
    T data[SIZE];

    queue() { clear(); }

    void clear() { head = tail = len = 0; }

    int capacity() const { return SIZE; }
    int length() const { return len; }
    bool empty() const { return !len; }
    bool full() const { return len == SIZE; }

    bool inrange(size_t i) const { return i<size_t(len); }
    bool inrange(int i) const { return i>=0 && i<len; }

    T &added() { return data[tail > 0 ? tail-1 : SIZE-1]; }
    T &added(int offset) { return data[tail-offset > 0 ? tail-offset-1 : tail-offset-1 + SIZE]; }
    T &adding() { return data[tail]; }
    T &adding(int offset) { return data[tail+offset >= SIZE ? tail+offset - SIZE : tail+offset]; }
    T &add()
    {
        T &t = data[tail];
        tail++;
        if(tail >= SIZE) tail -= SIZE;
        if(len < SIZE) len++;
        return t;
    }
    T &add(const T &e) { return add() = e; }

    databuf<T> reserve(int sz)
    {
        if(!len) head = tail = 0;
        return databuf<T>(&data[tail], std::min(sz, SIZE-tail));
    }

    void advance(int sz)
    {
        if(len + sz > SIZE) sz = SIZE - len;
        tail += sz;
        if(tail >= SIZE) tail -= SIZE;
        len += sz;
    }

    void addbuf(const databuf<T> &p)
    {
        advance(p.length());
    }

    T &insertback(int offset)
    {
        int cur = tail, next = tail;
        add();
        loopi(offset)
        {
            cur--;
            if(cur < 0) cur += SIZE;
            data[next] = data[cur];
            next = cur;
        }
        return data[cur];
    }
    T &insertback(int offset, const T &e) { return insertback(offset) = e; }

    T &pop()
    {
        tail--;
        if(tail < 0) tail += SIZE;
        len--;
        return data[tail];
    }

    T &removing() { return data[head]; }
    T &removing(int offset) { return data[head+offset >= SIZE ? head+offset - SIZE : head+offset]; }
    T &remove()
    {
        T &t = data[head];
        head++;
        if(head >= SIZE) head -= SIZE;
        len--;
        return t;
    }

    T remove(int offset)
    {
        T val = removing(offset);
        if(head+offset >= SIZE) for(int i = head+offset - SIZE + 1; i < tail; i++) data[i-1] = data[i];
        else if(head < tail) for(int i = head+offset + 1; i < tail; i++) data[i-1] = data[i];
        else
        {
            for(int i = head+offset + 1; i < SIZE; i++) data[i-1] = data[i];
            data[SIZE-1] = data[0];
            for(int i = 1; i < tail; i++) data[i-1] = data[i];
        }
        tail--;
        if(tail < 0) tail += SIZE;
        len--;
        return val;
    }

    T &operator[](int offset) { return removing(offset); }
    const T &operator[](int offset) const { return removing(offset); }
};

template <class T, int SIZE> struct reversequeue : queue<T, SIZE>
{
    T &insert(int offset) { return queue<T, SIZE>::insertback(offset); }
    T &insert(int offset, const T &e) { return queue<T, SIZE>::insertback(offset, e); }

    T &operator[](int offset) { return queue<T, SIZE>::added(offset); }
    const T &operator[](int offset) const { return queue<T, SIZE>::added(offset); }
};

struct slot
{
    int index;
    std::string name;

    slot() : index(-1) {}

    bool isfree() const { return index < 0; }
};

template<class T>
struct slotmanager
{
    vector<T> slots;
    hashnameset<slot> slotmap;

    int length() const { return slots.length(); }
    bool inrange(int index) const { return slots.inrange(index); }

    slot *getslot(const char *name)
    {
        slot *s = slotmap.access(name);

        if(!s)
        {
            s = &slotmap[name];
            s->name = newstring(name);
        }

        return s;
    }

    int getindex(const char *name)
    {
        slot *s = slotmap.access(name);
        return s ? s->index : -1;
    }

    const char *getname(int index)
    {
        enumerate(slotmap, slot, s, if(s.index == index) return s.name.c_str(); );
        return NULL;
    }

    int assign(const char *name)
    {
        slot *s = getslot(name);
        int index = s->isfree() ? slots.length() : s->index;
        s->index = index;

        return index;
    }

    int add(const T &x, const char *name = NULL)
    {
        int slotidx = (name && *name) ? assign(name) : slots.length();

        if(slotidx > slots.length()) slots.add(T(), slotidx - slots.length());
        if(slotidx == slots.length()) slots.add(x);
        else slots[slotidx] = x;

        return slotidx;
    }

    int add(const char *name = NULL) { return add(T(), name); }

    void clear(bool freeres = true)
    {
        if(freeres) slots.shrink(0);
        else slots.setsize(0);

        enumerate(slotmap, slot, s, s.index = -1);
    }

    T &operator[](int index) { return slots[index]; }
    const T &operator[](int index) const { return slots[index]; }

    T& last() { return slots.last(); }
    const T& last() const { return slots.last(); }

    T& first() { return slots.first(); }
    const T& first() const { return slots.first(); }

    T *get(const char *name)
    {
        if(!name || !*name) return NULL;

        slot *s = slotmap.access(name);
        if(s && slots.inrange(s->index)) return &slots[s->index];

        return NULL;
    }
};

#if defined(WIN32) && !defined(__GNUC__) && !defined(__clang__)
#ifdef _DEBUG
//#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
inline void *__cdecl operator new(size_t n, const char *fn, int l) { return ::operator new(n, 1, fn, l); }
inline void __cdecl operator delete(void *p, const char *fn, int l) { ::operator delete(p, 1, fn, l); }
#define new new(__FILE__,__LINE__)
#endif
#endif

static inline bool islittleendian() { union { int i; uchar b[sizeof(int)]; } conv; conv.i = 1; return conv.b[0] != 0; }
#ifdef SDL_BYTEORDER
#define endianswap16 SDL_Swap16
#define endianswap32 SDL_Swap32
#define endianswap64 SDL_Swap64
#else
inline ushort endianswap16(ushort n) { return (n<<8) | (n>>8); }
inline uint endianswap32(uint n) { return (n<<24) | (n>>24) | ((n>>8)&0xFF00) | ((n<<8)&0xFF0000); }
inline ullong endianswap64(ullong n) { return endianswap32(uint(n >> 32)) | ((ullong)endianswap32(uint(n)) << 32); }
#endif
template<class T> inline T endianswap(T n) { union { T t; uint i; } conv; conv.t = n; conv.i = endianswap32(conv.i); return conv.t; }
template<> inline ushort endianswap<ushort>(ushort n) { return endianswap16(n); }
template<> inline short endianswap<short>(short n) { return endianswap16(n); }
template<> inline uint endianswap<uint>(uint n) { return endianswap32(n); }
template<> inline int endianswap<int>(int n) { return endianswap32(n); }
template<> inline ullong endianswap<ullong>(ullong n) { return endianswap64(n); }
template<> inline llong endianswap<llong>(llong n) { return endianswap64(n); }
template<> inline double endianswap<double>(double n) { union { double t; uint i; } conv; conv.t = n; conv.i = endianswap64(conv.i); return conv.t; }
template<class T> inline void endianswap(T *buf, size_t len) { for(T *end = &buf[len]; buf < end; buf++) *buf = endianswap(*buf); }
template<class T> inline T endiansame(T n) { return n; }
template<class T> inline void endiansame(T *buf, size_t len) {}
#ifdef SDL_BYTEORDER
#if SDL_BYTEORDER == SDL_LIL_ENDIAN
#define lilswap endiansame
#define bigswap endianswap
#else
#define lilswap endianswap
#define bigswap endiansame
#endif
#elif defined(__BYTE_ORDER__)
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
#define lilswap endiansame
#define bigswap endianswap
#else
#define lilswap endianswap
#define bigswap endiansame
#endif
#else
template<class T> inline T lilswap(T n) { return islittleendian() ? n : endianswap(n); }
template<class T> inline void lilswap(T *buf, size_t len) { if(!islittleendian()) endianswap(buf, len); }
template<class T> inline T bigswap(T n) { return islittleendian() ? endianswap(n) : n; }
template<class T> inline void bigswap(T *buf, size_t len) { if(islittleendian()) endianswap(buf, len); }
#endif

/* workaround for some C platforms that have these two functions as macros - not used anywhere */
#ifdef getchar
#undef getchar
#endif
#ifdef putchar
#undef putchar
#endif

struct stream
{
#ifdef WIN32
#ifdef __GNUC__
    typedef off64_t offset;
#else
    typedef __int64 offset;
#endif
#else
    typedef off_t offset;
#endif

    virtual ~stream() {}
    virtual void close() = 0;
    virtual bool end() = 0;
    virtual offset tell() { return -1; }
    virtual offset rawtell() { return tell(); }
    virtual bool seek(offset pos, int whence = SEEK_SET) { return false; }
    virtual offset size();
    virtual offset rawsize() { return size(); }
    virtual size_t read(void *buf, size_t len) { return 0; }
    virtual size_t write(const void *buf, size_t len) { return 0; }
    virtual bool flush() { return true; }
    virtual int getchar() { uchar c; return read(&c, 1) == 1 ? c : -1; }
    virtual bool putchar(int n) { uchar c = n; return write(&c, 1) == 1; }
    virtual bool getline(char *str, size_t len);
    virtual bool putstring(const char *str) { size_t len = strlen(str); return write(str, len) == len; }
    virtual bool putline(const char *str) { return putstring(str) && putchar('\n'); }
    virtual size_t printf(const char *fmt, ...) PRINTFARGS(2, 3);
    virtual uint getcrc() { return 0; }

    template<class T> size_t put(const T *v, size_t n) { return write(v, n*sizeof(T))/sizeof(T); }
    template<class T> bool put(T n) { return write(&n, sizeof(n)) == sizeof(n); }
    template<class T> bool putlil(T n) { return put<T>(lilswap(n)); }
    template<class T> bool putbig(T n) { return put<T>(bigswap(n)); }

    template<class T> size_t get(T *v, size_t n) { return read(v, n*sizeof(T))/sizeof(T); }
    template<class T> T get() { T n; return read(&n, sizeof(n)) == sizeof(n) ? n : 0; }
    template<class T> T getlil() { return lilswap(get<T>()); }
    template<class T> T getbig() { return bigswap(get<T>()); }

#ifndef STANDALONE
    SDL_RWops *rwops();
#endif
};

template<class T>
struct streambuf
{
    stream *s;

    streambuf(stream *s) : s(s) {}

    T get() { return s->get<T>(); }
    size_t get(T *vals, size_t numvals) { return s->get(vals, numvals); }
    void put(const T &val) { s->put(&val, 1); }
    void put(const T *vals, size_t numvals) { s->put(vals, numvals); }
    size_t length() { return s->size(); }
};

enum
{
    CT_PRINT   = 1<<0,
    CT_SPACE   = 1<<1,
    CT_DIGIT   = 1<<2,
    CT_ALPHA   = 1<<3,
    CT_LOWER   = 1<<4,
    CT_UPPER   = 1<<5,
    CT_UNICODE = 1<<6
};
extern const uchar cubectype[256];
static inline int iscubeprint(uchar c) { return cubectype[c]&CT_PRINT; }
static inline int iscubespace(uchar c) { return cubectype[c]&CT_SPACE; }
static inline int iscubealpha(uchar c) { return cubectype[c]&CT_ALPHA; }
static inline int iscubealnum(uchar c) { return cubectype[c]&(CT_ALPHA|CT_DIGIT); }
static inline int iscubelower(uchar c) { return cubectype[c]&CT_LOWER; }
static inline int iscubeupper(uchar c) { return cubectype[c]&CT_UPPER; }
static inline int iscubepunct(uchar c) { return cubectype[c] == CT_PRINT; }
static inline int cube2uni(uchar c)
{
    extern const int cube2unichars[256];
    return cube2unichars[c];
}
static inline uchar uni2cube(int c)
{
    extern const int uni2cubeoffsets[8];
    extern const uchar uni2cubechars[];
    return uint(c) <= 0x7FF ? uni2cubechars[uni2cubeoffsets[c>>8] + (c&0xFF)] : 0;
}
static inline uchar cubelower(uchar c)
{
    extern const uchar cubelowerchars[256];
    return cubelowerchars[c];
}
static inline uchar cubeupper(uchar c)
{
    extern const uchar cubeupperchars[256];
    return cubeupperchars[c];
}
extern size_t decodeutf8(uchar *dst, size_t dstlen, const uchar *src, size_t srclen, size_t *carry = NULL);
extern size_t encodeutf8(uchar *dstbuf, size_t dstlen, const uchar *srcbuf, size_t srclen, size_t *carry = NULL);

extern int crcstream(stream *f);
extern int crcfile(const char *s);
extern char *makerelpath(const char *dir, const char *file, const char *prefix = NULL, const char *cmd = NULL);
extern char *makefile(const char *s, const char *e = "", int revision = 0, int start = 1, bool store = false, bool skip = false);
extern char *path(char *s, bool simple = false);
extern char *copypath(const char *s, bool simple = false);
extern const char *parentdir(const char *directory);
extern bool fileexists(const char *path, const char *mode);
extern bool createdir(const char *path);
extern void sethomedir(const char *dir);
extern void appendhomedir(const char *dir);
extern void addpackagedir(const char *dir, int flags = 0);
extern int maskpackagedirs(int flags);
extern const char *findfile(const char *filename, const char *mode);
extern bool findzipfile(const char *filename);
extern stream *openrawfile(const char *filename, const char *mode);
extern stream *openzipfile(const char *filename, const char *mode);
extern stream *openfile(const char *filename, const char *mode);
extern stream *opentempfile(const char *filename, const char *mode);
extern stream *opengzfile(const char *filename, const char *mode, stream *file = NULL, int level = Z_BEST_COMPRESSION);
extern stream *openutf8file(const char *filename, const char *mode, stream *file = NULL);
extern char *loadstream(stream *f, size_t *size, bool utf8 = true);
extern char *loadfile(const char *fn, size_t *size, bool utf8 = true);
extern bool listdir(const char *dir, bool rel, const char *ext, vector<char *> &files);
extern int listfiles(const char *dir, const char *ext, vector<char *> &files);
extern int listzipfiles(const char *dir, const char *ext, vector<char *> &files);
extern void backup(const char *fname, const char *ext, int revision = 0, int start = 1, bool store = false, bool full = true);

extern void endianswap(void *, int, int);
extern void seedMT(uint seed);
extern uint randomMT();
extern void putint(ucharbuf &p, int n);
extern void putint(packetbuf &p, int n);
extern void putint(vector<uchar> &p, int n);
extern int getint(ucharbuf &p);
extern void putuint(ucharbuf &p, int n);
extern void putuint(packetbuf &p, int n);
extern void putuint(vector<uchar> &p, int n);
extern int getuint(ucharbuf &p);
extern void putfloat(ucharbuf &p, float f);
extern void putfloat(packetbuf &p, float f);
extern void putfloat(vector<uchar> &p, float f);
extern float getfloat(ucharbuf &p);
extern void sendstring(const char *t, ucharbuf &p);
extern void sendstring(const char *t, packetbuf &p);
extern void sendstring(const char *t, vector<uchar> &p);
extern void getstring(char *t, ucharbuf &p, size_t len);
template<size_t N> static inline void getstring(char (&t)[N], ucharbuf &p) { getstring(t, p, N); }
struct ipmask
{
    enet_uint32 ip, mask;

    void parse(const char *name);
    int print(char *buf) const;
    bool check(enet_uint32 host) const { return (host & mask) == ip; }
};
extern char *cubecasestr(const char *str, const char *needle);
extern bool cubematchstr(const char *str, const char *match, bool nocase = false);
extern int cubepattern(const char *str, const char *pattern, bool nocase = false);
#endif
