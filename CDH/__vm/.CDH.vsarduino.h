/* 
	Editor: https://www.visualmicro.com/
			This file is for intellisense purpose only.
			Visual micro (and the arduino ide) ignore this code during compilation. This code is automatically maintained by visualmicro, manual changes to this file will be overwritten
			The contents of the _vm sub folder can be deleted prior to publishing a project
			All non-arduino files created by visual micro and all visual studio project or solution files can be freely deleted and are not required to compile a sketch (do not delete your own code!).
			Note: debugger breakpoints are stored in '.sln' or '.asln' files, knowledge of last uploaded breakpoints is stored in the upload.vmps.xml file. Both files are required to continue a previous debug session without needing to compile and upload again
	
	Hardware: COMM32(30MHz), Platform=pic32, Package=pic32
*/

#if defined(_VMICRO_INTELLISENSE)

#ifndef _VSARDUINO_H_
#define _VSARDUINO_H_
#define __PIC32_32MX795F512L__
#define __PIC32MX__
#define __32MX795F512L__
#define __LANGUAGE_C_PLUS_PLUS__
#define MPIDEVER 16778004
#define MPIDE 150
#define ARDUINO 150
#define _BOARD_MEGA_
#define VM_TOOLCHAIN_VER 10900
#define F_CPU 30000000L
#define __cplusplus 201103L
#define __inline__
#define __asm__(x)
#define __asm__(char)
#define __extension__
#define __ATTR_PURE__
#define __ATTR_CONST__
#define __inline__
#define __volatile__

typedef void *__builtin_va_list;
#define __builtin_va_start
#define __builtin_va_end
#define __DOXYGEN__
#define __attribute__(x)
#define NOINLINE __attribute__((noinline))
#define prog_void
#define PGM_VOID_P int


typedef unsigned char byte;
extern "C" void __cxa_pure_virtual() {;}

extern int ffs (int __val) __ATTR_CONST__;
extern int ffsl (long __val) __ATTR_CONST__;
extern int ffsll (long long __val) __ATTR_CONST__;
extern void *memccpy(void *, const void *, int, size_t);
extern void *memchr(const void *, int, size_t) __ATTR_PURE__;
extern int memcmp(const void *, const void *, size_t) __ATTR_PURE__;
extern void *memcpy(void *, const void *, size_t);
extern void *memmem(const void *, size_t, const void *, size_t) __ATTR_PURE__;
extern void *memmove(void *, const void *, size_t);
extern void *memrchr(const void *, int, size_t) __ATTR_PURE__;
extern void *memset(void *, int, size_t);
extern char *strcat(char *, const char *);
extern char *strchr(const char *, int) __ATTR_PURE__;
extern char *strchrnul(const char *, int) __ATTR_PURE__;
extern int strcmp(const char *, const char *) __ATTR_PURE__;
extern char *strcpy(char *, const char *);
extern int strcasecmp(const char *, const char *) __ATTR_PURE__;
extern char *strcasestr(const char *, const char *) __ATTR_PURE__;
extern size_t strcspn(const char *__s, const char *__reject) __ATTR_PURE__;
extern char *strdup(const char *s1);
extern size_t strlcat(char *, const char *, size_t);
extern size_t strlcpy(char *, const char *, size_t);
extern size_t strlen(const char *) __ATTR_PURE__;
extern char *strlwr(char *);
extern char *strncat(char *, const char *, size_t);
extern int strncmp(const char *, const char *, size_t) __ATTR_PURE__;
extern char *strncpy(char *, const char *, size_t);
extern int strncasecmp(const char *, const char *, size_t) __ATTR_PURE__;
extern size_t strnlen(const char *, size_t) __ATTR_PURE__;
extern char *strpbrk(const char *__s, const char *__accept) __ATTR_PURE__;
extern char *strrchr(const char *, int) __ATTR_PURE__;
extern char *strrev(char *);
extern char *strsep(char **, const char *);
extern size_t strspn(const char *__s, const char *__accept) __ATTR_PURE__;
extern char *strstr(const char *, const char *) __ATTR_PURE__;
extern char *strtok(char *, const char *);
extern char *strtok_r(char *, const char *, char **);
extern char *strupr(char *);

#include "arduino.h"
#include <Board_Defs.h> 
#undef cli
#define cli()
#include "CDH.ino"
#endif
#endif
