#include "fichas.h"

CFichas::CFichas(void)
{
	m_nSizeX= m_nSizeY= m_nNumero= 0;
	m_bDrag= false;
	m_nFichaDrag= -1;
}

CFichas::~CFichas(void)
{
}
const float zpos[] ={ -1.2f, -1.7f, -1.6f, -1.9f, -4.0f, -2.3f, -1.2f, -1.7f, -1.6f, -1.9f, -4.0f, -2.3f, 0.0f};
void CFichas::AddFicha( char Figura, float x, float y, bool seleccion, bool dragable) {

	if( m_nNumero>=NOFICHAS-1)

		return;
	if( Figura>CABALLOR)

		return;
	m_nFigura[m_nNumero]= Figura;
	m_nX[m_nNumero]= x;
	m_nY[m_nNumero]= y;
	m_nZ[m_nNumero]= zpos[Figura];
	m_bDragable[m_nNumero]= dragable;
	m_bSeleccion[m_nNumero]= seleccion;
	m_nNumero++;
}

void CFichas::AddFicha( char Figura, float x, float y, float z, bool seleccion, bool dragable) {

	if( m_nNumero>=NOFICHAS-1)

		return;
	if( Figura>CABALLOR)

		return;
	AddFicha( Figura, x, y, seleccion, dragable);
	m_nZ[m_nNumero-1]= z;
}

void CFichas::SetCoordenada( float x, float y, int indice) {

	if( indice>= m_nNumero)
		
		return;
	m_nX[indice]= x;
	m_nY[indice]= y;
}

void CFichas::SetSize( int x, int y) {

	m_nSizeX= x;
	m_nSizeY= y;
}

int CFichas::Buscar(float x, float y) {

	for( int i=0; i<m_nNumero; i++) 

		if( m_nX[i]==x && m_nY[i]== y)

			return i;
	return -1;
}
int CFichas::HitTest( float x, float y, int Tolerancia) {

	float max, min;
	for( int i=0; i<m_nNumero; i++) {
		
		max= m_nX[i]+m_nSizeX+Tolerancia;
		min= m_nX[i]-Tolerancia;
		if( x<min || x>max)

			continue;
		max= m_nY[i]+m_nSizeY+Tolerancia;
		min= m_nY[i]-Tolerancia;
		if( y<min || y>max)

			continue;
		return i;
	}
	return -1;
}

void CFichas::Eliminar( int indice) {

	if(indice<0 || indice>=m_nNumero)

		return;
	m_nNumero--;
	for( int i= indice; i<m_nNumero; i++) {

		m_nFigura[i]= m_nFigura[i+1];
		m_nX[i]= m_nX[i+1];
		m_nY[i]= m_nY[i+1];
		m_bDragable[i]= m_bDragable[i+1];
		m_bSeleccion[i]= m_bSeleccion[i+1];
	}
}

bool CFichas::Mensaje( int x, int y, int Mensaje) {

	if( !m_bDrag && Mensaje==MOUSEMOVE) 

		return false;
	if( m_bDrag) {

		if( Mensaje==MOUSEMOVE) {

			m_nX[m_nFichaDrag]= x;
			m_nY[m_nFichaDrag]= y;
		} else m_bDrag= false;
	}
	m_nFichaDrag= HitTest( x, y, -15);
	if( m_nFichaDrag!=-1) 
		
		m_bDrag= true;
	return false;
}