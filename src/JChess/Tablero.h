#pragma once

# include "Fichas.h"
# define CASILLAS 8

enum COLOR {

	BLANCA, NEGRA
};
class CTablero
{
private:
	
	int m_nCasillas;
public:
	int m_nTamano;
	int m_nX[CASILLAS][CASILLAS], m_nY[CASILLAS][CASILLAS];
	int m_nFichas[CASILLAS][CASILLAS];
public:
	CTablero(void);
	~CTablero(void);
	int HitTest( int x, int y, int Tolerancia=0);
	int HitTest( float x, float y, int Tolerancia=0);
	void AddCoordenada( int x, int y);
	void SetFicha( int pos, int Ficha=-1);
	int GetCoordenadaX( int pos);
	int GetCoordenadaY( int pos);
	bool EstaVacio();
	int CuentaCaballos();
	int GetColor( int pos);
	int GetColor( int CasillaX, int CasillaY);
	bool GetCasilla( int *CasillaX, int *CasillaY, int pos);
	int GetPosCaballo();
};
