//-----------------------------------------------------------------------------
// File: JChess.cpp
//
// Desc: DirectX window application created by the DirectX AppWizard
//-----------------------------------------------------------------------------
#define STRICT
#include "JChess.h"


bool g_bD3D= true, g_bFichas= true, g_bTablero= true, g_bDrag= true,
		g_bWCasilla= true, g_bWFicha= true;

INT_PTR CALLBACK DialogProc( HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam )
{
	
	switch( msg )
    {
	
	case WM_INITDIALOG:
		SendMessage( GetDlgItem( hDlg, IDC_RADIOD3D), BM_SETCHECK, 1, 0);
		SendMessage( GetDlgItem( hDlg, IDC_CHECKFICHAS), BM_SETCHECK, 1, 0);
		SendMessage( GetDlgItem( hDlg, IDC_CHECKTABLERO), BM_SETCHECK, 1, 0);
		SendMessage( GetDlgItem( hDlg, IDC_CHECKWTABLERO), BM_SETCHECK, 1, 0);
		SendMessage( GetDlgItem( hDlg, IDC_CHECKWFICHAS), BM_SETCHECK, 1, 0);
		SendMessage( GetDlgItem( hDlg, IDC_CHECKDRAG), BM_SETCHECK, 1, 0);
		break;
	case WM_CLOSE:
		EndDialog( hDlg, IDCANCEL );
		break;
	case WM_COMMAND:
		switch( LOWORD(wParam) )
		{
		case IDC_RADIOD3D:
			g_bD3D= true;
			EnableWindow( GetDlgItem( hDlg, IDC_CHECKFICHAS),true);
			EnableWindow( GetDlgItem( hDlg, IDC_CHECKTABLERO),true);
			EnableWindow( GetDlgItem( hDlg, IDC_CHECKWTABLERO),true);
			EnableWindow( GetDlgItem( hDlg, IDC_CHECKWFICHAS),true);
			EnableWindow( GetDlgItem( hDlg, IDC_CHECKDRAG),true);
			break;
		case IDC_RADIOD2D:
			g_bD3D= false;
			EnableWindow( GetDlgItem( hDlg, IDC_CHECKFICHAS),false);
			EnableWindow( GetDlgItem( hDlg, IDC_CHECKTABLERO),false);
			EnableWindow( GetDlgItem( hDlg, IDC_CHECKWTABLERO),false);
			EnableWindow( GetDlgItem( hDlg, IDC_CHECKWFICHAS),false);
			EnableWindow( GetDlgItem( hDlg, IDC_CHECKDRAG),false);
			break;
		case IDC_CHECKFICHAS:
			g_bFichas= !g_bFichas;
			break;
		case IDC_CHECKTABLERO:
			g_bTablero= !g_bTablero;
			break;
		case IDC_CHECKWTABLERO:
			g_bWCasilla= !g_bWCasilla;
			break;
		case IDC_CHECKWFICHAS:
			g_bWFicha= !g_bWFicha;
			break;
		case IDC_CHECKDRAG:
			g_bDrag= !g_bDrag;
			break;
		case IDOK:
			EndDialog( hDlg, IDOK );
	           break;
	    case IDCANCEL:
		    EndDialog( hDlg, IDCANCEL );
			break;
		}
		break;
	}
	return 0;
}


//-----------------------------------------------------------------------------
// Name: WinMain()
// Desc: Entry point to the program. Initializes everything, and goes into a
//       message-processing loop. Idle time is used to render the scene.
//-----------------------------------------------------------------------------

int PASCAL WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance,
                        LPSTR lpCmdLine, int nCmdShow)
{
	if( DialogBox( NULL, MAKEINTRESOURCE( IDD_INIT), 
			NULL, DialogProc )==IDOK) 
	
		if( !g_bD3D) {
		
			JChess2D *aplicacion;

			aplicacion= new JChess2D();
			return aplicacion->Run( hInstance, hPrevInstance, lpCmdLine, nCmdShow);
		} else {
	
			CJChess3D *d3dApp;

			d3dApp= new CJChess3D();
			d3dApp->SetParametros( g_bFichas, g_bTablero, g_bWCasilla, g_bWFicha, g_bDrag);
			InitCommonControls();
			if( FAILED( d3dApp->Create( hInstance ) ) )
				return 0;

			return d3dApp->Run();
		}
	return 0;
}