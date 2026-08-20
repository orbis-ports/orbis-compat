#include "orbis_netlog.h"

#include <orbis/Net.h>

#include <cstdarg>
#include <cstdint>
#include <cstdio>
#include <cstring>

#ifndef NETLOG_HOST
#define NETLOG_HOST "192.168.100.1"
#endif
#ifndef NETLOG_PORT
#define NETLOG_PORT 18194
#endif
#ifndef NETLOG_TAG
#define NETLOG_TAG "tempest"
#endif

// The SDK ships the generic OrbisNetSockaddr (len, family, sa_data[14]) but no
// OrbisNetSockaddrIn; the AF_INET layout fits inside sa_data, so build it here.
struct NetlogSin {
  uint8_t  sin_len;
  uint8_t  sin_family;
  uint16_t sin_port;   // network order
  uint32_t sin_addr;   // network order
  uint8_t  sin_zero[8];
  };

static OrbisNetId s_sock = -1;
static NetlogSin  s_dst;

void orbis_netlog_init(void) {
  if(s_sock>=0)
    return;

  sceNetInit();

  s_sock = sceNetSocket(NETLOG_TAG "-log", ORBIS_NET_AF_INET, ORBIS_NET_SOCK_DGRAM, 0);
  if(s_sock<0) {
    s_sock = -1;
    return;
    }

  memset(&s_dst,0,sizeof(s_dst));
  s_dst.sin_len    = sizeof(s_dst);
  s_dst.sin_family = ORBIS_NET_AF_INET;
  s_dst.sin_port   = sceNetHtons(NETLOG_PORT);
  if(sceNetInetPton(ORBIS_NET_AF_INET,NETLOG_HOST,&s_dst.sin_addr)!=1) {
    sceNetSocketClose(s_sock);
    s_sock = -1;
    return;
    }

  orbis_netlog("[" NETLOG_TAG "] netlog up -> %s:%d\n",NETLOG_HOST,NETLOG_PORT);
  }

void orbis_netlog(const char* fmt, ...) {
  if(s_sock<0)
    return;

  char    buf[512];
  va_list ap;
  va_start(ap,fmt);
  int n = vsnprintf(buf,sizeof(buf),fmt,ap);
  va_end(ap);
  if(n<0)
    return;
  if(n>int(sizeof(buf)))
    n = int(sizeof(buf));

  sceNetSendto(s_sock,buf,size_t(n),0,
               reinterpret_cast<const OrbisNetSockaddr*>(&s_dst),sizeof(s_dst));
  }

int orbis_netlog_ready(void) {
  return s_sock>=0;
  }

void orbis_netlog_close(void) {
  if(s_sock<0)
    return;
  sceNetSocketClose(s_sock);
  s_sock = -1;
  }
