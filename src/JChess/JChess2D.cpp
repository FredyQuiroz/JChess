#include "jchess2d.h"


//-----------------------------------------------------------------------------
// Global access to the app (needed for the global WndProc())
//-----------------------------------------------------------------------------
static JChess2D* g_pD3DApp = NULL;


//-----------------------------------------------------------------------------
// Name: WndProc2D()
// Desc: Static msg handler which passes messages to the application class.
//-----------------------------------------------------------------------------
LRESULT CALLBACK WndProc2D( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
    return g_pD3DApp->MsgProc( hWnd, uMsg, wParam, lParam );
}

JChess2D::JChess2D(void)
    : m_windowed(false), lpDD(NULL), bActive(FALSE)
{
    lpDDSPrimary = NULL;
    lpDDSBack = NULL;
    pantallafisica.Lines = NULL;

	g_pD3DApp = this;
	m_nPosTablero= -1;
	m_bCaballo= m_bEliminar= m_bDrag= false;
	m_nFicha= -1;
	m_nCaballo= m_nX= m_nY=0;
	m_pCaballo= NULL;
}

JChess2D::~JChess2D(void)
{

}

//Esta funcion es llamada al finalizar el programa y en ella
//se destruyen objetos que habian sido creados tales como
//las surfaces de Direct-Draw y el propio objeto Direct-Draw. 
void JChess2D::finiObjects( void )
{
 if( lpDD != NULL )
 {
  if( lpDDSPrimary != NULL )
  {
   lpDDSPrimary->Release();
   lpDDSPrimary = NULL;
  }
  lpDD->Release();
  lpDD = NULL;
 }
}


//Esta funcion contendra la inicializacion de las rutinas que se vayan creando. 
void JChess2D::GameInit()
{
 //Crea una lista de yres punteros que contendran el principio
 //de cada una de las lineas que forman la pantalla fisica.
 if (m_windowed)
 {
   m_windowPixels.resize(static_cast<size_t>(xres) * yres);
   m_windowRows.resize(yres);
   for (DWORD row = 0; row < yres; ++row)
     m_windowRows[row] = &m_windowPixels[static_cast<size_t>(row) * xres];
   pantallafisica.AX = static_cast<WORD>(xres);
   pantallafisica.AY = static_cast<WORD>(yres);
   pantallafisica.Lines = m_windowRows.data();
 }
 else
   NewScreenWin(pantallafisica,xres,yres);

 //La pantalla en uso sera la pantalla fisica
 SetActiveScreen(pantallafisica);

 NewScreen(pantalla,xres,yres);
  //La pantalla activa pasa a ser la pantalla 1.
  SetActiveScreen(pantalla);

  int x= -50;
  int y= 20;
  int j=-1;
  m_pTablero.m_nTamano= 69;
  bool blanco= true;
  DWORD color;
  for(int cont=0;cont<64;cont++, j++)
  {
	  if( j==7) {

		  j=-1;
		  x= 20;
		  y+=70;
		  blanco= !blanco;
	  } else x+=70;
	  if( blanco)
		color= RGB( 255,255,255);
      else color= RGB( 0,0,0);
      blanco= !blanco;
	  //Pinta el cuadrado en si.
	  Box(x,y,x+69,y+69,color);
	  m_pTablero.AddCoordenada( x, y);
  }
  m_pFichas.SetSize( 69, 69);
  NewScreen( m_pCursor, 31,31);
  SetActiveScreen( m_pCursor);
  LoadTGA("Util\\Cursor.tga", 0, 0);
  LoadFichas();
  LoadBotones();
}

void JChess2D::LoadFichas() {

	int y= 5;
  // cargamos las fichas blancas en memoria
  NewScreen(m_pPFichas[PEON],96,96);
  SetActiveScreen(m_pPFichas[PEON]);
  LoadTGA("imagenes\\peon.TGA",0,0);
  m_pFichas.AddFicha( PEON, xres-100, y, true);
  y+= 50;
  NewScreen(m_pPFichas[ALFIL],96,96);
  SetActiveScreen(m_pPFichas[ALFIL]);
  LoadTGA("imagenes\\alfil.TGA",0,0);
  m_pFichas.AddFicha( ALFIL, xres-100, y, true);
  y+= 50;
  NewScreen(m_pPFichas[CABALLO],96,96);
  SetActiveScreen(m_pPFichas[CABALLO]);
  LoadTGA("imagenes\\caballo.TGA",0,0);
  m_pFichas.AddFicha( CABALLO, xres-100, y, true);
  y+= 50;
  NewScreen(m_pPFichas[TORRE],96,96);
  SetActiveScreen(m_pPFichas[TORRE]);
  LoadTGA("imagenes\\torre.TGA",0,0);
  m_pFichas.AddFicha( TORRE, xres-100, y, true);
  y+= 50;
  NewScreen(m_pPFichas[DAMA],96,96);
  SetActiveScreen(m_pPFichas[DAMA]);
  LoadTGA("imagenes\\reina.TGA",0,0);
  m_pFichas.AddFicha( DAMA, xres-100, y, true);
  y+= 50;
  NewScreen(m_pPFichas[REY],96,96);
  SetActiveScreen(m_pPFichas[REY]);
  LoadTGA("imagenes\\rey.TGA",0,0);
  m_pFichas.AddFicha( REY, xres-100, y, true);
  y= 5;
  // cargamos las fichas negras a memoria
  NewScreen(m_pPFichas[PEONN],96,96);
  SetActiveScreen(m_pPFichas[PEONN]);
  LoadTGA("imagenes\\peonN.TGA",0,0);
  m_pFichas.AddFicha( PEONN, xres-150, y, true);
  y+= 50;
  NewScreen(m_pPFichas[ALFILN],96,96);
  SetActiveScreen(m_pPFichas[ALFILN]);
  LoadTGA("imagenes\\alfilN.TGA",0,0);
  m_pFichas.AddFicha( ALFILN, xres-150, y, true);
  y+= 50;
  NewScreen(m_pPFichas[CABALLON],96,96);
  SetActiveScreen(m_pPFichas[CABALLON]);
  LoadTGA("imagenes\\caballoN.TGA",0,0);
  m_pFichas.AddFicha( CABALLON, xres-150, y, true);
  y+= 50;
  NewScreen(m_pPFichas[TORREN],96,96);
  SetActiveScreen(m_pPFichas[TORREN]);
  LoadTGA("imagenes\\torreN.TGA",0,0);
  m_pFichas.AddFicha( TORREN, xres-150, y, true);
  y+= 50;
  NewScreen(m_pPFichas[DAMAN],96,96);
  SetActiveScreen(m_pPFichas[DAMAN]);
  LoadTGA("imagenes\\reinaN.TGA",0,0);
  m_pFichas.AddFicha( DAMAN, xres-150, y, true);
  y+= 50;
  NewScreen(m_pPFichas[REYN],96,96);
  SetActiveScreen(m_pPFichas[REYN]);
  LoadTGA("imagenes\\reyN.TGA",0,0);
  m_pFichas.AddFicha( REYN, xres-150, y, true);
  // cargamos el otro caballo a memoria
  NewScreen(m_pPFichas[CABALLOR],96,96);
  SetActiveScreen(m_pPFichas[CABALLOR]);
  LoadTGA("imagenes\\caballoR.TGA",0,0);
}

void JChess2D::LoadBotones() {

	int x= xres-200, y= yres-100;

	for( int i=0; i<4; i++) {
	
		m_pBotones[i].alto= 30;
		m_pBotones[i].ancho= 90;
		m_pBotones[i].x= x;
		m_pBotones[i].y= y;
		x+= 100;
		if( i==1) {

			x= xres-200;
			y+= 50;
		}
	}	
	NewScreen( m_pBotones[0].m_pScreen[NORMAL], 91, 31);
	SetActiveScreen( m_pBotones[0].m_pScreen[NORMAL]);
	LoadTGA( "Util\\bEliminar.tga", 0,0);
	NewScreen( m_pBotones[0].m_pScreen[UP], 91, 31);
	SetActiveScreen( m_pBotones[0].m_pScreen[UP]);
	LoadTGA( "Util\\bEliminarup.tga", 0,0);
	NewScreen( m_pBotones[0].m_pScreen[DOWN], 91, 31);
	SetActiveScreen( m_pBotones[0].m_pScreen[DOWN]);
	LoadTGA( "Util\\bEliminardown.tga", 0,0);
	NewScreen( m_pBotones[1].m_pScreen[NORMAL], 91, 31);
	SetActiveScreen( m_pBotones[1].m_pScreen[NORMAL]);
	LoadTGA( "Util\\bLimpiar.tga", 0,0);
	NewScreen( m_pBotones[1].m_pScreen[UP], 91, 31);
	SetActiveScreen( m_pBotones[1].m_pScreen[UP]);
	LoadTGA( "Util\\bLimpiarup.tga", 0,0);
	NewScreen( m_pBotones[1].m_pScreen[DOWN], 91, 31);
	SetActiveScreen( m_pBotones[1].m_pScreen[DOWN]);
	LoadTGA( "Util\\bLimpiardown.tga", 0,0);
	NewScreen( m_pBotones[2].m_pScreen[NORMAL], 91, 31);
	SetActiveScreen( m_pBotones[2].m_pScreen[NORMAL]);
	LoadTGA( "Util\\bcaballo.tga", 0,0);
	NewScreen( m_pBotones[2].m_pScreen[UP], 91, 31);
	SetActiveScreen( m_pBotones[2].m_pScreen[UP]);
	LoadTGA( "Util\\bcaballoup.tga", 0,0);
	NewScreen( m_pBotones[2].m_pScreen[DOWN], 91, 31);
	SetActiveScreen( m_pBotones[2].m_pScreen[DOWN]);
	LoadTGA( "Util\\bcaballodown.tga", 0,0);
	NewScreen( m_pBotones[3].m_pScreen[NORMAL], 91, 31);
	SetActiveScreen( m_pBotones[3].m_pScreen[NORMAL]);
	LoadTGA( "Util\\nivel0.tga", 0,0);
	NewScreen( m_pBotones[3].m_pScreen[UP], 91, 31);
	SetActiveScreen( m_pBotones[3].m_pScreen[UP]);
	LoadTGA( "Util\\nivel1.tga", 0,0);
	NewScreen( m_pBotones[3].m_pScreen[DOWN], 91, 31);
	SetActiveScreen( m_pBotones[3].m_pScreen[DOWN]);
	LoadTGA( "Util\\nivel2.tga", 0,0);
    NewScreen( m_pBotones[3].m_pScreen[3], 91, 31);
	SetActiveScreen( m_pBotones[3].m_pScreen[3]);
	LoadTGA( "Util\\nivel3.tga", 0,0);
	NewScreen( m_pBotones[3].m_pScreen[4], 91, 31);
	SetActiveScreen( m_pBotones[3].m_pScreen[4]);
	LoadTGA( "Util\\nivel4.tga", 0,0);
}

void JChess2D::game(void)
{

  //Obtiene el puntero a la pantalla
  if (!m_windowed)
    PrepareRealScreen();
	
  int i;
  //La primera pantalla se pinta siempre en unas cooordenadas fijas,
  //borrando todo el contenido que hubiera previamente en la pantalla
  //fisica. Es por eso que no requiere usar "ScreenCopyClip".
  ScreenCopy(pantalla,pantallafisica,0,0,xres-1,yres-1,0,0);

  // renderizamos los botones
  for( i=0; i<4; i++) 
    ScreenCopyClip( m_pBotones[i].m_pScreen[m_pBotones[i].estado], pantallafisica, 0, 0, 
	  m_pBotones[i].ancho, m_pBotones[i].alto, m_pBotones[i].x, m_pBotones[i].y, TRANSPARENTE);

  // renderizamos las fichas
  for( i=0; i<m_pFichas.m_nNumero; i++) 
	  ScreenCopyClip(m_pPFichas[m_pFichas.m_nFigura[i]],pantallafisica,0,0,95,95, m_pFichas.m_nX[i],
						m_pFichas.m_nY[i],TRANSPARENTE);

  // renderizamos las jugadas
  if( m_pCaballo && m_pBotones[3].estado>0) {
	
	  bool jugadas[64];
	  for( i=0; i<64; i++)

		  jugadas[i]=false;
	  Jugadas( m_pCaballo, jugadas, m_pBotones[3].estado);
	  for( i=0; i<64; i++) {

		  if( jugadas[i]) {

			  ScreenCopyClip( m_pPFichas[CABALLOR], pantallafisica, 0, 0, 95, 95, m_pTablero.GetCoordenadaX(i)-15, 
				  m_pTablero.GetCoordenadaY(i)-15, TRANSPARENTE);
		  }
	  }
  }
  // renderizamos el cursor
  ScreenCopyClip( m_pCursor, pantallafisica, 0 ,0, 30,30, m_mouseX, m_mouseY, TRANSPARENTE);
  //Actualiza la pantalla, copiando el BackSurface sobre el FrontSurface
  if (m_windowed)
  {
    HDC dc = GetDC(hwnd);
    if (dc)
    {
      PaintWindowBuffer(dc);
      ReleaseDC(hwnd, dc);
    }
    Sleep(12);
  }
  else
    UpdateRealScreen();

}

void JChess2D::Actualizar(int xCasillaCaballo, int yCasillaCaballo) {

	SAFEDELETE( m_pCaballo);
	m_pCaballo= new CCaballo( xCasillaCaballo, yCasillaCaballo);
	if(m_pTablero.GetColor( xCasillaCaballo, yCasillaCaballo)==BLANCA)
		m_bBlanco= true;
	else m_bBlanco= false;
	m_pCaballo->Calcular( m_pBotones[3].estado);
}

void JChess2D::Jugadas( CCaballo *info, bool a[64], int profundidad) {

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
//WindowProc. Se ejecuta continuamente, es el blucle de mensajes de Windows. 
LRESULT JChess2D::MsgProc( HWND hWnd, UINT message, 
                            WPARAM wParam, LPARAM lParam )
{

    //Analiza los mensajes
    switch( message )
    {
    case WM_PAINT:
      if (m_windowed)
      {
        PAINTSTRUCT paint = {};
        HDC dc = BeginPaint(hWnd, &paint);
        PaintWindowBuffer(dc);
        EndPaint(hWnd, &paint);
        return 0;
      }
      break;
    case WM_PRINTCLIENT:
      if (m_windowed)
      {
        PaintWindowBuffer(reinterpret_cast<HDC>(wParam));
        return 0;
      }
      break;

    //Si se activa la aplicacion, bACtiva pasa a ser verdadero.
    case WM_ACTIVATEAPP:
        bActive = TRUE;
        break;

    //En caso de crearse la aplicacion no hace nada en especial.
    case WM_CREATE:
        break;

    //En caso de variarse el curso tampoco realiza nada en especial.
    case WM_SETCURSOR:
        SetCursor(NULL);
        return TRUE;

    //En caso de pulsarse una tecla, se realiza lo siguiente
    case WM_KEYDOWN:
        switch( wParam )
        {
        //Si la tecla pulsada es "ESC" manda un mensaje de cerrar la
        //aplicacion. 
        case VK_ESCAPE:
            PostMessage(hWnd, WM_CLOSE, 0, 0);
            break;
        }
        break;
	case WM_LBUTTONDOWN:
		if( m_bEliminar || m_bCaballo)

			return 0;
		m_nFicha= m_pFichas.HitTest( LOWORD( lParam), HIWORD( lParam), -15);
		if( m_nFicha!=-1) {
			
			if( !m_pFichas.m_bDragable[m_nFicha])
				return 0;
			m_bDrag= true;
			m_nX= m_pFichas.m_nX[m_nFicha];
			m_nY= m_pFichas.m_nY[m_nFicha];
			m_pFichas.SetCoordenada(LOWORD( lParam)-47, HIWORD( lParam)-47, m_nFicha);

			if( !m_pFichas.m_bSeleccion[m_nFicha]) {
				
				m_nPosTablero= m_pTablero.HitTest( LOWORD( lParam), HIWORD( lParam));
				if( m_nPosTablero!=-1) 

					m_pTablero.SetFicha( m_nPosTablero);
			}
		}
		break;
	case WM_LBUTTONUP:

		if( m_bDrag) {

			int pos;
			m_bDrag= false;
			pos= m_pTablero.HitTest( LOWORD(lParam), HIWORD( lParam));
			if( pos!=-1) {

				m_pFichas.Eliminar( m_pFichas.Buscar(m_pTablero.GetCoordenadaX(pos)-15, m_pTablero.GetCoordenadaY(pos)-15));
				m_pTablero.SetFicha(pos, m_pFichas.m_nFigura[m_nFicha]);
				if( !m_pFichas.m_bSeleccion[m_nFicha])
				
					m_pFichas.SetCoordenada( m_pTablero.GetCoordenadaX(pos)-15, m_pTablero.GetCoordenadaY(pos)-15, m_nFicha);	
				else {

					m_pFichas.SetCoordenada(m_nX, m_nY, m_nFicha);
					m_pFichas.AddFicha( m_pFichas.m_nFigura[m_nFicha], m_pTablero.GetCoordenadaX(pos)-15, 
									m_pTablero.GetCoordenadaY(pos)-15);
				}
				if( m_pFichas.m_nFigura[m_nFicha]== CABALLO || m_pFichas.m_nFigura[m_nFicha]== CABALLON)

					if(m_pTablero.CuentaCaballos()==1) {
						
						int x, y;
						m_pTablero.GetCasilla(&x, &y, pos);
						Actualizar( x, y);
					} else SAFEDELETE( m_pCaballo);
			} else {

				m_pFichas.SetCoordenada( m_nX, m_nY, m_nFicha);
				m_pTablero.SetFicha( m_nPosTablero, m_pFichas.m_nFigura[m_nFicha]);
			}
		} else if( m_bEliminar) {
				
				m_nFicha= m_pFichas.HitTest( LOWORD( lParam), HIWORD( lParam), -15);
				if( m_nFicha!=-1) {
			
			
					if( m_pFichas.m_bSeleccion[m_nFicha] || !m_pFichas.m_bDragable[m_nFicha])
						return 0;
					
					m_pTablero.SetFicha(m_pTablero.HitTest( LOWORD( lParam), HIWORD( lParam)));
					if( m_pFichas.m_nFigura[m_nFicha]== CABALLO || m_pFichas.m_nFigura[m_nFicha]== CABALLON) {

						if(m_pTablero.CuentaCaballos()==1) {
						
							int x, y;
							m_pTablero.GetCasilla(&x, &y, m_pTablero.GetPosCaballo());
							Actualizar( x, y);
						} else SAFEDELETE( m_pCaballo);
					}
					m_pFichas.Eliminar(m_nFicha);
					m_bEliminar= false;
					m_pBotones[0].estado= NORMAL;
				}
		} else if( m_bCaballo) {

			m_nFicha= m_pFichas.HitTest( LOWORD( lParam), HIWORD( lParam), -15);
			if( m_nFicha!=-1) {
			
				if( m_pFichas.m_bSeleccion[m_nFicha] || !m_pFichas.m_bDragable[m_nFicha])

					return 0;
				if( m_pFichas.m_nFigura[m_nFicha]!= CABALLO && m_pFichas.m_nFigura[m_nFicha]!= CABALLON)

					return 0;
				int pos= m_pTablero.HitTest( LOWORD( lParam), HIWORD( lParam));
				int x, y;
				m_pTablero.GetCasilla(&x, &y, pos);
				Actualizar( x, y);
				m_bCaballo= false;
				m_pBotones[2].estado= NORMAL;
			}
		}
		for( int i=0; i<3; i++) 

				if( m_pBotones[i].mensaje( m_mouseX, m_mouseY, DOWN)) {

					BotonDown(i);
					return 0;
				}
		if( m_pBotones[3].hitTest(m_mouseX, m_mouseY)) {
				if( m_pBotones[3].estado>3) 
					m_pBotones[3].estado=0;
				else m_pBotones[3].estado++;
				if( m_pCaballo)
					
					m_pCaballo->Calcular( m_pBotones[3].estado);
		}
		break;
	case WM_MOUSEMOVE:

		m_mouseX= LOWORD( lParam);
		m_mouseY= HIWORD( lParam);
		if( m_bDrag) 

			m_pFichas.SetCoordenada( m_mouseX-47, m_mouseY-47, m_nFicha);
		else for( int i=0; i<3; i++) 

				m_pBotones[i].mensaje( m_mouseX, m_mouseY);
		break;
    //En caso de acabarse la aplicacion se destruyen los objetos que hay
    //creados para asi poder salir del programa. 
    case WM_DESTROY:
        finiObjects();
        PostQuitMessage( 0 );
        break;
	case WM_CLOSE:
		if( MessageBox( hWnd, "Esta Seguro que desea salir?", "JChess3D", MB_YESNO |MB_ICONQUESTION)==IDNO)
				
			return 0;
		break;
    }
return DefWindowProc(hWnd, message, wParam, lParam);
}
 
void JChess2D::BotonDown(int boton) {

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
void JChess2D::Limpiar() {

	// limpiamos las fichas en el tablero y dejamos la muestra que son 12
	while( m_pFichas.m_nNumero>CABALLOR) 
	
		m_pFichas.Eliminar( CABALLOR);
	// Limpiamos el tablero
	for( int j=0; j<64; j++) 

		m_pTablero.SetFicha( j);
}
BOOL JChess2D::doInit( HINSTANCE hInstance, int nCmdShow )
{
    //Estructuras de datos necesarias para crear la ventana.
    WNDCLASS            wc;

    //Estructuras usadas por Direct-Draw.
    DDSCAPS             ddscaps;
    HRESULT             ddrval;
    


    //Define las caracteristicas de la ventana de la aplicacion
    //que se esta creando.
    wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = WndProc2D;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = hInstance;
    wc.hIcon = LoadIcon( hInstance, IDI_APPLICATION );
    wc.hCursor = LoadCursor( NULL, IDC_ARROW );
    wc.hbrBackground = NULL;
    wc.lpszMenuName = NAME;
    wc.lpszClassName = NAME;
    RegisterClass( &wc );
    
    //Llamada a CreateWindowsEx, crea la ventana en si.
    hwnd = CreateWindowEx(
        WS_EX_TOPMOST,
        NAME,
        TITLE,
        WS_POPUP,
        0, 0,
        GetSystemMetrics( SM_CXSCREEN ),
        GetSystemMetrics( SM_CYSCREEN ),
        NULL,
        NULL,
        hInstance,
        NULL );


    //Si no se ha creado la ventana, retorna generando un error.
    if( !hwnd )
    {
        return FALSE;
    }


    //Muestra la ventana y la actualiza.
    ShowWindow( hwnd, nCmdShow );
    UpdateWindow( hwnd );


    char fullscreen[2] = {};
    const bool classic = GetEnvironmentVariableA("JCHESS_FULLSCREEN_2D", fullscreen, sizeof(fullscreen)) == 1 &&
                         fullscreen[0] == '1';
    if (!classic)
        return StartWindowBuffer();

    //Aqui se crea el objeto Direct-Draw
    ddrval = DirectDrawCreate( NULL, &lpDD, NULL );
    //Se comprueba si se ha creado correctamente
    if( ddrval == DD_OK )
    {
        //Activa el modo exclusivo.
        ddrval = lpDD->SetCooperativeLevel( hwnd,
                                DDSCL_EXCLUSIVE | DDSCL_FULLSCREEN );
        if(ddrval == DD_OK )
        {
            //Activa la resolucion de xres*yres en 16 bits de color
            ddrval = lpDD->SetDisplayMode( xres, yres, 16 );
            if( ddrval == DD_OK )
            {
                //Crea el FrontSurface y un BackSurface.
                ddsd.dwSize = sizeof( ddsd );
                ddsd.dwFlags = DDSD_CAPS | DDSD_BACKBUFFERCOUNT;
                ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE |
                                      DDSCAPS_FLIP | DDSCAPS_SYSTEMMEMORY |
                                      DDSCAPS_COMPLEX;
                //Se crea un solo BackSurface. La memoria que usa el BackSurface
                //no estara en la tarjeta grafica, sino en la propia Ram del PC.
                ddsd.dwBackBufferCount = 1;
                ddrval = lpDD->CreateSurface( &ddsd, &lpDDSPrimary, NULL );
                if( ddrval == DD_OK )
                {
                    //Se obtiene el puntero del BackSurface
                    ddscaps.dwCaps = DDSCAPS_BACKBUFFER;
                    ddrval = lpDDSPrimary->GetAttachedSurface(&ddscaps, 
                                                          &lpDDSBack);
                    if( ddrval == DD_OK )
                    {
                            //A continuacion se hace una comprobacion del formato
                            //de pixel que usa la tarjeta grafica en la resolucion
                            //de 16 bits de color, ya que existen tarjetas que
                            //usan 5 bits para el verde y otras que usan
                            //6 bits.
                            lpDDSBack->Lock (NULL, &ddsd,
                                       DDLOCK_SURFACEMEMORYPTR | DDLOCK_WAIT, NULL);
                            PixelFormat = ddsd.ddpfPixelFormat;
                            lpDDSBack->Unlock (ddsd.lpSurface);
                            //bitsazul contiene el numero de bits para el azul,
                            //siempre sera 5.
                            bitsazul = GetBits(PixelFormat.dwBBitMask);
                            //bitsverde contiene el numero de bits para el verde
                            //que podra ser 5 o 6, segun la tarjeta.
                            bitsverde = GetBits(PixelFormat.dwGBitMask)-bitsazul;
                            //bitsrojo son los bits para el rojo, siempre 5.
                            bitsrojo = GetBits(PixelFormat.dwRBitMask)-bitsazul-bitsverde; 
           
                            //En caso de que todo haya salido bien
                            //se inicializan otras rutinas y se
                            //retorna TRUE.
                            GameInit();
                            return TRUE;
                    }
                }
            }
        }
    }

    return StartWindowBuffer();
}

BOOL JChess2D::StartWindowBuffer()
{
    if (lpDDSBack)
    {
        lpDDSBack->Release();
        lpDDSBack = NULL;
    }
    if (lpDD)
        lpDD->RestoreDisplayMode();
    finiObjects();

    m_windowed = true;
    bitsrojo = 5;
    bitsverde = 6;
    bitsazul = 5;
    const DWORD style = WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;
    RECT size = {0, 0, static_cast<LONG>(xres), static_cast<LONG>(yres)};
    AdjustWindowRect(&size, style, FALSE);
    SetWindowLongPtr(hwnd, GWL_STYLE, style | WS_VISIBLE);
    SetWindowPos(hwnd, HWND_NOTOPMOST, 48, 48, size.right - size.left, size.bottom - size.top,
                 SWP_FRAMECHANGED | SWP_SHOWWINDOW);
    GameInit();
    bActive = TRUE;
    return TRUE;
}

void JChess2D::PaintWindowBuffer(HDC dc)
{
    if (m_windowPixels.empty())
        return;

    struct {
        BITMAPINFOHEADER header;
        DWORD colors[3];
    } format = {
        {sizeof(BITMAPINFOHEADER), static_cast<LONG>(xres), -static_cast<LONG>(yres),
         1, 16, BI_BITFIELDS, 0, 0, 0, 0, 0},
        {0xf800, 0x07e0, 0x001f}
    };
    SetDIBitsToDevice(dc, 0, 0, xres, yres, 0, 0, 0, yres, m_windowPixels.data(),
                      reinterpret_cast<BITMAPINFO*>(&format), DIB_RGB_COLORS);
}

// el main de la aplicacion

int PASCAL JChess2D::Run( HINSTANCE hInstance, HINSTANCE hPrevInstance,
				   LPSTR lpCmdLine, int nCmdShow) {
	
	MSG         msg;

    lpCmdLine = lpCmdLine;
    hPrevInstance = hPrevInstance;

    //En caso de que fallara la inicializacion, se retorna FALSE.
    if( !doInit( hInstance, nCmdShow ) )
    {
        return FALSE;
    }


	 //Se ejecuta siempre.
	while( 1 )
         {
                  //Si hay algun mensaje, se llama a WindowProc para
                  //que gestione dicho mensaje.
                  if( PeekMessage( &msg, NULL, 0, 0, PM_NOREMOVE ))
                  {
                                if( !GetMessage( &msg, NULL, 0, 0 ) ) return msg.wParam;
                                TranslateMessage(&msg);
                                DispatchMessage(&msg);
                  }
                  else
                  //Si la aplicacion esta activa, se llama al nucleo del
                  //programa en si.
                  if( bActive )
                  {
                        game();
                  }
                  //En otro caso, espera la llegada de mensajes.
                  else
                  {
                                WaitMessage();
                  }
         }
}