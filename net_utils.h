#ifndef __NET_UTILS_H__
#define __NET_UTILS_H__


#include <stdio.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

int utils_str_HexDumpStr(const char *pInData,  int iInSize, char *pOutData, int *piInOutLen);

void utils_str_Dump2File(char acInBuf[], int iInBufLen, FILE *out);

char *utils_net_Net2Addr( uint32_t dwIp);

int utils_str_Hex2Str(const char* pInBuff,  int iInLen, char* pOutBuff, int *piOutLen);

int utils_str_Str2Hex(const char* pInBuff,  int iInLen, char* pOutBuff, int *piOutLen);

int GetLocalIp(char *eth, char *ipaddr);                                


#ifdef __cplusplus
}
#endif // __cplusplus


#endif // __NET_UTILS_H__ 
