// Prueba.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
# include "Caballo.h"

void recojerDatos( CCaballo *info, char a[8][8], int profundidad) {

	char temp[5];

	if(!info || profundidad==0) return;

	for( int i=0; i<8; i++) 

		if( info->sig[i]) {

			sprintf(temp, "%d", profundidad);
			a[info->sig[i]->GetX()][info->sig[i]->GetY()]=temp[0];
			recojerDatos( info->sig[i], a, profundidad-1);
		}
}
int _tmain(int argc, _TCHAR* argv[])
{
	int i, j, h, q, r;
	char b[8][8];
	CCaballo *a;

	for( q=0; q<8; q++) 

		for( r=0; r<8; r++)

			b[q][r]='.';


	printf( "introduzca las coodenadas iniciales x y y del caballo: ");
	scanf("%d %d", &i, &j);
	printf( "introduzca el nivel de profundidad de las jugadas: ");
	scanf("%d", &h);
	a= new CCaballo( i, j);
	a->Calcular(h);
	// imprime el tablero
	getchar();
	for( int p=1; p<=h; p++) {
	
		recojerDatos( a, b, p);
		b[a->GetX()][a->GetY()]= 'K';
			for( i=0; i<8; i++) {

				for( j=0; j<8; j++)

				printf("%c", b[i][j]);
			printf("\n");
		}
		puts("presione enter para ver el siguiente nivel");
		getchar();
		for( q=0; q<8; q++) 

			for( r=0; r<8; r++)

				b[q][r]='.';
	}
	delete a;
	return 0;
}

