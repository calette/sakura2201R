/*!	@file
	@brief �A�E�g���C����̓_�C�A���O�{�b�N�X

	@author Norio Nakatani

	@date 2001/06/23 N.Nakatani Visual Basic�̃A�E�g���C�����
*/
/*
	Copyright (C) 1998-2001, Norio Nakatani
	Copyright (C) 2000, genta
	Copyright (C) 2001, Stonee, JEPRO, genta, hor
	Copyright (C) 2002, MIK, aroka, hor, genta, YAZAKI, Moca, frozen
	Copyright (C) 2003, zenryaku, Moca, naoh, little YOSHI, genta,
	Copyright (C) 2004, zenryaku, Moca, novice
	Copyright (C) 2005, genta, zenryaku, ������, D.S.Koba
	Copyright (C) 2006, genta, aroka, ryoji, Moca
	Copyright (C) 2006, genta, ryoji
	Copyright (C) 2007, ryoji
	Copyright (C) 2010, ryoji

	This source code is designed for sakura editor.
	Please contact the copyright holder to use this code for other purpose.
*/

#include "StdAfx.h"
#include <limits.h>
#include "outline/CDlgFuncList.h"
#include "outline/CFuncInfo.h"
#include "outline/CFuncInfoArr.h"// 2002/2/3 aroka
#include "outline/CDlgFileTree.h"
#include "window/CEditWnd.h"	//	2006/2/11 aroka �ǉ�
#include "doc/CEditDoc.h"
#include "uiparts/CGraphics.h"
#include "util/shell.h"
#include "util/os.h"
#include "util/input.h"
#include "util/window.h"
#include "env/CAppNodeManager.h"
#include "env/CDocTypeManager.h"
#include "env/CFileNameManager.h"
#include "env/CShareData.h"
#include "env/CShareData_IO.h"
#include "extmodule/CUxTheme.h"
#include "CGrepEnumKeys.h"
#include "CGrepEnumFilterFiles.h"
#include "CGrepEnumFilterFolders.h"
#include "CDataProfile.h"
#include "dlg/CDlgTagJumpList.h"
#include "typeprop/CImpExpManager.h"
#include "sakura_rc.h"
#include "sakura.hh"

// ��ʃh�b�L���O�p�̒�`	// 2010.06.05 ryoji
#define DEFINE_SYNCCOLOR
#define DOCK_SPLITTER_WIDTH		DpiScaleX(5)
#define DOCK_MIN_SIZE			DpiScaleX(60)
#define DOCK_BUTTON_NUM			(3)

// �r���[�̎��
#define VIEWTYPE_LIST	0
#define VIEWTYPE_TREE	1

//�A�E�g���C����� CDlgFuncList.cpp	//@@@ 2002.01.07 add start MIK
const DWORD p_helpids[] = {	//12200
	IDC_BUTTON_COPY,					HIDC_FL_BUTTON_COPY,	//�R�s�[
	IDOK,								HIDOK_FL,				//�W�����v
	IDCANCEL,							HIDCANCEL_FL,			//�L�����Z��
	IDC_BUTTON_HELP,					HIDC_FL_BUTTON_HELP,	//�w���v
	IDC_CHECK_bAutoCloseDlgFuncList,	HIDC_FL_CHECK_bAutoCloseDlgFuncList,	//�����I�ɕ���
	IDC_LIST_FL,						HIDC_FL_LIST1,			//�g�s�b�N���X�g	IDC_LIST1->IDC_LIST_FL	2008/7/3 Uchi
	IDC_TREE_FL,						HIDC_FL_TREE1,			//�g�s�b�N�c���[	IDC_TREE1->IDC_TREE_FL	2008/7/3 Uchi
	IDC_CHECK_bFunclistSetFocusOnJump,	HIDC_FL_CHECK_bFunclistSetFocusOnJump,	//�W�����v�Ńt�H�[�J�X�ړ�����
	IDC_CHECK_bMarkUpBlankLineEnable,	HIDC_FL_CHECK_bMarkUpBlankLineEnable,	//��s�𖳎�����
	IDC_COMBO_nSortType,				HIDC_COMBO_nSortType,	//����
	IDC_BUTTON_WINSIZE,					HIDC_FL_BUTTON_WINSIZE,	//�E�B���h�E�ʒu�ۑ�	// 2006.08.06 ryoji
	IDC_BUTTON_MENU,					HIDC_FL_BUTTON_MENU,	//�E�B���h�E�̈ʒu���j���[
	IDC_BUTTON_SETTING,					HIDC_FL_BUTTON_SETTING,	//�ݒ�
//	IDC_STATIC,							-1,
	0, 0
};	//@@@ 2002.01.07 add end MIK

static const SAnchorList anchorList[] = {
	{IDC_BUTTON_COPY, ANCHOR_BOTTOM},
	{IDOK, ANCHOR_BOTTOM},
	{IDCANCEL, ANCHOR_BOTTOM},
	{IDC_BUTTON_HELP, ANCHOR_BOTTOM},
	{IDC_CHECK_bAutoCloseDlgFuncList, ANCHOR_BOTTOM},
	{IDC_LIST_FL, ANCHOR_ALL},
	{IDC_TREE_FL, ANCHOR_ALL},
	{IDC_CHECK_bFunclistSetFocusOnJump, ANCHOR_BOTTOM},
	{IDC_CHECK_bMarkUpBlankLineEnable , ANCHOR_BOTTOM},
	{IDC_COMBO_nSortType, ANCHOR_TOP},
	{IDC_BUTTON_WINSIZE, ANCHOR_BOTTOM}, // 20060201 aroka
	{IDC_BUTTON_MENU, ANCHOR_BOTTOM},
};

//�֐����X�g�̗�
enum EFuncListCol {
	FL_COL_ROW		= 0,	//�s
	FL_COL_COL		= 1,	//��
	FL_COL_NAME		= 2,	//�֐���
	FL_COL_REMARK	= 3		//���l
};

/*! �\�[�g��r�p�v���V�[�W�� */
int CALLBACK CDlgFuncList::CompareFunc_Asc( LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort )
{
	CFuncInfo*		pcFuncInfo1;
	CFuncInfo*		pcFuncInfo2;
	CDlgFuncList*	pcDlgFuncList;
	pcDlgFuncList = (CDlgFuncList*)lParamSort;

	pcFuncInfo1 = pcDlgFuncList->m_pcFuncInfoArr->GetAt( lParam1 );
	if( NULL == pcFuncInfo1 ){
		return -1;
	}
	pcFuncInfo2 = pcDlgFuncList->m_pcFuncInfoArr->GetAt( lParam2 );
	if( NULL == pcFuncInfo2 ){
		return -1;
	}
	//	Apr. 23, 2005 genta �s�ԍ������[��
	if( FL_COL_NAME == pcDlgFuncList->m_nSortCol){	/* ���O�Ń\�[�g */
		return auto_stricmp( pcFuncInfo1->m_cmemFuncName.GetStringPtr(), pcFuncInfo2->m_cmemFuncName.GetStringPtr() );
	}
	//	Apr. 23, 2005 genta �s�ԍ������[��
	if( FL_COL_ROW == pcDlgFuncList->m_nSortCol){	/* �s�i�{���j�Ń\�[�g */
		if( pcFuncInfo1->m_nFuncLineCRLF < pcFuncInfo2->m_nFuncLineCRLF ){
			return -1;
		}else
		if( pcFuncInfo1->m_nFuncLineCRLF == pcFuncInfo2->m_nFuncLineCRLF ){
			if( pcFuncInfo1->m_nFuncColCRLF < pcFuncInfo2->m_nFuncColCRLF ){
				return -1;
			}else
			if( pcFuncInfo1->m_nFuncColCRLF == pcFuncInfo2->m_nFuncColCRLF ){
				return 0;
			}else{
				return 1;
			}
		}else{
			return 1;
		}
	}
	if( FL_COL_COL == pcDlgFuncList->m_nSortCol){	/* ���Ń\�[�g */
		if( pcFuncInfo1->m_nFuncColCRLF < pcFuncInfo2->m_nFuncColCRLF ){
			return -1;
		}else
		if( pcFuncInfo1->m_nFuncColCRLF == pcFuncInfo2->m_nFuncColCRLF ){
			return 0;
		}else{
			return 1;
		}
	}
	// From Here 2001.12.07 hor
	if( FL_COL_REMARK == pcDlgFuncList->m_nSortCol){	/* ���l�Ń\�[�g */
		if( pcFuncInfo1->m_nInfo < pcFuncInfo2->m_nInfo ){
			return -1;
		}else
		if( pcFuncInfo1->m_nInfo == pcFuncInfo2->m_nInfo ){
			return 0;
		}else{
			return 1;
		}
	}
	// To Here 2001.12.07 hor
	return -1;
}

int CALLBACK CDlgFuncList::CompareFunc_Desc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
	return -1 * CompareFunc_Asc(lParam1, lParam2, lParamSort);
}

EFunctionCode CDlgFuncList::GetFuncCodeRedraw(int outlineType)
{
	if( outlineType == OUTLINE_BOOKMARK ){
		return F_BOOKMARK_VIEW;
	}else if( outlineType == OUTLINE_FILETREE ){
		return F_FILETREE;
	}
	return F_OUTLINE;
}

static int GetOutlineTypeRedraw(int outlineType)
{
	if( outlineType == OUTLINE_BOOKMARK ){
		return OUTLINE_BOOKMARK;
	}else if( outlineType == OUTLINE_FILETREE ){
		return OUTLINE_FILETREE;
	}
	return OUTLINE_DEFAULT;
}

LPDLGTEMPLATE CDlgFuncList::m_pDlgTemplate = NULL;
DWORD CDlgFuncList::m_dwDlgTmpSize = 0;
HINSTANCE CDlgFuncList::m_lastRcInstance = 0;

CDlgFuncList::CDlgFuncList() : CDialog(true)
{
	/* �T�C�Y�ύX���Ɉʒu�𐧌䂷��R���g���[���� */
	assert( _countof(anchorList) == _countof(m_rcItems) );

	m_pcFuncInfoArr = NULL;		/* �֐����z�� */
	m_nCurLine = CLayoutInt(0);				/* ���ݍs */
	m_nOutlineType = OUTLINE_DEFAULT;
	m_nListType = OUTLINE_DEFAULT;
	//	Apr. 23, 2005 genta �s�ԍ������[��
	m_nSortCol = 0;				/* �\�[�g�����ԍ� 2004.04.06 zenryaku �W���͍s�ԍ�(1���) */
	m_nSortColOld = -1;
	m_bLineNumIsCRLF = false;	/* �s�ԍ��̕\�� false=�܂�Ԃ��P�ʁ^true=���s�P�� */
	m_bWaitTreeProcess = false;	// 2002.02.16 hor Tree�̃_�u���N���b�N�Ńt�H�[�J�X�ړ��ł���悤�� 2/4
	m_nSortType = 0;
	m_cFuncInfo = NULL;			/* ���݂̊֐���� */
	m_bEditWndReady = false;	/* �G�f�B�^��ʂ̏������� */
	m_bInChangeLayout = false;
	m_pszTimerJumpFile = NULL;
	m_ptDefaultSize.x = -1;
	m_ptDefaultSize.y = -1;
	m_bDummyLParamMode = false;
}


/*!
	�W���ȊO�̃��b�Z�[�W��ߑ�����

	@date 2007.11.07 ryoji �V�K
*/
INT_PTR CDlgFuncList::DispatchEvent( HWND hWnd, UINT wMsg, WPARAM wParam, LPARAM lParam )
{
	INT_PTR result;
	result = CDialog::DispatchEvent( hWnd, wMsg, wParam, lParam );

	switch( wMsg ){
	case WM_ACTIVATEAPP:
		if( IsDocking() )
			break;

		// �������ŏ��ɃA�N�e�B�u�����ꂽ�ꍇ�͈�U�ҏW�E�B���h�E���A�N�e�B�u�����Ė߂�
		//
		// Note. ���̃_�C�A���O�͑��Ƃ͈قȂ�E�B���h�E�X�^�C���̂��ߕ����Ƃ��̋������قȂ�D
		// ���̓X���b�h���ŋ߃A�N�e�B�u�ȃE�B���h�E���A�N�e�B�u�ɂȂ邪�C���̃_�C�A���O�ł�
		// �Z�b�V�������S�̂ł̍ŋ߃A�N�e�B�u�E�B���h�E���A�N�e�B�u�ɂȂ��Ă��܂��D
		// ����ł͓s���������̂ŁC���ʂɈȉ��̏������s���đ��Ɠ��l�ȋ�����������悤�ɂ���D
		if( (BOOL)wParam ){
			CEditView* pcEditView = (CEditView*)m_lParam;
			CEditWnd* pcEditWnd = pcEditView->m_pcEditWnd;
			if( ::GetActiveWindow() == GetHwnd() ){
				::SetActiveWindow( pcEditWnd->GetHwnd() );
				BlockingHook( NULL );	// �L���[���ɗ��܂��Ă��郁�b�Z�[�W������
				::SetActiveWindow( GetHwnd() );
				return 0L;
			}
		}
		break;

	case WM_NCPAINT:
		return OnNcPaint( hWnd, wMsg, wParam, lParam );
	case WM_NCCALCSIZE:
		return OnNcCalcSize( hWnd, wMsg, wParam, lParam );
	case WM_NCHITTEST:
		return OnNcHitTest( hWnd, wMsg, wParam, lParam );
	case WM_NCMOUSEMOVE:
		return OnNcMouseMove( hWnd, wMsg, wParam, lParam );
	case WM_MOUSEMOVE:
		return OnMouseMove( hWnd, wMsg, wParam, lParam );
	case WM_NCLBUTTONDOWN:
		return OnNcLButtonDown( hWnd, wMsg, wParam, lParam );
	case WM_LBUTTONUP:
		return OnLButtonUp( hWnd, wMsg, wParam, lParam );
	case WM_NCRBUTTONUP:
		if( IsDocking() && wParam == HTCAPTION ){
			// �h�b�L���O�̂Ƃ��̓R���e�L�X�g���j���[�𖾎��I�ɌĂяo���K�v������炵��
			::SendMessage( GetHwnd(), WM_CONTEXTMENU, (WPARAM)GetHwnd(), lParam );
			return 1L;
		}
		break;
	case WM_TIMER:
		return OnTimer( hWnd, wMsg, wParam, lParam );
	case WM_GETMINMAXINFO:
		return OnMinMaxInfo( lParam );
	case WM_SETTEXT:
		if( IsDocking() ){
			// �L���v�V�������ĕ`�悷��
			// �� ���̎��_�ł͂܂��e�L�X�g�ݒ肳��Ă��Ȃ��̂� RDW_UPDATENOW �ł� NG
			::RedrawWindow( hWnd, NULL, NULL, RDW_FRAME | RDW_INVALIDATE | RDW_NOINTERNALPAINT );
		}
		break;
	case WM_MOUSEACTIVATE:
		if( IsDocking() ){
			// �����o�[�ȊO�̏ꏊ�Ȃ�t�H�[�J�X�ړ�
			if( !(HTLEFT <= LOWORD(lParam) && LOWORD(lParam) <= HTBOTTOMRIGHT) ){
				::SetFocus( GetHwnd() );
			}
		}
		break;
	case WM_COMMAND:
		if( IsDocking() ){
			// �R���{�{�b�N�X�̃t�H�[�J�X���ω�������L���v�V�������ĕ`�悷��i�A�N�e�B�u�^��A�N�e�B�u�ؑցj
			if( LOWORD(wParam) == IDC_COMBO_nSortType ){
				if( HIWORD(wParam) == CBN_SETFOCUS || HIWORD(wParam) == CBN_KILLFOCUS ){
					::RedrawWindow( hWnd, NULL, NULL, RDW_FRAME | RDW_INVALIDATE | RDW_UPDATENOW | RDW_NOINTERNALPAINT );
				}
			}
		}
		break;
	case WM_NOTIFY:
		if( IsDocking() ){
			// �c���[�⃊�X�g�̃t�H�[�J�X���ω�������L���v�V�������ĕ`�悷��i�A�N�e�B�u�^��A�N�e�B�u�ؑցj
			NMHDR* pNMHDR = (NMHDR*)lParam;
			if( pNMHDR->code == NM_SETFOCUS || pNMHDR->code == NM_KILLFOCUS ){
				::RedrawWindow( hWnd, NULL, NULL, RDW_FRAME | RDW_INVALIDATE | RDW_UPDATENOW | RDW_NOINTERNALPAINT );
			}
		}
		{
			NMHDR* pNMHDR = (NMHDR*)lParam;
			if( pNMHDR->code == TVN_ITEMEXPANDING ){
				NMTREEVIEW* pNMTREEVIEW = (NMTREEVIEW*)lParam;
				TVITEM* pItem = &(pNMTREEVIEW->itemNew);
				if( m_nListType == OUTLINE_FILETREE ){
					SetTreeFileSub( pItem->hItem, NULL );
				}
			}
		}
		break;
	}

	return result;
}


/* ���[�h���X�_�C�A���O�̕\�� */
/*
 * @note 2011.06.25 syat nOutlineType��ǉ�
 *   nOutlineType��nListType�͂قƂ�ǂ̏ꍇ�����l�����A�v���O�C���̏ꍇ�͗�O�ŁA
 *   nOutlineType�̓A�E�g���C����͂�ID�AnListType�̓v���O�C�����Ŏw�肷�郊�X�g�`���ƂȂ�B
 */
HWND CDlgFuncList::DoModeless(
	HINSTANCE		hInstance,
	HWND			hwndParent,
	LPARAM			lParam,
	CFuncInfoArr*	pcFuncInfoArr,
	CLayoutInt		nCurLine,
	CLayoutInt		nCurCol,
	int				nOutlineType,		
	int				nListType,
	bool			bLineNumIsCRLF		/* �s�ԍ��̕\�� false=�܂�Ԃ��P�ʁ^true=���s�P�� */
)
{
	CEditView* pcEditView=(CEditView*)lParam;
	if( !pcEditView ) return NULL;
	m_pcFuncInfoArr = pcFuncInfoArr;	/* �֐����z�� */
	m_nCurLine = nCurLine;				/* ���ݍs */
	m_nCurCol = nCurCol;				/* ���݌� */
	m_nOutlineType = nOutlineType;		/* �A�E�g���C����͂̎�� */
	m_nListType = nListType;			/* �ꗗ�̎�� */
	m_bLineNumIsCRLF = bLineNumIsCRLF;	/* �s�ԍ��̕\�� false=�܂�Ԃ��P�ʁ^true=���s�P�� */
	m_nDocType = pcEditView->GetDocument()->m_cDocType.GetDocumentType().GetIndex();
	CDocTypeManager().GetTypeConfig(CTypeConfig(m_nDocType), m_type);
	m_nSortCol = m_type.m_nOutlineSortCol;
	m_nSortColOld = m_nSortCol;
	m_bSortDesc = m_type.m_bOutlineSortDesc;
	m_nSortType = m_type.m_nOutlineSortType;

	bool bType = (ProfDockSet() != 0);
	if( bType ){
		m_type.m_nDockOutline = m_nOutlineType;
		SetTypeConfig( CTypeConfig(m_nDocType), m_type );
	}else{
		CommonSet().m_nDockOutline = m_nOutlineType;
	}

	// 2007.04.18 genta : �u�t�H�[�J�X���ڂ��v�Ɓu�����I�ɕ���v���`�F�b�N����Ă���ꍇ��
	// �_�u���N���b�N���s���ƁCtrue�̂܂܎c���Ă��܂��̂ŁC�E�B���h�E���J�����Ƃ��Ƀ��Z�b�g����D
	m_bWaitTreeProcess = false;

	m_eDockSide = ProfDockSide();
	HWND hwndRet;
	if( IsDocking() ){
		// �h�b�L���O�p�Ƀ_�C�A���O�e���v���[�g�Ɏ�������Ă���\������iWS_CHILD���j
		HINSTANCE hInstance2 = CSelectLang::getLangRsrcInstance();
		if( !m_pDlgTemplate || m_lastRcInstance != hInstance2 ){
			HRSRC hResInfo = ::FindResource( hInstance2, MAKEINTRESOURCE(IDD_FUNCLIST), RT_DIALOG );
			if( !hResInfo ) return NULL;
			HGLOBAL hResData = ::LoadResource( hInstance2, hResInfo );
			if( !hResData ) return NULL;
			m_pDlgTemplate = (LPDLGTEMPLATE)::LockResource( hResData );
			if( !m_pDlgTemplate ) return NULL;
			m_dwDlgTmpSize = ::SizeofResource( hInstance2, hResInfo );
			// ����؂�ւ��Ń��\�[�X���A�����[�h����Ă��Ȃ����m�F���邽�߃C���X�^���X���L������
			m_lastRcInstance = hInstance2;
		}
		LPDLGTEMPLATE pDlgTemplate = (LPDLGTEMPLATE)::GlobalAlloc( GMEM_FIXED, m_dwDlgTmpSize );
		if( !pDlgTemplate ) return NULL;
		::CopyMemory( pDlgTemplate, m_pDlgTemplate, m_dwDlgTmpSize );
		pDlgTemplate->style = (WS_CHILD | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | DS_SETFONT);
		hwndRet = CDialog::DoModeless( hInstance, MyGetAncestor(hwndParent, GA_ROOT), pDlgTemplate, lParam, SW_HIDE );
		::GlobalFree( pDlgTemplate );
		pcEditView->m_pcEditWnd->EndLayoutBars( m_bEditWndReady );	// ��ʂ̍ă��C�A�E�g
	}else{
		hwndRet = CDialog::DoModeless( hInstance, MyGetAncestor(hwndParent, GA_ROOT), IDD_FUNCLIST, lParam, SW_SHOW );
	}
	return hwndRet;
}

/* ���[�h���X���F�����ΏۂƂȂ�r���[�̕ύX */
void CDlgFuncList::ChangeView( LPARAM pcEditView )
{
	m_lParam = pcEditView;
	return;
}

/*! �_�C�A���O�f�[�^�̐ݒ� */
void CDlgFuncList::SetData()
{
	HWND			hwndList;
	HWND			hwndTree;
	hwndList = ::GetDlgItem( GetHwnd(), IDC_LIST_FL );
	hwndTree = ::GetDlgItem( GetHwnd(), IDC_TREE_FL );

	m_bDummyLParamMode = false;
	m_vecDummylParams.clear();

	//2002.02.08 hor �B���Ƃ��ăA�C�e���폜�����Ƃŕ\��
	::ShowWindow( hwndList, SW_HIDE );
	::ShowWindow( hwndTree, SW_HIDE );
	ListView_DeleteAllItems( hwndList );
	TreeView_DeleteAllItems( hwndTree );
	::ShowWindow( GetItemHwnd(IDC_BUTTON_SETTING), SW_HIDE );


	SetDocLineFuncList();
	if( OUTLINE_C_CPP == m_nListType || OUTLINE_CPP2 == m_nListType ){	/* C++���\�b�h���X�g */
		m_nViewType = VIEWTYPE_TREE;
		SetTreeJava( GetHwnd(), TRUE );	// Jan. 04, 2002 genta Java Method Tree�ɓ���
		::SetWindowText( GetHwnd(), LS(STR_DLGFNCLST_TITLE_CPP) );
	}
	else if( OUTLINE_FILE == m_nListType ){	//@@@ 2002.04.01 YAZAKI �A�E�g���C����͂Ƀ��[���t�@�C������
		m_nViewType = VIEWTYPE_TREE;
		SetTree();
		::SetWindowText( GetHwnd(), LS(STR_DLGFNCLST_TITLE_RULE) );
	}
	else if( OUTLINE_WZTXT == m_nListType ){ //@@@ 2003.05.20 zenryaku �K�w�t�e�L�X�g�A�E�g���C�����
		m_nViewType = VIEWTYPE_TREE;
		SetTree();
		::SetWindowText( GetHwnd(), LS(STR_DLGFNCLST_TITLE_WZ) ); //	2003.06.22 Moca ���O�ύX
	}
	else if( OUTLINE_HTML == m_nListType ){ //@@@ 2003.05.20 zenryaku HTML�A�E�g���C�����
		m_nViewType = VIEWTYPE_TREE;
		SetTree();
		::SetWindowText( GetHwnd(), _T("HTML") );
	}
	else if( OUTLINE_TEX == m_nListType ){ //@@@ 2003.07.20 naoh TeX�A�E�g���C�����
		m_nViewType = VIEWTYPE_TREE;
		SetTree();
		::SetWindowText( GetHwnd(), _T("TeX") );
	}
	else if( OUTLINE_TEXT == m_nListType ){ /* �e�L�X�g�E�g�s�b�N���X�g */
		m_nViewType = VIEWTYPE_TREE;
		SetTree();	//@@@ 2002.04.01 YAZAKI �e�L�X�g�g�s�b�N�c���[���A�ėpSetTree���ĂԂ悤�ɕύX�B
		::SetWindowText( GetHwnd(), LS(STR_DLGFNCLST_TITLE_TEXT) );
	}
	else if( OUTLINE_JAVA == m_nListType ){ /* Java���\�b�h�c���[ */
		m_nViewType = VIEWTYPE_TREE;
		SetTreeJava( GetHwnd(), TRUE );
		::SetWindowText( GetHwnd(), LS(STR_DLGFNCLST_TITLE_JAVA) );
	}
	//	2007.02.08 genta Python�ǉ�
	else if( OUTLINE_PYTHON == m_nListType ){ /* Python ���\�b�h�c���[ */
		m_nViewType = VIEWTYPE_TREE;
		SetTree( true );
		::SetWindowText( GetHwnd(), LS(STR_DLGFNCLST_TITLE_PYTHON) );
	}
	else if( OUTLINE_COBOL == m_nListType ){ /* COBOL �A�E�g���C�� */
		m_nViewType = VIEWTYPE_TREE;
		SetTreeJava( GetHwnd(), FALSE );
		::SetWindowText( GetHwnd(), LS(STR_DLGFNCLST_TITLE_COBOL) );
	}
	else if( OUTLINE_VB == m_nListType ){	/* VisualBasic �A�E�g���C�� */
		m_nViewType = VIEWTYPE_LIST;
		SetListVB();
		::SetWindowText( GetHwnd(), LS(STR_DLGFNCLST_TITLE_VB) );
	}
	else if( OUTLINE_XML == m_nListType ){ // XML�c���[
		m_nViewType = VIEWTYPE_TREE;
		SetTree();
		::SetWindowText( GetHwnd(), _T("XML") );
	}
	else if ( OUTLINE_FILETREE == m_nListType ){
		m_nViewType = VIEWTYPE_TREE;
		SetTreeFile();
		::SetWindowText( GetHwnd(), LS(F_FILETREE) );	// �t�@�C���c���[
	}
	else if( OUTLINE_TREE == m_nListType ){ /* �ėp�c���[ */
		m_nViewType = VIEWTYPE_TREE;
		SetTree();
		::SetWindowText( GetHwnd(), _T("") );
	}
	else if( OUTLINE_TREE_TAGJUMP == m_nListType ){ /* �ėp�c���[(�^�O�W�����v�t��) */
		m_nViewType = VIEWTYPE_TREE;
		SetTree( true );
		::SetWindowText( GetHwnd(), _T("") );
	}
	else if( OUTLINE_CLSTREE == m_nListType ){ /* �ėp�N���X�c���[ */
		m_nViewType = VIEWTYPE_TREE;
		SetTreeJava( GetHwnd(), TRUE );
		::SetWindowText( GetHwnd(), _T("") );
	}
	else{
		m_nViewType = VIEWTYPE_LIST;
		switch( m_nListType ){
		case OUTLINE_C:
			::SetWindowText( GetHwnd(), LS(STR_DLGFNCLST_TITLE_C) );
			break;
		case OUTLINE_PLSQL:
			::SetWindowText( GetHwnd(), LS(STR_DLGFNCLST_TITLE_PLSQL) );
			break;
		case OUTLINE_ASM:
			::SetWindowText( GetHwnd(), LS(STR_DLGFNCLST_TITLE_ASM) );
			break;
		case OUTLINE_PERL:	//	Sep. 8, 2000 genta
			::SetWindowText( GetHwnd(), LS(STR_DLGFNCLST_TITLE_PERL) );
			break;
// Jul 10, 2003  little YOSHI  ��Ɉړ����܂���--->>
//		case OUTLINE_VB:	// 2001/06/23 N.Nakatani for Visual Basic
//			::SetWindowText( GetHwnd(), "Visual Basic �A�E�g���C��" );
//			break;
// <<---�����܂�
		case OUTLINE_ERLANG:	//	2009.08.10 genta
			::SetWindowText( GetHwnd(), LS(STR_DLGFNCLST_TITLE_ERLANG) );
			break;
		case OUTLINE_BOOKMARK:
			LV_COLUMN col;
			col.mask = LVCF_TEXT;
			col.pszText = const_cast<TCHAR*>(LS(STR_DLGFNCLST_LIST_TEXT));
			col.iSubItem = 0;
			//	Apr. 23, 2005 genta �s�ԍ������[��
			ListView_SetColumn( hwndList, FL_COL_NAME, &col );
			::SetWindowText( GetHwnd(), LS(STR_DLGFNCLST_TITLE_BOOK) );
			break;
		case OUTLINE_LIST:	// �ėp���X�g 2010.03.28 syat
			::SetWindowText( GetHwnd(), _T("") );
			break;
		}
		//	May 18, 2001 genta
		//	Window�����Ȃ��Ȃ�ƌ�œs���������̂ŁA�\�����Ȃ������ɂ��Ă���
		//::DestroyWindow( hwndTree );
//		::ShowWindow( hwndTree, SW_HIDE );
		int				i;
		TCHAR			szText[2048];
		const CFuncInfo*	pcFuncInfo;
		LV_ITEM			item;
		bool			bSelected;
		CLayoutInt		nFuncLineOld(-1);
		CLayoutInt		nFuncColOld(-1);
		CLayoutInt		nFuncLineTop(INT_MAX);
		CLayoutInt		nFuncColTop(INT_MAX);
		int				nSelectedLineTop = 0;
		int				nSelectedLine = 0;
		RECT			rc;

		m_cmemClipText.SetString(L"");	/* �N���b�v�{�[�h�R�s�[�p�e�L�X�g */
		{
			const int nBuffLenTag = 13 + wcslen(to_wchar(m_pcFuncInfoArr->m_szFilePath));
			const int nNum = m_pcFuncInfoArr->GetNum();
			int nBuffLen = 0;
			for(int i = 0; i < nNum; ++i ){
				const CFuncInfo* pcFuncInfo = m_pcFuncInfoArr->GetAt(i);
				nBuffLen += pcFuncInfo->m_cmemFuncName.GetStringLength();
			}
			m_cmemClipText.AllocStringBuffer( nBuffLen + nBuffLenTag * nNum );
		}

		::EnableWindow( ::GetDlgItem( GetHwnd() , IDC_BUTTON_COPY ), TRUE );
		bSelected = false;
		for( i = 0; i < m_pcFuncInfoArr->GetNum(); ++i ){
			pcFuncInfo = m_pcFuncInfoArr->GetAt( i );
			if( !bSelected ){
				if( pcFuncInfo->m_nFuncLineLAYOUT < nFuncLineTop
					|| (pcFuncInfo->m_nFuncLineLAYOUT == nFuncLineTop && pcFuncInfo->m_nFuncColLAYOUT <= nFuncColTop) ){
					nFuncLineTop = pcFuncInfo->m_nFuncLineLAYOUT;
					nFuncColTop = pcFuncInfo->m_nFuncColLAYOUT;
					nSelectedLineTop = i;
				}
			}
			{
				if( (nFuncLineOld	 < pcFuncInfo->m_nFuncLineLAYOUT
					|| (nFuncLineOld == pcFuncInfo->m_nFuncColLAYOUT && nFuncColOld <= pcFuncInfo->m_nFuncColLAYOUT))
				  && (pcFuncInfo->m_nFuncLineLAYOUT < m_nCurLine
					|| (pcFuncInfo->m_nFuncLineLAYOUT == m_nCurLine && pcFuncInfo->m_nFuncColLAYOUT <= m_nCurCol)) ){
					nFuncLineOld = pcFuncInfo->m_nFuncLineLAYOUT;
					nFuncColOld = pcFuncInfo->m_nFuncColLAYOUT;
					bSelected = true;
					nSelectedLine = i;
				}
			}
		}
		if( 0 < m_pcFuncInfoArr->GetNum() && !bSelected ){
			bSelected = true;
			nSelectedLine =  nSelectedLineTop;
		}
		for( i = 0; i < m_pcFuncInfoArr->GetNum(); ++i ){
			/* ���݂̉�͌��ʗv�f */
			pcFuncInfo = m_pcFuncInfoArr->GetAt( i );

			//	From Here Apr. 23, 2005 genta �s�ԍ������[��
			/* �s�ԍ��̕\�� false=�܂�Ԃ��P�ʁ^true=���s�P�� */
			if(m_bLineNumIsCRLF ){
				auto_sprintf( szText, _T("%d"), pcFuncInfo->m_nFuncLineCRLF );
			}else{
				auto_sprintf( szText, _T("%d"), pcFuncInfo->m_nFuncLineLAYOUT );
			}
			item.mask = LVIF_TEXT | LVIF_PARAM;
			item.pszText = szText;
			item.iItem = i;
			item.lParam	= i;
			item.iSubItem = FL_COL_ROW;
			ListView_InsertItem( hwndList, &item);

			// 2010.03.17 syat ���ǉ�
			/* �s�ԍ��̕\�� false=�܂�Ԃ��P�ʁ^true=���s�P�� */
			if(m_bLineNumIsCRLF ){
				auto_sprintf( szText, _T("%d"), pcFuncInfo->m_nFuncColCRLF );
			}else{
				auto_sprintf( szText, _T("%d"), pcFuncInfo->m_nFuncColLAYOUT );
			}
			item.mask = LVIF_TEXT;
			item.pszText = szText;
			item.iItem = i;
			item.iSubItem = FL_COL_COL;
			ListView_SetItem( hwndList, &item);

			item.mask = LVIF_TEXT;
			item.pszText = const_cast<TCHAR*>(pcFuncInfo->m_cmemFuncName.GetStringPtr());
			item.iItem = i;
			item.iSubItem = FL_COL_NAME;
			ListView_SetItem( hwndList, &item);
			//	To Here Apr. 23, 2005 genta �s�ԍ������[��

			item.mask = LVIF_TEXT;
			if(  1 == pcFuncInfo->m_nInfo ){item.pszText = const_cast<TCHAR*>(LS(STR_DLGFNCLST_REMARK01));}else
			if( 10 == pcFuncInfo->m_nInfo ){item.pszText = const_cast<TCHAR*>(LS(STR_DLGFNCLST_REMARK02));}else
			if( 20 == pcFuncInfo->m_nInfo ){item.pszText = const_cast<TCHAR*>(LS(STR_DLGFNCLST_REMARK03));}else
			if( 11 == pcFuncInfo->m_nInfo ){item.pszText = const_cast<TCHAR*>(LS(STR_DLGFNCLST_REMARK04));}else
			if( 21 == pcFuncInfo->m_nInfo ){item.pszText = const_cast<TCHAR*>(LS(STR_DLGFNCLST_REMARK05));}else
			if( 31 == pcFuncInfo->m_nInfo ){item.pszText = const_cast<TCHAR*>(LS(STR_DLGFNCLST_REMARK06));}else
			if( 41 == pcFuncInfo->m_nInfo ){item.pszText = const_cast<TCHAR*>(LS(STR_DLGFNCLST_REMARK07));}else
			if( 50 == pcFuncInfo->m_nInfo ){item.pszText = const_cast<TCHAR*>(LS(STR_DLGFNCLST_REMARK08));}else
			if( 51 == pcFuncInfo->m_nInfo ){item.pszText = const_cast<TCHAR*>(LS(STR_DLGFNCLST_REMARK09));}else
			if( 52 == pcFuncInfo->m_nInfo ){item.pszText = const_cast<TCHAR*>(LS(STR_DLGFNCLST_REMARK10));}else{
				// Jul 10, 2003  little YOSHI
				// �����ɂ�����VB�֌W�̏�����SetListVB()���\�b�h�Ɉړ����܂����B

				item.pszText = const_cast<TCHAR*>(_T(""));
			}
			item.iItem = i;
			item.iSubItem = FL_COL_REMARK;
			ListView_SetItem( hwndList, &item);

			/* �N���b�v�{�[�h�ɃR�s�[����e�L�X�g��ҏW */
			if(item.pszText[0] != _T('\0')){
				// ���o���ʂ̎��(�֐�,,,)������Ƃ�
				auto_sprintf(
					szText,
					_T("%ts(%d,%d): "),
					m_pcFuncInfoArr->m_szFilePath.c_str(),		/* ��͑Ώۃt�@�C���� */
					pcFuncInfo->m_nFuncLineCRLF,		/* ���o�s�ԍ� */
					pcFuncInfo->m_nFuncColCRLF		/* ���o���ԍ� */
				);
				m_cmemClipText.AppendStringT(szText);
				// "%ts(%ts)\r\n"
				m_cmemClipText.AppendNativeDataT(pcFuncInfo->m_cmemFuncName);
				m_cmemClipText.AppendString(L"(");
				m_cmemClipText.AppendStringT(item.pszText);
				m_cmemClipText.AppendString(L")\r\n");
			}else{
				// ���o���ʂ̎��(�֐�,,,)���Ȃ��Ƃ�
				auto_sprintf(
					szText,
					_T("%ts(%d,%d): "),
					m_pcFuncInfoArr->m_szFilePath.c_str(),		/* ��͑Ώۃt�@�C���� */
					pcFuncInfo->m_nFuncLineCRLF,		/* ���o�s�ԍ� */
					pcFuncInfo->m_nFuncColCRLF		/* ���o���ԍ� */
				);
				m_cmemClipText.AppendStringT(szText);
				m_cmemClipText.AppendNativeDataT(pcFuncInfo->m_cmemFuncName);
				m_cmemClipText.AppendString(L"\r\n");
			}
		}
		//2002.02.08 hor List�͗񕝒����Ƃ������s����O�ɕ\�����Ƃ��Ȃ��ƕςɂȂ�
		::ShowWindow( hwndList, SW_SHOW );
		/* ��̕����f�[�^�ɍ��킹�Ē��� */
		ListView_SetColumnWidth( hwndList, FL_COL_ROW, LVSCW_AUTOSIZE );
		ListView_SetColumnWidth( hwndList, FL_COL_COL, LVSCW_AUTOSIZE );
		ListView_SetColumnWidth( hwndList, FL_COL_NAME, LVSCW_AUTOSIZE );
		ListView_SetColumnWidth( hwndList, FL_COL_REMARK, LVSCW_AUTOSIZE );
		ListView_SetColumnWidth( hwndList, FL_COL_ROW, ListView_GetColumnWidth( hwndList, FL_COL_ROW ) + 16 );
		ListView_SetColumnWidth( hwndList, FL_COL_COL, ListView_GetColumnWidth( hwndList, FL_COL_COL ) + 16 );
		ListView_SetColumnWidth( hwndList, FL_COL_NAME, ListView_GetColumnWidth( hwndList, FL_COL_NAME ) + 16 );
		ListView_SetColumnWidth( hwndList, FL_COL_REMARK, ListView_GetColumnWidth( hwndList, FL_COL_REMARK ) + 16 );

		// 2005.07.05 ������
		DWORD dwExStyle  = ListView_GetExtendedListViewStyle( hwndList );
		dwExStyle |= LVS_EX_FULLROWSELECT;
		ListView_SetExtendedListViewStyle( hwndList, dwExStyle );

		if( bSelected ){
			ListView_GetItemRect( hwndList, 0, &rc, LVIR_BOUNDS );
			ListView_Scroll( hwndList, 0, nSelectedLine * ( rc.bottom - rc.top ) );
			ListView_SetItemState( hwndList, nSelectedLine, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED );
		}
	}
	/* �A�E�g���C�� �_�C�A���O�������I�ɕ��� */
	::CheckDlgButton( GetHwnd(), IDC_CHECK_bAutoCloseDlgFuncList, m_pShareData->m_Common.m_sOutline.m_bAutoCloseDlgFuncList );
	/* �A�E�g���C�� �u�b�N�}�[�N�ꗗ�ŋ�s�𖳎����� */
	::CheckDlgButton( GetHwnd(), IDC_CHECK_bMarkUpBlankLineEnable, m_pShareData->m_Common.m_sOutline.m_bMarkUpBlankLineEnable );
	/* �A�E�g���C�� �W�����v������t�H�[�J�X���ڂ� */
	::CheckDlgButton( GetHwnd(), IDC_CHECK_bFunclistSetFocusOnJump, m_pShareData->m_Common.m_sOutline.m_bFunclistSetFocusOnJump );

	/* �A�E�g���C�� ���ʒu�ƃT�C�Y���L������ */ // 20060201 aroka
	::CheckDlgButton( GetHwnd(), IDC_BUTTON_WINSIZE, m_pShareData->m_Common.m_sOutline.m_bRememberOutlineWindowPos );
	// �{�^����������Ă��邩�͂����肳���� 2008/6/5 Uchi
	::DlgItem_SetText( GetHwnd(), IDC_BUTTON_WINSIZE, 
		m_pShareData->m_Common.m_sOutline.m_bRememberOutlineWindowPos ? _T("��") : _T("��") );

	/* �_�C�A���O�������I�ɕ���Ȃ�t�H�[�J�X�ړ��I�v�V�����͊֌W�Ȃ� */
	if(m_pShareData->m_Common.m_sOutline.m_bAutoCloseDlgFuncList){
		::EnableWindow( ::GetDlgItem( GetHwnd(), IDC_CHECK_bFunclistSetFocusOnJump ), FALSE );
	}else{
		::EnableWindow( ::GetDlgItem( GetHwnd(), IDC_CHECK_bFunclistSetFocusOnJump ), TRUE );
	}

	//2002.02.08 hor
	//�iIDC_LIST_FL��IDC_TREE_FL����ɑ��݂��Ă��āAm_nViewType�ɂ���āA�ǂ����\�����邩��I��ł���j
	HWND hwndShow = (VIEWTYPE_LIST == m_nViewType)? hwndList: hwndTree;
	::ShowWindow( hwndShow, SW_SHOW );
	if( ::GetForegroundWindow() == MyGetAncestor( GetHwnd(), GA_ROOT ) && IsChild( GetHwnd(), GetFocus()) )
		::SetFocus( hwndShow );

	//2002.02.08 hor
	//��s���ǂ��������̃`�F�b�N�{�b�N�X�̓u�b�N�}�[�N�ꗗ�̂Ƃ������\������
	if(OUTLINE_BOOKMARK == m_nListType){
		::EnableWindow( ::GetDlgItem( GetHwnd(), IDC_CHECK_bMarkUpBlankLineEnable ), TRUE );
		if( !IsDocking() ) ::ShowWindow( GetDlgItem( GetHwnd(), IDC_CHECK_bMarkUpBlankLineEnable ), SW_SHOW );
	}else{
		::ShowWindow( GetDlgItem( GetHwnd(), IDC_CHECK_bMarkUpBlankLineEnable ), SW_HIDE );
		::EnableWindow( ::GetDlgItem( GetHwnd(), IDC_CHECK_bMarkUpBlankLineEnable ), FALSE );
	}
	// 2002/11/1 frozen ���ڂ̃\�[�g���ݒ肷��R���{�{�b�N�X�̓u�b�N�}�[�N�ꗗ�̈ȊO�̎��ɕ\������
	// Nov. 5, 2002 genta �c���[�\���̎������\�[�g��R���{�{�b�N�X��\��
	CEditView* pcEditView = (CEditView*)m_lParam;
	int nDocType = pcEditView->GetDocument()->m_cDocType.GetDocumentType().GetIndex();
	if( nDocType != m_nDocType ){
		// �ȑO�Ƃ̓h�L�������g�^�C�v���ς�����̂ŏ���������
		m_nDocType = nDocType;
		m_nSortCol = m_type.m_nOutlineSortCol;
		m_nSortColOld = m_nSortCol;
		m_bSortDesc = m_type.m_bOutlineSortDesc;
		m_nSortType = m_type.m_nOutlineSortType;
	}
	if( m_nViewType == VIEWTYPE_TREE && m_nListType != OUTLINE_FILETREE ){
		HWND hWnd_Combo_Sort = ::GetDlgItem( GetHwnd(), IDC_COMBO_nSortType );
		if( m_nListType == OUTLINE_FILETREE ){
			::EnableWindow( hWnd_Combo_Sort , FALSE );
		}else{
			::EnableWindow( hWnd_Combo_Sort , TRUE );
		}
		::ShowWindow( hWnd_Combo_Sort , SW_SHOW );
		Combo_ResetContent( hWnd_Combo_Sort ); // 2002.11.10 Moca �ǉ�
		Combo_AddString( hWnd_Combo_Sort , LS(STR_DLGFNCLST_SORTTYPE1));
		Combo_AddString( hWnd_Combo_Sort , LS(STR_DLGFNCLST_SORTTYPE2));
		Combo_SetCurSel( hWnd_Combo_Sort , m_nSortType );
		::ShowWindow( GetDlgItem( GetHwnd(), IDC_STATIC_nSortType ), SW_SHOW );
		// 2002.11.10 Moca �ǉ� �\�[�g����
		SortTree(::GetDlgItem( GetHwnd() , IDC_TREE_FL),TVI_ROOT);
	}else if( m_nListType == OUTLINE_FILETREE ){
		::ShowWindow( GetItemHwnd(IDC_COMBO_nSortType), SW_HIDE );
		::ShowWindow( GetItemHwnd(IDC_STATIC_nSortType), SW_HIDE );
		::ShowWindow( GetItemHwnd(IDC_BUTTON_SETTING), SW_SHOW );
	}else {
		::EnableWindow( ::GetDlgItem( GetHwnd(), IDC_COMBO_nSortType ), FALSE );
		::ShowWindow( GetDlgItem( GetHwnd(), IDC_COMBO_nSortType ), SW_HIDE );
		::ShowWindow( GetDlgItem( GetHwnd(), IDC_STATIC_nSortType ), SW_HIDE );
		//ListView_SortItems( hwndList, CompareFunc_Asc, (LPARAM)this );  // 2005.04.05 zenryaku �\�[�g��Ԃ�ێ�
		SortListView( hwndList, m_nSortCol );	// 2005.04.23 genta �֐���(�w�b�_���������̂���)
	}
}




bool CDlgFuncList::GetTreeFileFullName(HWND hwndTree, HTREEITEM target, std::tstring* pPath, int* pnItem)
{
	*pPath = _T("");
	*pnItem = -1;
	do{
		TVITEM tvItem;
		TCHAR szFileName[_MAX_PATH];
		tvItem.mask = TVIF_HANDLE | TVIF_TEXT;
		tvItem.pszText = szFileName;
		tvItem.cchTextMax = _countof(szFileName);
		tvItem.hItem = target;
		TreeView_GetItem( hwndTree, &tvItem );
		if( ((-tvItem.lParam) % 10) == 3 ){
			*pnItem = (-tvItem.lParam) / 10;
			*pPath = std::tstring(m_pcFuncInfoArr->GetAt(*pnItem)->m_cmemFileName.GetStringPtr()) + _T("\\") + *pPath;
			return true;
		}
		if( tvItem.lParam != -1 && tvItem.lParam != -2 ){
			return false;
		}
		if( *pPath != _T("") ){
			*pPath = std::tstring(szFileName) + _T("\\") + *pPath;
		}else{
			*pPath = szFileName;
		}
		target = TreeView_GetParent( hwndTree, target );
	}while( target != NULL );
	return false;
}


/*! LParam����FuncInfo�̔ԍ����Z�o
	vec�ɂ̓_�~�[��LParam�ԍ��������Ă���̂ł���Ă��鐔�𐔂���
*/
static int TreeDummylParamToFuncInfoIndex(std::vector<int>& vec, LPARAM lParam)
{
	// vec = { 3,6,7 }
	// lParam 0,1,2,3,4,5,6,7,8
	// return 0 1 2-1 3 4-1-1 5
	int nCount = (int)vec.size();
	int nDiff = 0;
	for( int i = 0; i < nCount; i++ ){
		if( vec[i] < lParam ){
			nDiff++;
		}else if( vec[i] == lParam ){
			return -1;
		}else{
			break;
		}
	}
	return lParam - nDiff;
}



/* �_�C�A���O�f�[�^�̎擾 */
/* 0==����������   0���傫��==����   0��菬����==���̓G���[ */
int CDlgFuncList::GetData( void )
{
	HWND			hwndList;
	HWND			hwndTree;
	int				nItem;
	LV_ITEM			item;
	HTREEITEM		htiItem;
	TV_ITEM			tvi;

	m_cFuncInfo = NULL;
	m_sJumpFile = _T("");
	hwndList = ::GetDlgItem( GetHwnd(), IDC_LIST_FL );
	if( m_nViewType == VIEWTYPE_LIST ){
		//	List
		nItem = ListView_GetNextItem( hwndList, -1, LVNI_ALL | LVNI_SELECTED );
		if( -1 == nItem ){
			return -1;
		}
		item.mask = LVIF_PARAM;
		item.iItem = nItem;
		item.iSubItem = 0;
		ListView_GetItem( hwndList, &item );
		m_cFuncInfo = m_pcFuncInfoArr->GetAt( item.lParam );
	}else{
		hwndTree = ::GetDlgItem( GetHwnd(), IDC_TREE_FL );
		if( NULL != hwndTree ){
			htiItem = TreeView_GetSelection( hwndTree );

			tvi.mask = TVIF_HANDLE | TVIF_PARAM;
			tvi.hItem = htiItem;
			tvi.pszText = NULL;
			tvi.cchTextMax = 0;
			if( TreeView_GetItem( hwndTree, &tvi ) ){
				// lParam��-1�ȉ��� pcFuncInfoArr�ɂ͊܂܂�Ȃ�����
				if( 0 <= tvi.lParam ){
					int nIndex;
					if( m_bDummyLParamMode ){
						// �_�~�[�v�f��r��:SetTreeJava
						nIndex = TreeDummylParamToFuncInfoIndex(m_vecDummylParams, tvi.lParam);
					}else{
						nIndex = tvi.lParam;
					}
					if( 0 <= nIndex ){
						m_cFuncInfo = m_pcFuncInfoArr->GetAt(nIndex);
					}
				}else{
					if( m_nListType == OUTLINE_FILETREE ){
						if( tvi.lParam == -1 ){
							int nItem;
							if( !GetTreeFileFullName( hwndTree, htiItem, &m_sJumpFile, &nItem ) ){
								m_sJumpFile = _T(""); // error
							}
						}
					}
				}
			}
		}
	}
	return 1;
}

/* Java/C++���\�b�h�c���[�̍ő�l�X�g�[�� */
// 2016.03.06 vector����16 -> 32 �܂ő��₵�Ă���
#define MAX_JAVA_TREE_NEST 32

/*! �c���[�R���g���[���̏������FJava���\�b�h�c���[

	Java Method Tree�̍\�z: �֐����X�g������TreeControl������������B

	@date 2002.01.04 genta C++�c���[�𓝍�
*/
void CDlgFuncList::SetTreeJava( HWND hwndDlg, BOOL bAddClass )
{
	int				i;
	const CFuncInfo*	pcFuncInfo;
	HWND			hwndTree;
	int				bSelected;
	CLayoutInt		nFuncLineOld;
	CLayoutInt		nFuncColOld;
	CLayoutInt		nFuncLineTop(INT_MAX);
	CLayoutInt		nFuncColTop(INT_MAX);
	TV_INSERTSTRUCT	tvis;
	const TCHAR*	pPos;
	HTREEITEM		htiGlobal = NULL;	// Jan. 04, 2001 genta C++�Ɠ���
	HTREEITEM		htiClass;
	HTREEITEM		htiItem;
	HTREEITEM		htiSelectedTop = NULL;
	HTREEITEM		htiSelected = NULL;
	TV_ITEM			tvi;
	int				nClassNest;
	std::tstring	strLabel;
	std::vector<std::tstring> vStrClasses;

	::EnableWindow( ::GetDlgItem( GetHwnd() , IDC_BUTTON_COPY ), TRUE );
	m_bDummyLParamMode = true;
	m_vecDummylParams.clear();
	int nlParamCount = 0;

	hwndTree = ::GetDlgItem( GetHwnd(), IDC_TREE_FL );

	m_cmemClipText.SetString( L"" );
	{
		const int nBuffLenTag = 13 + wcslen(to_wchar(m_pcFuncInfoArr->m_szFilePath));
		const int nNum = m_pcFuncInfoArr->GetNum();
		int nBuffLen = 0;
		for( int i = 0; i < nNum; i++ ){
			const CFuncInfo* pcFuncInfo = m_pcFuncInfoArr->GetAt(i);
			nBuffLen += pcFuncInfo->m_cmemFuncName.GetStringLength();
		}
		m_cmemClipText.AllocStringBuffer( nBuffLen + nBuffLenTag * nNum );
	}
	// �ǉ�������̏������i�v���O�C���Ŏw��ς݂̏ꍇ�͏㏑�����Ȃ��j
	m_pcFuncInfoArr->SetAppendText( FL_OBJ_DECLARE,		LSW(STR_DLGFNCLST_APND_DECLARE),	false );
	m_pcFuncInfoArr->SetAppendText( FL_OBJ_CLASS,		LSW(STR_DLGFNCLST_APND_CLASS),		false );
	m_pcFuncInfoArr->SetAppendText( FL_OBJ_STRUCT,		LSW(STR_DLGFNCLST_APND_STRUCT),		false );
	m_pcFuncInfoArr->SetAppendText( FL_OBJ_ENUM,		LSW(STR_DLGFNCLST_APND_ENUM),		false );
	m_pcFuncInfoArr->SetAppendText( FL_OBJ_UNION,		LSW(STR_DLGFNCLST_APND_UNION),		false );
	m_pcFuncInfoArr->SetAppendText( FL_OBJ_NAMESPACE,	LSW(STR_DLGFNCLST_APND_NAMESPACE),	false );
	m_pcFuncInfoArr->SetAppendText( FL_OBJ_INTERFACE,	LSW(STR_DLGFNCLST_APND_INTERFACE),	false );
	m_pcFuncInfoArr->SetAppendText( FL_OBJ_GLOBAL,		LSW(STR_DLGFNCLST_APND_GLOBAL),		false );
	
	nFuncLineOld = CLayoutInt(-1);
	nFuncColOld = CLayoutInt(-1);
	bSelected = FALSE;
	for( i = 0; i < m_pcFuncInfoArr->GetNum(); ++i ){
		pcFuncInfo = m_pcFuncInfoArr->GetAt( i );
		const TCHAR*		pWork;
		pWork = pcFuncInfo->m_cmemFuncName.GetStringPtr();
		int m = 0;
		vStrClasses.clear();
		nClassNest = 0;
		/* �N���X��::���\�b�h�̏ꍇ */
		if( NULL != ( pPos = _tcsstr( pWork, _T("::") ) )
			&& auto_strncmp( _T("operator "), pWork, 9) != 0 ){
			/* �C���i�[�N���X�̃l�X�g���x���𒲂ׂ� */
			int	k;
			int	nWorkLen;
			int	nCharChars;
			int	nNestTemplate = 0;
			nWorkLen = _tcslen( pWork );
			for( k = 0; k < nWorkLen; ++k ){
				//2009.9.21 syat �l�X�g���[������ۂ�BOF�΍�
				if( nClassNest == MAX_JAVA_TREE_NEST ){
					k = nWorkLen;
					break;
				}
				nCharChars = CNativeT::GetSizeOfChar( pWork, nWorkLen, k );
				if( 1 == nCharChars && 0 == nNestTemplate && _T(':') == pWork[k] ){
					//	Jan. 04, 2001 genta
					//	C++�̓����̂��߁A\�ɉ�����::���N���X��؂�Ƃ݂Ȃ��悤��
					if( k < nWorkLen - 1 && _T(':') == pWork[k+1] ){
						std::tstring strClass(&pWork[m], k - m);
						vStrClasses.push_back(strClass);
						++nClassNest;
						m = k + 2;
						++k;
						// Klass::operator std::string
						if( auto_strncmp( _T("operator "), pWork + m, 9) == 0 ){
							break;
						}
					}
					else 
						break;
				}
				else if( 1 == nCharChars && _T('\\') == pWork[k] ){
					std::tstring strClass(&pWork[m], k - m);
					vStrClasses.push_back(strClass);
					++nClassNest;
					m = k + 1;
				}
				else if( 1 == nCharChars && _T('<') == pWork[k] ){
					// namesp::function<std::string> �̂悤�Ȃ��̂���������
					nNestTemplate++;
				}
				else if( 1 == nCharChars && _T('>') == pWork[k] ){
					if( 0 < nNestTemplate ){
						nNestTemplate--;
					}
				}
				if( 2 == nCharChars ){
					++k;
				}
			}
		}
		if( 0 < nClassNest ){
			int	k;
			//	Jan. 04, 2001 genta
			//	�֐��擪�̃Z�b�g(�c���[�\�z�Ŏg��)
			pWork = pWork + m; // 2 == lstrlen( "::" );

			/* �N���X���̃A�C�e�����o�^����Ă��邩 */
			htiClass = TreeView_GetFirstVisible( hwndTree );
			HTREEITEM htiParent = TVI_ROOT;
			for( k = 0; k < nClassNest; ++k ){
				//	Apr. 1, 2001 genta
				//	�ǉ��������S�p�ɂ����̂Ń����������ꂾ���K�v
				//	6 == strlen( "�N���X" ), 1 == strlen( L'\0' )

				// 2002/10/30 frozen
				// bAddClass == true �̏ꍇ�̎d�l�ύX
				// �����̍��ڂ́@�u(�N���X��)(���p�X�y�[�X���)(�ǉ�������)�v
				// �ƂȂ��Ă���Ƃ݂Ȃ��AszClassArr[k] �� �u�N���X���v�ƈ�v����΁A�����e�m�[�h�ɐݒ�B
				// �������A��v���鍀�ڂ���������ꍇ�͍ŏ��̍��ڂ�e�m�[�h�ɂ���B
				// ��v���Ȃ��ꍇ�́u(�N���X��)(���p�X�y�[�X���)�N���X�v�̃m�[�h���쐬����B
				size_t nClassNameLen = vStrClasses[k].size();
				for( ; NULL != htiClass ; htiClass = TreeView_GetNextSibling( hwndTree, htiClass ))
				{
					tvi.mask = TVIF_HANDLE | TVIF_TEXT;
					tvi.hItem = htiClass;

					std::vector<TCHAR> vecStr;
					if( TreeView_GetItemTextVector(hwndTree, tvi, vecStr) ){
						const TCHAR* pszLabel = &vecStr[0];
						if( 0 == _tcsncmp(vStrClasses[k].c_str(), pszLabel, nClassNameLen) ){
							if( bAddClass ){
								if( pszLabel[nClassNameLen]==L' ' ){
									break;
								}
							}else{
								if( pszLabel[nClassNameLen]==L'\0' ){
									break;
								}
							}
						}
					}
				}

				/* �N���X���̃A�C�e�����o�^����Ă��Ȃ��̂œo�^ */
				if( NULL == htiClass ){
					// 2002/10/28 frozen �ォ�炱���ֈړ�
					std::tstring strClassName = vStrClasses[k];

					if( bAddClass )
					{
						if( pcFuncInfo->m_nInfo == FL_OBJ_NAMESPACE )
						{
							//_tcscat( pClassName, _T(" ���O���") );
							strClassName += to_tchar(m_pcFuncInfoArr->GetAppendText(FL_OBJ_NAMESPACE).c_str());
						}
						else
							//_tcscat( pClassName, _T(" �N���X") );
							strClassName += to_tchar(m_pcFuncInfoArr->GetAppendText(FL_OBJ_CLASS).c_str());
					}
					tvis.hParent = htiParent;
					tvis.hInsertAfter = TVI_LAST;
					tvis.item.mask = TVIF_TEXT | TVIF_PARAM;
					tvis.item.pszText = const_cast<TCHAR*>(strClassName.c_str());
					// 2016.03.06 item.lParam�͓o�^���̘A�ԂɕύX
					tvis.item.lParam = nlParamCount;
					m_vecDummylParams.push_back(nlParamCount);
					nlParamCount++;


					htiClass = TreeView_InsertItem( hwndTree, &tvis );
				}else{
					//none
				}
				htiParent = htiClass;
				//if( k + 1 >= nClassNest ){
				//	break;
				//}
				htiClass = TreeView_GetChild( hwndTree, htiClass );
			}
			htiClass = htiParent;
		}else{
			//	Jan. 04, 2001 genta
			//	Global��Ԃ̏ꍇ (C++�̂�)

			// 2002/10/27 frozen ��������
			// 2007.05.26 genta "__interface" ���N���X�ɗނ��鈵���ɂ���
			// 2011.09.25 syat �v���O�C���Œǉ����ꂽ�v�f���N���X�ɗނ��鈵���ɂ���
			if( FL_OBJ_CLASS <= pcFuncInfo->m_nInfo  && pcFuncInfo->m_nInfo <= FL_OBJ_ELEMENT_MAX )
				htiClass = TVI_ROOT;
			else
			{
			// 2002/10/27 frozen �����܂�
				if( htiGlobal == NULL ){
					TV_INSERTSTRUCT	tvg;
					std::tstring sGlobal = to_tchar(m_pcFuncInfoArr->GetAppendText( FL_OBJ_GLOBAL ).c_str());

					::ZeroMemory( &tvg, sizeof(tvg));
					tvg.hParent = TVI_ROOT;
					tvg.hInsertAfter = TVI_LAST;
					tvg.item.mask = TVIF_TEXT | TVIF_PARAM;
					//tvg.item.pszText = const_cast<TCHAR*>(_T("�O���[�o��"));
					tvg.item.pszText = const_cast<TCHAR*>(sGlobal.c_str());
					tvg.item.lParam = nlParamCount;
					m_vecDummylParams.push_back(nlParamCount);
					nlParamCount++;
					htiGlobal = TreeView_InsertItem( hwndTree, &tvg );
				}
				htiClass = htiGlobal;
			}
		}
		std::tstring strFuncName = pWork;

		// 2002/10/27 frozen �ǉ�������̎�ނ𑝂₵��
		switch(pcFuncInfo->m_nInfo)
		{
		case FL_OBJ_DEFINITION:		//�u��`�ʒu�v�ɒǉ�������͕s�v�Ȃ��ߏ��O
		case FL_OBJ_NAMESPACE:		//�u���O��ԁv�͕ʂ̏ꏊ�ŏ������Ă�̂ŏ��O
		case FL_OBJ_GLOBAL:			//�u�O���[�o���v�͕ʂ̏ꏊ�ŏ������Ă�̂ŏ��O
			break;
		default:
			strFuncName += to_tchar(m_pcFuncInfoArr->GetAppendText(pcFuncInfo->m_nInfo).c_str());
		}

/* �Y���N���X���̃A�C�e���̎q�Ƃ��āA���\�b�h�̃A�C�e����o�^ */
		tvis.hParent = htiClass;
		tvis.hInsertAfter = TVI_LAST;
		tvis.item.mask = TVIF_TEXT | TVIF_PARAM;
		tvis.item.pszText = const_cast<TCHAR*>(strFuncName.c_str());
		tvis.item.lParam = nlParamCount;
		nlParamCount++;
		htiItem = TreeView_InsertItem( hwndTree, &tvis );

		/* �N���b�v�{�[�h�ɃR�s�[����e�L�X�g��ҏW */
		WCHAR szText[2048];
		auto_sprintf(
			szText,
			L"%ts(%d,%d): ",
			m_pcFuncInfoArr->m_szFilePath.c_str(),		/* ��͑Ώۃt�@�C���� */
			pcFuncInfo->m_nFuncLineCRLF,		/* ���o�s�ԍ� */
			pcFuncInfo->m_nFuncColCRLF		/* ���o���ԍ� */
		);
		m_cmemClipText.AppendString( szText ); /* �N���b�v�{�[�h�R�s�[�p�e�L�X�g */
		// "%ts%ls\r\n"
		m_cmemClipText.AppendNativeDataT(pcFuncInfo->m_cmemFuncName);
		m_cmemClipText.AppendString(FL_OBJ_DECLARE == pcFuncInfo->m_nInfo ? m_pcFuncInfoArr->GetAppendText( FL_OBJ_DECLARE ).c_str() : L"" ); 	//	Jan. 04, 2001 genta C++�Ŏg�p
		m_cmemClipText.AppendString(L"\r\n");

		/* ���݃J�[�\���ʒu�̃��\�b�h���ǂ������ׂ� */
		if( !bSelected ){
			if( pcFuncInfo->m_nFuncLineLAYOUT < nFuncLineTop
				|| (pcFuncInfo->m_nFuncLineLAYOUT == nFuncLineTop && pcFuncInfo->m_nFuncColLAYOUT <= nFuncColTop) ){
				nFuncLineTop = pcFuncInfo->m_nFuncLineLAYOUT;
				nFuncColTop = pcFuncInfo->m_nFuncColLAYOUT;
				htiSelectedTop = htiItem;
			}
		}
		{
			if( (nFuncLineOld < pcFuncInfo->m_nFuncLineLAYOUT
				|| (nFuncLineOld == pcFuncInfo->m_nFuncColLAYOUT && nFuncColOld <= pcFuncInfo->m_nFuncColLAYOUT))
			  && (pcFuncInfo->m_nFuncLineLAYOUT < m_nCurLine
				|| (pcFuncInfo->m_nFuncLineLAYOUT == m_nCurLine && pcFuncInfo->m_nFuncColLAYOUT <= m_nCurCol)) ){
				nFuncLineOld = pcFuncInfo->m_nFuncLineLAYOUT;
				nFuncColOld = pcFuncInfo->m_nFuncColLAYOUT;
				bSelected = TRUE;
				htiSelected = htiItem;
			}
		}
		//	Jan. 04, 2001 genta
		//	delete�͂��̓s�x�s���̂ł����ł͕s�v
	}
	/* �\�[�g�A�m�[�h�̓W�J������ */
//	TreeView_SortChildren( hwndTree, TVI_ROOT, 0 );
	htiClass = TreeView_GetFirstVisible( hwndTree );
	while( NULL != htiClass ){
//		TreeView_SortChildren( hwndTree, htiClass, 0 );
		TreeView_Expand( hwndTree, htiClass, TVE_EXPAND );
		htiClass = TreeView_GetNextSibling( hwndTree, htiClass );
	}
	/* ���݃J�[�\���ʒu�̃��\�b�h��I����Ԃɂ��� */
	if( bSelected ){
		TreeView_SelectItem( hwndTree, htiSelected );
	}else{
		TreeView_SelectItem( hwndTree, htiSelectedTop );
	}
//	GetTreeTextNext( hwndTree, NULL, 0 );
	return;
}


/*! ���X�g�r���[�R���g���[���̏������FVisualBasic

  �����Ȃ����̂œƗ������܂����B

  @date Jul 10, 2003  little YOSHI
*/
void CDlgFuncList::SetListVB (void)
{
	int				i;
	TCHAR			szType[64];
	TCHAR			szOption[64];
	const CFuncInfo*	pcFuncInfo;
	LV_ITEM			item;
	HWND			hwndList;
	int				bSelected;
	CLayoutInt		nFuncLineOld;
	CLayoutInt		nFuncColOld;
	int				nSelectedLine = 0;
	RECT			rc;

	::EnableWindow( ::GetDlgItem( GetHwnd() , IDC_BUTTON_COPY ), TRUE );

	hwndList = ::GetDlgItem( GetHwnd(), IDC_LIST_FL );

	
	m_cmemClipText.SetString( L"" );
	{
		const int nBuffLenTag = 17 + wcslen(to_wchar(m_pcFuncInfoArr->m_szFilePath));
		const int nNum = m_pcFuncInfoArr->GetNum();
		int nBuffLen = 0;
		for( int i = 0; i < nNum; i++ ){
			const CFuncInfo* pcFuncInfo = m_pcFuncInfoArr->GetAt(i);
			nBuffLen += pcFuncInfo->m_cmemFuncName.GetStringLength();
		}
		m_cmemClipText.AllocStringBuffer( nBuffLen + nBuffLenTag * nNum );
	}

	nFuncLineOld = CLayoutInt(-1);
	nFuncColOld = CLayoutInt(-1);
	CLayoutInt nFuncLineTop(INT_MAX);
	CLayoutInt nFuncColTop(INT_MAX);
	int nSelectedLineTop = 0;
	bSelected = FALSE;
	for( i = 0; i < m_pcFuncInfoArr->GetNum(); ++i ){
		pcFuncInfo = m_pcFuncInfoArr->GetAt( i );
		if( !bSelected ){
			if( pcFuncInfo->m_nFuncLineLAYOUT < nFuncLineTop
				|| (pcFuncInfo->m_nFuncLineLAYOUT == nFuncLineTop && pcFuncInfo->m_nFuncColLAYOUT <= nFuncColTop) ){
				nFuncLineTop = pcFuncInfo->m_nFuncLineLAYOUT;
				nFuncColTop = pcFuncInfo->m_nFuncColLAYOUT;
				nSelectedLineTop = i;
			}
		}
		{
			if( (nFuncLineOld < pcFuncInfo->m_nFuncLineLAYOUT
				|| (nFuncLineOld == pcFuncInfo->m_nFuncColLAYOUT && nFuncColOld <= pcFuncInfo->m_nFuncColLAYOUT))
			  && (pcFuncInfo->m_nFuncLineLAYOUT < m_nCurLine
				|| (pcFuncInfo->m_nFuncLineLAYOUT == m_nCurLine && pcFuncInfo->m_nFuncColLAYOUT <= m_nCurCol)) ){
				nFuncLineOld = pcFuncInfo->m_nFuncLineLAYOUT;
				nFuncColOld = pcFuncInfo->m_nFuncColLAYOUT;
				bSelected = TRUE;
				nSelectedLine = i;
			}
		}
	}
	if( 0 < m_pcFuncInfoArr->GetNum() && !bSelected ){
		bSelected = TRUE;
		nSelectedLine =  nSelectedLineTop;
	}

	TCHAR			szText[2048];
	for( i = 0; i < m_pcFuncInfoArr->GetNum(); ++i ){
		/* ���݂̉�͌��ʗv�f */
		pcFuncInfo = m_pcFuncInfoArr->GetAt( i );

		//	From Here Apr. 23, 2005 genta �s�ԍ������[��
		/* �s�ԍ��̕\�� false=�܂�Ԃ��P�ʁ^true=���s�P�� */
		if(m_bLineNumIsCRLF ){
			auto_sprintf( szText, _T("%d"), pcFuncInfo->m_nFuncLineCRLF );
		}else{
			auto_sprintf( szText, _T("%d"), pcFuncInfo->m_nFuncLineLAYOUT );
		}
		item.mask = LVIF_TEXT | LVIF_PARAM;
		item.pszText = szText;
		item.iItem = i;
		item.iSubItem = FL_COL_ROW;
		item.lParam	= i;
		ListView_InsertItem( hwndList, &item);

		// 2010.03.17 syat ���ǉ�
		/* �s�ԍ��̕\�� false=�܂�Ԃ��P�ʁ^true=���s�P�� */
		if(m_bLineNumIsCRLF ){
			auto_sprintf( szText, _T("%d"), pcFuncInfo->m_nFuncColCRLF );
		}else{
			auto_sprintf( szText, _T("%d"), pcFuncInfo->m_nFuncColLAYOUT );
		}
		item.mask = LVIF_TEXT;
		item.pszText = szText;
		item.iItem = i;
		item.iSubItem = FL_COL_COL;
		ListView_SetItem( hwndList, &item);

		item.mask = LVIF_TEXT;
		item.pszText = const_cast<TCHAR*>(pcFuncInfo->m_cmemFuncName.GetStringPtr());
		item.iItem = i;
		item.iSubItem = FL_COL_NAME;
		ListView_SetItem( hwndList, &item);
		//	To Here Apr. 23, 2005 genta �s�ԍ������[��

		item.mask = LVIF_TEXT;

		// 2001/06/23 N.Nakatani for Visual Basic
		//	Jun. 26, 2001 genta ���p���ȁ��S�p��
		auto_memset(szText, _T('\0'), _countof(szText));
		auto_memset(szType, _T('\0'), _countof(szType));
		auto_memset(szOption, _T('\0'), _countof(szOption));
		if( 1 == ((pcFuncInfo->m_nInfo >> 8) & 0x01) ){
			// �X�^�e�B�b�N�錾(Static)
			// 2006.12.12 Moca �����ɃX�y�[�X�ǉ�
			_tcscpy(szOption, LS(STR_DLGFNCLST_VB_STATIC));
		}
		switch ((pcFuncInfo->m_nInfo >> 4) & 0x0f) {
			case 2  :	// �v���C�x�[�g(Private)
				_tcsncat(szOption, LS(STR_DLGFNCLST_VB_PRIVATE), _countof(szOption) - _tcslen(szOption)); //	2006.12.17 genta �T�C�Y���C��
				break;

			case 3  :	// �t�����h(Friend)
				_tcsncat(szOption, LS(STR_DLGFNCLST_VB_FRIEND), _countof(szOption) - _tcslen(szOption)); //	2006.12.17 genta �T�C�Y���C��
				break;

			default :	// �p�u���b�N(Public)
				_tcsncat(szOption, LS(STR_DLGFNCLST_VB_PUBLIC), _countof(szOption) - _tcslen(szOption)); //	2006.12.17 genta �T�C�Y���C��
		}
		int nInfo = pcFuncInfo->m_nInfo;
		switch (nInfo & 0x0f) {
			case 1:		// �֐�(Function)
				_tcscpy(szType, LS(STR_DLGFNCLST_VB_FUNCTION));
				break;

			// 2006.12.12 Moca �X�e�[�^�X���v���V�[�W���ɕύX
			case 2:		// �v���V�[�W��(Sub)
				_tcscpy(szType, LS(STR_DLGFNCLST_VB_PROC));
				break;

			case 3:		// �v���p�e�B �擾(Property Get)
				_tcscpy(szType, LS(STR_DLGFNCLST_VB_PROPGET));
				break;

			case 4:		// �v���p�e�B �ݒ�(Property Let)
				_tcscpy(szType, LS(STR_DLGFNCLST_VB_PROPLET));
				break;

			case 5:		// �v���p�e�B �Q��(Property Set)
				_tcscpy(szType, LS(STR_DLGFNCLST_VB_PROPSET));
				break;

			case 6:		// �萔(Const)
				_tcscpy(szType, LS(STR_DLGFNCLST_VB_CONST));
				break;

			case 7:		// �񋓌^(Enum)
				_tcscpy(szType, LS(STR_DLGFNCLST_VB_ENUM));
				break;

			case 8:		// ���[�U��`�^(Type)
				_tcscpy(szType, LS(STR_DLGFNCLST_VB_TYPE));
				break;

			case 9:		// �C�x���g(Event)
				_tcscpy(szType, LS(STR_DLGFNCLST_VB_EVENT));
				break;

			default:	// ����`�Ȃ̂ŃN���A
				nInfo	= 0;

		}
		if ( 2 == ((nInfo >> 8) & 0x02) ) {
			// �錾(Declare�Ȃ�)
			_tcsncat(szType, LS(STR_DLGFNCLST_VB_DECL), _countof(szType) - _tcslen(szType));
		}

		TCHAR szTypeOption[256]; // 2006.12.12 Moca auto_sprintf�̓��o�͂œ���ϐ����g��Ȃ����߂̍�Ɨ̈�ǉ�
		if ( 0 == nInfo ) {
			szTypeOption[0] = _T('\0');	//	2006.12.17 genta �S�̂�0�Ŗ��߂�K�v�͂Ȃ�
		} else
		if ( szOption[0] == _T('\0') ) {
			auto_sprintf(szTypeOption, _T("%ts"), szType);
		} else {
			auto_sprintf(szTypeOption, _T("%ts�i%ts�j"), szType, szOption);
		}
		item.pszText = szTypeOption;
		item.iItem = i;
		item.iSubItem = FL_COL_REMARK;
		ListView_SetItem( hwndList, &item);

		/* �N���b�v�{�[�h�ɃR�s�[����e�L�X�g��ҏW */
		if(item.pszText[0] != _T('\0')){
			// ���o���ʂ̎��(�֐�,,,)������Ƃ�
			// 2006.12.12 Moca szText ���������g�ɃR�s�[���Ă����o�O���C��
			auto_sprintf(
				szText,
				_T("%ts(%d,%d): "),
				m_pcFuncInfoArr->m_szFilePath.c_str(),		/* ��͑Ώۃt�@�C���� */
				pcFuncInfo->m_nFuncLineCRLF,		/* ���o�s�ԍ� */
				pcFuncInfo->m_nFuncColCRLF		/* ���o���ԍ� */
			);
			m_cmemClipText.AppendStringT(szText);
			// "%ts(%ts)\r\n"
			m_cmemClipText.AppendNativeDataT(pcFuncInfo->m_cmemFuncName);
			m_cmemClipText.AppendString(L"(");
			m_cmemClipText.AppendStringT(item.pszText);
			m_cmemClipText.AppendString(L")\r\n");
		}else{
			// ���o���ʂ̎��(�֐�,,,)���Ȃ��Ƃ�
			auto_sprintf(
				szText,
				_T("%ts(%d,%d): "),
				m_pcFuncInfoArr->m_szFilePath.c_str(),		/* ��͑Ώۃt�@�C���� */
				pcFuncInfo->m_nFuncLineCRLF,		/* ���o�s�ԍ� */
				pcFuncInfo->m_nFuncColCRLF		/* ���o���ԍ� */
			);
			m_cmemClipText.AppendStringT(szText);
			// "%ts\r\n"
			m_cmemClipText.AppendNativeDataT(pcFuncInfo->m_cmemFuncName);
			m_cmemClipText.AppendString(L"\r\n");
		}
	}

	//2002.02.08 hor List�͗񕝒����Ƃ������s����O�ɕ\�����Ƃ��Ȃ��ƕςɂȂ�
	::ShowWindow( hwndList, SW_SHOW );
	/* ��̕����f�[�^�ɍ��킹�Ē��� */
	ListView_SetColumnWidth( hwndList, FL_COL_ROW, LVSCW_AUTOSIZE );
	ListView_SetColumnWidth( hwndList, FL_COL_COL, LVSCW_AUTOSIZE );
	ListView_SetColumnWidth( hwndList, FL_COL_NAME, LVSCW_AUTOSIZE );
	ListView_SetColumnWidth( hwndList, FL_COL_REMARK, LVSCW_AUTOSIZE );
	ListView_SetColumnWidth( hwndList, FL_COL_ROW, ListView_GetColumnWidth( hwndList, FL_COL_ROW ) + 16 );
	ListView_SetColumnWidth( hwndList, FL_COL_COL, ListView_GetColumnWidth( hwndList, FL_COL_COL ) + 16 );
	ListView_SetColumnWidth( hwndList, FL_COL_NAME, ListView_GetColumnWidth( hwndList, FL_COL_NAME ) + 16 );
	ListView_SetColumnWidth( hwndList, FL_COL_REMARK, ListView_GetColumnWidth( hwndList, FL_COL_REMARK ) + 16 );
	if( bSelected ){
		ListView_GetItemRect( hwndList, 0, &rc, LVIR_BOUNDS );
		ListView_Scroll( hwndList, 0, nSelectedLine * ( rc.bottom - rc.top ) );
		ListView_SetItemState( hwndList, nSelectedLine, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED );
	}

	return;
}

/*! �ėp�c���[�R���g���[���̏������FCFuncInfo::m_nDepth�𗘗p���Đe�q��ݒ�

	@param[in] tagjump �^�O�W�����v�`���ŏo�͂���

	@date 2002.04.01 YAZAKI
	@date 2002.11.10 Moca �K�w�̐������Ȃ�����
	@date 2007.02.25 genta �N���b�v�{�[�h�o�͂��^�u�W�����v�\�ȏ����ɕύX
	@date 2007.03.04 genta �^�u�W�����v�\�ȏ����ɕύX���邩�ǂ����̃t���O��ǉ�
	@date 2014.06.06 Moca ���t�@�C���ւ̃^�O�W�����v�@�\��ǉ�
*/
void CDlgFuncList::SetTree(bool tagjump, bool nolabel)
{
	HTREEITEM hItemSelected = NULL;
	HTREEITEM hItemSelectedTop = NULL;
	HWND hwndTree = ::GetDlgItem( GetHwnd(), IDC_TREE_FL );

	int i;
	int nFuncInfoArrNum = m_pcFuncInfoArr->GetNum();
	int nStackPointer = 0;
	int nStackDepth = 32; // phParentStack �̊m�ۂ��Ă��鐔
	HTREEITEM* phParentStack;
	phParentStack = (HTREEITEM*)malloc( nStackDepth * sizeof( HTREEITEM ) );
	phParentStack[ nStackPointer ] = TVI_ROOT;
	CLayoutInt nFuncLineOld(-1);
	CLayoutInt nFuncColOld(-1);
	CLayoutInt nFuncLineTop(INT_MAX);
	CLayoutInt nFuncColTop(INT_MAX);
	bool bSelected = false;

	m_cmemClipText.SetString(L"");
	{
		int nCount = 0;
		int nBuffLen = 0;
		int nBuffLenTag = 3; // " \r\n"
		if( tagjump ){
			nBuffLenTag = 10 + wcslen(to_wchar(m_pcFuncInfoArr->m_szFilePath));
		}
		for( int i = 0; i < nFuncInfoArrNum; i++ ){
			const CFuncInfo* pcFuncInfo = m_pcFuncInfoArr->GetAt(i);
			if( pcFuncInfo->IsAddClipText() ){
				nBuffLen += pcFuncInfo->m_cmemFuncName.GetStringLength() + pcFuncInfo->m_nDepth * 2;
				nCount++;
			}
		}
		m_cmemClipText.AllocStringBuffer( nBuffLen + nBuffLenTag * nCount );
	}

	for (i = 0; i < nFuncInfoArrNum; i++){
		CFuncInfo* pcFuncInfo = m_pcFuncInfoArr->GetAt(i);

		/*	�V�����A�C�e�����쐬
			���݂̐e�̉��ɂԂ牺����`�ŁA�Ō�ɒǉ�����B
		*/
		HTREEITEM hItem;
		TV_INSERTSTRUCT cTVInsertStruct;
		cTVInsertStruct.hParent = phParentStack[ nStackPointer ];
		// 2016.04.24 TVI_LAST�͗v�f���������Ƃ������x���BTVI_FIRST���g����Ń\�[�g���Ȃ���
		cTVInsertStruct.hInsertAfter = TVI_FIRST;
		cTVInsertStruct.item.mask = TVIF_TEXT | TVIF_PARAM;
		cTVInsertStruct.item.pszText = pcFuncInfo->m_cmemFuncName.GetStringPtr();
		cTVInsertStruct.item.lParam = i;	//	���Ƃł��̐��l�i��m_pcFuncInfoArr�̉��Ԗڂ̃A�C�e�����j�����āA�ړI�n�ɃW�����v���邺!!�B

		/*	�e�q�֌W���`�F�b�N
		*/
		if (nStackPointer != pcFuncInfo->m_nDepth){
			//	���x�����ς��܂���!!
			//	�����A2�i�K�[���Ȃ邱�Ƃ͍l�����Ă��Ȃ��̂Œ��ӁB
			//	�@�������A2�i�K�ȏ�󂭂Ȃ邱�Ƃ͍l���ς݁B

			// 2002.11.10 Moca �ǉ� �m�ۂ����T�C�Y�ł͑���Ȃ��Ȃ����B�Ċm��
			if( nStackDepth <= pcFuncInfo->m_nDepth + 1 ){
				nStackDepth = pcFuncInfo->m_nDepth + 4; // ���߂Ɋm�ۂ��Ă���
				HTREEITEM* phTi;
				phTi = (HTREEITEM*)realloc( phParentStack, nStackDepth * sizeof( HTREEITEM ) );
				if( NULL != phTi ){
					phParentStack = phTi;
				}else{
					goto end_of_func;
				}
			}
			nStackPointer = pcFuncInfo->m_nDepth;
			cTVInsertStruct.hParent = phParentStack[ nStackPointer ];
		}
		hItem = TreeView_InsertItem( hwndTree, &cTVInsertStruct );
		phParentStack[ nStackPointer+1 ] = hItem;

		/*	pcFuncInfo�ɓo�^����Ă���s���A�����m�F���āA�I������A�C�e�����l����
		*/
		bool bFileSelect = false;
		if( pcFuncInfo->m_cmemFileName.GetStringPtr() && m_pcFuncInfoArr->m_szFilePath[0] ){
			if( 0 == auto_stricmp( pcFuncInfo->m_cmemFileName.GetStringPtr(), m_pcFuncInfoArr->m_szFilePath.c_str() ) ){
				bFileSelect = true;
			}
		}else{
			bFileSelect = true;
		}
		if( bFileSelect ){
			if( !bSelected ){
				if( pcFuncInfo->m_nFuncLineLAYOUT < nFuncLineTop
					|| (pcFuncInfo->m_nFuncLineLAYOUT == nFuncLineTop && pcFuncInfo->m_nFuncColLAYOUT <= nFuncColTop) ){
					nFuncLineTop = pcFuncInfo->m_nFuncLineLAYOUT;
					nFuncColTop = pcFuncInfo->m_nFuncColLAYOUT;
					hItemSelectedTop = hItem;
				}
			}
			if( (nFuncLineOld < pcFuncInfo->m_nFuncLineLAYOUT
				|| (nFuncLineOld == pcFuncInfo->m_nFuncColLAYOUT && nFuncColOld <= pcFuncInfo->m_nFuncColLAYOUT))
			  && (pcFuncInfo->m_nFuncLineLAYOUT < m_nCurLine
				|| (pcFuncInfo->m_nFuncLineLAYOUT == m_nCurLine && pcFuncInfo->m_nFuncColLAYOUT <= m_nCurCol)) ){
				nFuncLineOld = pcFuncInfo->m_nFuncLineLAYOUT;
				nFuncColOld = pcFuncInfo->m_nFuncColLAYOUT;
				hItemSelected = hItem;
				bSelected = true;
			}
		}

		/* �N���b�v�{�[�h�R�s�[�p�e�L�X�g���쐬���� */
		//	2003.06.22 Moca dummy�v�f�̓c���[�ɓ���邪TAGJUMP�ɂ͉����Ȃ�
		if( pcFuncInfo->IsAddClipText() ){
			CNativeT text;
			if( tagjump ){
				const TCHAR* pszFileName = pcFuncInfo->m_cmemFileName.GetStringPtr();
				if( pszFileName == NULL ){
					pszFileName = m_pcFuncInfoArr->m_szFilePath;
				}
				text.AllocStringBuffer(
					  pcFuncInfo->m_cmemFuncName.GetStringLength()
					+ nStackPointer * 2 + 1
					+ _tcslen( pszFileName )
					+ 20
				);
				//	2007.03.04 genta �^�O�W�����v�ł���`���ŏ�������
				text.AppendString( pszFileName );
				
				if( 0 < pcFuncInfo->m_nFuncLineCRLF ){
					TCHAR linenum[32];
					int len = auto_sprintf( linenum, _T("(%d,%d): "),
						pcFuncInfo->m_nFuncLineCRLF,				/* ���o�s�ԍ� */
						pcFuncInfo->m_nFuncColCRLF					/* ���o���ԍ� */
					);
					text.AppendString( linenum );
				}
			}

			if( !nolabel ){
				for( int cnt = 0; cnt < nStackPointer; cnt++ ){
					text.AppendString(_T("  "));
				}
				text.AppendString(_T(" "));
				
				text.AppendNativeData( pcFuncInfo->m_cmemFuncName );
			}
			text.AppendString( _T("\r\n") );
			m_cmemClipText.AppendNativeDataT( text );	/* �N���b�v�{�[�h�R�s�[�p�e�L�X�g */
		}
	}

end_of_func:;

	::EnableWindow( ::GetDlgItem( GetHwnd() , IDC_BUTTON_COPY ), TRUE );

	if( NULL != hItemSelected ){
		/* ���݃J�[�\���ʒu�̃��\�b�h��I����Ԃɂ��� */
		TreeView_SelectItem( hwndTree, hItemSelected );
	}else if( NULL != hItemSelectedTop ){
		TreeView_SelectItem( hwndTree, hItemSelectedTop );
	}

	free( phParentStack );
}



void CDlgFuncList::SetDocLineFuncList()
{
	if( m_nOutlineType == OUTLINE_BOOKMARK ){
		return;
	}
	if( m_nOutlineType == OUTLINE_FILETREE ){
		return;
	}
	CEditView* pcEditView=(CEditView*)m_lParam;
	CDocLineMgr* pcDocLineMgr = &pcEditView->GetDocument()->m_cDocLineMgr;
	
	CFuncListManager().ResetAllFucListMark(pcDocLineMgr, false);
	int i;
	int num = m_pcFuncInfoArr->GetNum();
	for( i = 0; i < num; ++i ){
		const CFuncInfo* pcFuncInfo = m_pcFuncInfoArr->GetAt(i);
		if( 0 < pcFuncInfo->m_nFuncLineCRLF ){
			CDocLine* pcDocLine = pcDocLineMgr->GetLine( pcFuncInfo->m_nFuncLineCRLF - 1 );
			if( pcDocLine ){
				CFuncListManager().SetLineFuncList( pcDocLine, true );
			}
		}
	}
}



/*! �t�@�C���c���[�쐬
	@note m_pcFuncInfoArr�Ƀt���p�X�����������݂c���[���쐬
*/
void CDlgFuncList::SetTreeFile()
{
	HWND hwndTree = ::GetDlgItem( GetHwnd(), IDC_TREE_FL );

	m_cmemClipText.SetString(L"");
	SFilePath IniDirPath;
	LoadFileTreeSetting( m_fileTreeSetting, IniDirPath );
	m_pcFuncInfoArr->Empty();
	int nFuncInfo = 0;
	std::vector<HTREEITEM> hParentTree;
	hParentTree.push_back(TVI_ROOT);
	for( int i = 0; i < (int)m_fileTreeSetting.m_aItems.size(); i++ ){
		TCHAR szPath[_MAX_PATH];
		TCHAR szPath2[_MAX_PATH];
		const SFileTreeItem& item = m_fileTreeSetting.m_aItems[i];
		// item.m_szTargetPath => szPath ���^�����̓W�J
		if( !CFileNameManager::ExpandMetaToFolder(item.m_szTargetPath, szPath, _countof(szPath)) ){
			auto_strcpy_s(szPath, _countof(szPath), _T("<Error:Long Path>"));
		}
		// szPath => szPath2 <iniroot>�W�J
		const TCHAR* pszFrom = szPath;
		if( m_fileTreeSetting.m_szLoadProjectIni[0] != _T('\0')){
			CNativeT strTemp(pszFrom);
			strTemp.Replace(_T("<iniroot>"), IniDirPath);
			if( _countof(szPath2) <= strTemp.GetStringLength() ){
				auto_strcpy_s(szPath2, _countof(szPath), _T("<Error:Long Path>"));
			}else{
				auto_strcpy_s(szPath2, _countof(szPath), strTemp.GetStringPtr());
			}
		}else{
			auto_strcpy(szPath2, pszFrom);
		}
		// szPath2 => szPath �u.�v��V���[�g�p�X���̓W�J
		pszFrom = szPath2;
		if( ::GetLongFileName(pszFrom, szPath) ){
		}else{
			auto_strcpy(szPath, pszFrom);
		}
		while( item.m_nDepth < (int)hParentTree.size() - 1 ){
			hParentTree.resize(hParentTree.size() - 1);
		}
		const TCHAR* pszLabel = szPath;
		if( item.m_szLabelName[0] != _T('\0') ){
			pszLabel = item.m_szLabelName;
		}
		// lvis.item.lParam
		// 0 �ȉ�(nFuncInfo): m_pcFuncInfoArr->At(nFuncInfo)�Ƀt�@�C����
		// -1: Grep�̃t�@�C�����v�f
		// -2: Grep�̃T�u�t�H���_�v�f
		// -(nFuncInfo * 10 + 3): Grep���[�g�t�H���_�v�f
		// -4: �f�[�^�E�ǉ�����Ȃ�
		TVINSERTSTRUCT tvis;
		tvis.hParent      = hParentTree.back();
		tvis.item.mask    = TVIF_TEXT | TVIF_PARAM;
		if( item.m_eFileTreeItemType == EFileTreeItemType_Grep ){
			m_pcFuncInfoArr->AppendData( CLogicInt(-1), CLogicInt(-1), CLayoutInt(-1), CLayoutInt(-1), _T(""), szPath, 0, 0 );
			tvis.item.pszText = const_cast<TCHAR*>(pszLabel);
			tvis.item.lParam  = -(nFuncInfo * 10 + 3);
			HTREEITEM hParent = TreeView_InsertItem(hwndTree, &tvis);
			nFuncInfo++;
			SetTreeFileSub( hParent, NULL );
		}else if( item.m_eFileTreeItemType == EFileTreeItemType_File ){
			m_pcFuncInfoArr->AppendData( CLogicInt(-1), CLogicInt(-1), CLayoutInt(-1), CLayoutInt(-1), _T(""), szPath, 0, 0 );
			tvis.item.pszText = const_cast<TCHAR*>(pszLabel);
			tvis.item.lParam  = nFuncInfo;
			TreeView_InsertItem(hwndTree, &tvis);
			nFuncInfo++;
		}else if( item.m_eFileTreeItemType == EFileTreeItemType_Folder ){
			pszLabel = item.m_szLabelName;
			if( pszLabel[0] == _T('\0') ){
				pszLabel = _T("Folder");
			}
			tvis.item.pszText = const_cast<TCHAR*>(pszLabel);
			tvis.item.lParam  = -4;
			HTREEITEM hParent = TreeView_InsertItem(hwndTree, &tvis);
			hParentTree.push_back(hParent);
		}
	}
}


void CDlgFuncList::SetTreeFileSub( HTREEITEM hParent, const TCHAR* pszFile )
{
	HWND hwndTree = ::GetDlgItem( GetHwnd(), IDC_TREE_FL );

	if( NULL != TreeView_GetChild( hwndTree, hParent ) ){
		return;
	}

	HTREEITEM hItemSelected = NULL;

	std::tstring basePath;
	int nItem = 0; // �ݒ�Item�ԍ�
	if( !GetTreeFileFullName( hwndTree, hParent, &basePath, &nItem ) ){
		return; // error
	}

	int count = 0;
	CGrepEnumKeys cGrepEnumKeys;
	int errNo = cGrepEnumKeys.SetFileKeys( m_fileTreeSetting.m_aItems[nItem].m_szTargetFile );
	if( errNo != 0 ){
		TVINSERTSTRUCT tvis;
		tvis.hParent      = hParent;
		tvis.item.mask    = TVIF_TEXT | TVIF_PARAM | TVIF_CHILDREN;
		tvis.item.pszText = const_cast<TCHAR*>(_T("<Wild Card Error>"));
		tvis.item.lParam = -4;
		TreeView_InsertItem(hwndTree, &tvis);
		return;
	}
	CGrepEnumOptions cGrepEnumOptions;
	cGrepEnumOptions.m_bIgnoreHidden   = m_fileTreeSetting.m_aItems[nItem].m_bIgnoreHidden;
	cGrepEnumOptions.m_bIgnoreReadOnly = m_fileTreeSetting.m_aItems[nItem].m_bIgnoreReadOnly;
	cGrepEnumOptions.m_bIgnoreSystem   = m_fileTreeSetting.m_aItems[nItem].m_bIgnoreSystem;
	CGrepEnumFiles cGrepExceptAbsFiles;
	cGrepExceptAbsFiles.Enumerates(_T(""), cGrepEnumKeys.m_vecExceptAbsFileKeys, cGrepEnumOptions);
	CGrepEnumFolders cGrepExceptAbsFolders;
	cGrepExceptAbsFolders.Enumerates(_T(""), cGrepEnumKeys.m_vecExceptAbsFolderKeys, cGrepEnumOptions);

	//�t�H���_�ꗗ�쐬
	CGrepEnumFilterFolders cGrepEnumFilterFolders;
	cGrepEnumFilterFolders.Enumerates( basePath.c_str(), cGrepEnumKeys, cGrepEnumOptions, cGrepExceptAbsFolders );
	int nItemCount = cGrepEnumFilterFolders.GetCount();
	count = nItemCount;
	for( int i = 0; i < nItemCount; i++ ){
		TVINSERTSTRUCT tvis;
		tvis.hParent      = hParent;
		tvis.item.mask    = TVIF_TEXT | TVIF_PARAM | TVIF_CHILDREN;
		tvis.item.pszText = const_cast<TCHAR*>(cGrepEnumFilterFolders.GetFileName(i));
		tvis.item.lParam  = -2;
		tvis.item.cChildren = 1; // �_�~�[�̎q�v�f����������[+]��\��
		TreeView_InsertItem(hwndTree, &tvis);
	}

	//�t�@�C���ꗗ�쐬
	CGrepEnumFilterFiles cGrepEnumFilterFiles;
	cGrepEnumFilterFiles.Enumerates( basePath.c_str(), cGrepEnumKeys, cGrepEnumOptions, cGrepExceptAbsFiles );
	nItemCount = cGrepEnumFilterFiles.GetCount();
	count += nItemCount;
	for( int i = 0; i < nItemCount; i ++ ){
		const TCHAR* pFile = cGrepEnumFilterFiles.GetFileName(i);
		TVINSERTSTRUCT tvis;
		tvis.hParent      = hParent;
		tvis.item.mask    = TVIF_TEXT | TVIF_PARAM;
		tvis.item.pszText = const_cast<TCHAR*>(pFile);
		tvis.item.lParam  = -1;
		HTREEITEM hItem = TreeView_InsertItem(hwndTree, &tvis);
		if( pszFile && auto_stricmp(pszFile, pFile) == 0 ){
			hItemSelected = hItem;
		}
	}
	if( hItemSelected ){
		TreeView_SelectItem( hwndTree, hItemSelected );
	}
	if( count == 0 ){
		// [+]�L���폜
		TVITEM item;
		item.mask  = TVIF_HANDLE | TVIF_CHILDREN;
		item.cChildren = 0;
		item.hItem = hParent;
		TreeView_SetItem(hwndTree, &item);
	}
}

BOOL CDlgFuncList::OnInitDialog( HWND hwndDlg, WPARAM wParam, LPARAM lParam )
{
	m_bStretching = false;
	m_bHovering = false;
	m_nHilightedBtn = -1;
	m_nCapturingBtn = -1;

	_SetHwnd( hwndDlg );

	HWND		hwndList;
	int			nCxVScroll;
	int			nColWidthArr[] = { 0, 10, 46, 80 };
	RECT		rc;
	LV_COLUMN	col;
	hwndList = ::GetDlgItem( hwndDlg, IDC_LIST_FL );
	::SetWindowLongPtr(hwndList, GWL_STYLE, ::GetWindowLongPtr(hwndList, GWL_STYLE) | LVS_SHOWSELALWAYS );
	// 2005.10.21 zenryaku 1�s�I��
	ListView_SetExtendedListViewStyle(hwndList,
		ListView_GetExtendedListViewStyle(hwndList) | LVS_EX_FULLROWSELECT | LVS_EX_INFOTIP);

	::GetWindowRect( hwndList, &rc );
	nCxVScroll = ::GetSystemMetrics( SM_CXVSCROLL );

	col.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
	col.fmt = LVCFMT_LEFT;
	col.cx = rc.right - rc.left - ( nColWidthArr[1] + nColWidthArr[2] + nColWidthArr[3] ) - nCxVScroll - 8;
	//	Apr. 23, 2005 genta �s�ԍ������[��
	col.pszText = const_cast<TCHAR*>(LS(STR_DLGFNCLST_LIST_LINE_M));
	col.iSubItem = FL_COL_ROW;
	ListView_InsertColumn( hwndList, FL_COL_ROW, &col);

	// 2010.03.17 syat ���ǉ�
	col.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
	col.fmt = LVCFMT_LEFT;
	col.cx = nColWidthArr[FL_COL_COL];
	col.pszText = const_cast<TCHAR*>(LS(STR_DLGFNCLST_LIST_COL));
	col.iSubItem = FL_COL_COL;
	ListView_InsertColumn( hwndList, FL_COL_COL, &col);

	col.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
	col.fmt = LVCFMT_LEFT;
	col.cx = nColWidthArr[FL_COL_NAME];
	//	Apr. 23, 2005 genta �s�ԍ������[��
	col.pszText = const_cast<TCHAR*>(LS(STR_DLGFNCLST_LIST_FUNC));
	col.iSubItem = FL_COL_NAME;
	ListView_InsertColumn( hwndList, FL_COL_NAME, &col);

	col.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
	col.fmt = LVCFMT_LEFT;
	col.cx = nColWidthArr[FL_COL_REMARK];
	col.pszText = const_cast<TCHAR*>(_T(" "));
	col.iSubItem = FL_COL_REMARK;
	ListView_InsertColumn( hwndList, FL_COL_REMARK, &col);

	/* �A�E�g���C���ʒu�ƃT�C�Y������������ */ // 20060201 aroka
	CEditView* pcEditView=(CEditView*)m_lParam;
	if( pcEditView != NULL ){
		if( !IsDocking() && m_pShareData->m_Common.m_sOutline.m_bRememberOutlineWindowPos ){
			WINDOWPLACEMENT cWindowPlacement;
			cWindowPlacement.length = sizeof( cWindowPlacement );
			if (::GetWindowPlacement( pcEditView->m_pcEditWnd->GetHwnd(), &cWindowPlacement )){
				/* �E�B���h�E�ʒu�E�T�C�Y��-1�ȊO�̒l�ɂ��Ă����ƁACDialog�Ŏg�p�����D */
				m_xPos = m_pShareData->m_Common.m_sOutline.m_xOutlineWindowPos + cWindowPlacement.rcNormalPosition.left;
				m_yPos = m_pShareData->m_Common.m_sOutline.m_yOutlineWindowPos + cWindowPlacement.rcNormalPosition.top;
				m_nWidth =  m_pShareData->m_Common.m_sOutline.m_widthOutlineWindow;
				m_nHeight = m_pShareData->m_Common.m_sOutline.m_heightOutlineWindow;
			}
		}else if( IsDocking() ){
			m_xPos = 0;
			m_yPos = 0;
			m_nShowCmd = SW_HIDE;
			::GetWindowRect( ::GetParent(pcEditView->GetHwnd()), &rc );	// �����ł͂܂� GetDockSpaceRect() �͎g���Ȃ�
			EDockSide eDockSide = GetDockSide();
			switch( eDockSide ){
			case DOCKSIDE_LEFT:		m_nWidth = ProfDockLeft();		break;
			case DOCKSIDE_TOP:		m_nHeight = ProfDockTop();		break;
			case DOCKSIDE_RIGHT:	m_nWidth = ProfDockRight();		break;
			case DOCKSIDE_BOTTOM:	m_nHeight = ProfDockBottom();	break;
			}
			if( eDockSide == DOCKSIDE_LEFT || eDockSide == DOCKSIDE_RIGHT ){
				if( m_nWidth == 0 )	// ����
					m_nWidth = (rc.right - rc.left) / 3;
				if( m_nWidth > rc.right - rc.left - DOCK_MIN_SIZE ) m_nWidth = rc.right - rc.left - DOCK_MIN_SIZE;
				if( m_nWidth < DOCK_MIN_SIZE ) m_nWidth = DOCK_MIN_SIZE;
			}else{
				if( m_nHeight == 0 )	// ����
					m_nHeight = (rc.bottom - rc.top) / 3;
				if( m_nHeight > rc.bottom - rc.top - DOCK_MIN_SIZE ) m_nHeight = rc.bottom - rc.top - DOCK_MIN_SIZE;
				if( m_nHeight < DOCK_MIN_SIZE ) m_nHeight = DOCK_MIN_SIZE;
			}
		}
	}

	if( !m_bInChangeLayout ){	// ChangeLayout() �������͐ݒ�ύX���Ȃ�
		bool bType = (ProfDockSet() != 0);
		if( bType ){
			CDocTypeManager().GetTypeConfig(CTypeConfig(m_nDocType), m_type);
		}
		ProfDockDisp() = TRUE;
		if( bType ){
			SetTypeConfig( CTypeConfig(m_nDocType), m_type );

		}
		// ���E�B���h�E�ɕύX��ʒm����
		if( ProfDockSync() ){
			HWND hwndEdit = pcEditView->m_pcEditWnd->GetHwnd();
			PostOutlineNotifyToAllEditors( (WPARAM)0, (LPARAM)hwndEdit );
		}
	}

	if( !IsDocking() ){
		/* ���N���X�����o */
		CreateSizeBox();

		LONG_PTR lStyle = ::GetWindowLongPtr( GetHwnd(), GWL_STYLE );
		::SetWindowLongPtr( GetHwnd(), GWL_STYLE, lStyle | WS_THICKFRAME );
		::SetWindowPos( GetHwnd(), NULL, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED );
	}

	m_hwndToolTip = NULL;
	if( IsDocking() ){
		//�c�[���`�b�v���쐬����B�i�u����v�Ȃǂ̃{�^���p�j
		m_hwndToolTip = ::CreateWindowEx(
			0,
			TOOLTIPS_CLASS,
			NULL,
			WS_POPUP | TTS_NOPREFIX | TTS_ALWAYSTIP,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			GetHwnd(),
			NULL,
			m_hInstance,
			NULL
			);

		// �c�[���`�b�v���}���`���C���\�ɂ���iSHRT_MAX: Win95��INT_MAX���ƕ\������Ȃ��j
		Tooltip_SetMaxTipWidth( m_hwndToolTip, SHRT_MAX );

		// �A�E�g���C���Ƀc�[���`�b�v��ǉ�����
		TOOLINFO	ti;
		ti.cbSize      = CCSIZEOF_STRUCT(TOOLINFO, lpszText);
		ti.uFlags      = TTF_SUBCLASS | TTF_IDISHWND;	// TTF_IDISHWND: uId �� HWND �� rect �͖����iHWND �S�́j
		ti.hwnd        = GetHwnd();
		ti.hinst       = m_hInstance;
		ti.uId         = (UINT_PTR)GetHwnd();
		ti.lpszText    = NULL;
		ti.rect.left   = 0;
		ti.rect.top    = 0;
		ti.rect.right  = 0;
		ti.rect.bottom = 0;
		Tooltip_AddTool( m_hwndToolTip, &ti );

		// �s�v�ȃR���g���[�����B��
		HWND hwndPrev;
		HWND hwnd = ::GetWindow( GetHwnd(), GW_CHILD );
		while( hwnd ){
			int nId = ::GetDlgCtrlID( hwnd );
			hwndPrev = hwnd;
			hwnd = ::GetWindow( hwnd, GW_HWNDNEXT );
			switch( nId ){
			case IDC_STATIC_nSortType:
			case IDC_COMBO_nSortType:
			case IDC_LIST_FL:
			case IDC_TREE_FL:
				continue;
			}
			ShowWindow( hwndPrev, SW_HIDE );
		}
	}

	SyncColor();

	::GetWindowRect( hwndDlg, &rc );
	m_ptDefaultSize.x = rc.right - rc.left;
	m_ptDefaultSize.y = rc.bottom - rc.top;
	
	::GetClientRect( hwndDlg, &rc );
	m_ptDefaultSizeClient.x = rc.right;
	m_ptDefaultSizeClient.y = rc.bottom;

	for( int i = 0; i < _countof(anchorList); i++ ){
		GetItemClientRect( anchorList[i].id, m_rcItems[i] );
		// �h�b�L���O���̓E�B���h�E�������ς��܂ŐL�΂�
		if( IsDocking() ){
			if( anchorList[i].anchor == ANCHOR_ALL ){
				::GetClientRect( hwndDlg, &rc );
				m_rcItems[i].right = rc.right;
				m_rcItems[i].bottom = rc.bottom;
			}
		}
	}

#if REI_MOD_OUTLINEDLG
	// �t�H���g�ݒ�
	{
		HFONT hFontOld = (HFONT)::SendMessageAny( GetItemHwnd( IDC_TREE_FL ), WM_GETFONT, 0, 0 );
		HFONT hFont = SetMainFont( GetItemHwnd( IDC_TREE_FL ) );
		m_cFontText[0].SetFont( hFontOld, hFont, GetItemHwnd( IDC_TREE_FL ) );
	}
	{
		HFONT hFontOld = (HFONT)::SendMessageAny( GetItemHwnd( IDC_LIST_FL ), WM_GETFONT, 0, 0 );
		HFONT hFont = SetMainFont( GetItemHwnd( IDC_LIST_FL ) );
		m_cFontText[1].SetFont( hFontOld, hFont, GetItemHwnd( IDC_LIST_FL ) );
	}
#endif  // rei_

	return CDialog::OnInitDialog( hwndDlg, wParam, lParam );
}


BOOL CDlgFuncList::OnBnClicked( int wID )
{
	switch( wID ){
	case IDC_BUTTON_MENU:
		RECT rcMenu;
		GetWindowRect( ::GetDlgItem( GetHwnd(), IDC_BUTTON_MENU ), &rcMenu );
		POINT ptMenu;
		ptMenu.x = rcMenu.left;
		ptMenu.y = rcMenu.bottom;
		DoMenu( ptMenu, GetHwnd() );
		return TRUE;
	case IDC_BUTTON_HELP:
		/* �u�A�E�g���C����́v�̃w���v */
		//Apr. 5, 2001 JEPRO �C���R���ǉ� (Stonee, 2001/03/12 ��l�������A�@�\�ԍ�����w���v�g�s�b�N�ԍ��𒲂ׂ�悤�ɂ���)
		MyWinHelp( GetHwnd(), HELP_CONTEXT, ::FuncID_To_HelpContextID(F_OUTLINE) );	// 2006.10.10 ryoji MyWinHelp�ɕύX�ɕύX
		return TRUE;
	case IDOK:
		return OnJump();
	case IDCANCEL:
		if( m_bModal ){		/* ���[�_�� �_�C�A���O�� */
			::EndDialog( GetHwnd(), 0 );
		}else{
			if( IsDocking() ){
				::SetFocus( ((CEditView*)m_lParam)->GetHwnd() );
			}else{
				::DestroyWindow( GetHwnd() );
			}
		}
		return TRUE;
	case IDC_BUTTON_COPY:
		// Windows�N���b�v�{�[�h�ɃR�s�[ 
		// 2004.02.17 Moca �֐���
		SetClipboardText( GetHwnd(), m_cmemClipText.GetStringPtr(), m_cmemClipText.GetStringLength() );
		return TRUE;
	case IDC_BUTTON_WINSIZE:
		{// �E�B���h�E�̈ʒu�ƃT�C�Y���L�� // 20060201 aroka
			m_pShareData->m_Common.m_sOutline.m_bRememberOutlineWindowPos = ::IsDlgButtonChecked( GetHwnd(), IDC_BUTTON_WINSIZE );
		}
		// �{�^����������Ă��邩�͂����肳���� 2008/6/5 Uchi
		::DlgItem_SetText( GetHwnd(), IDC_BUTTON_WINSIZE,
			m_pShareData->m_Common.m_sOutline.m_bRememberOutlineWindowPos ? _T("��") : _T("��") );
		return TRUE;
	//2002.02.08 �I�v�V�����ؑ֌�List/Tree�Ƀt�H�[�J�X�ړ�
	case IDC_CHECK_bAutoCloseDlgFuncList:
	case IDC_CHECK_bMarkUpBlankLineEnable:
	case IDC_CHECK_bFunclistSetFocusOnJump:
		m_pShareData->m_Common.m_sOutline.m_bAutoCloseDlgFuncList = ::IsDlgButtonChecked( GetHwnd(), IDC_CHECK_bAutoCloseDlgFuncList );
		m_pShareData->m_Common.m_sOutline.m_bMarkUpBlankLineEnable = ::IsDlgButtonChecked( GetHwnd(), IDC_CHECK_bMarkUpBlankLineEnable );
		m_pShareData->m_Common.m_sOutline.m_bFunclistSetFocusOnJump = ::IsDlgButtonChecked( GetHwnd(), IDC_CHECK_bFunclistSetFocusOnJump );
		if(m_pShareData->m_Common.m_sOutline.m_bAutoCloseDlgFuncList){
			::EnableWindow( ::GetDlgItem( GetHwnd(), IDC_CHECK_bFunclistSetFocusOnJump ), FALSE );
		}else{
			::EnableWindow( ::GetDlgItem( GetHwnd(), IDC_CHECK_bFunclistSetFocusOnJump ), TRUE );
		}
		if(wID==IDC_CHECK_bMarkUpBlankLineEnable&&m_nListType==OUTLINE_BOOKMARK){
			CEditView* pcEditView=(CEditView*)m_lParam;
			pcEditView->GetCommander().HandleCommand( F_BOOKMARK_VIEW, true, TRUE, 0, 0, 0 );
			m_nCurLine=pcEditView->GetCaret().GetCaretLayoutPos().GetY2() + CLayoutInt(1);
			CDocTypeManager().GetTypeConfig(pcEditView->GetDocument()->m_cDocType.GetDocumentType(), m_type);
			SetData();
		}else
		if(m_nViewType == VIEWTYPE_TREE){
			::SetFocus( ::GetDlgItem( GetHwnd(), IDC_TREE_FL ) );
		}else{
			::SetFocus( ::GetDlgItem( GetHwnd(), IDC_LIST_FL ) );
		}
		return TRUE;
	case IDC_BUTTON_SETTING:
		{
			CDlgFileTree cDlgFileTree;
			int nRet = cDlgFileTree.DoModal( G_AppInstance(), GetHwnd(), (LPARAM)this );
			if( nRet == TRUE ){
				EFunctionCode nFuncCode = GetFuncCodeRedraw(m_nOutlineType);
				CEditView* pcEditView = (CEditView*)m_lParam;
				pcEditView->GetCommander().HandleCommand( nFuncCode, true, SHOW_RELOAD, 0, 0, 0 );
			}
		}
	}
	/* ���N���X�����o */
	return CDialog::OnBnClicked( wID );
}

BOOL CDlgFuncList::OnNotify( WPARAM wParam, LPARAM lParam )
{
//	int				idCtrl;
	LPNMHDR			pnmh;
	NM_LISTVIEW*	pnlv;
	HWND			hwndList;
	HWND			hwndTree;
	NM_TREEVIEW*	pnmtv;
//	int				nLineTo;

//	idCtrl = (int) wParam;
	pnmh = (LPNMHDR) lParam;
	pnlv = (NM_LISTVIEW*)lParam;

	CEditView* pcEditView=(CEditView*)m_lParam;
	hwndList = ::GetDlgItem( GetHwnd(), IDC_LIST_FL );
	hwndTree = ::GetDlgItem( GetHwnd(), IDC_TREE_FL );

	if( hwndTree == pnmh->hwndFrom ){
		pnmtv = (NM_TREEVIEW *) lParam;
		switch( pnmtv->hdr.code ){
		case NM_CLICK:
			if( IsDocking() ){
				// ���̎��_�ł͂܂��I��ύX����Ă��Ȃ��� OnJump() �̗\������Ƃ��Đ�ɑI��ύX���Ă���
				TVHITTESTINFO tvht = {0};
				::GetCursorPos( &tvht.pt );
				::ScreenToClient( hwndTree, &tvht.pt );
				TreeView_HitTest( hwndTree, &tvht );
				if( (tvht.flags & TVHT_ONITEM) && tvht.hItem ){
					TreeView_SelectItem( hwndTree, tvht.hItem );
					OnJump( false, false );
					return TRUE;
				}
			}
			break;
		case NM_DBLCLK:
			// 2002.02.16 hor Tree�̃_�u���N���b�N�Ńt�H�[�J�X�ړ��ł���悤�� 3/4
			OnJump();
			m_bWaitTreeProcess=true;
			::SetWindowLongPtr( GetHwnd(), DWLP_MSGRESULT, TRUE );	// �c���[�̓W�J�^�k�������Ȃ�
			return TRUE;
			//return OnJump();
		case TVN_KEYDOWN:
			if( ((TV_KEYDOWN *)lParam)->wVKey == VK_SPACE ){
				OnJump( false );
				return TRUE;
			}
			Key2Command( ((TV_KEYDOWN *)lParam)->wVKey );
			return TRUE;
		case NM_KILLFOCUS:
			// 2002.02.16 hor Tree�̃_�u���N���b�N�Ńt�H�[�J�X�ړ��ł���悤�� 4/4
			if(m_bWaitTreeProcess){
				if(m_pShareData->m_Common.m_sOutline.m_bFunclistSetFocusOnJump){
					::SetFocus( pcEditView->GetHwnd() );
				}
				m_bWaitTreeProcess=false;
			}
			return TRUE;
		}
	}else
	if( hwndList == pnmh->hwndFrom ){
		switch( pnmh->code ){
		case LVN_COLUMNCLICK:
//			MYTRACE( _T("LVN_COLUMNCLICK\n") );
			m_nSortCol =  pnlv->iSubItem;
			if( m_nSortCol == m_nSortColOld ){
				m_bSortDesc = !m_bSortDesc;
			}
			m_nSortColOld = m_nSortCol;
			{
				STypeConfig* type = new STypeConfig();
				CDocTypeManager().GetTypeConfig( CTypeConfig(m_nDocType), *type );
				type->m_nOutlineSortCol = m_nSortCol;
				type->m_bOutlineSortDesc = m_bSortDesc;
				SetTypeConfig( CTypeConfig(m_nDocType), *type );
				delete type;
			}
			//	Apr. 23, 2005 genta �֐��Ƃ��ēƗ�������
			SortListView( hwndList, m_nSortCol );
			return TRUE;
		case NM_CLICK:
			if( IsDocking() ){
				OnJump( false, false );
				return TRUE;
			}
			break;
		case NM_DBLCLK:
				OnJump();
			return TRUE;
		case LVN_KEYDOWN:
			if( ((LV_KEYDOWN *)lParam)->wVKey == VK_SPACE ){
				OnJump( false );
				return TRUE;
			}
			Key2Command( ((LV_KEYDOWN *)lParam)->wVKey );
			return TRUE;
		}
	}

#ifdef DEFINE_SYNCCOLOR
#if REI_MOD_OUTLINEDLG
  bool dock_color_sync = !!RegGetDword(L"OutlineDockColorDefault", 0);
  if (dock_color_sync)
#endif // rei_
	if( IsDocking() ){
		if( hwndList == pnmh->hwndFrom || hwndTree == pnmh->hwndFrom ){
			if( pnmh->code == NM_CUSTOMDRAW ){
				LPNMCUSTOMDRAW lpnmcd = (LPNMCUSTOMDRAW)lParam;
				switch( lpnmcd->dwDrawStage ){
				case CDDS_PREPAINT:
					::SetWindowLongPtr( GetHwnd(), DWLP_MSGRESULT, CDRF_NOTIFYITEMDRAW );
					break;
				case CDDS_ITEMPREPAINT:
					{	// �I���A�C�e���𔽓]�\���ɂ���
						const STypeConfig	*TypeDataPtr = &(pcEditView->m_pcEditDoc->m_cDocType.GetDocumentAttribute());
						COLORREF clrText = TypeDataPtr->m_ColorInfoArr[COLORIDX_TEXT].m_sColorAttr.m_cTEXT;
						COLORREF clrTextBk = TypeDataPtr->m_ColorInfoArr[COLORIDX_TEXT].m_sColorAttr.m_cBACK;
						if( hwndList == pnmh->hwndFrom ){
							//if( lpnmcd->uItemState & CDIS_SELECTED ){	// ��I���̃A�C�e�������ׂ� CDIS_SELECTED �ŗ���H
							if( ListView_GetItemState( hwndList, lpnmcd->dwItemSpec, LVIS_SELECTED ) ){
								((LPNMLVCUSTOMDRAW)lpnmcd)->clrText = clrText ^ RGB(255, 255, 255);
								((LPNMLVCUSTOMDRAW)lpnmcd)->clrTextBk = clrTextBk ^ RGB(255, 255, 255);
								lpnmcd->uItemState = 0;	// ���X�g�r���[�ɂ͑I���Ƃ��Ă̕`��������Ȃ��悤�ɂ���H
							}
						}else{
							if( lpnmcd->uItemState & CDIS_SELECTED ){
								((LPNMTVCUSTOMDRAW)lpnmcd)->clrText = clrText ^ RGB(255, 255, 255);
								((LPNMTVCUSTOMDRAW)lpnmcd)->clrTextBk = clrTextBk ^ RGB(255, 255, 255);
							}
						}
					}
					::SetWindowLongPtr( GetHwnd(), DWLP_MSGRESULT, CDRF_DODEFAULT );
					break;
				}

				return TRUE;
			}
		}
	}
#endif

	return FALSE;
}
/*!
	�w�肳�ꂽ�J�����Ń��X�g�r���[���\�[�g����D
	�����Ƀw�b�_������������D

	�\�[�g��̓t�H�[�J�X����ʓ��Ɍ����悤�ɕ\���ʒu�𒲐�����D

	@par �\���ʒu�����̏��Z
	EnsureVisible�̌��ʂ́C��X�N���[���̏ꍇ�͏�[�ɁC���X�N���[���̏ꍇ��
	���[�ɖړI�̍��ڂ������D�[���班�����������ꍇ�̓I�t�Z�b�g��^����K�v��
	���邪�C�X�N���[���������킩��Ȃ��Ɓ}���킩��Ȃ�
	���̂��ߍŏ��Ɉ�ԉ��Ɉ��X�N���[�������邱�Ƃ�EnsureVisible�ł�
	���Ȃ炸��X�N���[���ɂȂ�悤�ɂ��邱�ƂŁC�\�[�g��̕\���ʒu��
	�Œ肷��

	@param[in] hwndList	���X�g�r���[�̃E�B���h�E�n���h��
	@param[in] sortcol	�\�[�g����J�����ԍ�(0-2)

	@date 2005.04.23 genta �֐��Ƃ��ēƗ�������
	@date 2005.04.29 genta �\�[�g��̕\���ʒu����
	@date 2010.03.17 syat ���ǉ�
*/
void CDlgFuncList::SortListView(HWND hwndList, int sortcol)
{
	LV_COLUMN		col;
	int col_no;

	//	Apr. 23, 2005 genta �s�ԍ������[��

//	if( sortcol == 1 ){
	{
		col_no = FL_COL_NAME;
		col.mask = LVCF_TEXT;
	// From Here 2001.12.03 hor
	//	col.pszText = _T("�֐��� *");
		if(OUTLINE_BOOKMARK == m_nListType){
			col.pszText = const_cast<TCHAR*>( sortcol == col_no ? LS(STR_DLGFNCLST_LIST_TEXT_M) : LS(STR_DLGFNCLST_LIST_TEXT) );
		}else{
			col.pszText = const_cast<TCHAR*>( sortcol == col_no ? LS(STR_DLGFNCLST_LIST_FUNC_M) : LS(STR_DLGFNCLST_LIST_FUNC) );
		}
	// To Here 2001.12.03 hor
		col.iSubItem = 0;
		ListView_SetColumn( hwndList, col_no, &col );

		col_no = FL_COL_ROW;
		col.mask = LVCF_TEXT;
		col.pszText = const_cast<TCHAR*>( sortcol == col_no ? LS(STR_DLGFNCLST_LIST_LINE_M) : LS(STR_DLGFNCLST_LIST_LINE) );
		col.iSubItem = 0;
		ListView_SetColumn( hwndList, col_no, &col );

		// 2010.03.17 syat ���ǉ�
		col_no = FL_COL_COL;
		col.mask = LVCF_TEXT;
		col.pszText = const_cast<TCHAR*>( sortcol == col_no ? LS(STR_DLGFNCLST_LIST_COL_M) : LS(STR_DLGFNCLST_LIST_COL) );
		col.iSubItem = 0;
		ListView_SetColumn( hwndList, col_no, &col );

		col_no = FL_COL_REMARK;
	// From Here 2001.12.07 hor
		col.mask = LVCF_TEXT;
		col.pszText = const_cast<TCHAR*>( sortcol == col_no ? LS(STR_DLGFNCLST_LIST_M) : _T("") );
		col.iSubItem = 0;
		ListView_SetColumn( hwndList, col_no, &col );
	// To Here 2001.12.07 hor

		ListView_SortItems( hwndList, (m_bSortDesc ? CompareFunc_Desc : CompareFunc_Asc), (LPARAM)this );
	}
	//	2005.04.23 zenryaku �I�����ꂽ���ڂ�������悤�ɂ���

	//	Apr. 29, 2005 genta ��U��ԉ��ɃX�N���[��������
	ListView_EnsureVisible( hwndList,
		ListView_GetItemCount(hwndList) - 1,
		FALSE );
	
	//	Jan.  9, 2006 genta �擪����1�ڂ�2�ڂ̊֐���
	//	�I�����ꂽ�ꍇ�ɃX�N���[������Ȃ�����
	int keypos = ListView_GetNextItem(hwndList, -1, LVNI_FOCUSED) - 2;
	ListView_EnsureVisible( hwndList,
		keypos >= 0 ? keypos : 0,
		FALSE );
}

/*!	�E�B���h�E�T�C�Y���ύX���ꂽ

	@date 2003.06.22 Moca �R�[�h�̐���(�R���g���[���̏������@���e�[�u���Ɏ�������)
	@date 2003.08.16 genta �z���static��(���ʂȏ��������s��Ȃ�����)
*/
BOOL CDlgFuncList::OnSize( WPARAM wParam, LPARAM lParam )
{
	// ���̂Ƃ��� CEditWnd::OnSize() ����̌Ăяo���ł� lParam �� CEditWnd �� �� lParam �̂܂ܓn�����	// 2010.06.05 ryoji
	RECT rcDlg;
	::GetClientRect( GetHwnd(), &rcDlg );
	lParam = MAKELONG(rcDlg.right - rcDlg.left, rcDlg.bottom -  rcDlg.top);	// ���O�ŕ␳

	/* ���N���X�����o */
	CDialog::OnSize( wParam, lParam );

	RECT  rc;
	POINT ptNew;
	ptNew.x = rcDlg.right - rcDlg.left;
	ptNew.y = rcDlg.bottom - rcDlg.top;

	for( int i = 0 ; i < _countof(anchorList); i++ ){
		HWND hwndCtrl = GetItemHwnd(anchorList[i].id);
		ResizeItem( hwndCtrl, m_ptDefaultSizeClient, ptNew, m_rcItems[i], anchorList[i].anchor, (anchorList[i].anchor != ANCHOR_ALL));
//	2013.2.6 aroka ������h�~�p�̎��s����
		if(anchorList[i].anchor == ANCHOR_ALL){
			::UpdateWindow( hwndCtrl );
		}
	}

//	if( IsDocking() )
	{
		// �_�C�A���O�������ĕ`��i�c���[�^���X�g�͈̔͂͂�����Ȃ��悤�ɏ��O�j
		::InvalidateRect( GetHwnd(), NULL, FALSE );
		POINT pt;
		::GetWindowRect( ::GetDlgItem( GetHwnd(), IDC_TREE_FL ), &rc );
		pt.x = rc.left;
		pt.y = rc.top;
		::ScreenToClient( GetHwnd(), &pt );
		::OffsetRect( &rc, pt.x - rc.left, pt.y - rc.top );
		::ValidateRect( GetHwnd(), &rc );
	}
	return TRUE;
}

BOOL CDlgFuncList::OnMinMaxInfo( LPARAM lParam )
{
	LPMINMAXINFO lpmmi = (LPMINMAXINFO) lParam;
	if( m_ptDefaultSize.x < 0 ){
		return 0;
	}
	lpmmi->ptMinTrackSize.x = m_ptDefaultSize.x/2;
	lpmmi->ptMinTrackSize.y = m_ptDefaultSize.y/3;
	lpmmi->ptMaxTrackSize.x = m_ptDefaultSize.x*2;
	lpmmi->ptMaxTrackSize.y = m_ptDefaultSize.y*2;
	return 0;
}
int CALLBACK Compare_by_ItemData(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
	if( lParam1< lParam2 )
		return -1;
	if( lParam1 > lParam2 )
		return 1;
	else
		return 0;
}

BOOL CDlgFuncList::OnDestroy( void )
{
#if REI_MOD_OUTLINEDLG
	m_cFontText[0].ReleaseOnDestroy();
	m_cFontText[1].ReleaseOnDestroy();
#endif  // rei_
	CDialog::OnDestroy();

	/* �A�E�g���C�� ���ʒu�ƃT�C�Y���L������ */ // 20060201 aroka
	// �O������Fm_lParam �� CDialog::OnDestroy �ŃN���A����Ȃ�����
	CEditView* pcEditView=(CEditView*)m_lParam;
	HWND hwndEdit = pcEditView->m_pcEditWnd->GetHwnd();
	if( !IsDocking() && m_pShareData->m_Common.m_sOutline.m_bRememberOutlineWindowPos ){
		/* �e�̃E�B���h�E�ʒu�E�T�C�Y���L�� */
		WINDOWPLACEMENT cWindowPlacement;
		cWindowPlacement.length = sizeof( cWindowPlacement );
		if (::GetWindowPlacement( hwndEdit, &cWindowPlacement )){
			/* �E�B���h�E�ʒu�E�T�C�Y���L�� */
			m_pShareData->m_Common.m_sOutline.m_xOutlineWindowPos = m_xPos - cWindowPlacement.rcNormalPosition.left;
			m_pShareData->m_Common.m_sOutline.m_yOutlineWindowPos = m_yPos - cWindowPlacement.rcNormalPosition.top;
			m_pShareData->m_Common.m_sOutline.m_widthOutlineWindow = m_nWidth;
			m_pShareData->m_Common.m_sOutline.m_heightOutlineWindow = m_nHeight;
		}
	}

	// �h�b�L���O��ʂ����Ƃ��͉�ʂ��ă��C�A�E�g����
	// �h�b�L���O�ŃA�v���I�����ɂ� hwndEdit �� NULL �ɂȂ��Ă���i�e�ɐ�� WM_DESTROY �������邽�߁j
	if( IsDocking() && hwndEdit )
		pcEditView->m_pcEditWnd->EndLayoutBars();

	// �����I�ɃA�E�g���C����ʂ�����Ƃ������A�E�g���C���\���t���O�� OFF �ɂ���
	// �t���[�e�B���O�ŃA�v���I������^�u���[�h�ŗ��ɂ���ꍇ�� ::IsWindowVisible( hwndEdit ) �� FALSE ��Ԃ�
	if( hwndEdit && ::IsWindowVisible( hwndEdit ) && !m_bInChangeLayout ){	// ChangeLayout() �������͐ݒ�ύX���Ȃ�
		bool bType = (ProfDockSet() != 0);
		if( bType ){
			CDocTypeManager().GetTypeConfig(CTypeConfig(m_nDocType), m_type);
		}
		ProfDockDisp() = FALSE;
		if( bType ){
			SetTypeConfig( CTypeConfig(m_nDocType), m_type );
		}
		// ���E�B���h�E�ɕύX��ʒm����
		if( ProfDockSync() ){
			PostOutlineNotifyToAllEditors( (WPARAM)0, (LPARAM)hwndEdit );
		}
	}

	if( m_hwndToolTip ){
		::DestroyWindow( m_hwndToolTip );
		m_hwndToolTip = NULL;
	}
	::KillTimer( GetHwnd(), 1 );

	return TRUE;
}


BOOL CDlgFuncList::OnCbnSelChange( HWND hwndCtl, int wID )
{
	int nSelect = Combo_GetCurSel( hwndCtl );
	switch(wID)
	{
	case IDC_COMBO_nSortType:
		if( m_nSortType != nSelect )
		{
			m_nSortType = nSelect;
			STypeConfig* type = new STypeConfig();
			CDocTypeManager().GetTypeConfig( CTypeConfig(m_nDocType), *type );
			type->m_nOutlineSortType = m_nSortType;
			SetTypeConfig( CTypeConfig(m_nDocType), *type );
			delete type;
			SortTree(::GetDlgItem( GetHwnd() , IDC_TREE_FL),TVI_ROOT);
		}
		return TRUE;
	}
	return FALSE;

}
void  CDlgFuncList::SortTree(HWND hWndTree,HTREEITEM htiParent)
{
	if( m_nSortType == 1 )
		TreeView_SortChildren(hWndTree,htiParent,TRUE);
	else
	{
		TVSORTCB sort;
		sort.hParent =  htiParent;
		sort.lpfnCompare = Compare_by_ItemData;
		sort.lParam = 0;
		TreeView_SortChildrenCB(hWndTree , &sort , TRUE);
	}
	for(HTREEITEM htiItem = TreeView_GetChild( hWndTree, htiParent ); NULL != htiItem ; htiItem = TreeView_GetNextSibling( hWndTree, htiItem ))
		SortTree(hWndTree,htiItem);
}



bool CDlgFuncList::TagJumpTimer( const TCHAR* pFile, CMyPoint point, bool bCheckAutoClose )
{
	CEditView* pcView = reinterpret_cast<CEditView*>(m_lParam);

	// �t�@�C�����J���Ă��Ȃ��ꍇ�͎����ŊJ��
	if( pcView->GetDocument()->IsAcceptLoad() ){
		std::wstring strFile = to_wchar(pFile);
		pcView->GetCommander().Command_FILEOPEN( strFile.c_str(), CODE_AUTODETECT, CAppMode::getInstance()->IsViewMode(), NULL );
		if( point.y != -1 ){
			if( pcView->GetDocument()->m_cDocFile.GetFilePathClass().IsValidPath() ){
				CLogicPoint pt;
				pt.x = CLogicInt(point.GetX() - 1);
				pt.y = CLogicInt(point.GetY() - 1);
				if( pt.x < 0 ){
					pt.x = 0;
				}
				pcView->GetCommander().Command_MOVECURSOR( pt, 0 );
			}
		}
		return true;
	}
	m_pszTimerJumpFile = pFile;
	m_pointTimerJump = point;
	m_bTimerJumpAutoClose = bCheckAutoClose;
	::SetTimer( GetHwnd(), 2, 200, NULL ); // id == 2
	return false;
}


BOOL CDlgFuncList::OnJump( bool bCheckAutoClose, bool bFileJump )	//2002.02.08 hor �����ǉ�
{
	int				nLineTo;
	int				nColTo;
	/* �_�C�A���O�f�[�^�̎擾 */
	if( 0 < GetData() && (m_cFuncInfo != NULL || 0 < m_sJumpFile.size() ) ){
		if( m_bModal ){		/* ���[�_�� �_�C�A���O�� */
			//���[�_���\������ꍇ�́Am_cFuncInfo���擾����A�N�Z�T���������Č��ʎ擾���邱�ƁB
			::EndDialog( GetHwnd(), 1 );
		}else{
			bool bFileJumpSelf = true;
			if( 0 < m_sJumpFile.size() ){
				if( bFileJump ){
					// �t�@�C���c���[�̏ꍇ
					if( m_bModal ){		/* ���[�_�� �_�C�A���O�� */
						//���[�_���\������ꍇ�́Am_cFuncInfo���擾����A�N�Z�T���������Č��ʎ擾���邱�ƁB
						::EndDialog( GetHwnd(), 1 );
					}
					CMyPoint poCaret;
					poCaret.x = -1;
					poCaret.y = -1;
					bFileJumpSelf = TagJumpTimer(m_sJumpFile.c_str(), poCaret, bCheckAutoClose);
				}
			}else
			if( m_cFuncInfo != NULL && 0 < m_cFuncInfo->m_cmemFileName.GetStringLength() ){
				if( bFileJump ){
					nLineTo = m_cFuncInfo->m_nFuncLineCRLF;
					nColTo = m_cFuncInfo->m_nFuncColCRLF;
					// �ʂ̃t�@�C���փW�����v
					CMyPoint poCaret; // TagJumpSub��1�J�n
					poCaret.x = nColTo;
					poCaret.y = nLineTo;
					bFileJumpSelf = TagJumpTimer(m_cFuncInfo->m_cmemFileName.GetStringPtr(), poCaret, bCheckAutoClose);
				}
			}else{
				nLineTo = m_cFuncInfo->m_nFuncLineCRLF;
				nColTo = m_cFuncInfo->m_nFuncColCRLF;
				/* �J�[�\�����ړ������� */
				CLogicPoint	poCaret;
				poCaret.x = nColTo - 1;
				poCaret.y = nLineTo - 1;

				m_pShareData->m_sWorkBuffer.m_LogicPoint = poCaret;

				//	2006.07.09 genta �ړ����ɑI����Ԃ�ێ�����悤��
				::SendMessageAny( ((CEditView*)m_lParam)->m_pcEditWnd->GetHwnd(),
					MYWM_SETCARETPOS, 0, PM_SETCARETPOS_KEEPSELECT );
			}
			if( bCheckAutoClose && bFileJumpSelf ){
				/* �A�E�g���C�� �_�C�A���O�������I�ɕ��� */
				if( IsDocking() ){
					::PostMessageAny( ((CEditView*)m_lParam)->GetHwnd(), MYWM_SETACTIVEPANE, 0, 0 );
				}
				else if( m_pShareData->m_Common.m_sOutline.m_bAutoCloseDlgFuncList ){
					::DestroyWindow( GetHwnd() );
				}
				else if( m_pShareData->m_Common.m_sOutline.m_bFunclistSetFocusOnJump ){
					::SetFocus( ((CEditView*)m_lParam)->GetHwnd() );
				}
			}
		}
	}
	return TRUE;
}


//@@@ 2002.01.18 add start
LPVOID CDlgFuncList::GetHelpIdTable(void)
{
	return (LPVOID)p_helpids;
}
//@@@ 2002.01.18 add end


/*!	�L�[������R�}���h�ɕϊ�����w���p�[�֐�
	
*/
void CDlgFuncList::Key2Command(WORD KeyCode)
{
	CEditView*	pcEditView;
// novice 2004/10/10
	/* Shift,Ctrl,Alt�L�[��������Ă����� */
	int nIdx = getCtrlKeyState();
	EFunctionCode nFuncCode=CKeyBind::GetFuncCode(
			((WORD)(((BYTE)(KeyCode)) | ((WORD)((BYTE)(nIdx))) << 8)),
			m_pShareData->m_Common.m_sKeyBind.m_nKeyNameArrNum,
			m_pShareData->m_Common.m_sKeyBind.m_pKeyNameArr
	);
	switch( nFuncCode ){
	case F_REDRAW:
		nFuncCode=GetFuncCodeRedraw(m_nOutlineType);
		/*FALLTHROUGH*/
	case F_OUTLINE:
	case F_OUTLINE_TOGGLE: // 20060201 aroka �t�H�[�J�X������Ƃ��̓����[�h
	case F_BOOKMARK_VIEW:
	case F_FILETREE:
		pcEditView=(CEditView*)m_lParam;
		pcEditView->GetCommander().HandleCommand( nFuncCode, true, SHOW_RELOAD, 0, 0, 0 ); // �����̕ύX 20060201 aroka

		break;
	case F_BOOKMARK_SET:
		OnJump( false );
		pcEditView=(CEditView*)m_lParam;
		pcEditView->GetCommander().HandleCommand( nFuncCode, true, 0, 0, 0, 0 );

		break;
	case F_COPY:
	case F_CUT:
		OnBnClicked( IDC_BUTTON_COPY );
		break;
	}
}

/*!
	@date 2002.10.05 genta
*/
void CDlgFuncList::Redraw( int nOutLineType, int nListType, CFuncInfoArr* pcFuncInfoArr, CLayoutInt nCurLine, CLayoutInt nCurCol )
{
	CEditView* pcEditView = (CEditView*)m_lParam;
	m_nDocType = pcEditView->GetDocument()->m_cDocType.GetDocumentType().GetIndex();
	CDocTypeManager().GetTypeConfig(CTypeConfig(m_nDocType), m_type);
	SyncColor();

	m_nOutlineType = nOutLineType;
	m_nListType = nListType;
	m_pcFuncInfoArr = pcFuncInfoArr;	/* �֐����z�� */
	m_nCurLine = nCurLine;				/* ���ݍs */
	m_nCurCol = nCurCol;				/* ���݌� */

	bool bType = (ProfDockSet() != 0);
	if( bType ){
		m_type.m_nDockOutline = m_nOutlineType;
		SetTypeConfig( CTypeConfig(m_nDocType), m_type );
	}else{
		CommonSet().m_nDockOutline = m_nOutlineType;
	}

	SetData();
}

//�_�C�A���O�^�C�g���̐ݒ�
void CDlgFuncList::SetWindowText( const TCHAR* szTitle )
{
	::SetWindowText( GetHwnd(), szTitle );
}

/** �z�F�K�p����
	@date 2010.06.05 ryoji �V�K�쐬
*/
void CDlgFuncList::SyncColor( void )
{
	if( !IsDocking() )
		return;
#ifdef DEFINE_SYNCCOLOR
#if REI_MOD_OUTLINEDLG
  bool dock_color_sync = !!RegGetDword(L"OutlineDockColorDefault", 0);
  if (!dock_color_sync) return;
#endif // rei_
	// �e�L�X�g�F�E�w�i�F���r���[�Ɠ��F�ɂ���
	CEditView* pcEditView = (CEditView*)m_lParam;
	const STypeConfig	*TypeDataPtr = &(pcEditView->m_pcEditDoc->m_cDocType.GetDocumentAttribute());
	COLORREF clrText = TypeDataPtr->m_ColorInfoArr[COLORIDX_TEXT].m_sColorAttr.m_cTEXT;
	COLORREF clrBack = TypeDataPtr->m_ColorInfoArr[COLORIDX_TEXT].m_sColorAttr.m_cBACK;

	HWND hwndTree = ::GetDlgItem( GetHwnd(), IDC_TREE_FL );
	TreeView_SetTextColor( hwndTree, clrText );
	TreeView_SetBkColor( hwndTree, clrBack );
	{
		// WinNT4.0 ������ł̓E�B���h�E�X�^�C���������I�ɍĐݒ肵�Ȃ���
		// �c���[�A�C�e���̍������^�����ɂȂ�
		LONG lStyle = (LONG)GetWindowLongPtr(hwndTree, GWL_STYLE);
		SetWindowLongPtr( hwndTree, GWL_STYLE, lStyle & ~(TVS_HASBUTTONS|TVS_HASLINES|TVS_LINESATROOT) );
		SetWindowLongPtr( hwndTree, GWL_STYLE, lStyle );
	}
	::SetWindowPos( hwndTree, NULL, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED );	// �Ȃ����������Ȃ��Ǝl�ӂP�h�b�g���������F�ύX�������K�p����Ȃ��i���X�^�C���Đݒ�Ƃ͖��֌W�j
	::InvalidateRect( hwndTree, NULL, TRUE );

	HWND hwndList = ::GetDlgItem( GetHwnd(), IDC_LIST_FL );
	ListView_SetTextColor( hwndList, clrText );
	ListView_SetTextBkColor( hwndList, clrBack );
	ListView_SetBkColor( hwndList, clrBack );
	::InvalidateRect( hwndList, NULL, TRUE );
#endif
}

/** �h�b�L���O�Ώۋ�`�̎擾�i�X�N���[�����W�j
	@date 2010.06.05 ryoji �V�K�쐬
*/
void CDlgFuncList::GetDockSpaceRect( LPRECT pRect )
{
	CEditView* pcEditView = (CEditView*)m_lParam;
	// CDlgFuncList �� CSplitterWnd �̊O�ڋ�`
	// 2014.12.02 �~�j�}�b�v�Ή�
	HWND hwnd[3];
	RECT rc[3];
	hwnd[0] = ::GetParent( pcEditView->GetHwnd() );	// CSplitterWnd
	int nCount = 1;
	if( IsDocking() ){
		hwnd[nCount] = GetHwnd();
		nCount++;
	}
	hwnd[nCount] = pcEditView->m_pcEditWnd->GetMiniMap().GetHwnd();
	if( hwnd[nCount] != NULL ){
		nCount++;
	}
	for( int i = 0; i < nCount; i++ ){
		::GetWindowRect(hwnd[i], &rc[i]);
	}
	if( 1 == nCount ){
		*pRect = rc[0];
	}else if( 2 == nCount ){
		::UnionRect(pRect, &rc[0], &rc[1]);
	}else{
		RECT rcTemp;
		::UnionRect(&rcTemp, &rc[0], &rc[1]);
		::UnionRect(pRect, &rcTemp, &rc[2]);
	}
}

/**�L���v�V������`�擾�i�X�N���[�����W�j
	@date 2010.06.05 ryoji �V�K�쐬
*/
void CDlgFuncList::GetCaptionRect( LPRECT pRect )
{
	RECT rc;
	::GetWindowRect( GetHwnd(), &rc );
	EDockSide eDockSide = GetDockSide();
	pRect->left = rc.left + ((eDockSide == DOCKSIDE_RIGHT)? DOCK_SPLITTER_WIDTH: 0);
	pRect->top = rc.top + ((eDockSide == DOCKSIDE_BOTTOM)? DOCK_SPLITTER_WIDTH: 0);
	pRect->right = rc.right - ((eDockSide == DOCKSIDE_LEFT)? DOCK_SPLITTER_WIDTH: 0);
	pRect->bottom = pRect->top + (::GetSystemMetrics( SM_CYSMCAPTION ) + 1);
}

/** �L���v�V������̃{�^����`�擾�i�X�N���[�����W�j
	@date 2010.06.05 ryoji �V�K�쐬
*/
bool CDlgFuncList::GetCaptionButtonRect( int nButton, LPRECT pRect )
{
	if( !IsDocking() )
		return false;
	if( nButton >= DOCK_BUTTON_NUM )
		return false;
	GetCaptionRect( pRect );
	::OffsetRect( pRect, 0, 1 );
	int cx = ::GetSystemMetrics( SM_CXSMSIZE );
	pRect->left = pRect->right - cx * (nButton + 1);
	pRect->right = pRect->left + cx;
	pRect->bottom = pRect->top + ::GetSystemMetrics( SM_CYSMSIZE );
	return true;
}

/** �����o�[�ւ̃q�b�g�e�X�g�i�X�N���[�����W�j
	@date 2010.06.05 ryoji �V�K�쐬
*/
bool CDlgFuncList::HitTestSplitter( int xPos, int yPos )
{
	if( !IsDocking() )
		return false;

	bool bRet = false;
	RECT rc;
	::GetWindowRect(GetHwnd(), &rc);

	EDockSide eDockSide = GetDockSide();
	switch( eDockSide ){
	case DOCKSIDE_LEFT:		bRet = (rc.right - xPos < DOCK_SPLITTER_WIDTH);		break;
	case DOCKSIDE_TOP:		bRet = (rc.bottom - yPos < DOCK_SPLITTER_WIDTH);	break;
	case DOCKSIDE_RIGHT:	bRet = (xPos - rc.left< DOCK_SPLITTER_WIDTH);		break;
	case DOCKSIDE_BOTTOM:	bRet = (yPos - rc.top < DOCK_SPLITTER_WIDTH);		break;
	}

	return bRet;
}

/** �L���v�V������̃{�^���ւ̃q�b�g�e�X�g�i�X�N���[�����W�j
	@date 2010.06.05 ryoji �V�K�쐬
*/
int CDlgFuncList::HitTestCaptionButton( int xPos, int yPos )
{
	if( !IsDocking() )
		return -1;

	POINT pt;
	pt.x = xPos;
	pt.y = yPos;

	RECT rcBtn;
	GetCaptionRect( &rcBtn );
	::OffsetRect( &rcBtn, 0, 1 );
	rcBtn.left = rcBtn.right - ::GetSystemMetrics( SM_CXSMSIZE );
	rcBtn.bottom = rcBtn.top + ::GetSystemMetrics( SM_CYSMSIZE );
	int nBtn = -1;
	for( int i = 0; i < DOCK_BUTTON_NUM; i++ ){
		if( ::PtInRect( &rcBtn, pt ) ){
			nBtn = i;	// �E�[���� i �Ԗڂ̃{�^����
			break;
		}
		::OffsetRect( &rcBtn, -(rcBtn.right - rcBtn.left), 0 );
	}

	return nBtn;
}

/** WM_NCCALCSIZE ����
	@date 2010.06.05 ryoji �V�K�쐬
*/
INT_PTR CDlgFuncList::OnNcCalcSize( HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	if( !IsDocking() )
		return 0L;

	// ���E�B���h�E�̃N���C�A���g�̈���`����
	// ����ŃL���v�V�����╪���o�[���N���C�A���g�̈�ɂ��邱�Ƃ��ł���
	NCCALCSIZE_PARAMS* pNCS = (NCCALCSIZE_PARAMS*)lParam;
	pNCS->rgrc[0].top += (::GetSystemMetrics( SM_CYSMCAPTION ) + 1);
	switch( GetDockSide() ){
	case DOCKSIDE_LEFT:		pNCS->rgrc[0].right -= DOCK_SPLITTER_WIDTH;		break;
	case DOCKSIDE_TOP:		pNCS->rgrc[0].bottom -= DOCK_SPLITTER_WIDTH;	break;
	case DOCKSIDE_RIGHT:	pNCS->rgrc[0].left += DOCK_SPLITTER_WIDTH;		break;
	case DOCKSIDE_BOTTOM:	pNCS->rgrc[0].top += DOCK_SPLITTER_WIDTH;		break;
	}
	return 1L;
}

/** WM_NCHITTEST ����
	@date 2010.06.05 ryoji �V�K�쐬
*/
INT_PTR CDlgFuncList::OnNcHitTest( HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	if( !IsDocking() )
		return 0L;

	INT_PTR nRet = HTERROR;
	POINT pt;
	pt.x = MAKEPOINTS(lParam).x;
	pt.y = MAKEPOINTS(lParam).y;
	if( HitTestSplitter(pt.x, pt.y) ){
		switch( GetDockSide() ){
		case DOCKSIDE_LEFT:		nRet = HTRIGHT;		break;
		case DOCKSIDE_TOP:		nRet = HTBOTTOM;	break;
		case DOCKSIDE_RIGHT:	nRet = HTLEFT;		break;
		case DOCKSIDE_BOTTOM:	nRet = HTTOP;		break;
		}
	}else {
		RECT rc;
		GetCaptionRect( &rc );
		nRet = ::PtInRect( &rc, pt )? HTCAPTION: HTCLIENT;
	}
	::SetWindowLongPtr( GetHwnd(), DWLP_MSGRESULT, nRet );

	return nRet;
}

/** WM_TIMER ����
	@date 2010.06.05 ryoji �V�K�쐬
*/
INT_PTR CDlgFuncList::OnTimer( HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	if( wParam == 2 ){
		CEditView* pcView = reinterpret_cast<CEditView*>(m_lParam);
		if( m_pszTimerJumpFile ){
			const TCHAR* pszFile = m_pszTimerJumpFile;
			m_pszTimerJumpFile = NULL;
			bool bSelf = false;
			pcView->TagJumpSub( pszFile, m_pointTimerJump, false, false, &bSelf );
			if( m_bTimerJumpAutoClose ){
				if( IsDocking() ){
					if( bSelf ){
						::PostMessageAny( pcView->GetHwnd(), MYWM_SETACTIVEPANE, 0, 0 );
					}
				}
				else if( m_pShareData->m_Common.m_sOutline.m_bAutoCloseDlgFuncList ){
					::DestroyWindow( GetHwnd() );
				}
				else if( m_pShareData->m_Common.m_sOutline.m_bFunclistSetFocusOnJump ){
					if( bSelf ){
						::SetFocus( pcView->GetHwnd() );
					}
				}
			}
		}
		::KillTimer(hwnd, 2);
		return 0L;
	}
	if( !IsDocking() )
		return 0L;

	if( wParam == 1 ){
		// �J�[�\�����E�B���h�E�O�ɂ���ꍇ�ɂ� WM_NCMOUSEMOVE �𑗂�
		POINT pt;
		RECT rc;
		::GetCursorPos( &pt );
		::GetWindowRect( hwnd, &rc );
		if( !::PtInRect( &rc, pt ) ){
			::SendMessageAny( hwnd, WM_NCMOUSEMOVE, 0, MAKELONG( pt.x, pt.y ) );
		}
	}

	return 0L;
}

/** WM_NCMOUSEMOVE ����
	@date 2010.06.05 ryoji �V�K�쐬
*/
INT_PTR CDlgFuncList::OnNcMouseMove( HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	if( !IsDocking() )
		return 0L;

	POINT pt;
	pt.x = MAKEPOINTS(lParam).x;
	pt.y = MAKEPOINTS(lParam).y;

	// �J�[�\�����E�B���h�E���ɓ�������^�C�}�[�N��
	// �E�B���h�E�O�ɏo����^�C�}�[�폜
	RECT rc;
	::GetWindowRect( GetHwnd(), &rc );
	bool bHovering = ::PtInRect( &rc, pt )? true: false;
	if( bHovering != m_bHovering )
	{
		m_bHovering = bHovering;
		if( m_bHovering )
			::SetTimer( hwnd, 1, 200, NULL );
		else
			::KillTimer( hwnd, 1 );
	}

	// �}�E�X�J�[�\�����{�^����ɂ���΃n�C���C�g
	int nHilightedBtn = HitTestCaptionButton(pt.x, pt.y);
	if( nHilightedBtn != m_nHilightedBtn ){
		// �n�C���C�g��Ԃ̕ύX�𔽉f���邽�߂ɍĕ`�悷��
		m_nHilightedBtn = nHilightedBtn;
		::RedrawWindow( GetHwnd(), NULL, NULL, RDW_FRAME | RDW_INVALIDATE | RDW_UPDATENOW | RDW_NOINTERNALPAINT );

		// �c�[���`�b�v�X�V
		TOOLINFO ti;
		::ZeroMemory( &ti, sizeof(ti) );
		ti.cbSize       = CCSIZEOF_STRUCT(TOOLINFO, lpszText);
		ti.hwnd         = GetHwnd();
		ti.hinst        = m_hInstance;
		ti.uId          = (UINT_PTR)GetHwnd();
		switch( m_nHilightedBtn ){
		case 0: ti.lpszText = const_cast<TCHAR*>(LS(STR_DLGFNCLST_TIP_CLOSE)); break;
		case 1: ti.lpszText = const_cast<TCHAR*>(LS(STR_DLGFNCLST_TIP_WIN)); break;
		case 2: ti.lpszText = const_cast<TCHAR*>(LS(STR_DLGFNCLST_TIP_UPDATE)); break;
		default: ti.lpszText = NULL;	// ����
		}
		Tooltip_UpdateTipText( m_hwndToolTip, &ti );
	}

	return 0L;
}

/** WM_MOUSEMOVE ����
	@date 2010.06.05 ryoji �V�K�쐬
*/
INT_PTR CDlgFuncList::OnMouseMove( HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	if( !IsDocking() )
		return 0L;

	if( m_bStretching ){	// �}�E�X�̃h���b�O�ʒu�ɂ��킹�ăT�C�Y��ύX����
		POINT pt;
		pt.x = MAKEPOINTS(lParam).x;
		pt.y = MAKEPOINTS(lParam).y;
		::ClientToScreen( GetHwnd(), &pt );

		RECT rc;
		GetDockSpaceRect(&rc);

		// ��ʃT�C�Y������������Ƃ��͉������Ȃ�
		EDockSide eDockSide = GetDockSide();
		if( eDockSide == DOCKSIDE_LEFT || eDockSide == DOCKSIDE_RIGHT ){
			if( rc.right - rc.left < DOCK_MIN_SIZE )
				return 0L;
		}else{
			if( rc.bottom - rc.top < DOCK_MIN_SIZE )
				return 0L;
		}

		// �}�E�X���㉺���E�ɍs���߂��Ȃ�͈͓��ɒ�������
		if( pt.x > rc.right - DOCK_MIN_SIZE ) pt.x = rc.right - DOCK_MIN_SIZE;
		if( pt.x < rc.left + DOCK_MIN_SIZE ) pt.x = rc.left + DOCK_MIN_SIZE;
		if( pt.y > rc.bottom - DOCK_MIN_SIZE ) pt.y = rc.bottom - DOCK_MIN_SIZE;
		if( pt.y < rc.top + DOCK_MIN_SIZE ) pt.y = rc.top + DOCK_MIN_SIZE;

		// �N���C�A���g���W�n�ɕϊ����ĐV�����ʒu�ƃT�C�Y���v�Z����
		POINT ptLT;
		ptLT.x = rc.left;
		ptLT.y = rc.top;
		::ScreenToClient( m_hwndParent, &ptLT );
		::OffsetRect( &rc, ptLT.x - rc.left, ptLT.y - rc.top );
		::ScreenToClient( m_hwndParent, &pt );
		switch( eDockSide ){
		case DOCKSIDE_LEFT:		rc.right = pt.x - DOCK_SPLITTER_WIDTH / 2 + DOCK_SPLITTER_WIDTH;	break;
		case DOCKSIDE_TOP:		rc.bottom = pt.y - DOCK_SPLITTER_WIDTH / 2 + DOCK_SPLITTER_WIDTH;	break;
		case DOCKSIDE_RIGHT:	rc.left = pt.x - DOCK_SPLITTER_WIDTH / 2;	break;
		case DOCKSIDE_BOTTOM:	rc.top = pt.y - DOCK_SPLITTER_WIDTH / 2;	break;
		}

		// �ȑO�Ɠ����z�u�Ȃ疳�ʂɈړ����Ȃ�
		RECT rcOld;
		::GetWindowRect( GetHwnd(), &rcOld );
		ptLT.x = rcOld.left;
		ptLT.y = rcOld.top;
		::ScreenToClient( m_hwndParent, &ptLT );
		::OffsetRect( &rcOld, ptLT.x - rcOld.left, ptLT.y - rcOld.top );
		if( ::EqualRect( &rcOld, &rc ) )
			return 0L;

		// �ړ�����
		::SetWindowPos( GetHwnd(), NULL,
			rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top,
			SWP_NOOWNERZORDER | SWP_NOZORDER | SWP_NOACTIVATE );
		((CEditView*)m_lParam)->m_pcEditWnd->EndLayoutBars( m_bEditWndReady );

		// �ړ���̔z�u�����L������
		GetWindowRect( GetHwnd(), &rc );
		bool bType = (ProfDockSet() != 0);
		if( bType ){
			CDocTypeManager().GetTypeConfig(CTypeConfig(m_nDocType), m_type);
		}
		switch( GetDockSide() ){
		case DOCKSIDE_LEFT:		ProfDockLeft() = rc.right - rc.left;	break;
		case DOCKSIDE_TOP:		ProfDockTop() = rc.bottom - rc.top;		break;
		case DOCKSIDE_RIGHT:	ProfDockRight() = rc.right - rc.left;	break;
		case DOCKSIDE_BOTTOM:	ProfDockBottom() = rc.bottom - rc.top;	break;
		}
		if( bType ){
			SetTypeConfig(CTypeConfig(m_nDocType), m_type);
		}
		return 1L;
	}

	return 0L;
}

/** WM_NCLBUTTONDOWN ����
	@date 2010.06.05 ryoji �V�K�쐬
*/
INT_PTR CDlgFuncList::OnNcLButtonDown( HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	POINT pt;
	pt.x = MAKEPOINTS(lParam).x;
	pt.y = MAKEPOINTS(lParam).y;

	if( !IsDocking() ){
		if( GetDockSide() == DOCKSIDE_FLOAT ){
			if( wParam == HTCAPTION  && !::IsZoomed(GetHwnd()) && !::IsIconic(GetHwnd()) ){
				::SetActiveWindow( GetHwnd() );
				// ��� SetActiveWindow() �� WM_ACTIVATEAPP �֍s���P�[�X�ł́AWM_ACTIVATEAPP �ɓ��ꂽ���ꏈ���i�G�f�B�^�{�̂��ꎞ�I�ɃA�N�e�B�u�����Ė߂��j
				// �ɗ]�v�Ɏ��Ԃ������邽�߁A��� SetActiveWindow() ��ɂ̓{�^����������Ă��邱�Ƃ�����B���̏ꍇ�� Track() ���J�n�����ɔ�����B
				if( (::GetAsyncKeyState( ::GetSystemMetrics(SM_SWAPBUTTON)? VK_RBUTTON: VK_LBUTTON ) & 0x8000) == 0 )
					return 1L;	// �{�^���͊��ɗ�����Ă���
				Track( pt );	// �^�C�g���o�[�̃h���b�O���h���b�v�ɂ��h�b�L���O�z�u�ύX
				return 1L;
			}
		}
		return 0L;
	}

	int nBtn;
	if( HitTestSplitter(pt.x, pt.y) ){	// �����o�[
		m_bStretching = true;
		::SetCapture( GetHwnd() );	// OnMouseMove�ł̃T�C�Y�����̂��߂Ɏ��O�̃L���v�`�����K�v
	}else{
		if( (nBtn = HitTestCaptionButton(pt.x, pt.y)) >= 0 ){	// �L���v�V������̃{�^��
			if( nBtn == 1 ){	// ���j���[
				RECT rcBtn;
				GetCaptionButtonRect( nBtn, &rcBtn );
				pt.x = rcBtn.left;
				pt.y = rcBtn.bottom;
				DoMenu( pt, GetHwnd() );
				// ���j���[�I�������Ƀ��X�g��c���[���N���b�N������{�^�����n�C���C�g�̂܂܂ɂȂ�̂ōX�V
				::RedrawWindow( GetHwnd(), NULL, NULL, RDW_FRAME | RDW_INVALIDATE | RDW_UPDATENOW | RDW_NOINTERNALPAINT );
			}else{
				m_nCapturingBtn = nBtn;
				::SetCapture( GetHwnd() );
			}
		}else{	// �c��̓^�C�g���o�[�̂�
			Track( pt );	// �^�C�g���o�[�̃h���b�O���h���b�v�ɂ��h�b�L���O�z�u�ύX
		}
	}

	return 1L;
}

/** WM_LBUTTONUP ����
	@date 2010.06.05 ryoji �V�K�쐬
*/
INT_PTR CDlgFuncList::OnLButtonUp( HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	if( !IsDocking() )
		return 0L;

	if( m_bStretching ){
		::ReleaseCapture();
		m_bStretching = false;

		if( ProfDockSync() ){
			// ���E�B���h�E�ɕύX��ʒm����
			HWND hwndEdit = ((CEditView*)m_lParam)->m_pcEditWnd->GetHwnd();
			PostOutlineNotifyToAllEditors( (WPARAM)0, (LPARAM)hwndEdit );
		}
		return 1L;
	}

	if( m_nCapturingBtn >= 0 ){
		::ReleaseCapture();
		POINT pt;
		pt.x = MAKEPOINTS(lParam).x;
		pt.y = MAKEPOINTS(lParam).y;
		::ClientToScreen( GetHwnd(), &pt );
		int nBtn = HitTestCaptionButton( pt.x, pt.y);
		if( nBtn == m_nCapturingBtn ){
			if( nBtn == 0 ){	// ����
				::DestroyWindow( GetHwnd() );
			}else if( m_nCapturingBtn == 2 ){	// �X�V
				EFunctionCode nFuncCode = GetFuncCodeRedraw(m_nOutlineType);
				CEditView* pcEditView = (CEditView*)m_lParam;
				pcEditView->GetCommander().HandleCommand( nFuncCode, true, SHOW_RELOAD, 0, 0, 0 );
			}
		}
		m_nCapturingBtn = -1;
		return 1L;
	}

	return 0L;
}

/** WM_NCPAINT ����
	@date 2010.06.05 ryoji �V�K�쐬
*/
INT_PTR CDlgFuncList::OnNcPaint( HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	if( !IsDocking() )
		return 0L;

	EDockSide eDockSide = GetDockSide();

	HDC hdc;
	RECT rc, rcScr, rcWk;

	//�`��Ώ�
	hdc = ::GetWindowDC( hwnd );
	CGraphics gr(hdc);
	::GetWindowRect( hwnd, &rcScr );
	rc = rcScr;
	::OffsetRect( &rc, -rcScr.left, -rcScr.top );

	// �w�i��`�悷��
	//::FillRect( gr, &rc, (HBRUSH)(COLOR_3DFACE + 1) );

	// ��������`�悷��
	rcWk = rc;
	switch( eDockSide ){
	case DOCKSIDE_LEFT:		rcWk.left = rcWk.right - DOCK_SPLITTER_WIDTH; break;
	case DOCKSIDE_TOP:		rcWk.top = rcWk.bottom - DOCK_SPLITTER_WIDTH; break;
	case DOCKSIDE_RIGHT:	rcWk.right = rcWk.left + DOCK_SPLITTER_WIDTH; break;
	case DOCKSIDE_BOTTOM:	rcWk.bottom = rcWk.top + DOCK_SPLITTER_WIDTH; break;
	}
	::FillRect( gr, &rcWk, (HBRUSH)(COLOR_3DFACE + 1) );
	::DrawEdge( gr, &rcWk, EDGE_ETCHED, BF_TOPLEFT );

	// �^�C�g����`�悷��
	BOOL bThemeActive = CUxTheme::getInstance()->IsThemeActive();
	BOOL bGradient = FALSE;
	::SystemParametersInfo( SPI_GETGRADIENTCAPTIONS, 0, &bGradient, 0 );
	if( !bThemeActive ) bGradient = FALSE;	// �K���ɒ���
	HWND hwndFocus = ::GetFocus();
	BOOL bActive = (GetHwnd() == hwndFocus || ::IsChild(GetHwnd(), hwndFocus));
	RECT rcCaption;
	GetCaptionRect( &rcCaption );
	::OffsetRect( &rcCaption, -rcScr.left, -rcScr.top );
	rcWk = rcCaption;
	rcWk.top += 1;
	rcWk.right -= DOCK_BUTTON_NUM * (::GetSystemMetrics( SM_CXSMSIZE ));
	// ��DrawCaption() �� DC_SMALLCAP ���w�肵�Ă͂����Ȃ����ۂ�
	// ��DC_SMALLCAP �w��̂��̂� Win7(64bit��) �œ������Ă݂���`��ʒu�����ɂ���ď㔼�����������Ȃ������ix86�r���h/x64�r���h�̂ǂ���� NG�j
	::DrawCaption( hwnd, gr, &rcWk, DC_TEXT | (bGradient? DC_GRADIENT: 0) /*| DC_SMALLCAP*/ | (bActive? DC_ACTIVE: 0) );
	rcWk.left = rcCaption.right;
	int nClrCaption;
	if( bGradient )
		nClrCaption = ( bActive? COLOR_GRADIENTACTIVECAPTION: COLOR_GRADIENTINACTIVECAPTION );
	else
		nClrCaption = ( bActive? COLOR_ACTIVECAPTION: COLOR_INACTIVECAPTION );
	::FillRect( gr, &rcWk, ::GetSysColorBrush( nClrCaption ) );
	::DrawEdge( gr, &rcCaption, BDR_SUNKENOUTER, BF_TOP );

	// �^�C�g����̃{�^����`�悷��
	NONCLIENTMETRICS ncm;
	ncm.cbSize = CCSIZEOF_STRUCT( NONCLIENTMETRICS, lfMessageFont );	// �ȑO�̃v���b�g�t�H�[���� WINVER >= 0x0600 �Œ�`�����\���̂̃t���T�C�Y��n���Ǝ��s����
	::SystemParametersInfo( SPI_GETNONCLIENTMETRICS, ncm.cbSize, (PVOID)&ncm, 0 );
	LOGFONT lf;
	memset( &lf, 0, sizeof(LOGFONT) );
	lf.lfCharSet = DEFAULT_CHARSET;
	lf.lfHeight = ncm.lfCaptionFont.lfHeight;
	::lstrcpy( lf.lfFaceName, _T("Marlett") );
	HFONT hFont = ::CreateFontIndirect( &lf );
	::lstrcpy( lf.lfFaceName, _T("Wingdings") );
	HFONT hFont2 = ::CreateFontIndirect( &lf );
	gr.SetTextBackTransparent( true );

	static const TCHAR szBtn[DOCK_BUTTON_NUM] = { (TCHAR)0x72/* ���� */, (TCHAR)0x36/* ���j���[ */, (TCHAR)0xFF/* �X�V */ };
	HFONT hFontBtn[DOCK_BUTTON_NUM] = { hFont/* ���� */, hFont/* ���j���[ */, hFont2/* �X�V */ };
	POINT pt;
	::GetCursorPos( &pt );
	pt.x -= rcScr.left;
	pt.y -= rcScr.top;
	RECT rcBtn = rcCaption;
	::OffsetRect( &rcBtn, 0, 1 );
	rcBtn.left = rcBtn.right - ::GetSystemMetrics( SM_CXSMSIZE );
	rcBtn.bottom = rcBtn.top + ::GetSystemMetrics( SM_CYSMSIZE );
	for( int i = 0; i < DOCK_BUTTON_NUM; i++ ){
		int nClrCaptionText;
		// �}�E�X�J�[�\�����{�^����ɂ���΃n�C���C�g
		if( ::PtInRect( &rcBtn, pt ) ){
			::FillRect( gr, &rcBtn, ::GetSysColorBrush( (bGradient && !bActive)? COLOR_INACTIVECAPTION: COLOR_ACTIVECAPTION ) );
			nClrCaptionText = ( (bGradient && !bActive)? COLOR_INACTIVECAPTIONTEXT: COLOR_CAPTIONTEXT );
		}else{
			nClrCaptionText = ( bActive? COLOR_CAPTIONTEXT: COLOR_INACTIVECAPTIONTEXT );
		}
		gr.PushMyFont( hFontBtn[i] );
		::SetTextColor( gr, ::GetSysColor( nClrCaptionText ) );
		::DrawText( gr, &szBtn[i], 1, &rcBtn, DT_SINGLELINE | DT_CENTER | DT_VCENTER );
		::OffsetRect( &rcBtn, -(rcBtn.right - rcBtn.left), 0 );
		gr.PopMyFont();
	}

	::DeleteObject( hFont );
	::DeleteObject( hFont2 );

	::ReleaseDC( hwnd, hdc );
	return 1L;
}

/** ���j���[����
	@date 2010.06.05 ryoji �V�K�쐬
*/
void CDlgFuncList::DoMenu( POINT pt, HWND hwndFrom )
{
	// ���j���[���쐬����
	CEditView* pcEditView = &CEditDoc::GetInstance(0)->m_pcEditWnd->GetActiveView();
	CDocTypeManager().GetTypeConfig( CTypeConfig(m_nDocType), m_type );
	EDockSide eDockSide = ProfDockSide();	// �ݒ��̔z�u
	UINT uFlags = MF_BYPOSITION | MF_STRING;
	const bool bDropDown = (hwndFrom == GetHwnd()); // true=�h���b�v�_�E��, false=�E�N���b�N
	HMENU hMenu = ::CreatePopupMenu();
	HMENU hMenuSub = bDropDown ? NULL : ::CreatePopupMenu();
	int iPos = 0;
	int iPosSub = 0;
	HMENU& hMenuRef = bDropDown ? hMenu : hMenuSub;
	int& iPosRef = bDropDown ? iPos : iPosSub;

	if( bDropDown == false ){
		// �����A������ hwndFrom �ɉ������󋵈ˑ����j���[��ǉ�����Ƃ�������
		// �i�c���[�Ȃ�u���ׂēW�J�v�^�u���ׂďk���v�Ƃ��A���������́j
		::InsertMenu( hMenu, iPos++, MF_BYPOSITION | MF_STRING, 450, LS(STR_DLGFNCLST_MENU_UPDATE) );
		int flag = 0;
		if( FALSE == ::IsWindowEnabled( GetItemHwnd(IDC_BUTTON_COPY) ) ){
			flag |= MF_GRAYED;
		}
		::InsertMenu( hMenu, iPos++, MF_BYPOSITION | MF_STRING | flag, 451, LS(STR_DLGFNCLST_MENU_COPY) );
		::InsertMenu( hMenu, iPos++, MF_BYPOSITION | MF_SEPARATOR, 0,	NULL );
		::InsertMenu( hMenu, iPos++, MF_BYPOSITION | MF_STRING | MF_POPUP, (UINT_PTR)hMenuSub,	LS(STR_DLGFNCLST_MENU_WINPOS) );
	}

	int iFrom = iPosRef;
	::InsertMenu( hMenuRef, iPosRef++, uFlags, 100 + DOCKSIDE_LEFT,       LS(STR_DLGFNCLST_MENU_LEFTDOC) );
	::InsertMenu( hMenuRef, iPosRef++, uFlags, 100 + DOCKSIDE_RIGHT,      LS(STR_DLGFNCLST_MENU_RIGHTDOC) );
	::InsertMenu( hMenuRef, iPosRef++, uFlags, 100 + DOCKSIDE_TOP,        LS(STR_DLGFNCLST_MENU_TOPDOC) );
	::InsertMenu( hMenuRef, iPosRef++, uFlags, 100 + DOCKSIDE_BOTTOM,     LS(STR_DLGFNCLST_MENU_BOTDOC) );
	::InsertMenu( hMenuRef, iPosRef++, uFlags, 100 + DOCKSIDE_FLOAT,      LS(STR_DLGFNCLST_MENU_FLOATING) );
	::InsertMenu( hMenuRef, iPosRef++, uFlags, 100 + DOCKSIDE_UNDOCKABLE, LS(STR_DLGFNCLST_MENU_NODOCK) );
	int iTo = iPosRef - 1;
	for( int i = iFrom; i <= iTo; i++ ){
		if( ::GetMenuItemID( hMenuRef, i ) == (100 + eDockSide) ){
			::CheckMenuRadioItem( hMenuRef, iFrom, iTo, i, MF_BYPOSITION );
			break;
		}
	}
	::InsertMenu( hMenuRef, iPosRef++, MF_BYPOSITION | MF_SEPARATOR, 0,	NULL );
	::InsertMenu( hMenuRef, iPosRef++, uFlags, 200, LS(STR_DLGFNCLST_MENU_SYNC) );
	::CheckMenuItem( hMenuRef, 200, MF_BYCOMMAND | ProfDockSync()? MF_CHECKED: MF_UNCHECKED );
	::InsertMenu( hMenuRef, iPosRef++, MF_BYPOSITION | MF_SEPARATOR, 0,	NULL );
	::InsertMenu( hMenuRef, iPosRef++, MF_BYPOSITION | MF_STRING, 300, LS(STR_DLGFNCLST_MENU_INHERIT) );
	::InsertMenu( hMenuRef, iPosRef++, MF_BYPOSITION | MF_STRING, 301, LS(STR_DLGFNCLST_MENU_TYPE) );
	::CheckMenuRadioItem( hMenuRef, 300, 301, (ProfDockSet() == 0)? 300: 301, MF_BYCOMMAND );
	::InsertMenu( hMenuRef, iPosRef++, MF_BYPOSITION | MF_SEPARATOR, 0,	NULL );
	::InsertMenu( hMenuRef, iPosRef++, MF_BYPOSITION | MF_STRING, 305, LS(STR_DLGFNCLST_MENU_UNIFY) );

	if( bDropDown == false ){
		::InsertMenu( hMenu, iPos++, MF_BYPOSITION | MF_SEPARATOR, 0,	NULL );
		::InsertMenu( hMenu, iPos++, MF_BYPOSITION | MF_STRING, 452, LS(STR_DLGFNCLST_MENU_CLOSE) );
	}

	// ���j���[��\������
	RECT rcWork;
	GetMonitorWorkRect( pt, &rcWork );	// ���j�^�̃��[�N�G���A
	int nId = ::TrackPopupMenu( hMenu, TPM_LEFTALIGN | TPM_TOPALIGN | TPM_LEFTBUTTON | TPM_RETURNCMD,
								( pt.x > rcWork.left )? pt.x: rcWork.left,
								( pt.y < rcWork.bottom )? pt.y: rcWork.bottom,
								0, GetHwnd(), NULL);
	::DestroyMenu( hMenu );	// �T�u���j���[�͍ċA�I�ɔj�������

	// ���j���[�I�����ꂽ��Ԃɐ؂�ւ���
	HWND hwndEdit = pcEditView->m_pcEditWnd->GetHwnd();
	if( nId == 450 ){	// �X�V
		EFunctionCode nFuncCode = GetFuncCodeRedraw(m_nOutlineType);
		CEditView* pcEditView = (CEditView*)m_lParam;
		pcEditView->GetCommander().HandleCommand( nFuncCode, true, SHOW_RELOAD, 0, 0, 0 );
	}
	else if( nId == 451 ){	// �R�s�[
		// Windows�N���b�v�{�[�h�ɃR�s�[ 
		SetClipboardText( GetHwnd(), m_cmemClipText.GetStringPtr(), m_cmemClipText.GetStringLength() );
	}
	else if( nId == 452 ){	// ����
		::DestroyWindow( GetHwnd() );
	}
	else if( nId == 300 || nId == 301 ){	// �h�b�L���O�z�u�̌p�����@
		ProfDockSet() = nId - 300;
		ChangeLayout( OUTLINE_LAYOUT_FOREGROUND );	// �������g�ւ̋����ύX
		if( ProfDockSync() ){
			PostOutlineNotifyToAllEditors( (WPARAM)0, (LPARAM)hwndEdit );	// ���E�B���h�E�Ƀh�b�L���O�z�u�ύX��ʒm����
		}
	}
	else if( nId == 305 ){	// �ݒ�R�s�[
		if( IDOK == ::MYMESSAGEBOX( hwndEdit,
						MB_OKCANCEL | MB_ICONINFORMATION, GSTR_APPNAME,
						LS(STR_DLGFNCLST_UNIFY) ) ){
			CommonSet().m_bOutlineDockDisp = GetHwnd()? TRUE: FALSE;
			CommonSet().m_eOutlineDockSide = GetDockSide();
			if( GetHwnd() ){
				RECT rc;
				GetWindowRect( GetHwnd(), &rc );
				switch( GetDockSide() ){	// ���݂̃h�b�L���O���[�h
					case DOCKSIDE_LEFT:		CommonSet().m_cxOutlineDockLeft = rc.right - rc.left;	break;
					case DOCKSIDE_TOP:		CommonSet().m_cyOutlineDockTop = rc.bottom - rc.top;	break;
					case DOCKSIDE_RIGHT:	CommonSet().m_cxOutlineDockRight = rc.right - rc.left;	break;
					case DOCKSIDE_BOTTOM:	CommonSet().m_cyOutlineDockBottom = rc.bottom - rc.top;	break;
				}
			}
			STypeConfig* type = new STypeConfig();
			for( int i = 0; i < GetDllShareData().m_nTypesCount; i++ ){
				CDocTypeManager().GetTypeConfig( CTypeConfig(i), *type );
				type->m_bOutlineDockDisp = CommonSet().m_bOutlineDockDisp;
				type->m_eOutlineDockSide = CommonSet().m_eOutlineDockSide;
				type->m_cxOutlineDockLeft = CommonSet().m_cxOutlineDockLeft;
				type->m_cyOutlineDockTop = CommonSet().m_cyOutlineDockTop;
				type->m_cxOutlineDockRight = CommonSet().m_cxOutlineDockRight;
				type->m_cyOutlineDockBottom = CommonSet().m_cyOutlineDockBottom;
				CDocTypeManager().SetTypeConfig( CTypeConfig(i), *type );
			}
			delete type;
			ChangeLayout( OUTLINE_LAYOUT_FOREGROUND );	// �������g�ւ̋����ύX
			PostOutlineNotifyToAllEditors( (WPARAM)0, (LPARAM)hwndEdit );	// ���E�B���h�E�Ƀh�b�L���O�z�u�ύX��ʒm����
		}
	}
	else if( nId == 200 ){	// �h�b�L���O�z�u�̓������Ƃ�
		ProfDockSync() = !ProfDockSync();
		ChangeLayout( OUTLINE_LAYOUT_FOREGROUND );	// �������g�ւ̋����ύX
		if( ProfDockSync() ){
			PostOutlineNotifyToAllEditors( (WPARAM)0, (LPARAM)hwndEdit );	// ���E�B���h�E�Ƀh�b�L���O�z�u�ύX��ʒm����
		}
	}
	else if( nId >= 100 - 1 ){	// �h�b�L���O���[�h �i�� DOCKSIDE_UNDOCKABLE �� -1 �ł��j */
		int* pnWidth = NULL;
		int* pnHeight = NULL;
		RECT rc;
		GetDockSpaceRect( &rc );
		eDockSide = EDockSide(nId - 100);	// �V�����h�b�L���O���[�h
		bool bType = (ProfDockSet() != 0);
		if( bType ){
			CDocTypeManager().GetTypeConfig(CTypeConfig(m_nDocType), m_type);
		}
		if( eDockSide > DOCKSIDE_FLOAT ){
			switch( eDockSide ){
			case DOCKSIDE_LEFT:		pnWidth = &ProfDockLeft();		break;
			case DOCKSIDE_TOP:		pnHeight = &ProfDockTop();		break;
			case DOCKSIDE_RIGHT:	pnWidth = &ProfDockRight();		break;
			case DOCKSIDE_BOTTOM:	pnHeight = &ProfDockBottom();	break;
			}
			if( eDockSide == DOCKSIDE_LEFT || eDockSide == DOCKSIDE_RIGHT ){
				if( *pnWidth == 0 )	// ����
					*pnWidth = (rc.right - rc.left) / 3;
				if( *pnWidth > rc.right - rc.left - DOCK_MIN_SIZE ) *pnWidth = rc.right - rc.left - DOCK_MIN_SIZE;
				if( *pnWidth < DOCK_MIN_SIZE ) *pnWidth = DOCK_MIN_SIZE;
			}else{
				if( *pnHeight == 0 )	// ����
					*pnHeight = (rc.bottom - rc.top) / 3;
				if( *pnHeight > rc.bottom - rc.top - DOCK_MIN_SIZE ) *pnHeight = rc.bottom - rc.top - DOCK_MIN_SIZE;
				if( *pnHeight < DOCK_MIN_SIZE ) *pnHeight = DOCK_MIN_SIZE;
			}
		}

		// �h�b�L���O�z�u�ύX
		ProfDockDisp() = GetHwnd()? TRUE: FALSE;
		ProfDockSide() = eDockSide;	// �V�����h�b�L���O���[�h��K�p
		if( bType ){
			SetTypeConfig(CTypeConfig(m_nDocType), m_type);
		}
		ChangeLayout( OUTLINE_LAYOUT_FOREGROUND );	// �������g�ւ̋����ύX
		if( ProfDockSync() ){
			PostOutlineNotifyToAllEditors( (WPARAM)0, (LPARAM)hwndEdit );	// ���E�B���h�E�Ƀh�b�L���O�z�u�ύX��ʒm����
		}
	}
}

/** ���݂̐ݒ�ɉ����ĕ\�������V����
	@date 2010.06.05 ryoji �V�K�쐬
*/
void CDlgFuncList::Refresh( void )
{
	CEditWnd* pcEditWnd = CEditDoc::GetInstance(0)->m_pcEditWnd;
	BOOL bReloaded = ChangeLayout( OUTLINE_LAYOUT_FILECHANGED );	// ���ݐݒ�ɏ]���ăA�E�g���C����ʂ��Ĕz�u����
	if( !bReloaded && pcEditWnd->m_cDlgFuncList.GetHwnd() ){
		int nOutlineType = GetOutlineTypeRedraw(m_nOutlineType);
		pcEditWnd->GetActiveView().GetCommander().Command_FUNCLIST( SHOW_RELOAD, nOutlineType );	// �J��	�� HandleCommand(F_OUTLINE,...) ���ƈ���v���r���[��ԂŎ��s����Ȃ��̂� Command_FUNCLIST()
	}
	if( MyGetAncestor( ::GetForegroundWindow(), GA_ROOTOWNER2 ) == pcEditWnd->GetHwnd() )
		::SetFocus( pcEditWnd->GetActiveView().GetHwnd() );	// �t�H�[�J�X��߂�
}

/** ���݂̐ݒ�ɉ����Ĕz�u��ύX����i�ł������ĉ�͂��Ȃ��j

	@param nId [in] ����w��DOUTLINE_LAYOUT_FOREGROUND: �O�ʗp�̓��� / OUTLINE_LAYOUT_BACKGROUND: �w��p�̓��� / OUTLINE_LAYOUT_FILECHANGED: �t�@�C���ؑ֗p�̓���i�O�ʂ�������j
	@retval ��͂����s�������ǂ����Dtrue: ���s���� / false: ���s���Ȃ�����

	@date 2010.06.10 ryoji �V�K�쐬
*/
bool CDlgFuncList::ChangeLayout( int nId )
{
	struct SAutoSwitch
	{
		SAutoSwitch( bool* pbSwitch ): m_pbSwitch( pbSwitch ) { *m_pbSwitch = true; }
		~SAutoSwitch() { *m_pbSwitch = false; }
		bool* m_pbSwitch;
	} SAutoSwitch( &m_bInChangeLayout );	// �������� m_bInChangeLayout �t���O�� ON �ɂ��Ă���

	CEditDoc* pDoc = CEditDoc::GetInstance(0);	// ���͔�\����������Ȃ��̂� (CEditView*)m_lParam �͎g���Ȃ�
	m_nDocType = pDoc->m_cDocType.GetDocumentType().GetIndex();
	CDocTypeManager().GetTypeConfig( CTypeConfig(m_nDocType), m_type );

	BOOL bDockDisp = ProfDockDisp();
	EDockSide eDockSideNew = ProfDockSide();

	if( !GetHwnd() ){	// ���݂͔�\��
		if( bDockDisp ){	// �V�ݒ�͕\��
			if( eDockSideNew <= DOCKSIDE_FLOAT ){
				if( nId == OUTLINE_LAYOUT_BACKGROUND ) return false;	// ���ł̓t���[�e�B���O�͊J���Ȃ��i�]���݊��j�������ɊJ���ƃ^�u���[�h���͉�ʂ��؂�ւ���Ă��܂�
				if( nId == OUTLINE_LAYOUT_FILECHANGED ) return false;	// �t�@�C���ؑւł̓t���[�e�B���O�͊J���Ȃ��i�]���݊��j
			}
			// �� ���ł͈ꎞ�I�� Disable �����Ă����ĊJ���i�^�u���[�h�ł̕s���ȉ�ʐ؂�ւ��}�~�j
			CEditView* pcEditView = &pDoc->m_pcEditWnd->GetActiveView();
			if( nId == OUTLINE_LAYOUT_BACKGROUND ) ::EnableWindow( pcEditView->m_pcEditWnd->GetHwnd(), FALSE );
			if( m_nOutlineType == OUTLINE_DEFAULT ){
				bool bType = (ProfDockSet() != 0);
				if( bType ){
					m_nOutlineType = m_type.m_nDockOutline;
					SetTypeConfig( CTypeConfig(m_nDocType), m_type );
				}else{
					m_nOutlineType = CommonSet().m_nDockOutline;
				}
			}
			int nOutlineType = GetOutlineTypeRedraw(m_nOutlineType);	// �u�b�N�}�[�N���A�E�g���C����͂��͍Ō�ɊJ���Ă������̏�Ԃ������p���i������Ԃ̓A�E�g���C����́j
			pcEditView->GetCommander().Command_FUNCLIST( SHOW_NORMAL, nOutlineType );	// �J��	�� HandleCommand(F_OUTLINE,...) ���ƈ���v���r���[��ԂŎ��s����Ȃ��̂� Command_FUNCLIST()
			if( nId == OUTLINE_LAYOUT_BACKGROUND ) ::EnableWindow( pcEditView->m_pcEditWnd->GetHwnd(), TRUE );
			return true;	// ��͂���
		}
	}else{	// ���݂͕\��
		EDockSide eDockSideOld = GetDockSide();

		CEditView* pcEditView = (CEditView*)m_lParam;
		if( !bDockDisp ){	// �V�ݒ�͔�\��
			if( eDockSideOld <= DOCKSIDE_FLOAT ){	// ���݂̓t���[�e�B���O
				if( nId == OUTLINE_LAYOUT_BACKGROUND ) return false;	// ���ł̓t���[�e�B���O�͕��Ȃ��i�]���݊��j
				if( nId == OUTLINE_LAYOUT_FILECHANGED && eDockSideNew <= DOCKSIDE_FLOAT ) return false;	// �t�@�C���ؑւł͐V�ݒ���t���[�e�B���O�Ȃ�ė��p�i�]���݊��j
			}
			::DestroyWindow( GetHwnd() );	// ����
			return false;
		}

		// �h�b�L���O�̃t���[�e�B���O�ؑւł͕��ĊJ��
		if( (eDockSideOld <= DOCKSIDE_FLOAT) != (eDockSideNew <= DOCKSIDE_FLOAT) ){
			::DestroyWindow( GetHwnd() );	// ����
			if( eDockSideNew <= DOCKSIDE_FLOAT ){	// �V�ݒ�̓t���[�e�B���O
				m_xPos = m_yPos = -1;	// ��ʈʒu������������
				if( nId == OUTLINE_LAYOUT_BACKGROUND ) return false;	// ���ł̓t���[�e�B���O�͊J���Ȃ��i�]���݊��j�������ɊJ���ƃ^�u���[�h���͉�ʂ��؂�ւ���Ă��܂�
				if( nId == OUTLINE_LAYOUT_FILECHANGED ) return false;	// �t�@�C���ؑւł̓t���[�e�B���O�͊J���Ȃ��i�]���݊��j
			}
			// �� ���ł͈ꎞ�I�� Disable �����Ă����ĊJ���i�^�u���[�h�ł̕s���ȉ�ʐ؂�ւ��}�~�j
			if( nId == OUTLINE_LAYOUT_BACKGROUND ) ::EnableWindow( pcEditView->m_pcEditWnd->GetHwnd(), FALSE );
			if( m_nOutlineType == OUTLINE_DEFAULT ){
				bool bType = (ProfDockSet() != 0);
				if( bType ){
					m_nOutlineType = m_type.m_nDockOutline;
					SetTypeConfig( CTypeConfig(m_nDocType), m_type );
				}else{
					m_nOutlineType = CommonSet().m_nDockOutline;
				}
			}
			int nOutlineType = GetOutlineTypeRedraw(m_nOutlineType);
			pcEditView->GetCommander().Command_FUNCLIST( SHOW_NORMAL, nOutlineType );	// �J��	�� HandleCommand(F_OUTLINE,...) ���ƈ���v���r���[��ԂŎ��s����Ȃ��̂� Command_FUNCLIST()
			if( nId == OUTLINE_LAYOUT_BACKGROUND ) ::EnableWindow( pcEditView->m_pcEditWnd->GetHwnd(), TRUE );
			return true;	// ��͂���
		}

		// �t���[�e�B���O���t���[�e�B���O�ł͔z�u���������Ɍ���ێ�
		if( eDockSideOld <= DOCKSIDE_FLOAT ){
			m_eDockSide = eDockSideNew;
			return false;
		}

		// �h�b�L���O���h�b�L���O�ł͔z�u����
		RECT rc;
		POINT ptLT;
		GetDockSpaceRect( &rc );
		ptLT.x = rc.left;
		ptLT.y = rc.top;
		::ScreenToClient( m_hwndParent, &ptLT );
		::OffsetRect( &rc, ptLT.x - rc.left, ptLT.y - rc.top );

		switch( eDockSideNew ){
		case DOCKSIDE_LEFT:		rc.right = rc.left + ProfDockLeft();	break;
		case DOCKSIDE_TOP:		rc.bottom = rc.top + ProfDockTop();		break;
		case DOCKSIDE_RIGHT:	rc.left = rc.right - ProfDockRight();	break;
		case DOCKSIDE_BOTTOM:	rc.top = rc.bottom - ProfDockBottom();	break;
		}

		// �ȑO�Ɠ����z�u�Ȃ疳�ʂɈړ����Ȃ�
		RECT rcOld;
		::GetWindowRect( GetHwnd(), &rcOld );
		ptLT.x = rcOld.left;
		ptLT.y = rcOld.top;
		::ScreenToClient( m_hwndParent, &ptLT );
		::OffsetRect( &rcOld, ptLT.x - rcOld.left, ptLT.y - rcOld.top );
		if( eDockSideOld == eDockSideNew && ::EqualRect( &rcOld, &rc ) ){
			::InvalidateRect( GetHwnd(), NULL, TRUE );	// ���������ĕ`�悾��
			return false;	// �z�u�ύX�s�v�i��F�ʂ̃t�@�C���^�C�v����̒ʒm�j
		}

		// �ړ�����
		m_eDockSide = eDockSideNew;	// ���g�̃h�b�L���O�z�u�̋L�����X�V
		::SetWindowPos( GetHwnd(), NULL,
			rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top,
			SWP_NOOWNERZORDER | SWP_NOZORDER | SWP_NOACTIVATE | ((eDockSideOld == eDockSideNew)? 0: SWP_FRAMECHANGED) );	// SWP_FRAMECHANGED �w��� WM_NCCALCSIZE�i��N���C�A���g�̈�̍Čv�Z�j�ɗU������
		pcEditView->m_pcEditWnd->EndLayoutBars( m_bEditWndReady );
	}
	return false;
}

/** �A�E�g���C���ʒm(MYWM_OUTLINE_NOTIFY)����

	wParam: �ʒm���
	lParam: ��ʖ��̃p�����[�^

	@date 2010.06.07 ryoji �V�K�쐬
*/
void CDlgFuncList::OnOutlineNotify( WPARAM wParam, LPARAM lParam )
{
	CEditDoc* pDoc = CEditDoc::GetInstance(0);	// ���͔�\����������Ȃ��̂� (CEditView*)m_lParam �͎g���Ȃ�
	switch( wParam ){
	case 0:	// �ݒ�ύX�ʒm�i�h�b�L���O���[�h or �T�C�Y�j, lParam: �ʒm���� HWND
		if( (HWND)lParam == pDoc->m_pcEditWnd->GetHwnd() )
			return;	// ��������̒ʒm�͖���
		ChangeLayout( OUTLINE_LAYOUT_BACKGROUND );	// �A�E�g���C����ʂ��Ĕz�u
		break;
	}
	return;
}

/** ���E�B���h�E�ɃA�E�g���C���ʒm���|�X�g����
	@date 2010.06.10 ryoji �V�K�쐬
*/
BOOL CDlgFuncList::PostOutlineNotifyToAllEditors( WPARAM wParam, LPARAM lParam )
{
	return CAppNodeGroupHandle(0).PostMessageToAllEditors( MYWM_OUTLINE_NOTIFY, (WPARAM)wParam, (LPARAM)lParam, GetHwnd() );
}

void CDlgFuncList::SetTypeConfig( CTypeConfig docType, const STypeConfig& type )
{
	CDocTypeManager().SetTypeConfig(docType, type);
}

/** �R���e�L�X�g���j���[����
	@date 2010.06.07 ryoji �V�K�쐬
*/
BOOL CDlgFuncList::OnContextMenu( WPARAM wParam, LPARAM lParam )
{
	// �L���v�V���������X�g�^�c���[��Ȃ烁�j���[��\������
	HWND hwndFrom = (HWND)wParam;
	if( ::SendMessage( GetHwnd(), WM_NCHITTEST, 0, lParam ) == HTCAPTION
			|| hwndFrom == ::GetDlgItem( GetHwnd(), IDC_LIST_FL )
			|| hwndFrom == ::GetDlgItem( GetHwnd(), IDC_TREE_FL )
	){
		POINT pt;
		pt.x = MAKEPOINTS(lParam).x;
		pt.y = MAKEPOINTS(lParam).y;
		if( pt.x == -1 && pt.y == -1 ){	// �L�[�{�[�h�i���j���[�L�[ �� Shift F10�j����̌Ăяo��
			RECT rc;
			::GetWindowRect( hwndFrom, &rc );
			pt.x = rc.left;
			pt.y = rc.top;
		}
		DoMenu( pt, hwndFrom );
		return TRUE;
	}

	return CDialog::OnContextMenu( wParam, lParam );	// ���̑��̃R���g���[����ł̓|�b�v�A�b�v�w���v��\������
}

/** �^�C�g���o�[�̃h���b�O���h���b�v�Ńh�b�L���O�z�u����ۂ̈ړ����`�����߂�
	@date 2010.06.17 ryoji �V�K�쐬
*/
EDockSide CDlgFuncList::GetDropRect( POINT ptDrag, POINT ptDrop, LPRECT pRect, bool bForceFloat )
{
	struct CDockStretch{
		static int GetIdealStretch( int nStretch, int nMaxStretch )
		{
			if( nStretch == 0 )
				nStretch = nMaxStretch / 3;
			if( nStretch > nMaxStretch - DOCK_MIN_SIZE ) nStretch = nMaxStretch - DOCK_MIN_SIZE;
			if( nStretch < DOCK_MIN_SIZE ) nStretch = DOCK_MIN_SIZE;
			return nStretch;
		}
	};

	// �ړ����Ȃ���`���擾����
	RECT rcWnd;
	::GetWindowRect( GetHwnd(), &rcWnd );
	if( IsDocking() && !bForceFloat ){
		if( ::PtInRect( &rcWnd, ptDrop ) ){
			*pRect = rcWnd;
			return GetDockSide();	// �ړ����Ȃ��ʒu������
		}
	}

	// �h�b�L���O�p�̋�`���擾����
	EDockSide eDockSide = DOCKSIDE_FLOAT;	// �t���[�e�B���O�ɉ�����
	RECT rcDock;
	GetDockSpaceRect( &rcDock );
	if( !bForceFloat && ::PtInRect( &rcDock, ptDrop ) ){
		int cxLeft		= CDockStretch::GetIdealStretch( ProfDockLeft(), rcDock.right - rcDock.left );
		int cyTop		= CDockStretch::GetIdealStretch( ProfDockTop(), rcDock.bottom - rcDock.top );
		int cxRight		= CDockStretch::GetIdealStretch( ProfDockRight(), rcDock.right - rcDock.left );
		int cyBottom	= CDockStretch::GetIdealStretch( ProfDockBottom(), rcDock.bottom - rcDock.top );

		int nDock = ::GetSystemMetrics( SM_CXCURSOR );
		if( ptDrop.x - rcDock.left < nDock ){
			eDockSide = DOCKSIDE_LEFT;
			rcDock.right = rcDock.left + cxLeft;
		}
		else if( rcDock.right - ptDrop.x < nDock ){
			eDockSide = DOCKSIDE_RIGHT;
			rcDock.left = rcDock.right - cxRight;
		}
		else if( ptDrop.y - rcDock.top < nDock ){
			eDockSide = DOCKSIDE_TOP;
			rcDock.bottom = rcDock.top + cyTop;
		}
		else if( rcDock.bottom - ptDrop.y < nDock ){
			eDockSide = DOCKSIDE_BOTTOM;
			rcDock.top = rcDock.bottom - cyBottom;
		}
		if( eDockSide != DOCKSIDE_FLOAT ){
			*pRect = rcDock;
			return eDockSide;	// �h�b�L���O�ʒu������
		}
	}

	// �t���[�e�B���O�p�̋�`���擾����
	if( !IsDocking() ){	// �t���[�e�B���O �� �t���[�e�B���O
		::OffsetRect( &rcWnd, ptDrop.x - ptDrag.x, ptDrop.y - ptDrag.y );
		*pRect = rcWnd;
	}else{	// �h�b�L���O �� �t���[�e�B���O
		int cx, cy;
		RECT rcFloat;
		rcFloat.left = 0;
		rcFloat.top = 0;
		if( m_pShareData->m_Common.m_sOutline.m_bRememberOutlineWindowPos
				&& m_pShareData->m_Common.m_sOutline.m_widthOutlineWindow	// �����l���� 0 �ɂȂ��Ă���
				&& m_pShareData->m_Common.m_sOutline.m_heightOutlineWindow	// �����l���� 0 �ɂȂ��Ă���
		){
			// �L�����Ă���T�C�Y
			rcFloat.right = m_pShareData->m_Common.m_sOutline.m_widthOutlineWindow;
			rcFloat.bottom = m_pShareData->m_Common.m_sOutline.m_heightOutlineWindow;
			cx = ::GetSystemMetrics( SM_CXMIN );
			cy = ::GetSystemMetrics( SM_CYMIN );
			if( rcFloat.right < cx ) rcFloat.right = cx;
			if( rcFloat.bottom < cy ) rcFloat.bottom = cy;
		}
		else{
			HINSTANCE hInstance2 = CSelectLang::getLangRsrcInstance();
			if ( m_lastRcInstance != hInstance2 ) {
				HRSRC hResInfo = ::FindResource( hInstance2, MAKEINTRESOURCE(IDD_FUNCLIST), RT_DIALOG );
				if( !hResInfo ) return eDockSide;
				HGLOBAL hResData = ::LoadResource( hInstance2, hResInfo );
				if( !hResData ) return eDockSide;
				m_pDlgTemplate = (LPDLGTEMPLATE)::LockResource( hResData );
				if( !m_pDlgTemplate ) return eDockSide;
				m_dwDlgTmpSize = ::SizeofResource( hInstance2, hResInfo );
				// ����؂�ւ��Ń��\�[�X���A�����[�h����Ă��Ȃ����m�F���邽�߃C���X�^���X���L������
				m_lastRcInstance = hInstance2;
			}
			// �f�t�H���g�̃T�C�Y�i�_�C�A���O�e���v���[�g�Ō��܂�T�C�Y�j
			rcFloat.right = m_pDlgTemplate->cx;
			rcFloat.bottom = m_pDlgTemplate->cy;
			::MapDialogRect( GetHwnd(), &rcFloat );
			rcFloat.right += ::GetSystemMetrics( SM_CXDLGFRAME ) * 2;	// �� Create ���̃X�^�C���ύX�ŃT�C�Y�ύX�s����T�C�Y�ύX�\�ɂ��Ă���
			rcFloat.bottom += ::GetSystemMetrics( SM_CYCAPTION ) + ::GetSystemMetrics( SM_CYDLGFRAME ) * 2;
		}
		cy = ::GetSystemMetrics( SM_CYCAPTION );
		::OffsetRect( &rcFloat, ptDrop.x - cy * 2, ptDrop.y - cy / 2 );
		*pRect = rcFloat;
	}

	return DOCKSIDE_FLOAT;	// �t���[�e�B���O�ʒu������
}

/** �^�C�g���o�[�̃h���b�O���h���b�v�Ńh�b�L���O�z�u��ύX����
	@date 2010.06.17 ryoji �V�K�쐬
*/
BOOL CDlgFuncList::Track( POINT ptDrag )
{
	if( ::GetCapture() )
		return FALSE;

	struct SLockWindowUpdate
	{	// ��ʂɃS�~���c��Ȃ��悤��
		SLockWindowUpdate(){ ::LockWindowUpdate( ::GetDesktopWindow() ); }
		~SLockWindowUpdate(){ ::LockWindowUpdate( NULL ); }
	} sLockWindowUpdate;

	const SIZE sizeFull = {8, 8};	// �t���[�e�B���O�z�u�p�̘g���̑���
	const SIZE sizeHalf = {4, 4};	// �h�b�L���O�z�u�p�̘g���̑���
	const SIZE sizeClear = {0, 0};	// �g���`�悵�Ȃ�

	POINT pt;
	RECT rc;
	RECT rcDragLast;
	SIZE sizeLast = sizeClear;
	BOOL bDragging = false;	// �܂��{�i�J�n���Ȃ�
	int cxDragSm = ::GetSystemMetrics( SM_CXDRAG );
	int cyDragSm = ::GetSystemMetrics( SM_CYDRAG );

	::SetCapture( GetHwnd() );	// �L���v�`���J�n

	while( ::GetCapture() == GetHwnd() )
	{
		MSG msg;
		if (!::GetMessage(&msg, NULL, 0, 0)){
			::PostQuitMessage( (int)msg.wParam );
			break;
		}

		switch (msg.message){
		case WM_MOUSEMOVE:
			::GetCursorPos( &pt );

			bool bStart;
			bStart = false;
			if( !bDragging ){
				// �������ʒu���炢���炩�����Ă���h���b�O�J�n�ɂ���
				if( abs(pt.x - ptDrag.x) >= cxDragSm || abs(pt.y - ptDrag.y) >= cyDragSm ){
					bDragging = bStart = true;	// ��������J�n
				}
			}
			if( bDragging ){	// �h���b�O��
				// �h���b�v���`��`�悷��
				EDockSide eDockSide = GetDropRect( ptDrag, pt, &rc, GetKeyState_Control() );
				SIZE sizeNew = (eDockSide <= DOCKSIDE_FLOAT)? sizeFull: sizeHalf;
				CGraphics::DrawDropRect( &rc, sizeNew, bStart? NULL: &rcDragLast, sizeLast );
				rcDragLast = rc;
				sizeLast = sizeNew;
			}
			break;

		case WM_LBUTTONUP:
			::GetCursorPos( &pt );

			::ReleaseCapture();
			if( bDragging ){
				// �h�b�L���O�z�u��ύX����
				EDockSide eDockSide = GetDropRect( ptDrag, pt, &rc, GetKeyState_Control() );
				CGraphics::DrawDropRect( NULL, sizeClear, &rcDragLast, sizeLast );

				bool bType = (ProfDockSet() != 0);
				if( bType ){
					CDocTypeManager().GetTypeConfig(CTypeConfig(m_nDocType), m_type);
				}
				ProfDockDisp() = GetHwnd()? TRUE: FALSE;
				ProfDockSide() = eDockSide;	// �V�����h�b�L���O���[�h��K�p
				switch( eDockSide ){
				case DOCKSIDE_LEFT:		ProfDockLeft() = rc.right - rc.left;	break;
				case DOCKSIDE_TOP:		ProfDockTop() = rc.bottom - rc.top;		break;
				case DOCKSIDE_RIGHT:	ProfDockRight() = rc.right - rc.left;	break;
				case DOCKSIDE_BOTTOM:	ProfDockBottom() = rc.bottom - rc.top;	break;
				}
				if( bType ){
					SetTypeConfig(CTypeConfig(m_nDocType), m_type);
				}
				ChangeLayout( OUTLINE_LAYOUT_FOREGROUND );	// �������g�ւ̋����ύX
				if( !IsDocking() ){
					::MoveWindow( GetHwnd(), rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top, TRUE );
				}
				if( ProfDockSync() ){
					PostOutlineNotifyToAllEditors( (WPARAM)0, (LPARAM)((CEditView*)m_lParam)->m_pcEditWnd->GetHwnd() );	// ���E�B���h�E�Ƀh�b�L���O�z�u�ύX��ʒm����
				}
				return TRUE;
			}
			return FALSE;

		case WM_KEYUP:
			if( bDragging ){
				if( msg.wParam == VK_CONTROL ){
					// �t���[�e�B���O���������郂�[�h�𔲂���
					::GetCursorPos( &pt );
					EDockSide eDockSide = GetDropRect( ptDrag, pt, &rc, false );
					SIZE sizeNew = (eDockSide <= DOCKSIDE_FLOAT)? sizeFull: sizeHalf;
					CGraphics::DrawDropRect( &rc, sizeNew, &rcDragLast, sizeLast );
					rcDragLast = rc;
					sizeLast = sizeNew;
				}
			}
			break;

		case WM_KEYDOWN:
			if( bDragging ){
				if( msg.wParam == VK_CONTROL ){
					// �t���[�e�B���O���������郂�[�h�ɓ���
					::GetCursorPos( &pt );
					GetDropRect( ptDrag, pt, &rc, true );
					CGraphics::DrawDropRect( &rc, sizeFull, &rcDragLast, sizeLast );
					sizeLast = sizeFull;
					rcDragLast = rc;
				}
			}
			if( msg.wParam == VK_ESCAPE ){
				// �L�����Z��
				::ReleaseCapture();
				if( bDragging )
					CGraphics::DrawDropRect( NULL, sizeClear, &rcDragLast, sizeLast );
				return FALSE;
			}
			break;

		case WM_RBUTTONDOWN:
			// �L�����Z��
			::ReleaseCapture();
			if( bDragging )
				CGraphics::DrawDropRect( NULL, sizeClear, &rcDragLast, sizeLast );
			return FALSE;

		default:
			::DispatchMessage( &msg );
			break;
		}
	}

	::ReleaseCapture();
	return FALSE;
}
void CDlgFuncList::LoadFileTreeSetting( CFileTreeSetting& data, SFilePath& IniDirPath )
{
	const SFileTree* pFileTree;
	if( ProfDockSet() == 0 ){
		pFileTree = &(CommonSet().m_sFileTree);
		data.m_eFileTreeSettingOrgType = EFileTreeSettingFrom_Common;
	}else{
		CDocTypeManager().GetTypeConfig(CTypeConfig(m_nDocType), m_type);
		pFileTree = &(TypeSet().m_sFileTree);
		data.m_eFileTreeSettingOrgType = EFileTreeSettingFrom_Type;
	}
	data.m_eFileTreeSettingLoadType = data.m_eFileTreeSettingOrgType;
	data.m_bProject = pFileTree->m_bProject;
	data.m_szDefaultProjectIni = pFileTree->m_szProjectIni;
	data.m_szLoadProjectIni = _T("");
	if( data.m_bProject ){
		// �e�t�H���_�̃v���W�F�N�g�t�@�C���ǂݍ���
		TCHAR szPath[_MAX_PATH];
		::GetLongFileName( _T("."), szPath );
		auto_strcat( szPath, _T("\\") );
		int maxDir = CDlgTagJumpList::CalcMaxUpDirectory( szPath );
		for( int i = 0; i <= maxDir; i++ ){
			CDataProfile cProfile;
			cProfile.SetReadingMode();
			std::tstring strIniFileName;
			strIniFileName += szPath;
			strIniFileName += CommonSet().m_sFileTreeDefIniName;
			if( cProfile.ReadProfile(strIniFileName.c_str()) ){
				CImpExpFileTree::IO_FileTreeIni(cProfile, data.m_aItems);
				data.m_eFileTreeSettingLoadType = EFileTreeSettingFrom_File;
				IniDirPath = szPath;
				CutLastYenFromDirectoryPath( IniDirPath );
				data.m_szLoadProjectIni = strIniFileName.c_str();
				break;
			}
			CDlgTagJumpList::DirUp( szPath );
		}
	}
	if( data.m_szLoadProjectIni[0] == _T('\0') ){
		// �f�t�H���g�v���W�F�N�g�t�@�C���ǂݍ���
		bool bReadIni = false;
		if( pFileTree->m_szProjectIni[0] != _T('\0') ){
			CDataProfile cProfile;
			cProfile.SetReadingMode();
			const TCHAR* pszIniFileName;
			TCHAR szDir[_MAX_PATH * 2];
			if( _IS_REL_PATH( pFileTree->m_szProjectIni ) ){
				// sakura.ini����̑��΃p�X
				GetInidirOrExedir( szDir, pFileTree->m_szProjectIni );
				pszIniFileName = szDir;
			}else{
				pszIniFileName = pFileTree->m_szProjectIni;
			}
			if( cProfile.ReadProfile(pszIniFileName) ){
				CImpExpFileTree::IO_FileTreeIni(cProfile, data.m_aItems);
				data.m_szLoadProjectIni = pszIniFileName;
				bReadIni = true;
			}
		}
		if( !bReadIni ){
			// ���ʐݒ�or�^�C�v�ʐݒ肩��ǂݍ���
			//m_fileTreeSetting = *pFileTree;
			data.m_aItems.resize( pFileTree->m_nItemCount );
			for( int i = 0; i < pFileTree->m_nItemCount; i++ ){
				data.m_aItems[i] = pFileTree->m_aItems[i];
			}
		}
	}
}
