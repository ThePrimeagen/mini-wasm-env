#pragma once

// TODO: IMPLEMENT printf?
// TODO
extern int printf(const char*, ...);
extern void println(const void*);
extern void printi(int);
extern void printci(const char*, int);
extern void printPtr(const char* name, const void* ptr);
extern void printRegion(const char* name, const void* ptr, int length);

#ifndef bool
#define bool unsigned char
#endif

