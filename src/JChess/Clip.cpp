#include "clip.h"

CClip::CClip(void)
{
	estado= NORMAL;
	y= x =ancho= alto= 0;
}

CClip::~CClip(void)
{
}


bool CClip::hitTest( int x1, int y1, int tolerancia) {

	int max= x+tolerancia+ ancho;
	int min= x-tolerancia;
	if( x1>max || x1<min)

		return false;
	max= y+tolerancia+ alto;
	min= y-tolerancia;
	if( y1>max || y1<min)

		return false;
	return true;
}

bool CClip::mensaje( int x, int y, int Mensaje) {

	if( hitTest( x, y)) {

		if( Mensaje== DOWN)

			if( estado==DOWN)

				estado =NORMAL;
			else estado = DOWN;
		if( Mensaje==UP) 

			if( estado!= DOWN)
				
				estado= Mensaje;
		return true;
	}
	if( estado!=DOWN) 
		
		estado= NORMAL;
	return false;
}