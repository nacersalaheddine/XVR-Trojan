#ifndef __GEOIP_H
#define __GEOIP_H

#define GEOIP_NO_ERROR 0
#define GEOIP_ERROR_NO_WEB 1
#define GEOIP_ERROR_EMPTY_IP 2
#define GEOIP_ERROR_SOCKET 3
#define GEOIP_ERROR_CONNECT 4
#define GEOIP_ERROR_SEND 5
#define GEOIP_ERROR_RECV 6
#define GEOIP_ERROR_FAILED 7

#define GEOIP_BUFFER 2048
#define GEOIP_IP "freegeoip.net"
#define GEOIP_PORT 80
#define GEOIP_HOST "www.freegeoip.net"
#define GEOIP_CMD_1 "GET /xml/"
#define GEOIP_CMD_2 " HTTP/1.1\r\nHost: "
#define GEOIP_CMD_3 "\r\nConnection: keep-alive\r\n\r\n"
#define GEOIP_COUNTRYNAME "<CountryName>"
#define GEOIP_COUNTRYNAME_LEN 13

extern int geoIP_IsInUse;
extern int geoIP_Init(void);
extern int geoIP_CheckIP(char* ip, char** country);

#endif