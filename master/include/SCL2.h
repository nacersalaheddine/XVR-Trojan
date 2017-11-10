#ifndef __SCL2_H
#define __SCL2_H

#define SCL2_DEF_KEY_LEN 10
#define SCL2_DEF_KEY_KEY "\x7D\x50\x72\x4F\x4C\x8E\x52\x19\xAD\xC1"
#define SCL2_DEF_KEY_SALT "\xF1\x39\xA8\x0C\xC2\x63\x57\xE9\xBC\x14"

typedef struct _scl_info_t
{
	int size;
	unsigned int seedRestore;
	unsigned int seed;
	unsigned int upSeed;
	unsigned int lastRand;
	unsigned int upMax;
	unsigned int ranHelper;
	unsigned int randSalt; 
}_scl_info;

typedef struct _scl_key_t
{
	int size;
	unsigned int times; //how many times to encrypt
	unsigned int keyLen; //salt & key len == equ
	unsigned char* key;
	unsigned char* salt;
}_scl_key;

extern void SCL2_Init(_scl_info sclinfo, _scl_key** sclkey);
extern void SCL2_Destroy(void);
extern void SCL2_PrintInfo(void);
extern void SCL2_Reset(void);
extern void SCL2_SeedUp(void);
extern void SCL2_Encrypt(unsigned char** data, unsigned int len);
extern void SCL2_Decrypt(unsigned char** data, unsigned int len);

#endif