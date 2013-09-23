#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <stdint.h>
#include <sys/socket.h>
#include <string.h>
#include <time.h>

#include "net_utils.h"

#define COL_TITLE "______00_01_02_03_04_05_06_07_08_09_0A_0B_0C_0D_0E_0F\n"
int utils_str_HexDumpStr(const char *pInData,  int iInSize, char *pOutData, int *piInOutLen)
{
	if (NULL==pInData || NULL==pOutData || NULL==piInOutLen) return -1;
	int i, x;
	int r = 1, offset = 0, iTmpLen=0;
	char d[32];
	unsigned char acVisible[64];

	for(i = 0, x = 0;; i++)
	{
		if(i < iInSize)
		{
			if(x == 0) // row title 0x0000: 
			{
				iTmpLen = sprintf((char *) &(pOutData[offset]), "%04lx: ", i);
				offset += iTmpLen;
			}
			// hex content to str
			snprintf(d, 9, "%08x", pInData[i]);
			iTmpLen      = sprintf((char *) &(pOutData[offset]), "%c%c ", d[6], d[7]);
			offset      += iTmpLen;
			acVisible[x] = pInData[i];
			if(acVisible[x] < 0x20)  acVisible[x] = '.';
			if(acVisible[x] > 0x7f)  acVisible[x] = '.';
		}
		else
		{
			if(x == 0) 	break;
			else // add blank
			{
				iTmpLen = sprintf((char *) &(pOutData[offset]), "   ");
				offset += iTmpLen;
				acVisible[x] = ' ';
				r = 0;
			}
		}
		x++;
		if(!(x < 16)) // one line 
		{
			acVisible[x] = 0;
			iTmpLen = sprintf((char *) &(pOutData[offset]), "| %s\n", acVisible);
			offset += iTmpLen;
			x = 0;
			if(!r) 	break;
		}
	}
	*piInOutLen = offset;
	return 0;
}

void utils_str_Dump2File(char acInBuf[], int iInBufLen, FILE *out){

	static char acOutBuf[15000];
	int iOutBufLen = 0;
	if (out == NULL) out = stdout;
	if (iInBufLen > sizeof(acOutBuf)) iInBufLen = 15000;

	fprintf(out, "BUFFLEN=%d\n%s", iInBufLen, COL_TITLE);
	iOutBufLen = sizeof(acOutBuf);
	utils_str_HexDumpStr(acInBuf, iInBufLen, acOutBuf, &iOutBufLen);
	fprintf(out, "%s\n", acOutBuf);

}

//net order (int)ip to string ip
char *utils_net_Net2Addr( uint32_t dwIp)
{
	struct in_addr stAddr;
	stAddr.s_addr = dwIp;

	return (inet_ntoa(stAddr));
}

int utils_str_Hex2Str(const char* pInBuff,  int iInLen, char* pOutBuff, int *piOutLen)
{
	if (NULL==pInBuff || NULL==pOutBuff || NULL==piOutLen) return -1;
	if (0 == iInLen) return 0;
	if (2*iInLen+1 > *piOutLen) return -2;

	int i, j = 0;
	char cTmp;
	for (i = 0; i < iInLen; i++)
	{
		cTmp = (pInBuff[i] & 0xf0) >> 4;
		if (cTmp < 10)
			pOutBuff[j++] = cTmp + '0';
		else
			pOutBuff[j++] = cTmp - 10 + 'A';
		cTmp = pInBuff[i] & 0x0f;
		if (cTmp < 10)
			pOutBuff[j++] = cTmp + '0';
		else
			pOutBuff[j++] = cTmp - 10 + 'A';
	}
	pOutBuff[j] = 0;
	*piOutLen = j;

	return 0;
}

int utils_str_Str2Hex(const char* pInBuff,  int iInLen, char* pOutBuff, int *piOutLen)
{
	if (NULL==pInBuff || NULL==pOutBuff || NULL==piOutLen) return -1;
	if ((iInLen/2>*piOutLen) || (iInLen%2!=0)) return -2;
	if (0 == iInLen) {
		*piOutLen = 0;
		return 0;
	}

	int  i = 0, j=0;
	char  acTmpHex[4] = {0};
	char* pCur= (char*)pInBuff;

	for (i=0; i<iInLen; i=i + 2)
	{
		strncpy(acTmpHex, pCur+i, 2);
		*(pOutBuff+j) = strtoul((const char *)acTmpHex, NULL, 16);
		j++;
	}
	*piOutLen = j;
	return 0;
}

 int GetLocalIp(char *eth, char *ipaddr)                                
{                                                                  

	int sock_get_ip;                                               

	struct   sockaddr_in *sin;                                     
	struct   ifreq ifr_ip;                                         

	if ((sock_get_ip=socket(AF_INET, SOCK_STREAM, 0)) == -1)       
	{                                                              
		return -__LINE__;                                          
	}                                                              
	memset(&ifr_ip, 0, sizeof(ifr_ip));                            
	strncpy(ifr_ip.ifr_name, eth, sizeof(ifr_ip.ifr_name) - 1); 
	int iRet = 0;                                                  
	if( (iRet=ioctl( sock_get_ip, SIOCGIFADDR, &ifr_ip)) < 0 )     
	{                                                              
		return -__LINE__;                                          
	}                                                              
	sin = (struct sockaddr_in *)&ifr_ip.ifr_addr;                  
	strcpy(ipaddr,inet_ntoa(sin->sin_addr));                       

	close( sock_get_ip );                                          

	return 0;                                                      
}                                                                  




#if 0 

int main(){

	char acOutBuff[256];
	int iLen = 0;

	printf("ip=%s\n", utils_net_Net2Addr(12345678));

	iLen = sizeof(acOutBuff);
/*
	utils_str_Dump2File(utils_net_Net2Addr(12345678), strlen(utils_net_Net2Addr(12345678)), NULL);
	utils_str_Hex2Str(utils_net_Net2Addr(12345678), strlen(utils_net_Net2Addr(12345678)), acOutBuff, &iLen);
*/
	char acTmpHex[1024];
	int iTmpLen = sizeof(acTmpHex);
	//utils_str_Hex2Str(acOutBuff, sizeof(acOutBuff)/2, acTmpHex, &iTmpLen);
	utils_str_Dump2File(acOutBuff, sizeof(acOutBuff), NULL);

	utils_str_Hex2Str(acOutBuff, iLen, acTmpHex, &iTmpLen);
//	utils_str_Dump2File(acTmpHex, iTmpLen, NULL);
	printf("str=%s\n", acTmpHex);

	return 0;

}



#endif 
