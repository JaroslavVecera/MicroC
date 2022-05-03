#ifndef  MikroC_h
#define  MikroC_h
#define _CRT_SECURE_NO_WARNINGS

struct Uzel { int Typ;
              union { struct { Uzel *prvni,*druhy,*treti,*ctvrty; } z;
                      int Cislo;
                      const char *Retez;
                      int *Adresa; } z; };

Uzel *GenUzel(int,Uzel * =0,Uzel * =0,Uzel * =0,Uzel * =0);
Uzel *GenCislo(int);
Uzel *GenRetez(const char *);
Uzel *GenPromen(const char *);

#define  TABSIZE  4

union YYSTYPE { int c; const char *r; Uzel *u; };

#define  YYSTYPE_IS_DECLARED

enum Pozice { BEZPOZICE,RADEK,SLOUPEC };

extern Uzel* Koren;

void Chyba(const char *, Pozice=SLOUPEC);

void LexInit();

void Flush();

void exec(const char*);

int Interpr(const Uzel*);

#include "mikroC.tab.hpp"

#endif
