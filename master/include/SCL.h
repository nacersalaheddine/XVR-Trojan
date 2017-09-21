#ifndef __SCL_H
#define __SCL_H

/*
i made the seeds to be unsigned long, so the value must be between 0x1 and 0xFFF FFF
*/
#define SCL_SEED 0x8EDE78
#define SCL_UP_SEED 0x4F4ABF
#define SCL_UP_SEED_MAX_VALUE 10

#define SCL_KEY ((unsigned char*)"\x18\xC0\xBB\xF5\x7A\x73\xA9\xAB\xFC\xA9")
#define SCL_SALT ((unsigned char*)"\x21\x23\xA3\x4C\x73\x81\xDD\xB5\x72\x54")
#define SCL_KEY_LEN 10

//just add a little bit of salt!
#define SCL_KEY_SALT_ENCRYPT(C, K, S) (C ^ K) ^ S //first key then salt
#define SCL_KEY_SALT_DECRYPT(C, K, S) (C ^ S) ^ K //first salt then key

extern void SCL_SeedUp(void);
extern void SCL_ResetSeed(void);
extern void SCL_PrintInfo(void);
extern void SCL_Encrypt(unsigned char** msg, int len);
extern void SCL_Decrypt(unsigned char**, int len);

#endif