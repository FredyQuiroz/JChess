#pragma once

# include "D2D.h"
# include "Fichas.h"
# include "Tablero.h"
# include "Boton.h"
# include "Caballo.h"

//Se definen algunos datos generales como el nombre del programa. 
#define NAME "JChess"
#define TITLE "JChess"

#define SAFEDELETE(p) if(p) { 	delete p;	p=NULL; }

class JChess2D : public CD2D
{

private:
	
	//Estas estructuras de datos son necesarias para inicializar
	//y utilizar Direct-Draw:
LPDIRECTDRAW            lpDD;           // El objeto Direct-Draw
BOOL                    bActive;        // almacena si la aplicacion esta activa
DDPIXELFORMAT           PixelFormat;    //Es usado para almacenar el formato
                                        //de pixel que usa la tarjeta grafica

//Espacio reservado para las pantallas que se van a usar.
CFichas m_pFichas;
CTablero m_pTablero;
Screen m_pPFichas[CABALLOR+1];
Screen pantalla;
// posicion inicial de la ficha
int m_nX, m_nY;
// manejo del mouse
Screen m_pCursor;
int m_mouseX, m_mouseY;
bool m_bDrag;
int m_nFicha;
int m_nPosTablero;
CBoton m_pBotones[4];
// manejo de botones
bool m_bEliminar, m_bCaballo;
// arbol con las posibles jugadas del caballo
CCaballo *m_pCaballo;
int m_nCaballo;
// color del caballo seleccionado 
bool m_bBlanco;
// funciones 
private:
	void finiObjects( void );
	void GameInit();
	void game(void);
	void LoadFichas();
	BOOL doInit( HINSTANCE hInstance, int nCmdShow );
	void LoadBotones();
	// funciones de boton
	// funcion principal que atiende a los botones
	void BotonDown(int boton);
	void Limpiar();
	void Actualizar(int xCasillaCaballo, int yCasillaCaballo);
	void Jugadas( CCaballo *info, bool a[64], int profundidad); 
public:
	// funcion principal
	int PASCAL Run( HINSTANCE hInstance, HINSTANCE hPrevInstance,
				   LPSTR lpCmdLine, int nCmdShow);
	virtual LRESULT MsgProc( HWND hWnd, UINT message, 
                            WPARAM wParam, LPARAM lParam );
public:
	JChess2D(void);
	~JChess2D(void);
};
