#pragma once

#include "MacierzGL.h"
#include "Werteks.h"

class Aktor {
protected:
	unsigned int vao, vbo;
	unsigned int liczbaWerteksow;

private:
	void InicjujBuforWerteksow();
	void UsunBuforWerteksow();

protected:
	virtual unsigned int TworzTabliceWerteksow(Werteks*& werteksy) = 0;

public:
	Macierz4 MacierzSwiata;
	virtual void Inicjuj(GLuint atrybutPolozenie,GLuint atrybutNormalna, GLuint atrybutWspolrzedneTeksturowania,GLuint atrybutKolor);
	virtual void Rysuj();
	GLuint IndeksTekstury;
	GLuint IndeksTekstury2;

	//material
	Wektor4 MaterialSwiatloOtoczenia;
	Wektor4 MaterialSwiatloRozpraszane;
	Wektor4 MaterialSwiatloRozblysku;
	float MaterialWykladnikRozblysku;
	//dopisane
	float przezroczystosc;


	Aktor();
	virtual ~Aktor();
};

class Kwadrat : public Aktor {

private:
	float dlugoscKrawedzi;
	unsigned int TworzTabliceWerteksow(Werteks*& werteksy);

public:
	Kwadrat(GLuint atrybutPolozenie,GLuint atrybutNormalna, GLuint atrybutWspolrzedneTeksturowania,GLuint atrybuKolor, float dlugoscKrawedzi = 2.0f);
};

class Prostopadloscian : public Aktor {

private:
	float dlugoscKrawedziX, dlugoscKrawedziY, dlugoscKrawedziZ;
	bool koloruj;
	unsigned int TworzTabliceWerteksow(Werteks*& werteksy);
	int pierwszeWerteksyPodciagow[6];
	int liczbyWerteksowWPodciagach[6];

public:
	void Rysuj();
	Prostopadloscian(GLuint atrybutPolozenie,GLuint atrybutNormalna,GLuint atrybutWspolrzedneTeksturowania, GLuint atrybutKolor, float dlugoscKrawedziX, float dlugoscKrawedziY, float dlugoscKrawedziZ, bool koloruj);
	static Prostopadloscian* StworzSzescian(GLuint atrybutPolozenie,GLuint atrybutNormalna, GLuint atrybutWspolrzedneTeksturowania,GLuint atrybutKolor, float dlugoscKrawedzi, bool koloruj) {

		return new Prostopadloscian(atrybutPolozenie,atrybutNormalna, atrybutWspolrzedneTeksturowania,atrybutKolor, dlugoscKrawedzi, dlugoscKrawedzi, dlugoscKrawedzi, koloruj);
	}
};

class AktorZBuforemIndeksow : public Aktor {

protected:
	unsigned int liczbaIndeksow;
	unsigned int vbo_indeksy;

private:
	void InicjujBuforIndeksow();
	void UsunBuforIndeksow();

protected:
	virtual unsigned int TworzTabliceIndeksow(GLuint*& indeksy) = 0;

public:
	virtual void Inicjuj(GLuint atrybutPolozenie, GLuint atrybutNormalna,GLuint atrybutWspolrzedneTeksturowania,GLuint atrybutKolor);
	virtual void Rysuj();
	AktorZBuforemIndeksow();
	virtual ~AktorZBuforemIndeksow();
};

class KwadratZBuforemIndeksow : public AktorZBuforemIndeksow {

private:
	float dlugoscKrawedzi;
	unsigned int TworzTabliceWerteksow(Werteks*& werteksy);
	unsigned int TworzTabliceIndeksow(GLuint*& indeksy);

public:
	KwadratZBuforemIndeksow(GLuint atrybutPolozenie,GLuint atrybutNormalna, GLuint atrybutWspolrzedneTeksturowania,GLuint atrybutKolor, float dlugoscKrawedzi = 2.0f);
	static KwadratZBuforemIndeksow* StworzKwadratZBuforemIndeksow(GLuint atrybutPolozenie,GLuint atrybutNormalna, GLuint atrybutWspolrzedneTeksturowania,GLuint atrybutKolor, float dlugoscKrawedzi) {

		return new KwadratZBuforemIndeksow(atrybutPolozenie,atrybutNormalna, atrybutWspolrzedneTeksturowania,atrybutKolor, dlugoscKrawedzi);
	}
};

/////////////////////////
class Prostokat : public Aktor {

private:
	float dlugoscKrawedziX,dlugoscKrawedziY;
	unsigned int TworzTabliceWerteksow(Werteks*& werteksy);

public:
	Prostokat(GLuint atrybutPolozenie, GLuint atrybutNormalna, GLuint atrybutWspolrzedneTeksturowania, GLuint atrybuKolor, float dlugoscKrawedziX = 2.0f,float dlugoscKrawedziY = 2.0f);
};