#ifndef _CONFIG_H
#define _CONFIG_H
#include "Utils.h"

typedef size_t Cfg_Stream_t(void);

typedef struct Cfg_Variable
{
	const char rom *name;
	void (*type)(const rom struct Cfg_Variable *var, bool parse);
	void *value;
	unsigned char length;
}
Cfg_Variable;

extern char *Cfg_BufPtr;
extern char *Cfg_BufEnd;

signed char Cfg_StringCompare(const char *p1, const char rom *p2);
int Cfg_ParseAll(const rom Cfg_Variable *set, Cfg_Stream_t *stream);
bool Cfg_ParseOne(const rom Cfg_Variable *set, const char *name, Cfg_Stream_t *stream);
bool Cfg_SaveAll(const rom Cfg_Variable *set, Cfg_Stream_t *stream);
bool Cfg_SaveOne(const rom Cfg_Variable *set, const char *name, Cfg_Stream_t *stream);
int Cfg_ParseFile(const rom Cfg_Variable *set, const char rom *name);
bool Cfg_SaveFile(const rom Cfg_Variable *set, const char rom *name);
int Cfg_ParseFlash(const rom Cfg_Variable *set);
bool Cfg_SaveFlash(const rom Cfg_Variable *set);

void Cfg_FormatString(const rom Cfg_Variable *var, bool parse);
void Cfg_FormatInt(const rom Cfg_Variable *var, bool parse);
void Cfg_FormatUInt(const rom Cfg_Variable *var, bool parse);
void Cfg_FormatIP(const rom Cfg_Variable *var, bool parse);
void Cfg_FormatMAC(const rom Cfg_Variable *var, bool parse);
extern void Cfg_CheckString(char *);
extern void Cfg_CheckInt8(int8_t *);
extern void Cfg_CheckInt16(int16_t *);
extern void Cfg_CheckInt24(int24_t  *);
extern void Cfg_CheckInt32(int32_t  *);
extern void Cfg_CheckUInt8(uint8_t *);
extern void Cfg_CheckUInt16(uint16_t *);
extern void Cfg_CheckUInt24(uint24_t  *);
extern void Cfg_CheckUInt32(uint32_t  *);
extern void Cfg_CheckIP(IP_ADDR *);
extern void Cfg_CheckMAC(MAC_ADDR *);

#define CFG_STRING(id, var)                                                     \
{ (id), Cfg_FormatString, (sizeof Cfg_CheckString((var)),   (var)), sizeof(var) }
#define CFG_INT8(id, var)                                                       \
{ (id), Cfg_FormatInt,    (sizeof Cfg_CheckInt8(&(var)),   &(var)), sizeof(var) }
#define CFG_INT16(id, var)                                                      \
{ (id), Cfg_FormatInt,    (sizeof Cfg_CheckInt16(&(var)),  &(var)), sizeof(var) }
#define CFG_INT24(id, var)                                                      \
{ (id), Cfg_FormatInt,    (sizeof Cfg_CheckInt24(&(var)),  &(var)), sizeof(var) }
#define CFG_INT32(id, var)                                                      \
{ (id), Cfg_FormatInt,    (sizeof Cfg_CheckInt32(&(var)),  &(var)), sizeof(var) }
#define CFG_UINT8(id, var)                                                      \
{ (id), Cfg_FormatUInt,   (sizeof Cfg_CheckUInt8(&(var)),  &(var)), sizeof(var) }
#define CFG_UINT16(id, var)                                                     \
{ (id), Cfg_FormatUInt,   (sizeof Cfg_CheckUInt16(&(var)), &(var)), sizeof(var) }
#define CFG_UINT24(id, var)                                                     \
{ (id), Cfg_FormatUInt,   (sizeof Cfg_CheckUInt24(&(var)), &(var)), sizeof(var) }
#define CFG_UINT32(id, var)                                                     \
{ (id), Cfg_FormatUInt,   (sizeof Cfg_CheckUInt32(&(var)), &(var)), sizeof(var) }
#define CFG_IP(id, var)                                                         \
{ (id), Cfg_FormatIP,     (sizeof Cfg_CheckIP(&(var)),     &(var))              }
#define CFG_MAC(id, var)                                                        \
{ (id), Cfg_FormatMAC,    (sizeof Cfg_CheckMAC(&(var)),    &(var))              }
#define CFG_END()                                                               \
{ NULL                                                                          }

#endif