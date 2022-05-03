#define _CRT_SECURE_NO_WARNINGS

#include "mikroC.h"
#include <stdio.h>
#include <iostream>
#include <string>
#include <unordered_map>

using namespace std;

extern FILE* yyin;
extern unsigned Radek;
extern unsigned Sloupec;
extern unsigned Delka;
bool lex_error;
Uzel* Koren = 0;
unordered_map<string, int *> *table = nullptr;

void yyerror(char const* message) {
	std::cout << message << endl;
}

int main(int argc, char **argv) {
	if (argc != 2) {
		cout << "Zadejte cestu ke zdrojovemu souboru\n";
		return 0;
	}
	LexInit();
	exec(argv[1]);
	return 0;
}

void exec(const char *source) {
	if (!(yyin = fopen(source, "rt"))) {
		std::cout << "Nelze otevrit zdrojovy soubor.\n";
		return;
	}
	if (table)
		delete(table);
	table = new unordered_map<string, int*>();
	lex_error = false;
	Flush();
	int parse = yyparse();
	fclose(yyin);
	if (lex_error || parse)
		return;
	Interpr(Koren);
}

Uzel* GenUzel(int i, Uzel* first, Uzel* second, Uzel* third, Uzel* fourth) {
	Uzel* node = new Uzel();
	node->Typ = i;
	node->z.z.prvni = first;
	node->z.z.druhy = second;
	node->z.z.treti = third;
	node->z.z.ctvrty = fourth;
	return node;
}

Uzel* GenCislo(int i) {
	Uzel* num = new Uzel();
	num->Typ = CISLO;
	num->z.Cislo = i;
	return num;
}

Uzel* GenRetez(const char* s) {
	Uzel* str = new Uzel();
	str->Typ = RETEZ;
	char* copy = new char[string(s).length() + 1];
	strcpy(copy, s);
	str->z.Retez = copy;
	return str;
}

Uzel* GenPromen(const char* var) {
	Uzel* variable = new Uzel();
	variable->Typ = PROMENNA;
	if ((*table).find(string(var)) == (*table).end())
		(*table)[string(var)] = new int();
	variable->z.Adresa = (*table)[string(var)];
	return variable;
}

void Chyba(const char* mess, Pozice poz) {
	lex_error = true;
	string specification = "";
	if (poz != BEZPOZICE) {
		specification += " na radku ";
		specification += std::to_string(Radek);
	}
	if (poz == SLOUPEC) {
		specification += " ve sloupci ";
		specification += to_string(Sloupec);
	}
	std::cout << "Lexikalni chyba" << specification << ": " << mess << endl;
}

int Interpr(const Uzel* u) {
	if (u == NULL) 
		return 0;
	#define prvni u->z.z.prvni
	#define druhy u->z.z.druhy
	#define treti u->z.z.treti
	#define ctvrty u->z.z.ctvrty

	switch (u->Typ) {
	case 0: Interpr(prvni); Interpr(druhy);
		return 0;
	case '=': return *(prvni->z.Adresa) = Interpr(druhy);
	case P_DELEN: { int d = Interpr(druhy);
		if (d == 0) { printf("\nDeleni nulou\n"); abort(); }
		return *prvni->z.Adresa /= d; }
	case PRINT: if (prvni->Typ != RETEZ) printf("%i", Interpr(prvni));
			  else if (druhy) printf(prvni->z.Retez, Interpr(druhy));
			  else printf(prvni->z.Retez);
		return 0;
	case SCAN: {int a = 0; scanf("%i", &a); *prvni->z.Adresa = a;
		return a; }
	case FOR: Interpr(prvni);
		while (Interpr(druhy)) {
			Interpr(ctvrty);
			Interpr(treti);
		}
		return 0;
	case IF: if (Interpr(prvni)) Interpr(druhy);
		   else Interpr(treti);
		return 0;
	case INKREM: if (prvni) return ++ * prvni->z.Adresa;
		return (*druhy->z.Adresa)++;
	case '!': return !Interpr(prvni);
	case '<': return Interpr(prvni) < Interpr(druhy);
	case OR: return Interpr(prvni) || Interpr(druhy);
	case '-': if (druhy) return Interpr(prvni) - Interpr(druhy);
		return -Interpr(prvni);
	case '/': { int d = Interpr(druhy);
		if (d == 0) { printf("\nDeleni nulou\n"); abort(); }
		return Interpr(prvni) / d; }
	case '+': return Interpr(prvni) + Interpr(druhy);
	case CISLO: return u->z.Cislo;
	case RETEZ: return 0;
	case PROMENNA: return *u->z.Adresa;
	case WHILE:
		while (Interpr(prvni)) {
			Interpr(druhy);
		}
		return 0;
	case DO:
		do {
			Interpr(prvni);
		} while (Interpr(druhy));
		return 0;
	case P_OR: { int d = Interpr(druhy);
		return *prvni->z.Adresa |= d; }
	case P_XOR: { int d = Interpr(druhy);
		return *prvni->z.Adresa ^= d; }
	case P_AND: { int d = Interpr(druhy);
		return *prvni->z.Adresa &= d; }
	case P_PRICT: { int d = Interpr(druhy);
		return *prvni->z.Adresa += d; }
	case P_ODECT: { int d = Interpr(druhy);
		return *prvni->z.Adresa -= d; }
	case P_POSUNVLEVO: { int d = Interpr(druhy);
		return *prvni->z.Adresa <<= d; }
	case P_POSUNVPRAVO: { int d = Interpr(druhy);
		return *prvni->z.Adresa >>= d; }
	case P_MODUL: { int d = Interpr(druhy);
		if (d == 0) { printf("\nDeleni nulou se zbytkem\n"); abort(); }
		return *prvni->z.Adresa %= d; }
	case P_NASOB: { int d = Interpr(druhy);
		return *prvni->z.Adresa * d; }
	case ROVNO: return Interpr(prvni) == Interpr(druhy);
	case '%': { int d = Interpr(druhy);
		if (d == 0) { printf("\nDeleni nulou se zbytkem\n"); abort(); }
		return Interpr(prvni) % d; }
	case AND: return Interpr(prvni) && Interpr(druhy);
	case NENIROVNO: return Interpr(prvni) != Interpr(druhy);
	case VETSIROVNO: return Interpr(prvni) >= Interpr(druhy);
	case MENSIROVNO: return Interpr(prvni) <= Interpr(druhy);
	case POSUNVPRAVO: return Interpr(prvni) >> Interpr(druhy);
	case POSUNVLEVO: return Interpr(prvni) << Interpr(druhy);
	case '*': return Interpr(prvni) * Interpr(druhy);
	case DEKREM: if (prvni) return -- * prvni->z.Adresa;
		return (*druhy->z.Adresa)--;
	case '>': return Interpr(prvni) > Interpr(druhy);
	case '&': return Interpr(prvni) & Interpr(druhy);
	case '|': return Interpr(prvni) | Interpr(druhy);
	case '^': return Interpr(prvni) ^ Interpr(druhy);
	case '~': return ~Interpr(prvni);
	default: printf("\nNeznama operace: %i\n", u->Typ); abort();
	}
}