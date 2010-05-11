#include <assert.h>
#include <stdio.h>
#include <math.h>
#include <arpa/inet.h>


// http://www.dmh2000.com/cpp/dswap.shtml
// http://www.gamedev.net/community/forums/topic.asp?topic_id=406142

int am_big_endian()
{
    long one= 1;
    return !(*((char *)(&one)));
}

unsigned long htonf(float f)
{
    assert(sizeof(float) == sizeof(unsigned long));
    return htonl(*reinterpret_cast<long *>(&f));
}

float ntohf(unsigned long l)
{
    assert(sizeof(float) == sizeof(unsigned long));
    unsigned long g = ntohl(l);
    return *reinterpret_cast<float *> (&g);
}

unsigned long long htond(double d)
{
    assert(sizeof(double) == sizeof(unsigned long long));
    if (am_big_endian()) return *reinterpret_cast<unsigned long long *> (&d);

    unsigned long long a;
    unsigned char *dst = (unsigned char *)&a;
    unsigned char *src = (unsigned char *)&d;

    dst[0] = src[7];
    dst[1] = src[6];
    dst[2] = src[5];
    dst[3] = src[4];
    dst[4] = src[3];
    dst[5] = src[2];
    dst[6] = src[1];
    dst[7] = src[0];

    return a;
}

double ntohd(unsigned long long a) 
{
    assert(sizeof(double) == sizeof(unsigned long long));
    if (am_big_endian()) return *reinterpret_cast<double *> (&a);

    double d;
    assert(sizeof(d) == sizeof(a));
    unsigned char *src = (unsigned char *)&a;
    unsigned char *dst = (unsigned char *)&d;

    dst[0] = src[7];
    dst[1] = src[6];
    dst[2] = src[5];
    dst[3] = src[4];
    dst[4] = src[3];
    dst[5] = src[2];
    dst[6] = src[1];
    dst[7] = src[0];

    return d;
}

int main()
{
    double d = M_PI;
    printf("              d: %.50f\n", d);
    printf("       htond(d): %d\n", htond(d));
    printf("ntohd(htond(d)): %.50f\n", ntohd(htond(d)));

    float f = M_PI;
    printf("              f: %.50f\n", f);
    printf("       htonf(f): %ld\n", htonf(f));
    printf("ntohf(htonf(f)): %.50f\n", ntohf(htonf(f)));

    return 0;
}
