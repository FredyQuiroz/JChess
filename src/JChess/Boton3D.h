#pragma once
# include "Clip.h"
#include "D3DUtil.h"
#include "DXUtil.h"
#include "D3DFont.h"

class CBoton3D: public CClip
{

public:
	
	char m_szTexto[255];
public:
	CBoton3D(void);
	~CBoton3D(void);
};
