#include "jchess3d.h"

//-----------------------------------------------------------------------------
// Name: g_szEffect
// Desc: String containing effect used to render shiny teapot.  Two techniques
//       are shown.. one which simply uses cubemaps, and a fallback which uses 
//       a vertex shader to do a sphere-map lookup.
//-----------------------------------------------------------------------------

const char g_szEffect[] = 

    "textureCUBE texCubeMap;\n"
    "texture texSphereMap;\n"

    "matrix matWorld;\n"
    "matrix matView;\n"
    "matrix matProject;\n"
    "matrix matWorldView;\n"


    "technique Cube\n"
    "{\n"
        "pass P0\n"
        "{\n"
            // Vertex state
            "VertexShader = null;\n"
            "WorldTransform[0] = <matWorld>;\n"
            "ViewTransform = <matView>;\n"
            "ProjectionTransform = <matProject>;\n"

            // Pixel state
            "Texture[0] = <texCubeMap>;\n"
            
            "MinFilter[0] = Linear;\n"
            "MagFilter[0] = Linear;\n"

            "AddressU[0] = Clamp;\n"
            "AddressV[0] = Clamp;\n"
            "AddressW[0] = Clamp;\n"

            "ColorOp[0] = SelectArg1;\n"
            "ColorArg1[0] = Texture;\n"

            "TexCoordIndex[0] = CameraSpaceReflectionVector;\n"
            "TextureTransformFlags[0] = Count3;\n"
        "}\n"
    "}\n"


    "technique Sphere\n"
    "{\n"
        "pass P0\n"
        "{\n"

            // Vertex state
            "VertexShader =\n"
                "decl\n"
                "{\n"
                    // Decls no longer associated with vertex shaders in DX9
                "}\n"               
                "asm\n"
                "{\n"
                    "vs.1.1\n"
                    "def c64, 0.25f, 0.5f, 1.0f, -1.0f\n"

                    "dcl_position v0\n"
                    "dcl_normal v1\n"
        
                    // r0: camera-space position
                    // r1: camera-space normal
                    // r2: camera-space vertex-eye vector
                    // r3: camera-space reflection vector
                    // r4: texture coordinates

                    // Transform position and normal into camera-space
                    "m4x4 r0, v0, c0\n"
                    "m3x3 r1.xyz, v1, c0\n"
                    "mov r1.w, c64.z\n"

                    // Compute normalized view vector
                    "mov r2, -r0\n"
                    "dp3 r3, r2, r2\n"
                    "rsq r3, r3.w\n"
                    "mul r2, r2, r3\n"

                    // Compute camera-space reflection vector
                    "dp3 r3, r1, r2\n"
                    "mul r1, r1, r3\n"
                    "add r1, r1, r1\n"
                    "add r3, r1, -r2\n"

                    // Compute sphere-map texture coords
                    "mad r4.w, -r3.z, c64.y, c64.y\n"
                    "rsq r4, r4.w\n"
                    "mul r4, r3, r4\n"
                    "mad r4, r4, c64.x, c64.y\n"

                    // Project position
                    "m4x4 oPos, r0, c4\n"
                    "mul oT0.xy, r4.xy, c64.zw\n"
                    "mov oT0.zw, c64.z\n"
                "};\n"

            "VertexShaderConstant4[0] = <matWorldView>;\n"
            "VertexShaderConstant4[4] = <matProject>;\n"

            // Pixel state
            "Texture[0] = <texSphereMap>;\n"
            "AddressU[0] = Wrap;\n"
            "AddressV[0] = Wrap;\n"
            "MinFilter[0] = Linear;\n"
            "MagFilter[0] = Linear;\n"
            "ColorOp[0] = SelectArg1;\n"
            "ColorArg1[0] = Texture;\n"
        "}\n"
    "}\n";

const UINT g_cchEffect = sizeof(g_szEffect) - 1;

const DWORD ENVMAPPEDVERTEX::FVF = D3DFVF_XYZ | D3DFVF_NORMAL;
inline bool EsBlanca( int Ficha) {

	if( Ficha<PEONN) return true;
	else return false;
}
//-----------------------------------------------------------------------------
// Name: CJChess3D()
// Desc: Application constructor. Sets attributes for the app.
//-----------------------------------------------------------------------------
CJChess3D::CJChess3D()
{
    m_strWindowTitle    = _T("JChess");
    m_d3dEnumeration.AppUsesDepthBuffer = TRUE;
    m_bCapture          = FALSE;

	m_pFontB            = NULL;
    m_pSkyBox           = NULL;

    m_pEffect           = NULL;
    m_pRenderToEnvMap   = NULL;

    m_pCubeMap          = NULL;
    m_pSphereMap        = NULL;
	m_pCasillaBlanca	= NULL;
	m_pCasillaNegra		= NULL;
	m_pTexture			= NULL;
	m_pTexture1			= NULL;
	m_fRot				= -1.0f;
	m_fDistancia		= 30.0f;

	m_bRotar= m_bCaballo= m_bEliminar= m_bDrag= false;
	m_pCaballo			=NULL;
	m_nPosAntTablero= m_nPosTablero= m_nFicha= -1;
	m_nX= m_nY= 0;
	// mostrar el cursor en pantalla completa
	m_bShowCursorWhenFullscreen= true;
	// inicio pantalla completa
	m_bStartFullscreen= true;
	for( int i=0; i<CABALLOR; i++) 

		m_pFichas[i]= NULL;
	m_bWCasilla= m_bWFicha= m_bPDrag= m_bEfTablero= m_bEfFichas= false;
}




//-----------------------------------------------------------------------------
// Name: OneTimeSceneInit()
// Desc: Called during initial app startup, this function performs all the
//       permanent initialization.
//-----------------------------------------------------------------------------
HRESULT CJChess3D::OneTimeSceneInit()
{
    D3DXMatrixIdentity( &m_matWorld );

	m_pFontB       = new CD3DFont( _T("Arial"), 20, D3DFONT_BOLD );
    m_pSkyBox      = new CD3DMesh();
	m_pCasillaBlanca= new CD3DMesh();
	m_pCasillaNegra= new CD3DMesh();

	if( !m_pFontB || !m_pSkyBox || !m_pCasillaBlanca || !m_pCasillaNegra)

		return E_OUTOFMEMORY;
	for( int i=0; i<CABALLOR; i++) {
		
		m_pFichas[i]= new CD3DMesh();
		if( !m_pFichas[i])
			return E_OUTOFMEMORY;
	}

	D3DXMatrixIdentity( &m_matTrackBall );
    D3DXMatrixTranslation( &m_matView, 0.0f, 0.0f, m_fDistancia );
    return S_OK;
}

//-----------------------------------------------------------------------------
// Name: FrameMove()
// Desc: Called once per frame, the call is the entry point for animating
//       the scene.
//-----------------------------------------------------------------------------
HRESULT CJChess3D::FrameMove()
{
    // When the window has focus, let the mouse adjust the camera view
    if( m_bCapture )
    {
        D3DXMATRIXA16 matCursor;
        D3DXQUATERNION qCursor = D3DUtil_GetRotationFromCursor( m_hWnd );
        D3DXMatrixRotationQuaternion( &matCursor, &qCursor );
		D3DXMatrixMultiply( &m_matView, &m_matTrackBall, &matCursor );
		
        D3DXMATRIXA16 matTrans;
        D3DXMatrixTranslation( &matTrans, 0.0f, 0.0f, m_fDistancia );
        D3DXMatrixMultiply( &m_matView, &m_matView, &matTrans );
    }

    // Render the scene into the surfaces of the cubemap
    if( FAILED( RenderSceneIntoEnvMap() ) )
        return E_FAIL;

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: RenderSceneIntoEnvMap()
// Desc: Renders the scene to each of the 6 faces of the cube map
//-----------------------------------------------------------------------------
HRESULT CJChess3D::RenderSceneIntoEnvMap()
{
    HRESULT hr;

    // Set the projection matrix for a field of view of 90 degrees
    D3DXMATRIXA16 matProj;
    D3DXMatrixPerspectiveFovLH( &matProj, D3DX_PI * 0.5f, 1.0f, 0.5f, 1000.0f );


    // Get the current view matrix, to concat it with the cubemap view vectors
    D3DXMATRIXA16 matViewDir( m_matView );
    matViewDir._41 = 0.0f; matViewDir._42 = 0.0f; matViewDir._43 = 0.0f;



    // Render the six cube faces into the environment map
    if( m_pCubeMap )
        hr = m_pRenderToEnvMap->BeginCube( m_pCubeMap );
    else
        hr = m_pRenderToEnvMap->BeginSphere( m_pSphereMap );

    if(FAILED(hr))
        return hr;

    for( UINT i = 0; i < 6; i++ )
    {
        m_pRenderToEnvMap->Face( (D3DCUBEMAP_FACES) i, 0 );

        // Set the view transform for this cubemap surface
        D3DXMATRIXA16 matView;
        matView = D3DUtil_GetCubeMapViewMatrix( (D3DCUBEMAP_FACES) i );
        D3DXMatrixMultiply( &matView, &matViewDir, &matView );

        // Render the scene (except for the teapot)
		RenderScene(&matView, &matProj, false);
    }
    m_pRenderToEnvMap->End( 0 );
    return S_OK;
}

HRESULT CJChess3D::RenderSkybox( CONST D3DXMATRIXA16* pView, CONST D3DXMATRIXA16* pProject) {

		D3DXMATRIXA16 matWorld;
        // D3DXMatrixScaling( &matWorld, 10.0f, 10.0f, 10.0f );
		D3DXMatrixScaling( &matWorld, 20.0f, 20.0f, 20.0f );

        D3DXMATRIXA16 matView(*pView);
        matView._41 = matView._42 = matView._43 = 0.0f;

        m_pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld );
        m_pd3dDevice->SetTransform( D3DTS_VIEW, &matView );
        m_pd3dDevice->SetTransform( D3DTS_PROJECTION, pProject );

        m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
        m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 );
        m_pd3dDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
        m_pd3dDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
        if( (m_d3dCaps.TextureAddressCaps & D3DPTADDRESSCAPS_MIRROR) == D3DPTADDRESSCAPS_MIRROR )
        {
            m_pd3dDevice->SetSamplerState( 0, D3DSAMP_ADDRESSU,  D3DTADDRESS_MIRROR );
            m_pd3dDevice->SetSamplerState( 0, D3DSAMP_ADDRESSV,  D3DTADDRESS_MIRROR );
        }

        // Always pass Z-test, so we can avoid clearing color and depth buffers
        m_pd3dDevice->SetRenderState( D3DRS_ZFUNC, D3DCMP_ALWAYS );
        m_pSkyBox->Render( m_pd3dDevice );
        m_pd3dDevice->SetRenderState( D3DRS_ZFUNC, D3DCMP_LESSEQUAL );
		return S_OK;
}

HRESULT CJChess3D::RenderBlack( CONST D3DXMATRIXA16 *pView, CONST D3DXMATRIXA16 *pProject,
									   CD3DMesh* Objeto, D3DXMATRIXA16* matWorld, bool wireframe, 
									   bool ExtraTextura, bool blanca) 
{

		D3DXMatrixMultiply( &m_matWorld, matWorld, matWorld);
		m_pd3dDevice->SetTransform( D3DTS_WORLD, &m_matWorld);
        m_pd3dDevice->SetTransform( D3DTS_VIEW, pView );
        m_pd3dDevice->SetTransform( D3DTS_PROJECTION, pProject );

		m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
	    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 );
		m_pd3dDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
	    m_pd3dDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
		m_pd3dDevice->SetSamplerState( 0, D3DSAMP_ADDRESSU,  D3DTADDRESS_WRAP );
		m_pd3dDevice->SetSamplerState( 0, D3DSAMP_ADDRESSV,  D3DTADDRESS_WRAP );

		if( ExtraTextura) {

			m_pd3dDevice->SetTexture( 0, m_pTexture );
			Objeto->UseMeshMaterials(false);
		}
		if( blanca) {

			m_pd3dDevice->SetTexture( 0, m_pTexture1);
			Objeto->UseMeshMaterials(false);
		}
		if( wireframe) m_pd3dDevice->SetRenderState( D3DRS_FILLMODE, D3DFILL_WIREFRAME );
		Objeto->Render( m_pd3dDevice);
		if( wireframe) {

			m_pd3dDevice->SetRenderState( D3DRS_FILLMODE, D3DFILL_SOLID );
			m_pd3dDevice->SetRenderState( D3DRS_ZFUNC, D3DCMP_LESSEQUAL );
		}
		if( ExtraTextura || blanca) Objeto->UseMeshMaterials( true);
		m_pd3dDevice->SetTransform( D3DTS_WORLD, &m_matWorld);
		return S_OK;
}

HRESULT CJChess3D::RenderWhite( CONST D3DXMATRIXA16 *pView, CONST D3DXMATRIXA16 *pProject, 
									   CD3DMesh* Objeto, D3DXMATRIXA16* matWorld, bool wireframe) 
{

		D3DXMatrixMultiply( &m_matWorld, matWorld, matWorld);
		// Set transform state
        if( m_pCubeMap )
        {
           
			m_pEffect->SetMatrix( "matWorld", &m_matWorld);
            m_pEffect->SetMatrix( "matView", (D3DXMATRIXA16*) pView );
        }
        else
        {
            D3DXMATRIXA16 matWorldView;
            D3DXMatrixMultiply( &matWorldView, &m_matWorld, pView );
            m_pEffect->SetMatrix( "matWorldView", &matWorldView );
        }

		m_pEffect->SetMatrix( "matWorld", &m_matWorld);
        m_pEffect->SetMatrix( "matProject", (D3DXMATRIXA16*) pProject );

        LPDIRECT3DVERTEXBUFFER9 pVB;
        LPDIRECT3DINDEXBUFFER9 pIB;

		Objeto->m_pLocalMesh->GetVertexBuffer( &pVB);
        Objeto->m_pLocalMesh->GetIndexBuffer(&pIB);

        m_pd3dDevice->SetStreamSource(0, pVB, 0, sizeof(ENVMAPPEDVERTEX));
        m_pd3dDevice->SetFVF(ENVMAPPEDVERTEX::FVF);
        m_pd3dDevice->SetIndices( pIB );

        UINT uPasses;
        m_pEffect->Begin( &uPasses, 0 );
		if( wireframe) m_pd3dDevice->SetRenderState( D3DRS_FILLMODE, D3DFILL_WIREFRAME );
        for( UINT iPass = 0; iPass < uPasses; iPass++ )
        {
            m_pEffect->Pass( iPass );

            m_pd3dDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0,
                0, Objeto->m_pLocalMesh->GetNumVertices(),
                0, Objeto->m_pLocalMesh->GetNumFaces());

        }
		if( wireframe) {

			m_pd3dDevice->SetRenderState( D3DRS_FILLMODE, D3DFILL_SOLID );
			m_pd3dDevice->SetRenderState( D3DRS_ZFUNC, D3DCMP_LESSEQUAL );
		}
        m_pEffect->End();

        SAFE_RELEASE(pVB);
        SAFE_RELEASE(pIB);
		return S_OK;
}

HRESULT CJChess3D::RenderScene( CONST D3DXMATRIXA16* pView, CONST D3DXMATRIXA16* pProject, 
									   bool RenderBlancas) 
{

	// matriz de posicion de las fichas
	D3DXMATRIXA16 matWorld;
	// renderizamos el escenario
	RenderSkybox( pView, pProject);
	//renderizamos el tablero
	int j=-1;
	bool blanco= true;

	for(int cont=0;cont<64;cont++, j++)
	{
	  if( j==7) {

		  j=-1;
		  blanco= !blanco;
	  } 
	  if( !blanco) {

		D3DXMatrixTranslation( &matWorld, (float) m_pTablero.GetCoordenadaX(cont), (float)m_pTablero.GetCoordenadaY(cont), 0.0f);
		if( cont!= m_nPosTablero || !m_bWCasilla) RenderBlack( pView, pProject, m_pCasillaNegra, &matWorld);
		else RenderBlack( pView, pProject, m_pCasillaNegra, &matWorld, true);
		
	  }  else if( RenderBlancas) {

		D3DXMatrixTranslation( &matWorld, (float) m_pTablero.GetCoordenadaX(cont), (float) m_pTablero.GetCoordenadaY(cont), 0.0f);

		if( m_bEfTablero)
			if( cont!= m_nPosTablero || !m_bWCasilla) RenderWhite( pView, pProject, m_pCasillaBlanca, &matWorld);
			else RenderWhite( pView, pProject, m_pCasillaBlanca, &matWorld, true);
		else if( cont!= m_nPosTablero|| !m_bWCasilla) RenderBlack( pView, pProject, m_pCasillaBlanca, &matWorld, false, false, true);
			else RenderBlack( pView, pProject, m_pCasillaBlanca, &matWorld, true);
	  }
      blanco= !blanco;	  
	}
	// renderizamos las fichas
	for( int i=0; i<m_pCFichas.m_nNumero; i++) {

		D3DXMatrixTranslation( &matWorld, (float) m_pCFichas.m_nX[i], (float) m_pCFichas.m_nY[i],m_pCFichas.m_nZ[i]);
		if(!EsBlanca(m_pCFichas.m_nFigura[i]) || !m_bEfFichas) 
			if( i!= m_nFicha || !m_bWFicha) RenderBlack( pView, pProject, m_pFichas[m_pCFichas.m_nFigura[i]], &matWorld);
			else RenderBlack( pView, pProject, m_pFichas[m_pCFichas.m_nFigura[i]], &matWorld, true);
		else if( RenderBlancas && m_bEfFichas) 
			if( i!= m_nFicha || !m_bWFicha) RenderWhite( pView, pProject, m_pFichas[m_pCFichas.m_nFigura[i]], &matWorld);	  
			else RenderWhite( pView, pProject, m_pFichas[m_pCFichas.m_nFigura[i]], &matWorld, true);
	}

	 // renderizamos las jugadas
	if( m_pCaballo && m_pBotones[3].estado>0) {
	
		  bool jugadas[64];
		for( int i=0; i<64; i++)

			jugadas[i]=false;
		Jugadas( m_pCaballo, jugadas, m_pBotones[3].estado);
		for( i=0; i<64; i++) {

			 if( jugadas[i]) {

				 D3DXMatrixTranslation( &matWorld, (float) m_pTablero.GetCoordenadaX(i), (float) m_pTablero.GetCoordenadaY(i),-1.6f);
				 RenderBlack( pView, pProject, m_pFichas[CABALLON], &matWorld, false, true);
		  }
	  }
  }
	return S_OK;
}
//-----------------------------------------------------------------------------
// Name: Render()
// Desc: Called once per frame, the call is the entry point for 3d
//       rendering. This function sets up render states, clears the
//       viewport, and renders the scene.
//-----------------------------------------------------------------------------
HRESULT CJChess3D::Render()
{

    // Begin the scene
    if( SUCCEEDED( m_pd3dDevice->BeginScene() ) )
    {
        // Render the scene
		RenderScene( &m_matView, &m_matProject);
		if( m_bRotar) {
			
			m_pFontB->DrawText( 1, 20, D3DCOLOR_ARGB(255,255,255,0), _T("Arriba y Abajo: para alejar y acercar la escena"));
			m_pFontB->DrawText( 1, 50, D3DCOLOR_ARGB(255,255,255,0), _T("Rotar Escena con el mouse"));
			m_pFontB->DrawText( 1, 80, D3DCOLOR_ARGB(255,255,255,0), _T("Esc: para Continuar"));
		} else {
			
			m_pFontB->DrawText( 1, 20, D3DCOLOR_ARGB(255,255,255,0), _T("Presione 'r' para mover la camara o Esc para salir"));
			// renderizamos los botones
			for( int i=0; i< 3; i++) {
            
				int r=0, g=0, b=0;
				if( m_pBotones[i].estado==NORMAL) 
					b=255;
				else if( m_pBotones[i].estado==UP) 
					r=255;
				else if( m_pBotones[i].estado==DOWN)
					g=255;
				m_pFontB->DrawText( m_pBotones[i].x, m_pBotones[i].y, D3DCOLOR_ARGB(255,r,g,b), m_pBotones[i].m_szTexto);
			}		
			sprintf(m_pBotones[3].m_szTexto, "Nivel %d", m_pBotones[3].estado);
			m_pFontB->DrawText( m_pBotones[3].x, m_pBotones[3].y, D3DCOLOR_ARGB(255,255,255,255), m_pBotones[3].m_szTexto);
		}
        m_pd3dDevice->EndScene();
    }

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: InitDeviceObjects()
// Desc: Initialize scene objects.
//-----------------------------------------------------------------------------
HRESULT CJChess3D::InitDeviceObjects()
{
    // Load the file objects
    if( FAILED( m_pSkyBox->Create( m_pd3dDevice, _T("meshes\\lobby_skybox.x") ) ) )
        return D3DAPPERR_MEDIANOTFOUND;
    // Restore the device-dependent objects
	m_pFontB->InitDeviceObjects( m_pd3dDevice );

	if( FAILED( m_pFichas[PEON]->Create( m_pd3dDevice, _T("meshes\\peon.x"))))
		return D3DAPPERR_MEDIANOTFOUND;
	if( FAILED( m_pFichas[PEONN]->Create( m_pd3dDevice, _T("meshes\\peonn.x"))))
		return D3DAPPERR_MEDIANOTFOUND;
	if( FAILED( m_pFichas[TORRE]->Create( m_pd3dDevice, _T("meshes\\torre.x"))))
		return D3DAPPERR_MEDIANOTFOUND;
	if( FAILED( m_pFichas[TORREN]->Create( m_pd3dDevice, _T("meshes\\torren.x"))))
		return D3DAPPERR_MEDIANOTFOUND;
	if( FAILED( m_pFichas[CABALLO]->Create( m_pd3dDevice, _T("meshes\\caballo.x"))))
		return D3DAPPERR_MEDIANOTFOUND;
	if( FAILED( m_pFichas[CABALLON]->Create( m_pd3dDevice, _T("meshes\\caballon.x"))))
		return D3DAPPERR_MEDIANOTFOUND;
	if( FAILED( m_pFichas[ALFIL]->Create( m_pd3dDevice, _T("meshes\\alfil.x"))))
		return D3DAPPERR_MEDIANOTFOUND;
	if( FAILED( m_pFichas[ALFILN]->Create( m_pd3dDevice, _T("meshes\\alfiln.x"))))
		return D3DAPPERR_MEDIANOTFOUND;
	if( FAILED( m_pFichas[DAMA]->Create( m_pd3dDevice, _T("meshes\\reina.x"))))
		return D3DAPPERR_MEDIANOTFOUND;
	if( FAILED( m_pFichas[DAMAN]->Create( m_pd3dDevice, _T("meshes\\reinan.x"))))
		return D3DAPPERR_MEDIANOTFOUND;
	if( FAILED( m_pFichas[REY]->Create( m_pd3dDevice, _T("meshes\\rey.x"))))
		return D3DAPPERR_MEDIANOTFOUND;
	if( FAILED( m_pFichas[REYN]->Create( m_pd3dDevice, _T("meshes\\reyn.x"))))
		return D3DAPPERR_MEDIANOTFOUND;
	if( FAILED( m_pCasillaNegra->Create( m_pd3dDevice, _T("meshes\\casilla.x"))))
		return D3DAPPERR_MEDIANOTFOUND;
    if( FAILED( m_pCasillaBlanca->Create( m_pd3dDevice, _T("meshes\\casilla.x"))))
		return D3DAPPERR_MEDIANOTFOUND;
	
	if( m_bEfFichas) 
		for( int i=0; i<PEONN; i++) 

			m_pFichas[i]->SetFVF( m_pd3dDevice, ENVMAPPEDVERTEX::FVF);
	else for( int i=0; i<PEONN; i++) 

			m_pFichas[i]->SetFVF( m_pd3dDevice, D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_TEX1);

	if( m_bEfTablero)
		
		m_pCasillaBlanca->SetFVF( m_pd3dDevice, ENVMAPPEDVERTEX::FVF);
	else m_pCasillaBlanca->SetFVF( m_pd3dDevice, D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_TEX1);

	for( int j= PEONN; j<CABALLOR; j++)

		m_pFichas[j]->SetFVF( m_pd3dDevice, D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_TEX1);
	m_pCasillaNegra->SetFVF( m_pd3dDevice, D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_TEX1);
	// cargamos la textura extra del caballo
	if( FAILED( D3DXCreateTextureFromFile( m_pd3dDevice, "meshes\\extra.bmp", &m_pTexture ) ) )
		return D3DAPPERR_MEDIANOTFOUND;
	if( !m_bEfTablero) 
		if( FAILED( D3DXCreateTextureFromFile( m_pd3dDevice, "meshes\\blanco.bmp", &m_pTexture1 ) ) )
			return D3DAPPERR_MEDIANOTFOUND;
    // Create Effect object
	if( m_bEfTablero || m_bEfFichas)
		if( FAILED( D3DXCreateEffect( m_pd3dDevice, g_szEffect, g_cchEffect, NULL, NULL, 0, NULL, &m_pEffect, NULL ) ) )
			return E_FAIL;
	
	float fy= 7.0f;
	float fx= 5.0f;
	m_pCFichas.SetSize(2,2);
	// incializamos las fichas del tablero
	m_pCFichas.AddFicha( PEON, fx, fy, -1.2f, true);
	fy-=2.0f;
	m_pCFichas.AddFicha( TORRE, fx, fy, -1.7f, true);
	fy-=2.0f;
	m_pCFichas.AddFicha( CABALLO, fx, fy, -1.6f, true);
	fy-=2.0f;
	m_pCFichas.AddFicha( ALFIL, fx, fy, -1.9f, true);
	fy-=2.0f;
	m_pCFichas.AddFicha( DAMA, fx, fy, -4.0f, true);
	fy-=2.0f;
	m_pCFichas.AddFicha( REY, fx, fy, -2.3f, true);
	fx= 9.0f;	
	fy=7.0f;
	m_pCFichas.AddFicha( PEONN, fx, fy, -1.2f, true);
	fy-=2.0f;
	m_pCFichas.AddFicha( TORREN, fx, fy, -1.7f, true);
	fy-=2.0f;
	m_pCFichas.AddFicha( CABALLON, fx, fy, -1.6f, true);
	fy-=2.0f;
	m_pCFichas.AddFicha( ALFILN, fx, fy, -1.9f, true);
	fy-=2.0f;
	m_pCFichas.AddFicha( DAMAN, fx, fy, -4.0f, true);
	fy-=2.0f;
	m_pCFichas.AddFicha( REYN, fx, fy, -2.3f, true);
	// inicializamos el tablero
	int x= -14;
	int y= 7;
	m_pTablero.m_nTamano= 2;

	for( int i=0; i<8; i++) {

		for( int j=0; j<8; j++) {

		  x+=2;
		  m_pTablero.AddCoordenada( x, y);
		}
		x= -14; 
		y-=2;
	}
	x= m_d3dSettings.Fullscreen_DisplayMode.Width-300;
	y= m_d3dSettings.Fullscreen_DisplayMode.Height-150;
	for( int i=0; i<4; i++) {
	
		m_pBotones[i].alto= 30;
		m_pBotones[i].ancho= 120;
		m_pBotones[i].x= x;
		m_pBotones[i].y= y;
		x+= 120;
		if( i==1) {

			x= m_d3dSettings.Fullscreen_DisplayMode.Width-300;
			y+= 40;
		}
	}	
	strcpy(	m_pBotones[0].m_szTexto, "Eliminar");
	strcpy(	m_pBotones[1].m_szTexto, "Limpiar");
	strcpy( m_pBotones[2].m_szTexto, "Caballo");
	strcpy( m_pBotones[3].m_szTexto, "Nivel 0");
	return S_OK;
}




//-----------------------------------------------------------------------------
// Name: RestoreDeviceObjects()
// Desc: Restore device-memory objects and state after a device is created or
//       resized.
//-----------------------------------------------------------------------------
HRESULT CJChess3D::RestoreDeviceObjects()
{
	
	for( int i=0; i<CABALLOR; i++) 

		m_pFichas[i]->RestoreDeviceObjects( m_pd3dDevice);
    // InitDeviceObjects for file objects (build textures and vertex buffers)
    m_pSkyBox->RestoreDeviceObjects( m_pd3dDevice );
	m_pCasillaBlanca->RestoreDeviceObjects( m_pd3dDevice);
	m_pCasillaNegra->RestoreDeviceObjects( m_pd3dDevice);
	m_pFontB->RestoreDeviceObjects();
	if( m_bEfTablero || m_bEfFichas)
		m_pEffect->OnResetDevice();

    // Create RenderToEnvMap object
    if( FAILED( D3DXCreateRenderToEnvMap( m_pd3dDevice, CUBEMAP_RESOLUTION, 1,
        m_d3dsdBackBuffer.Format, TRUE, D3DFMT_D16, &m_pRenderToEnvMap ) ) )
    {
        return E_FAIL;
    }


    // Create the cubemap, with a format that matches the backbuffer
    if( m_d3dCaps.TextureCaps & D3DPTEXTURECAPS_CUBEMAP )
    {
        if( FAILED( D3DXCreateCubeTexture( m_pd3dDevice, CUBEMAP_RESOLUTION, 1,
            D3DUSAGE_RENDERTARGET, m_d3dsdBackBuffer.Format, D3DPOOL_DEFAULT, &m_pCubeMap ) ) )
        {
            if( FAILED( D3DXCreateCubeTexture( m_pd3dDevice, CUBEMAP_RESOLUTION, 1,
                0, m_d3dsdBackBuffer.Format, D3DPOOL_DEFAULT, &m_pCubeMap ) ) )
            {
                m_pCubeMap = NULL;
            }
        }
    }


    // Create the spheremap, with a format that matches the backbuffer
    if( !m_pCubeMap )
    {
        if( FAILED( D3DXCreateTexture( m_pd3dDevice, CUBEMAP_RESOLUTION, CUBEMAP_RESOLUTION, 
            1, D3DUSAGE_RENDERTARGET, m_d3dsdBackBuffer.Format, D3DPOOL_DEFAULT, &m_pSphereMap ) ) )
        {
            if( FAILED( D3DXCreateTexture( m_pd3dDevice, CUBEMAP_RESOLUTION, CUBEMAP_RESOLUTION, 
                1, 0, m_d3dsdBackBuffer.Format, D3DPOOL_DEFAULT, &m_pSphereMap ) ) )
            {
                return E_FAIL;
            }
        }
    }


    // Initialize effect
	if( m_bEfTablero || m_bEfFichas) {
		
		m_pEffect->SetTexture( "texCubeMap", m_pCubeMap );
		m_pEffect->SetTexture( "texSphereMap", m_pSphereMap );

		if( m_pCubeMap  )
		{
			m_pEffect->SetTechnique( "Cube" );
			SetWindowText( m_hWnd, _T("JChess: Environment cube-mapping") );
		}
		else
		{
			m_pEffect->SetTechnique( "Sphere" );
			SetWindowText( m_hWnd, _T("JChess: Environment cube-mapping (using dynamic spheremap)") );
		}

	}
    // Set the transform matrices
    FLOAT fAspect = (FLOAT) m_d3dsdBackBuffer.Width / (FLOAT) m_d3dsdBackBuffer.Height;
    D3DXMatrixPerspectiveFovLH( &m_matProject, D3DX_PI * 0.4f, fAspect, 0.5f, 100.0f );


    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: InvalidateDeviceObjects()
// Desc: Called when the device-dependent objects are about to be lost.
//-----------------------------------------------------------------------------
HRESULT CJChess3D::InvalidateDeviceObjects()
{
	for( int i=0; i<CABALLOR; i++)

		m_pFichas[i]->InvalidateDeviceObjects();
    m_pSkyBox->InvalidateDeviceObjects();
    m_pFontB->InvalidateDeviceObjects();
	m_pCasillaBlanca->InvalidateDeviceObjects();
	m_pCasillaNegra->InvalidateDeviceObjects();

    if(m_pEffect)
        m_pEffect->OnLostDevice();

    SAFE_RELEASE( m_pRenderToEnvMap );
    SAFE_RELEASE( m_pCubeMap );
    SAFE_RELEASE( m_pSphereMap );

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: DeleteDeviceObjects()
// Desc: Called when the app is exiting, or the device is being changed,
//       this function deletes any device dependent objects.
//-----------------------------------------------------------------------------
HRESULT CJChess3D::DeleteDeviceObjects()
{
	m_pFontB->DeleteDeviceObjects();
    m_pSkyBox->Destroy();
	m_pCasillaBlanca->Destroy();
	m_pCasillaNegra->Destroy();
	if( m_pTexture != NULL )
        m_pTexture->Release();
	if( m_pTexture1 != NULL )
        m_pTexture1->Release();
	for( int i=0; i<CABALLOR; i++)

		m_pFichas[i]->Destroy();
    SAFE_RELEASE( m_pEffect );

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: FinalCleanup()
// Desc: Called before the app exits, this function gives the app the chance
//       to cleanup after itself.
//-----------------------------------------------------------------------------
HRESULT CJChess3D::FinalCleanup()
{
    SAFE_DELETE( m_pFontB );
    SAFE_DELETE( m_pSkyBox );
	SAFE_DELETE( m_pCasillaBlanca);
	SAFE_DELETE( m_pCasillaNegra);
	for( int i=0; i<CABALLOR; i++)

		SAFE_DELETE( m_pFichas[i]);
    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: ConfirmDevice()
// Desc: Called during device initialization, this code checks the device
//       for some minimum set of capabilities
//-----------------------------------------------------------------------------
HRESULT CJChess3D::ConfirmDevice( D3DCAPS9* pCaps, DWORD dwBehavior,
                                          D3DFORMAT adapterFormat, D3DFORMAT backBufferFormat )
{
    if( dwBehavior & D3DCREATE_PUREDEVICE )
        return E_FAIL;

    if( !(pCaps->TextureCaps & D3DPTEXTURECAPS_CUBEMAP) &&
        !(dwBehavior & D3DCREATE_SOFTWARE_VERTEXPROCESSING) &&
        !(pCaps->VertexShaderVersion >= D3DVS_VERSION(1, 0)) )
    {
        return E_FAIL;
    }

    return S_OK;
}


//-----------------------------------------------------------------------------
// Name: MsgProc()
// Desc: Message proc function to handle key and menu input
//-----------------------------------------------------------------------------
LRESULT CJChess3D::MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam,
                                    LPARAM lParam )
{

	D3DXMATRIXA16 matCursor;
	D3DXQUATERNION qCursor;

	switch( uMsg) {

		case WM_LBUTTONDOWN:

			if( !m_bRotar) {

				if( m_bEliminar || m_bCaballo)

					return 0;
				MouseLButtonDown( TransX(LOWORD( lParam)), TransY(HIWORD( lParam)));
			} else {

				qCursor = D3DUtil_GetRotationFromCursor( m_hWnd );
				D3DXMatrixRotationQuaternion( &matCursor, &qCursor );
				D3DXMatrixTranspose( &matCursor, &matCursor );
				D3DXMatrixMultiply( &m_matTrackBall, &m_matTrackBall, &matCursor );
				SetCapture( m_hWnd );
				m_bCapture = TRUE;
			}
			break;
		case WM_LBUTTONUP:

			if( !m_bRotar) {
			
				MouseLButtonUp( TransX(LOWORD( lParam)), TransY(HIWORD( lParam)));
				for( int i=0; i<3; i++) 

					if( m_pBotones[i].mensaje( LOWORD( lParam),HIWORD( lParam), DOWN)) {

						BotonDown(i);
						return 0;
					}
				if( m_pBotones[3].hitTest(LOWORD( lParam),HIWORD( lParam))) {
					if( m_pBotones[3].estado>3) 
						m_pBotones[3].estado=0;
					else m_pBotones[3].estado++;
					if( m_pCaballo)
					
						m_pCaballo->Calcular( m_pBotones[3].estado);
				}
			} else {

				qCursor = D3DUtil_GetRotationFromCursor( m_hWnd );
		        D3DXMatrixRotationQuaternion( &matCursor, &qCursor );
				D3DXMatrixMultiply( &m_matTrackBall, &m_matTrackBall, &matCursor );
		        ReleaseCapture();
		        m_bCapture = FALSE;
			}
			break;
		case WM_MOUSEMOVE:
			if( m_bRotar)

				return 0;
			MouseMove( TransX(LOWORD( lParam)), TransY(HIWORD( lParam)));
			if( !m_bDrag) {

				for( int i=0; i<3; i++) 

					if( m_pBotones[i].estado==DOWN)

						return 0;
				for( int i=0; i<3; i++) 

					m_pBotones[i].mensaje( LOWORD( lParam), HIWORD( lParam));
			}
			break;
		case WM_KEYDOWN:
			 switch( wParam )
		    {
			
			case VK_DOWN:
				if( m_bRotar) {
					
					m_fDistancia-=1.0f;
					D3DXMatrixTranslation( &matCursor, 0.0f, 0.0f, m_fDistancia );
					D3DXMatrixMultiply( &m_matView, &m_matTrackBall, &matCursor);
				}
				break;
			case VK_UP:
				if( m_bRotar) {
					
					m_fDistancia+=1.0f;
					D3DXMatrixTranslation( &matCursor, 0.0f, 0.0f, m_fDistancia );
					D3DXMatrixMultiply( &m_matView, &m_matTrackBall, &matCursor);
				}
				break;
			}
			break;
		case WM_CHAR:
			switch( wParam) {

			case 'r':
				m_bRotar= true;
				break;
			}
			break;
		case WM_CLOSE:
			if( m_bRotar) {

				m_bRotar= false;
				m_fDistancia=30.0f;
				D3DXMatrixTranslation( &m_matView, 0.0f, 0.0f, m_fDistancia );
				return 0;
			}
			if( MessageBox( m_hWnd, "Esta Seguro que desea salir?", "JChess3D", MB_YESNO |MB_ICONQUESTION)==IDNO)
				
				return 0;
			break;
	}
    // Pass remaining messages to default handler
    return CD3DApplication::MsgProc( hWnd, uMsg, wParam, lParam );
}

void CJChess3D::MouseLButtonDown( float x, float y) {

		m_nFicha= m_pCFichas.HitTest( x, y);
		if( m_nFicha!=-1) {
			
			if( !m_pCFichas.m_bDragable[m_nFicha])
				return;
			m_bDrag= true;
			m_nX= m_pCFichas.m_nX[m_nFicha];
			m_nY= m_pCFichas.m_nY[m_nFicha];
			m_pCFichas.SetCoordenada(x, y, m_nFicha);
			if( !m_pCFichas.m_bSeleccion[m_nFicha]) {
				
				m_nPosAntTablero= m_pTablero.HitTest( x, y);
				if( m_nPosTablero!=-1) 

					m_pTablero.SetFicha( m_nPosTablero);
			}
		}
}

void CJChess3D::MouseLButtonUp( float x, float y) {

	if( m_bDrag) {

			int pos;
			m_bDrag= false;
			pos= m_pTablero.HitTest(x, y);
			if( pos!=-1) {

				m_pCFichas.Eliminar( m_pCFichas.Buscar(m_pTablero.GetCoordenadaX(pos), m_pTablero.GetCoordenadaY(pos)));
				m_pTablero.SetFicha(pos, m_pCFichas.m_nFigura[m_nFicha]);
				if( !m_pCFichas.m_bSeleccion[m_nFicha])
				
					m_pCFichas.SetCoordenada( m_pTablero.GetCoordenadaX(pos), m_pTablero.GetCoordenadaY(pos), m_nFicha);	
				else {

					m_pCFichas.SetCoordenada(m_nX, m_nY, m_nFicha);
					m_pCFichas.AddFicha( m_pCFichas.m_nFigura[m_nFicha], m_pTablero.GetCoordenadaX(pos), 
									m_pTablero.GetCoordenadaY(pos));
				}
				if( m_pCFichas.m_nFigura[m_nFicha]== CABALLO || m_pCFichas.m_nFigura[m_nFicha]== CABALLON)

					if(m_pTablero.CuentaCaballos()==1) {
						
						int xpos, ypos;
						m_pTablero.GetCasilla(&xpos, &ypos, pos);
						 Actualizar( xpos, ypos);
					} else SAFEDELETE( m_pCaballo);
			} else {

				m_pCFichas.SetCoordenada( m_nX, m_nY, m_nFicha);
				m_pTablero.SetFicha( m_nPosAntTablero, m_pCFichas.m_nFigura[m_nFicha]);
			}
		} else if( m_bEliminar) {
				
				m_nFicha= m_pCFichas.HitTest( x, y);
				if( m_nFicha!=-1) {
			
			
					if( m_pCFichas.m_bSeleccion[m_nFicha] || !m_pCFichas.m_bDragable[m_nFicha])
						return;
					
					m_pTablero.SetFicha(m_pTablero.HitTest( x, y));
					if( m_pCFichas.m_nFigura[m_nFicha]== CABALLO || m_pCFichas.m_nFigura[m_nFicha]== CABALLON) {

						if(m_pTablero.CuentaCaballos()==1) {
						
							int xpos, ypos;
							m_pTablero.GetCasilla(&xpos, &ypos, m_pTablero.GetPosCaballo());
							Actualizar( xpos, ypos);
						} else SAFEDELETE( m_pCaballo);
					}
					m_pCFichas.Eliminar(m_nFicha);
					m_bEliminar= false;
					m_pBotones[0].estado= NORMAL;
				}
		} else if( m_bCaballo) {

			m_nFicha= m_pCFichas.HitTest( x, y);
			if( m_nFicha!=-1) {
			
				if( m_pCFichas.m_bSeleccion[m_nFicha] || !m_pCFichas.m_bDragable[m_nFicha])

					return ;
				if( m_pCFichas.m_nFigura[m_nFicha]!= CABALLO && m_pCFichas.m_nFigura[m_nFicha]!= CABALLON)

					return;
				int pos= m_pTablero.HitTest( x, y);
				int xpos, ypos;
				m_pTablero.GetCasilla(&xpos, &ypos, pos);
				Actualizar( xpos, ypos);
				m_bCaballo= false;
				m_pBotones[2].estado= NORMAL;
			}
		}
}

void CJChess3D::MouseMove( float x, float y) {

	if( m_bWCasilla)
		m_nPosTablero= m_pTablero.HitTest( x, y);
	if( m_bDrag && m_bPDrag) 

		m_pCFichas.SetCoordenada( x, y, m_nFicha);
	else if( m_bWFicha) 

		m_nFicha= m_pCFichas.HitTest( x, y);
}

void CJChess3D::Limpiar() {

	// limpiamos las fichas en el tablero y dejamos la muestra que son 12
	while( m_pCFichas.m_nNumero>CABALLOR) 
	
		m_pCFichas.Eliminar( CABALLOR);
	// Limpiamos el tablero
	for( int j=0; j<64; j++) 

		m_pTablero.SetFicha( j);
}

void CJChess3D::Jugadas( CCaballo *info, bool a[64], int profundidad) {

	int pos;
	bool blanco;
	if(!info || profundidad==0) return;

	for( int i=0; i<8; i++) 

		if( info->sig[i]) {

			pos= (info->sig[i]->GetX()*8)+info->sig[i]->GetY();
			pos= m_pTablero.GetColor(pos);
			if( pos==-1)
				blanco=!m_bBlanco;
			else if( pos==NEGRA) blanco= false;
			else blanco= true;
			if( m_bBlanco!= blanco) {
				
				a[(info->sig[i]->GetX()*8)+info->sig[i]->GetY()]=true;
				Jugadas( info->sig[i], a, profundidad-1);
			}
		}
}

void CJChess3D::Actualizar(int xCasillaCaballo, int yCasillaCaballo) {

	SAFEDELETE( m_pCaballo);
	m_pCaballo= new CCaballo( xCasillaCaballo, yCasillaCaballo);
	if(m_pTablero.GetColor( xCasillaCaballo, yCasillaCaballo)==BLANCA)
		m_bBlanco= true;
	else m_bBlanco= false;
	m_pCaballo->Calcular( m_pBotones[3].estado);
}

void CJChess3D::BotonDown(int boton) {

	switch( boton) {

		case 0:

			if( m_pTablero.EstaVacio()) {

				m_pBotones[boton].estado= NORMAL;
				return;
			}
			if( m_bEliminar)
				m_pBotones[boton].estado= NORMAL;
			m_bEliminar= !m_bEliminar;
			break;
		case 1:

			SAFEDELETE(m_pCaballo);
			Limpiar();
			// volvemos el boton a su estado original
			m_pBotones[boton].estado= NORMAL;
			break;
		case 2:
			if( m_pTablero.EstaVacio() || m_pTablero.CuentaCaballos()<2) {

				m_pBotones[boton].estado= NORMAL;
				return;
			}
			if( m_bCaballo)
				m_pBotones[boton].estado= NORMAL;
			m_bCaballo= !m_bCaballo;
			SAFEDELETE(m_pCaballo);
			break;
	}
}

void CJChess3D::SetParametros( bool EFichas, bool ETablero, bool WireFrameCasilla, 
							  bool WireFrameFicha, bool drag) {

	m_bEfTablero= ETablero;
	m_bEfFichas= EFichas;
	m_bWCasilla= WireFrameCasilla;
	m_bWFicha = WireFrameFicha;
	m_bPDrag= drag;
}