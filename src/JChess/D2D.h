#pragma once
//Numero maximo de pantallas.
#define maxpantallas 1000
//Definimos el valor solido como valor 0 
#define SOLIDO 0
//Definimos el valor transparente como valor 1.
#define TRANSPARENTE 1

//Se incluyen los ficheros fuente que son necesarios.
#include <windows.h>
#include <windowsx.h>
//Es necesario incluir las Direct-Draw.
#include <ddraw.h>

//Se incluyen otros ficheros fuente necesarios.
#include <stdlib.h>
#include <stdarg.h> 
#include <stdio.h>
#include <conio.h> 

//Estructura de datos que contiene la cabecera de un fichero grafico
//TGA que esta sin comprimir y que no usa mapa de colores.
typedef struct
{
 BYTE idsize; //Normamente 0.
 BYTE colormaptype; //Tiene que ser 0.
 BYTE image; //Tiene que ser 2.
 BYTE colormapspec[5]; //Se ignora.
 WORD Xorigin; //Origen X.
 WORD Yorigin; //Origen Y.
 WORD AX; //Ancho.
 WORD AY; //Alto.
 BYTE bitsperpixel; //Tiene que ser 24 o 32.
 BYTE imagedesc; //Se ignora.
} TGA;

//Tipo de datos que contiene un puntero a una linea de una pantalla
typedef WORD *ScreenLines;
//Estructura de datos que contiene una pantalla
typedef struct{
	WORD AX,AY; //Alto y Ancho
	ScreenLines *Lines; //Puntero a las lineas de la pantalla
} Screen;

class CD2D
{
public:
	CD2D(void);
	~CD2D(void);
protected:

    //Este es el manejador de la ventana de la aplicacion.
	HWND                hwnd;

	LPDIRECTDRAWSURFACE     lpDDSPrimary;   // FrontSurface de Direct-Draw
	LPDIRECTDRAWSURFACE     lpDDSBack;      // BackSurface de Direct-Draw
	DDSURFACEDESC           ddsd;           // Contiene la descripcion de una surface
	// resolucion de la pantalla
	DWORD xres, yres;
	//contendra los bits de rojo, verde y azul que usa la tarjeta grafica
	//en el formato de 16 bits de color.
	//Lo normal para RGB son 5-5-5 o 5-6-5.                                     
	long bitsrojo,bitsverde,bitsazul; 
	//Es un puntero a la pantalla que se esta usando actualmente
	Screen *pantallaenuso;
	//Contiene la pantalla fisica.
	Screen pantallafisica;
protected:
	void Error(char *c,char *buf);
	void NewScreenWin(Screen& SCR,WORD AX,WORD AY);
	void NewScreen(Screen& SCR,WORD AX,WORD AY);
	void ClearScreen(Screen& SCR);
	void ScreenCopy(Screen& Sc,Screen& Dt,int Xo,int Yo,int Xo1,int Yo1,
                int Xd,int Yd);
	void ScreenCopyTrans(Screen& Sc,Screen& Dt,int Xo,int Yo,int Xo1,int Yo1,
                int Xd,int Yd);
	void ScreenCopyClip(Screen& Sc,Screen& Sd,int Xo,int Yo,int Xo1,int Yo1,
                int Xd,int Yd,int Trans);
	void Box(int X,int Y,int X1,int Y1,WORD Color);
	void UpdateScreenPointers(Screen& SCR,WORD *pointer);
	void PrepareRealScreen();
	void UpdateRealScreen();
	void SetActiveScreen(Screen &s);
	void PutPixel(WORD x,WORD y,WORD color);
	WORD GetPixel(WORD x,WORD y);
	long GetBits(long n);
	void LoadTGA(char *name,long x,long y);
};
