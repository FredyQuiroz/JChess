#include "StdAfx.h"
#include "caballo.h"

CCaballo::CCaballo( int x, int y) {

	m_nPosX= x;
	m_nPosY= y;
	for( int i=0; i<8; i++)

		sig[i]= NULL;
}

CCaballo::~CCaballo(void)
{
	for( int i=0; i<8; i++)

		if( sig[i])

			delete sig[i];
}

void CCaballo::Calcular( int profundidad) {

	int x, y;
	if( profundidad==0)

		return;
	for( int i=0; i<8; i++) {

		x= m_nPosX;
		y= m_nPosY;
		if( SigPos(i, &x, &y)) {
			
			if( !sig[i])

				sig[i]= new CCaballo(x, y);
			else sig[i]->SetXY(x, y);
			sig[i]->Calcular(profundidad-1);
		}
	}
}

bool CCaballo::SigPos( int paso, int *x, int *y) {

	switch( paso) {

		case 0:
			(*x)+=2;
			(*y)++;
			break;
		case 1:
			(*x)+=2;
			(*y)--;
			break;
		case 2:
			(*x)-=2;
			(*y)--;
			break;
		case 3:
			(*x)-=2;
			(*y)++;
			break;
		case 4:
			(*x)++;
			(*y)+=2;
			break;
		case 5:
			(*x)--;
			(*y)+=2;
			break;
		case 6:
			(*x)--;
			(*y)-=2;
			break;
		case 7:
			(*x)++;
			(*y)-=2;
			break;
	}	
	return EsValido(*x, *y);
}

void CCaballo::SetXY( int x, int y) {

	m_nPosX= x;
	m_nPosY= y;
}

int CCaballo::GetX() {

	return m_nPosX;
}

int CCaballo::GetY() {

	return m_nPosY;
}