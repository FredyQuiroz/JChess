#pragma once
enum ESTADO {
	
	NORMAL, UP, DOWN
};
class CClip
{

public:
	int x, y, ancho, alto;
	char estado;
public:
	CClip(void);
	~CClip(void);
	bool mensaje( int x, int y, int Mensaje=UP);
	bool hitTest( int x, int y, int tolerancia=0);
};
