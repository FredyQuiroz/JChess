#include "tablero.h"
# include <math.h>

CTablero::CTablero(void)
{
	for( int i=0; i<8; i++)
		for( int j=0; j<8; j++)

			m_nFichas[i][j]=-1;
	m_nCasillas= m_nTamano=0;
}

CTablero::~CTablero(void)
{
}

void CTablero::AddCoordenada( int x, int y) {

	int h;
	if( m_nCasillas>= CASILLAS*CASILLAS)

		return;
	h= (int) floor(m_nCasillas/8);
	m_nX[h][m_nCasillas%8]=x;
	m_nY[h][m_nCasillas%8]=y;
	m_nCasillas++;
}
int CTablero::HitTest( int x, int y, int Tolerancia) {

	int max, min, h;

	for( int i=0; i<m_nCasillas; i++) {
	
		h= (int) floor(i/8);
		max= m_nX[h][i%8]+m_nTamano+Tolerancia;
		min= m_nX[h][i%8]-Tolerancia;
		if( x<min || x>max)

			continue;
		max= m_nY[h][i%8]+m_nTamano+Tolerancia;
		min= m_nY[h][i%8]-Tolerancia;
		if( y<min || y>max)

			continue;
		return i;
	}
	return -1;
}

int CTablero::HitTest( float x, float y, int Tolerancia) {

	int max, min, h;

	for( int i=0; i<m_nCasillas; i++) {
	
		h= (int) floor(i/8);
		max= m_nX[h][i%8]+m_nTamano+Tolerancia;
		min= m_nX[h][i%8]-Tolerancia;
		if( x<min || x>max)

			continue;
		max= m_nY[h][i%8]+m_nTamano+Tolerancia;
		min= m_nY[h][i%8]-Tolerancia;
		if( y<min || y>max)

			continue;
		return i;
	}
	return -1;
}

void CTablero::SetFicha( int pos, int Ficha) {

	if( pos>= m_nCasillas || pos<0)

		return;
	if( Ficha>CABALLOR)

		return;
	int h= (int) floor(pos/8);
	m_nFichas[h][pos%8]=Ficha;
}

int CTablero::GetCoordenadaX( int pos) {

	if( pos>= m_nCasillas || pos<0)

		return -1;
	return m_nX[( int) floor( pos/8)][pos%8];
}

int CTablero::GetCoordenadaY( int pos) {

	if( pos>= m_nCasillas || pos<0)

		return -1;

	return m_nY[( int) floor( pos/8)][pos%8];
}

bool CTablero::EstaVacio() {

	for( int i=0; i<CASILLAS; i++)

		for( int j=0; j<CASILLAS; j++)

			if( m_nFichas[i][j]!=-1) 

				return false;
	return true;
}

int CTablero::CuentaCaballos() {

	int caballos=0;

	for( int i=0; i<CASILLAS; i++)

		for( int j=0; j<CASILLAS; j++)

			if( m_nFichas[i][j]== CABALLO || m_nFichas[i][j]== CABALLON) 

				caballos++;
	return caballos;
}

bool CTablero::GetCasilla( int *CasillaX, int *CasillaY,int pos) {

	if( pos>=CASILLAS*CASILLAS)

		return false;
	*CasillaX= (int) floor(pos/8);
	*CasillaY= pos%8;
	return true;
}

int CTablero::GetColor( int pos) {

	int i= (int) floor(pos/8);
	int j= pos%8;
	if( pos>=CASILLAS*CASILLAS)

		return -1;
	if( m_nFichas[i][j]==-1)

		return -1;
	if( m_nFichas[i][j]< PEONN)

		return BLANCA;
	else return NEGRA;
}

int CTablero::GetColor( int CasillaX, int CasillaY) {

	if( CasillaX>=CASILLAS || CasillaX<0 || CasillaY>=CASILLAS || CasillaY<0)

		return -1;
	if( m_nFichas[CasillaX][CasillaY]==-1)

		return -1;
	else if(m_nFichas[CasillaX][CasillaY]< PEONN)
		return BLANCA;
	else return NEGRA;
}

int CTablero::GetPosCaballo() {

	for( int i=0; i<CASILLAS; i++)

		for( int j=0; j<CASILLAS; j++)

			if( m_nFichas[i][j]== CABALLO || m_nFichas[i][j]== CABALLON)

				return (i*8)+j;
	return -1;
}