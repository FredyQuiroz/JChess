#pragma once	
# include <stdio.h>

class CCaballo
{
	int m_nPosX, m_nPosY;

public:
	class CCaballo *sig[8];
public:
	void SetXY( int x, int y);
	int GetX();
	int GetY();
	void Calcular( int profundidad=1);
	CCaballo( int x, int y);
	~CCaballo(void);
private:
	bool SigPos( int paso, int *x, int *y);
	inline bool EsValido( int x, int y) {
		if( x>=0 && x<8 && y>=0 && y<8)
			return true;
		return false;
	}
};
