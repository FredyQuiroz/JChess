#pragma once

# define NOFICHAS 100
# define MOUSEUP 0
# define MOUSEDOWN 1
# define MOUSEMOVE 2

enum _FICHAS {

	PEON, TORRE, CABALLO, ALFIL, DAMA, REY, PEONN, TORREN, CABALLON, ALFILN, DAMAN, REYN, CABALLOR
};

class CFichas
{
public:

	bool m_bDrag;
	int m_nFichaDrag;
	char m_nFigura[NOFICHAS];
	float m_nX[NOFICHAS], m_nY[NOFICHAS];
	int m_nNumero;
	float m_nZ[NOFICHAS];
	bool m_bDragable[NOFICHAS], m_bSeleccion[NOFICHAS];
private:

	int m_nSizeX, m_nSizeY;
public:

	CFichas(void);
	~CFichas(void);
	void AddFicha( char Figura, float x, float y, bool seleccion= false, bool dragable= true);
	void AddFicha( char Figura, float x, float y, float z, bool seleccion= false, bool dragable= true);
	void SetCoordenada( float x, float y, int indice);
	void SetSize( int x, int y);
	int HitTest( float x, float y, int Tolerancia=0);
	int Buscar( float x, float y);
	void Eliminar( int indice);
	bool Mensaje( int x, int y, int Mensaje=MOUSEMOVE);
};
