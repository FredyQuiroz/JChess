//-----------------------------------------------------------------------------
// File: JChess.h
//
// Desc: Header file JChess sample app
//-----------------------------------------------------------------------------
#pragma once

#include "JChess.h"
# include "Fichas.h"
# include "Tablero.h"
# include "Boton.h"
# include "Caballo.h"
# include "Boton3D.h"

//-----------------------------------------------------------------------------
// Name: struct ENVMAPPEDVERTEX
// Desc: D3D vertex type for environment-mapped objects
//-----------------------------------------------------------------------------
struct ENVMAPPEDVERTEX
{
    D3DXVECTOR3 p; // Position
    D3DXVECTOR3 n; // Normal

    static const DWORD FVF;
};
// CUBEMAP_RESOLUTION indicates how big to make the cubemap texture.  Larger
// textures will generate a better-looking reflection.
#define CUBEMAP_RESOLUTION 256

//-----------------------------------------------------------------------------
// Name: class CJChess3D
// Desc: Application class. The base class (CD3DApplication) provides the 
//       generic functionality needed in all Direct3D samples. CJChess3D 
//       adds functionality specific to this sample program.
//-----------------------------------------------------------------------------
class CJChess3D : public CD3DApplication
{
    BOOL m_bCapture;

    D3DXMATRIXA16 m_matProject;
    D3DXMATRIXA16 m_matView;
    D3DXMATRIXA16 m_matWorld;
    D3DXMATRIXA16 m_matTrackBall;

	CD3DFont* m_pFontB;
	// enviroment
    CD3DMesh* m_pSkyBox;
	CD3DMesh* m_pFichas[CABALLOR];
	// mesh de tablero
	CD3DMesh* m_pCasillaBlanca;
	CD3DMesh* m_pCasillaNegra;

    ID3DXEffect* m_pEffect;
    ID3DXRenderToEnvMap* m_pRenderToEnvMap;

    IDirect3DCubeTexture9* m_pCubeMap;
    IDirect3DTexture9* m_pSphereMap;

	// textura extra para el caballo
	LPDIRECT3DTEXTURE9	m_pTexture;
	// textura extra para la casilla blanca
	LPDIRECT3DTEXTURE9	m_pTexture1;
	// rotacion necesaria para enderezar las fichas
	float m_fRot;
	// distancia de la camara
	float m_fDistancia;
	CFichas m_pCFichas;
	CTablero m_pTablero;
	// arbol con las posibles jugadas del caballo
	CCaballo *m_pCaballo;

	// funciones de mouse
	bool m_bDrag;
	bool m_bBlanco;
	bool m_bEliminar;
	bool m_bCaballo;
	int m_nFicha;
	int m_nX, m_nY;
	int m_nPosTablero;
	int m_nPosAntTablero;
	// botones
	CBoton3D m_pBotones[4];

	// nivel de visualizacion
	bool m_bRotar;
	// nivel de renderizacion
	bool m_bEfFichas, m_bEfTablero, m_bWCasilla, m_bWFicha, m_bPDrag;
protected:


    HRESULT RenderSceneIntoEnvMap();
	HRESULT RenderSkybox( CONST D3DXMATRIXA16* pView, CONST D3DXMATRIXA16* pProject);
	HRESULT RenderBlack( CONST D3DXMATRIXA16* pView, CONST D3DXMATRIXA16* pProject, 
						CD3DMesh* Objeto, D3DXMATRIXA16* matWorld, bool wireframe= false, 
						bool ExtraTextura= false, bool blanca= false);
	HRESULT RenderWhite( CONST D3DXMATRIXA16* pView, CONST D3DXMATRIXA16* pProject, 
						CD3DMesh* Objeto, D3DXMATRIXA16* matWorld, bool wireframe= false);
	HRESULT RenderScene( CONST D3DXMATRIXA16* pView, CONST D3DXMATRIXA16* pProject, bool RenderBlancas= true);

    HRESULT ConfirmDevice( D3DCAPS9*, DWORD, D3DFORMAT, D3DFORMAT );
    HRESULT OneTimeSceneInit();
    HRESULT InitDeviceObjects();
    HRESULT RestoreDeviceObjects();
    HRESULT InvalidateDeviceObjects();
    HRESULT DeleteDeviceObjects();
    HRESULT Render();
    HRESULT FrameMove();
    HRESULT FinalCleanup();

private:

inline float TransX( float x) {
		
		return ((x*30)/m_d3dSettings.Fullscreen_DisplayMode.Width)-14.0f;
}
inline float TransY( float y) {

	return 12.0f-((y*22)/m_d3dSettings.Fullscreen_DisplayMode.Height);
}
void Limpiar();
void Actualizar(int xCasillaCaballo, int yCasillaCaballo);
void Jugadas( CCaballo *info, bool a[64], int profundidad); 
void MouseLButtonDown( float x, float y);
void MouseLButtonUp( float x, float y);
void MouseMove( float x, float y);
void BotonDown(int boton);

public:

	void SetParametros(  bool EFichas, bool ETablero, bool WireFrameCasilla, 
							  bool WireFrameFicha, bool drag);
    LRESULT MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
    CJChess3D();
};