/*!	@file
	@brief GREP�_�C�A���O�{�b�N�X

	@author Norio Nakatani
*/
/*
	Copyright (C) 1998-2001, Norio Nakatani
	Copyright (C) 2001, Stonee, genta
	Copyright (C) 2002, MIK, genta, Moca, YAZAKI
	Copyright (C) 2003, Moca
	Copyright (C) 2006, ryoji
	Copyright (C) 2010, ryoji
	Copyright (C) 2012, Uchi

	This source code is designed for sakura editor.
	Please contact the copyright holder to use this code for other purpose.
*/
#include "StdAfx.h"
#include <ShellAPI.h>
#include "dlg/CDlgGrep.h"
#include "CGrepAgent.h"
#include "CGrepEnumKeys.h"
#include "func/Funccode.h"		// Stonee, 2001/03/12
#include "charset/CCodePage.h"
#include "util/module.h"
#include "util/shell.h"
#include "util/os.h"
#include "util/window.h"
#include "env/DLLSHAREDATA.h"
#include "env/CSakuraEnvironment.h"
#include "sakura_rc.h"
#include "sakura.hh"

//GREP CDlgGrep.cpp	//@@@ 2002.01.07 add start MIK
const DWORD p_helpids[] = {	//12000
	IDC_BUTTON_FOLDER,				HIDC_GREP_BUTTON_FOLDER,			//�t�H���_
	IDC_BUTTON_CURRENTFOLDER,		HIDC_GREP_BUTTON_CURRENTFOLDER,		//���t�H���_
	IDOK,							HIDOK_GREP,							//����
	IDCANCEL,						HIDCANCEL_GREP,						//�L�����Z��
	IDC_BUTTON_HELP,				HIDC_GREP_BUTTON_HELP,				//�w���v
	IDC_CHK_WORD,					HIDC_GREP_CHK_WORD,					//�P��P��
	IDC_CHK_SUBFOLDER,				HIDC_GREP_CHK_SUBFOLDER,			//�T�u�t�H���_������
	IDC_CHK_FROMTHISTEXT,			HIDC_GREP_CHK_FROMTHISTEXT,			//���̃t�@�C������
	IDC_CHK_LOHICASE,				HIDC_GREP_CHK_LOHICASE,				//�啶��������
	IDC_CHK_REGULAREXP,				HIDC_GREP_CHK_REGULAREXP,			//���K�\��
	IDC_COMBO_CHARSET,				HIDC_GREP_COMBO_CHARSET,			//�����R�[�h�Z�b�g
	IDC_CHECK_CP,					HIDC_GREP_CHECK_CP,					//�R�[�h�y�[�W
	IDC_COMBO_TEXT,					HIDC_GREP_COMBO_TEXT,				//����
	IDC_COMBO_FILE,					HIDC_GREP_COMBO_FILE,				//�t�@�C��
	IDC_COMBO_FOLDER,				HIDC_GREP_COMBO_FOLDER,				//�t�H���_
	IDC_BUTTON_FOLDER_UP,			HIDC_GREP_BUTTON_FOLDER_UP,			//��
	IDC_RADIO_OUTPUTLINE,			HIDC_GREP_RADIO_OUTPUTLINE,			//���ʏo�́F�s�P��
	IDC_RADIO_OUTPUTMARKED,			HIDC_GREP_RADIO_OUTPUTMARKED,		//���ʏo�́F�Y������
	IDC_RADIO_OUTPUTSTYLE1,			HIDC_GREP_RADIO_OUTPUTSTYLE1,		//���ʏo�͌`���F�m�[�}��
	IDC_RADIO_OUTPUTSTYLE2,			HIDC_GREP_RADIO_OUTPUTSTYLE2,		//���ʏo�͌`���F�t�@�C����
	IDC_RADIO_OUTPUTSTYLE3,			HIDC_RADIO_OUTPUTSTYLE3,			//���ʏo�͌`���F���ʂ̂�
	IDC_STATIC_JRE32VER,			HIDC_GREP_STATIC_JRE32VER,			//���K�\���o�[�W����
	IDC_CHK_DEFAULTFOLDER,			HIDC_GREP_CHK_DEFAULTFOLDER,		//�t�H���_�̏����l���J�����g�t�H���_�ɂ���
	IDC_CHECK_FILE_ONLY,			HIDC_CHECK_FILE_ONLY,				//�t�@�C�����ŏ��̂݌���
	IDC_CHECK_BASE_PATH,			HIDC_CHECK_BASE_PATH,				//�x�[�X�t�H���_�\��
	IDC_CHECK_SEP_FOLDER,			HIDC_CHECK_SEP_FOLDER,				//�t�H���_���ɕ\��
//	IDC_STATIC,						-1,
	0, 0
};	//@@@ 2002.01.07 add end MIK

static void SetGrepFolder( HWND hwndCtrl, LPCTSTR folder );

CDlgGrep::CDlgGrep()
{
	m_bSubFolder = FALSE;				// �T�u�t�H���_�������������
	m_bFromThisText = FALSE;			// ���̕ҏW���̃e�L�X�g���猟������
	m_sSearchOption.Reset();			// �����I�v�V����
	m_nGrepCharSet = CODE_SJIS;			// �����R�[�h�Z�b�g
	m_nGrepOutputLineType = 1;			// �s���o��/�Y������/�ۃ}�b�`�s ���o��
	m_nGrepOutputStyle = 1;				// Grep: �o�͌`��
	m_bGrepOutputFileOnly = false;
	m_bGrepOutputBaseFolder = false;
	m_bGrepSeparateFolder = false;

	m_bSetText = false;
	m_szFile[0] = 0;
	m_szFolder[0] = 0;
#if REI_MOD_GREP
	m_bFolder99 = true;
	m_bFolder2 = false;
	m_bFolder3 = false;
	m_bFolder4 = false;
	m_szFolder2[0] = 0;
	m_szFolder3[0] = 0;
	m_szFolder4[0] = 0;
#endif // rei_
	return;
}

/*!
	�R���{�{�b�N�X�̃h���b�v�_�E�����b�Z�[�W��ߑ�����

	@date 2013.03.24 novice �V�K�쐬
*/
BOOL CDlgGrep::OnCbnDropDown( HWND hwndCtl, int wID )
{
	switch( wID ){
	case IDC_COMBO_TEXT:
		if ( ::SendMessage(hwndCtl, CB_GETCOUNT, 0L, 0L) == 0) {
			int nSize = m_pShareData->m_sSearchKeywords.m_aSearchKeys.size();
			for( int i = 0; i < nSize; ++i ){
				Combo_AddString( hwndCtl, m_pShareData->m_sSearchKeywords.m_aSearchKeys[i] );
			}
		}
		break;
	case IDC_COMBO_FILE:
		if ( ::SendMessage(hwndCtl, CB_GETCOUNT, 0L, 0L) == 0) {
			int nSize = m_pShareData->m_sSearchKeywords.m_aGrepFiles.size();
			for( int i = 0; i < nSize; ++i ){
				Combo_AddString( hwndCtl, m_pShareData->m_sSearchKeywords.m_aGrepFiles[i] );
			}
		}
		break;
	case IDC_COMBO_FOLDER:
#if REI_MOD_GREP
	case IDC_COMBO_FOLDER2:
	case IDC_COMBO_FOLDER3:
	case IDC_COMBO_FOLDER4:
#endif // rei_
		if ( ::SendMessage(hwndCtl, CB_GETCOUNT, 0L, 0L) == 0) {
			int nSize = m_pShareData->m_sSearchKeywords.m_aGrepFolders.size();
			for( int i = 0; i < nSize; ++i ){
				Combo_AddString( hwndCtl, m_pShareData->m_sSearchKeywords.m_aGrepFolders[i] );
			}
		}
		break;
	}
	return CDialog::OnCbnDropDown( hwndCtl, wID );
}

/* ���[�_���_�C�A���O�̕\�� */
int CDlgGrep::DoModal( HINSTANCE hInstance, HWND hwndParent, const TCHAR* pszCurrentFilePath )
{
	m_bSubFolder = m_pShareData->m_Common.m_sSearch.m_bGrepSubFolder;			// Grep: �T�u�t�H���_������
	m_sSearchOption = m_pShareData->m_Common.m_sSearch.m_sSearchOption;		// �����I�v�V����
	m_nGrepCharSet = m_pShareData->m_Common.m_sSearch.m_nGrepCharSet;			// �����R�[�h�Z�b�g
	m_nGrepOutputLineType = m_pShareData->m_Common.m_sSearch.m_nGrepOutputLineType;	// �s���o��/�Y������/�ۃ}�b�`�s ���o��
	m_nGrepOutputStyle = m_pShareData->m_Common.m_sSearch.m_nGrepOutputStyle;	// Grep: �o�͌`��
	m_bGrepOutputFileOnly = m_pShareData->m_Common.m_sSearch.m_bGrepOutputFileOnly;
	m_bGrepOutputBaseFolder = m_pShareData->m_Common.m_sSearch.m_bGrepOutputBaseFolder;
	m_bGrepSeparateFolder = m_pShareData->m_Common.m_sSearch.m_bGrepSeparateFolder;

	// 2013.05.21 �R���X�g���N�^����DoModal�Ɉړ�
	// m_strText �͌Ăяo�����Őݒ�ς�
	if( m_szFile[0] == _T('\0') && m_pShareData->m_sSearchKeywords.m_aGrepFiles.size() ){
		_tcscpy( m_szFile, m_pShareData->m_sSearchKeywords.m_aGrepFiles[0] );		/* �����t�@�C�� */
	}
	if( m_szFolder[0] == _T('\0') && m_pShareData->m_sSearchKeywords.m_aGrepFolders.size() ){
		_tcscpy( m_szFolder, m_pShareData->m_sSearchKeywords.m_aGrepFolders[0] );	/* �����t�H���_ */
	}
#if REI_MOD_GREP
	m_bFolder99 = m_pShareData->m_sSearchKeywords.m_bGrepFolders99;
	m_bFolder2 = m_pShareData->m_sSearchKeywords.m_bGrepFolders2;
	if( m_szFolder2[0] == _T('\0') && m_pShareData->m_sSearchKeywords.m_szGrepFolders2.At(0) != _T('\0')){
		_tcscpy( m_szFolder2, m_pShareData->m_sSearchKeywords.m_szGrepFolders2.GetBufferPointer() );	/* �����t�H���_ */
	}
	m_bFolder3 = m_pShareData->m_sSearchKeywords.m_bGrepFolders3;
	if( m_szFolder3[0] == _T('\0') && m_pShareData->m_sSearchKeywords.m_szGrepFolders3.At(0) != _T('\0')){
		_tcscpy( m_szFolder3, m_pShareData->m_sSearchKeywords.m_szGrepFolders3.GetBufferPointer() );	/* �����t�H���_ */
	}
	m_bFolder4 = m_pShareData->m_sSearchKeywords.m_bGrepFolders4;
	if( m_szFolder4[0] == _T('\0') && m_pShareData->m_sSearchKeywords.m_szGrepFolders4.At(0) != _T('\0')){
		_tcscpy( m_szFolder4, m_pShareData->m_sSearchKeywords.m_szGrepFolders4.GetBufferPointer() );	/* �����t�H���_ */
	}
#endif // rei_

	if( pszCurrentFilePath ){	// 2010.01.10 ryoji
		_tcscpy(m_szCurrentFilePath, pszCurrentFilePath);
	}

	return (int)CDialog::DoModal( hInstance, hwndParent, IDD_GREP, (LPARAM)NULL );
}

//	2007.02.09 bosagami
LRESULT CALLBACK OnFolderProc(HWND hwnd,UINT msg,WPARAM wparam,LPARAM lparam);
WNDPROC g_pOnFolderProc;

BOOL CDlgGrep::OnInitDialog( HWND hwndDlg, WPARAM wParam, LPARAM lParam )
{
	_SetHwnd( hwndDlg );

	/* ���[�U�[���R���{�{�b�N�X�̃G�f�B�b�g �R���g���[���ɓ��͂ł���e�L�X�g�̒����𐧌����� */
	//	Combo_LimitText( ::GetDlgItem( GetHwnd(), IDC_COMBO_TEXT ), _MAX_PATH - 1 );
	Combo_LimitText( ::GetDlgItem( GetHwnd(), IDC_COMBO_FILE ), _MAX_PATH - 1 );
	Combo_LimitText( ::GetDlgItem( GetHwnd(), IDC_COMBO_FOLDER ), _MAX_PATH - 1 );
#if REI_MOD_GREP
	Combo_LimitText( ::GetDlgItem( GetHwnd(), IDC_COMBO_FOLDER2 ), _MAX_PATH - 1 );
	Combo_LimitText( ::GetDlgItem( GetHwnd(), IDC_COMBO_FOLDER3 ), _MAX_PATH - 1 );
	Combo_LimitText( ::GetDlgItem( GetHwnd(), IDC_COMBO_FOLDER4 ), _MAX_PATH - 1 );
#endif // rei_

	/* �R���{�{�b�N�X�̃��[�U�[ �C���^�[�t�F�C�X���g���C���^�[�t�F�[�X�ɂ��� */
	Combo_SetExtendedUI( ::GetDlgItem( GetHwnd(), IDC_COMBO_TEXT ), TRUE );
	Combo_SetExtendedUI( ::GetDlgItem( GetHwnd(), IDC_COMBO_FILE ), TRUE );
	Combo_SetExtendedUI( ::GetDlgItem( GetHwnd(), IDC_COMBO_FOLDER ), TRUE );
#if REI_MOD_GREP
	Combo_SetExtendedUI( ::GetDlgItem( GetHwnd(), IDC_COMBO_FOLDER2 ), TRUE );
	Combo_SetExtendedUI( ::GetDlgItem( GetHwnd(), IDC_COMBO_FOLDER3 ), TRUE );
	Combo_SetExtendedUI( ::GetDlgItem( GetHwnd(), IDC_COMBO_FOLDER4 ), TRUE );
#endif // rei_

	/* �_�C�A���O�̃A�C�R�� */
//2002.02.08 Grep�A�C�R�����傫���A�C�R���Ə������A�C�R����ʁX�ɂ���B
	HICON	hIconBig, hIconSmall;
	//	Dec, 2, 2002 genta �A�C�R���ǂݍ��ݕ��@�ύX
	hIconBig   = GetAppIcon( m_hInstance, ICON_DEFAULT_GREP, FN_GREP_ICON, false );
	hIconSmall = GetAppIcon( m_hInstance, ICON_DEFAULT_GREP, FN_GREP_ICON, true );
	::SendMessageAny( GetHwnd(), WM_SETICON, ICON_SMALL, (LPARAM)hIconSmall );
	::SendMessageAny( GetHwnd(), WM_SETICON, ICON_BIG, (LPARAM)hIconBig );

	// 2002/09/22 Moca Add
	int i;
	/* �����R�[�h�Z�b�g�I���R���{�{�b�N�X������ */
	CCodeTypesForCombobox cCodeTypes;
	for( i = 0; i < cCodeTypes.GetCount(); ++i ){
		int idx = Combo_AddString( ::GetDlgItem( GetHwnd(), IDC_COMBO_CHARSET ), cCodeTypes.GetName(i) );
		Combo_SetItemData( ::GetDlgItem( GetHwnd(), IDC_COMBO_CHARSET ), idx, cCodeTypes.GetCode(i) );
	}
	//	2007.02.09 bosagami
	HWND hFolder = ::GetDlgItem( GetHwnd(), IDC_COMBO_FOLDER );
	DragAcceptFiles(hFolder, true);
	g_pOnFolderProc = (WNDPROC)GetWindowLongPtr(hFolder, GWLP_WNDPROC);
	SetWindowLongPtr(hFolder, GWLP_WNDPROC, (LONG_PTR)OnFolderProc);
#if REI_MOD_GREP
	HWND hFolder2 = ::GetDlgItem( GetHwnd(), IDC_COMBO_FOLDER2 );
	DragAcceptFiles(hFolder2, true);
	SetWindowLongPtr(hFolder2, GWLP_WNDPROC, (LONG_PTR)OnFolderProc);
	HWND hFolder3 = ::GetDlgItem( GetHwnd(), IDC_COMBO_FOLDER3 );
	DragAcceptFiles(hFolder3, true);
	SetWindowLongPtr(hFolder3, GWLP_WNDPROC, (LONG_PTR)OnFolderProc);
	HWND hFolder4 = ::GetDlgItem( GetHwnd(), IDC_COMBO_FOLDER4 );
	DragAcceptFiles(hFolder4, true);
	SetWindowLongPtr(hFolder4, GWLP_WNDPROC, (LONG_PTR)OnFolderProc);
#endif // rei_

	m_comboDelText = SComboBoxItemDeleter();
	m_comboDelText.pRecent = &m_cRecentSearch;
	SetComboBoxDeleter(GetItemHwnd(IDC_COMBO_TEXT), &m_comboDelText);
	m_comboDelFile = SComboBoxItemDeleter();
	m_comboDelFile.pRecent = &m_cRecentGrepFile;
	SetComboBoxDeleter(GetItemHwnd(IDC_COMBO_FILE), &m_comboDelFile);
	m_comboDelFolder = SComboBoxItemDeleter();
	m_comboDelFolder.pRecent = &m_cRecentGrepFolder;
	SetComboBoxDeleter(GetItemHwnd(IDC_COMBO_FOLDER), &m_comboDelFolder);

	// �t�H���g�ݒ�	2012/11/27 Uchi
	HFONT hFontOld = (HFONT)::SendMessageAny( GetItemHwnd( IDC_COMBO_TEXT ), WM_GETFONT, 0, 0 );
	HFONT hFont = SetMainFont( GetItemHwnd( IDC_COMBO_TEXT ) );
	m_cFontText.SetFont( hFontOld, hFont, GetItemHwnd( IDC_COMBO_TEXT ) );

	/* ���N���X�����o */
//	CreateSizeBox();
	return CDialog::OnInitDialog( hwndDlg, wParam, lParam );
}

/*! @brief �t�H���_�w��EditBox�̃R�[���o�b�N�֐�

	@date 2007.02.09 bosagami �V�K�쐬
	@date 2007.09.02 genta �f�B���N�g���`�F�b�N������
*/
LRESULT CALLBACK OnFolderProc(HWND hwnd,UINT msg,WPARAM wparam,LPARAM lparam)
{
	if(msg == WM_DROPFILES){
		//	From Here 2007.09.02 genta 
		SFilePath sPath;
		if( DragQueryFile((HDROP)wparam, 0, NULL, 0 ) > _countof2(sPath) - 1 ){
			// skip if the length of the path exceeds buffer capacity
			::DragFinish((HDROP)wparam);
			return 0;
		}
		DragQueryFile((HDROP)wparam, 0, sPath, _countof2(sPath) - 1);
		::DragFinish((HDROP)wparam);

		//�t�@�C���p�X�̉���
		CSakuraEnvironment::ResolvePath(sPath);
		
		//	�t�@�C�����h���b�v���ꂽ�ꍇ�̓t�H���_��؂�o��
		//	�t�H���_�̏ꍇ�͍Ōオ������̂�split���Ă͂����Ȃ��D
		if( IsFileExists( sPath, true )){	//	��2������true���ƃf�B���N�g���͑ΏۊO
			SFilePath szWork;
			SplitPath_FolderAndFile( sPath, szWork, NULL );
			_tcscpy( sPath, szWork );
		}

		return 0;
	}

	return  CallWindowProc(g_pOnFolderProc,hwnd,msg,wparam,lparam);
}

BOOL CDlgGrep::OnDestroy()
{
	m_cFontText.ReleaseOnDestroy();
	return CDialog::OnDestroy();
}

BOOL CDlgGrep::OnBnClicked( int wID )
{
	switch( wID ){
	case IDC_BUTTON_HELP:
		/* �uGrep�v�̃w���v */
		//Stonee, 2001/03/12 ��l�������A�@�\�ԍ�����w���v�g�s�b�N�ԍ��𒲂ׂ�悤�ɂ���
		MyWinHelp( GetHwnd(), HELP_CONTEXT, ::FuncID_To_HelpContextID(F_GREP_DIALOG) );	// 2006.10.10 ryoji MyWinHelp�ɕύX�ɕύX
		return TRUE;
#if REI_MOD_GREP
	case IDC_CHK_FOLDER99:
	case IDC_CHK_FOLDER2:
	case IDC_CHK_FOLDER3:
	case IDC_CHK_FOLDER4:
	case IDC_CHK_FROMTHISTEXT:	/* ���̕ҏW���̃e�L�X�g���猟������ */
	  // 2010.05.30 �֐���
		SetDataFromThisText(false);
		return TRUE;
#else // rei_
	case IDC_CHK_FROMTHISTEXT:	/* ���̕ҏW���̃e�L�X�g���猟������ */
		// 2010.05.30 �֐���
		SetDataFromThisText( 0 != ::IsDlgButtonChecked( GetHwnd(), IDC_CHK_FROMTHISTEXT ) );
		return TRUE;
#endif  // rei_
	case IDC_BUTTON_CURRENTFOLDER:	/* ���ݕҏW���̃t�@�C���̃t�H���_ */
		/* �t�@�C�����J���Ă��邩 */
		if( m_szCurrentFilePath[0] != _T('\0') ){
			TCHAR	szWorkFolder[MAX_PATH];
			TCHAR	szWorkFile[MAX_PATH];
			SplitPath_FolderAndFile( m_szCurrentFilePath, szWorkFolder, szWorkFile );
			SetGrepFolder( GetItemHwnd(IDC_COMBO_FOLDER), szWorkFolder );
		}
		else{
			/* ���݂̃v���Z�X�̃J�����g�f�B���N�g�����擾���܂� */
			TCHAR	szWorkFolder[MAX_PATH];
			::GetCurrentDirectory( _countof( szWorkFolder ) - 1, szWorkFolder );
			SetGrepFolder( GetItemHwnd(IDC_COMBO_FOLDER), szWorkFolder );
		}
		return TRUE;
	case IDC_BUTTON_FOLDER_UP:
		{
			HWND hwnd = GetItemHwnd( IDC_COMBO_FOLDER );
			TCHAR szFolder[_MAX_PATH];
			::GetWindowText( hwnd, szFolder, _countof(szFolder) );
			std::vector<std::tstring> vPaths;
			CGrepAgent::CreateFolders( szFolder, vPaths );
			if( 0 < vPaths.size() ){
				// �Ō�̃p�X������Ώ�
				auto_strncpy( szFolder, vPaths.rbegin()->c_str(), _MAX_PATH );
				szFolder[_MAX_PATH-1] = _T('\0');
				if( DirectoryUp( szFolder ) ){
					*(vPaths.rbegin()) = szFolder;
					szFolder[0] = _T('\0');
					for( int i = 0 ; i < (int)vPaths.size(); i++ ){
						TCHAR szFolderItem[_MAX_PATH];
						auto_strncpy( szFolderItem, vPaths[i].c_str(), _MAX_PATH );
						szFolderItem[_MAX_PATH-1] = _T('\0');
						if( auto_strchr( szFolderItem, _T(';') ) ){
							szFolderItem[0] = _T('"');
							auto_strncpy( szFolderItem + 1, vPaths[i].c_str(), _MAX_PATH - 1 );
							szFolderItem[_MAX_PATH-1] = _T('\0');
							auto_strcat( szFolderItem, _T("\"") );
							szFolderItem[_MAX_PATH-1] = _T('\0');
						}
						if( i ){
							auto_strcat( szFolder, _T(";") );
							szFolder[_MAX_PATH-1] = _T('\0');
						}
						auto_strcat_s( szFolder, _MAX_PATH, szFolderItem );
					}
					::SetWindowText( hwnd, szFolder );
				}
			}
		}
		return TRUE;


//	case IDC_CHK_LOHICASE:	/* �p�啶���Ɖp����������ʂ��� */
//		MYTRACE( _T("IDC_CHK_LOHICASE\n") );
//		return TRUE;
	case IDC_CHK_REGULAREXP:	/* ���K�\�� */
//		MYTRACE( _T("IDC_CHK_REGULAREXP ::IsDlgButtonChecked( GetHwnd(), IDC_CHK_REGULAREXP ) = %d\n"), ::IsDlgButtonChecked( GetHwnd(), IDC_CHK_REGULAREXP ) );
		if( ::IsDlgButtonChecked( GetHwnd(), IDC_CHK_REGULAREXP ) ){
			// From Here Jun. 26, 2001 genta
			//	���K�\�����C�u�����̍����ւ��ɔ��������̌�����
			if( !CheckRegexpVersion( GetHwnd(), IDC_STATIC_JRE32VER, true ) ){
				::CheckDlgButton( GetHwnd(), IDC_CHK_REGULAREXP, 0 );
			}else{
				//	To Here Jun. 26, 2001 genta
				/* �p�啶���Ɖp����������ʂ��� */
				//	���K�\���̂Ƃ����I���ł���悤�ɁB
//				::CheckDlgButton( GetHwnd(), IDC_CHK_LOHICASE, 1 );
//				::EnableWindow( ::GetDlgItem( GetHwnd(), IDC_CHK_LOHICASE ), FALSE );

				//2001/06/23 N.Nakatani
				/* �P��P�ʂŌ��� */
				::EnableWindow( ::GetDlgItem( GetHwnd(), IDC_CHK_WORD ), FALSE );
			}
		}else{
			/* �p�啶���Ɖp����������ʂ��� */
			//	���K�\���̂Ƃ����I���ł���悤�ɁB
//			::EnableWindow( ::GetDlgItem( GetHwnd(), IDC_CHK_LOHICASE ), TRUE );
//			::CheckDlgButton( GetHwnd(), IDC_CHK_LOHICASE, 0 );


//2001/06/23 N.Nakatani
//�P��P�ʂ�grep���������ꂽ��R�����g���O���Ǝv���܂�
//2002/03/07�������Ă݂��B
			/* �P��P�ʂŌ��� */
			::EnableWindow( ::GetDlgItem( GetHwnd(), IDC_CHK_WORD ), TRUE );

		}
		return TRUE;

	case IDC_BUTTON_FOLDER:
		/* �t�H���_�Q�ƃ{�^�� */
		{
			TCHAR	szFolder[MAX_PATH];
			/* �����t�H���_ */
			::DlgItem_GetText( GetHwnd(), IDC_COMBO_FOLDER, szFolder, _MAX_PATH - 1 );
			if( szFolder[0] == _T('\0') ){
				::GetCurrentDirectory( _countof( szFolder ), szFolder );
			}
			if( SelectDir( GetHwnd(), LS(STR_DLGGREP1), szFolder, szFolder ) ){
				SetGrepFolder( GetItemHwnd(IDC_COMBO_FOLDER), szFolder );
			}
		}
		
		return TRUE;
#if REI_MOD_GREP
	case IDC_BUTTON_FOLDER2:
		/* �t�H���_�Q�ƃ{�^�� */
		{
			TCHAR	szFolder[MAX_PATH];
			/* �����t�H���_ */
			::DlgItem_GetText( GetHwnd(), IDC_COMBO_FOLDER2, szFolder, _MAX_PATH - 1 );
			if( szFolder[0] == _T('\0') ){
				::GetCurrentDirectory( _countof( szFolder ), szFolder );
			}
			if( SelectDir( GetHwnd(), LS(STR_DLGGREP1), szFolder, szFolder ) ){
				SetGrepFolder( GetItemHwnd(IDC_COMBO_FOLDER2), szFolder );
			}
		}
		
		return TRUE;
	case IDC_BUTTON_FOLDER3:
		/* �t�H���_�Q�ƃ{�^�� */
		{
			TCHAR	szFolder[MAX_PATH];
			/* �����t�H���_ */
			::DlgItem_GetText( GetHwnd(), IDC_COMBO_FOLDER3, szFolder, _MAX_PATH - 1 );
			if( szFolder[0] == _T('\0') ){
				::GetCurrentDirectory( _countof( szFolder ), szFolder );
			}
			if( SelectDir( GetHwnd(), LS(STR_DLGGREP1), szFolder, szFolder ) ){
				SetGrepFolder( GetItemHwnd(IDC_COMBO_FOLDER3), szFolder );
			}
		}
		
		return TRUE;
	case IDC_BUTTON_FOLDER4:
		/* �t�H���_�Q�ƃ{�^�� */
		{
			TCHAR	szFolder[MAX_PATH];
			/* �����t�H���_ */
			::DlgItem_GetText( GetHwnd(), IDC_COMBO_FOLDER4, szFolder, _MAX_PATH - 1 );
			if( szFolder[0] == _T('\0') ){
				::GetCurrentDirectory( _countof( szFolder ), szFolder );
			}
			if( SelectDir( GetHwnd(), LS(STR_DLGGREP1), szFolder, szFolder ) ){
				SetGrepFolder( GetItemHwnd(IDC_COMBO_FOLDER4), szFolder );
			}
		}
		
		return TRUE;
#endif // rei_
	case IDC_CHECK_CP:
		{
			if( IsDlgButtonChecked( GetHwnd(), IDC_CHECK_CP ) ){
				::EnableWindow( ::GetDlgItem( GetHwnd(), IDC_CHECK_CP ), FALSE );
				HWND combo = ::GetDlgItem( GetHwnd(), IDC_COMBO_CHARSET );
				CCodePage::AddComboCodePages(GetHwnd(), combo, -1);
			}
		}
		return TRUE;
	case IDC_CHK_DEFAULTFOLDER:
		/* �t�H���_�̏����l���J�����g�t�H���_�ɂ��� */
		{
			m_pShareData->m_Common.m_sSearch.m_bGrepDefaultFolder = ::IsDlgButtonChecked( GetHwnd(), IDC_CHK_DEFAULTFOLDER );
		}
		return TRUE;
	case IDC_RADIO_OUTPUTSTYLE3:
		{
			::EnableWindow( ::GetDlgItem( GetHwnd(), IDC_CHECK_BASE_PATH ), FALSE );
			::EnableWindow( ::GetDlgItem( GetHwnd(), IDC_CHECK_SEP_FOLDER ),FALSE );
		}
		break;
	case IDC_RADIO_OUTPUTSTYLE1:
	case IDC_RADIO_OUTPUTSTYLE2:
		{
			::EnableWindow( ::GetDlgItem( GetHwnd(), IDC_CHECK_BASE_PATH ), TRUE );
			::EnableWindow( ::GetDlgItem( GetHwnd(), IDC_CHECK_SEP_FOLDER ),TRUE );
		}
		break;
	case IDOK:
		/* �_�C�A���O�f�[�^�̎擾 */
		if( GetData() ){
//			::EndDialog( hwndDlg, TRUE );
			CloseDialog( TRUE );
		}
		return TRUE;
	case IDCANCEL:
//		::EndDialog( hwndDlg, FALSE );
		CloseDialog( FALSE );
		return TRUE;
	}

	/* ���N���X�����o */
	return CDialog::OnBnClicked( wID );
}



/* �_�C�A���O�f�[�^�̐ݒ� */
void CDlgGrep::SetData( void )
{
	/* ���������� */
	::DlgItem_SetText( GetHwnd(), IDC_COMBO_TEXT, m_strText.c_str() );

	/* �����t�@�C�� */
	::DlgItem_SetText( GetHwnd(), IDC_COMBO_FILE, m_szFile );

	/* �����t�H���_ */
	::DlgItem_SetText( GetHwnd(), IDC_COMBO_FOLDER, m_szFolder );
#if REI_MOD_GREP
	::CheckDlgButton( GetHwnd(), IDC_CHK_FOLDER99, m_bFolder99 );
	::CheckDlgButton( GetHwnd(), IDC_CHK_FOLDER2, m_bFolder2 );
	::CheckDlgButton( GetHwnd(), IDC_CHK_FOLDER3, m_bFolder3 );
	::CheckDlgButton( GetHwnd(), IDC_CHK_FOLDER4, m_bFolder4 );
	::DlgItem_SetText( GetHwnd(), IDC_COMBO_FOLDER2, m_szFolder2 );
	::DlgItem_SetText( GetHwnd(), IDC_COMBO_FOLDER3, m_szFolder3 );
	::DlgItem_SetText( GetHwnd(), IDC_COMBO_FOLDER4, m_szFolder4 );
#endif // rei_

	if((m_szFolder[0] == _T('\0') || m_pShareData->m_Common.m_sSearch.m_bGrepDefaultFolder) &&
		m_szCurrentFilePath[0] != _T('\0')
	){
		TCHAR	szWorkFolder[MAX_PATH];
		TCHAR	szWorkFile[MAX_PATH];
		SplitPath_FolderAndFile( m_szCurrentFilePath, szWorkFolder, szWorkFile );
		SetGrepFolder( GetItemHwnd(IDC_COMBO_FOLDER), szWorkFolder );
	}

	/* �T�u�t�H���_������������� */
	::CheckDlgButton( GetHwnd(), IDC_CHK_SUBFOLDER, m_bSubFolder );

#if REI_MOD_GREP == 0
	// ���̕ҏW���̃e�L�X�g���猟������
	::CheckDlgButton( GetHwnd(), IDC_CHK_FROMTHISTEXT, m_bFromThisText );
#endif  // rei_
	// 2010.05.30 �֐���
	SetDataFromThisText( m_bFromThisText != FALSE );

	/* �p�啶���Ɖp����������ʂ��� */
	::CheckDlgButton( GetHwnd(), IDC_CHK_LOHICASE, m_sSearchOption.bLoHiCase );

	// 2001/06/23 N.Nakatani �����_�ł�Grep�ł͒P��P�ʂ̌����̓T�|�[�g�ł��Ă��܂���
	// 2002/03/07 �e�X�g�T�|�[�g
	/* ��v����P��̂݌������� */
	::CheckDlgButton( GetHwnd(), IDC_CHK_WORD, m_sSearchOption.bWordOnly );
//	::EnableWindow( ::GetDlgItem( GetHwnd(), IDC_CHK_WORD ) , false );	//�`�F�b�N�{�b�N�X���g�p�s�ɂ���


	/* �����R�[�h�������� */
//	::CheckDlgButton( GetHwnd(), IDC_CHK_KANJICODEAUTODETECT, m_bKanjiCode_AutoDetect );

	// 2002/09/22 Moca Add
	/* �����R�[�h�Z�b�g */
	{
		int		nIdx, nCurIdx = -1;
		ECodeType nCharSet;
		HWND	hWndCombo = ::GetDlgItem( GetHwnd(), IDC_COMBO_CHARSET );
		nCurIdx = Combo_GetCurSel( hWndCombo );
		CCodeTypesForCombobox cCodeTypes;
		for( nIdx = 0; nIdx < cCodeTypes.GetCount(); nIdx++ ){
			nCharSet = (ECodeType)Combo_GetItemData( hWndCombo, nIdx );
			if( nCharSet == m_nGrepCharSet ){
				nCurIdx = nIdx;
			}
		}
		if( nCurIdx != -1 ){
			Combo_SetCurSel( hWndCombo, nCurIdx );
		}else{
			::CheckDlgButton( GetHwnd(), IDC_CHECK_CP, TRUE );
			::EnableWindow( ::GetDlgItem( GetHwnd(), IDC_CHECK_CP ), FALSE );
			nCurIdx = CCodePage::AddComboCodePages(GetHwnd(), hWndCombo, m_nGrepCharSet);
			if( nCurIdx == -1 ){
				Combo_SetCurSel( hWndCombo, 0 );
			}
		}
	}

	/* �s���o�͂��邩�Y�����������o�͂��邩 */
	if( m_nGrepOutputLineType == 1 ){
		::CheckDlgButton( GetHwnd(), IDC_RADIO_OUTPUTLINE, TRUE );
	}else if( m_nGrepOutputLineType == 2 ){
		::CheckDlgButton( GetHwnd(), IDC_RADIO_NOHIT, TRUE );
	}else{
		::CheckDlgButton( GetHwnd(), IDC_RADIO_OUTPUTMARKED, TRUE );
	}

	::EnableWindow( ::GetDlgItem( GetHwnd(), IDC_CHECK_BASE_PATH ), TRUE );
	::EnableWindow( ::GetDlgItem( GetHwnd(), IDC_CHECK_SEP_FOLDER ),TRUE );
	/* Grep: �o�͌`�� */
	if( 1 == m_nGrepOutputStyle ){
		::CheckDlgButton( GetHwnd(), IDC_RADIO_OUTPUTSTYLE1, TRUE );
	}else
	if( 2 == m_nGrepOutputStyle ){
		::CheckDlgButton( GetHwnd(), IDC_RADIO_OUTPUTSTYLE2, TRUE );
	}else
	if( 3 == m_nGrepOutputStyle ){
		::CheckDlgButton( GetHwnd(), IDC_RADIO_OUTPUTSTYLE3, TRUE );
		::EnableWindow( ::GetDlgItem( GetHwnd(), IDC_CHECK_BASE_PATH ), FALSE );
		::EnableWindow( ::GetDlgItem( GetHwnd(), IDC_CHECK_SEP_FOLDER ),FALSE );
	}else{
		::CheckDlgButton( GetHwnd(), IDC_RADIO_OUTPUTSTYLE1, TRUE );
	}

	// From Here Jun. 29, 2001 genta
	// ���K�\�����C�u�����̍����ւ��ɔ��������̌�����
	// �����t���[�y�є�������̌������B�K�����K�\���̃`�F�b�N��
	// ���֌W��CheckRegexpVersion��ʉ߂���悤�ɂ����B
	if( CheckRegexpVersion( GetHwnd(), IDC_STATIC_JRE32VER, false )
		&& m_sSearchOption.bRegularExp){
		/* �p�啶���Ɖp����������ʂ��� */
		::CheckDlgButton( GetHwnd(), IDC_CHK_REGULAREXP, 1 );
		//	���K�\���̂Ƃ����I���ł���悤�ɁB
//		::CheckDlgButton( GetHwnd(), IDC_CHK_LOHICASE, 1 );
//		::EnableWindow( ::GetDlgItem( GetHwnd(), IDC_CHK_LOHICASE ), FALSE );

		// 2001/06/23 N.Nakatani
		/* �P��P�ʂŒT�� */
		::EnableWindow( ::GetDlgItem( GetHwnd(), IDC_CHK_WORD ), FALSE );
	}
	else {
		::CheckDlgButton( GetHwnd(), IDC_CHK_REGULAREXP, 0 );
	}
	// To Here Jun. 29, 2001 genta

#if REI_MOD_GREP == 0
	if( m_szCurrentFilePath[0] != _T('\0') ){
		::EnableWindow( ::GetDlgItem( GetHwnd(), IDC_CHK_FROMTHISTEXT ), TRUE );
	}else{
		::EnableWindow( ::GetDlgItem( GetHwnd(), IDC_CHK_FROMTHISTEXT ), FALSE );
	}
#endif  // rei_

	CheckDlgButtonBool( GetHwnd(), IDC_CHECK_FILE_ONLY, m_bGrepOutputFileOnly );
	CheckDlgButtonBool( GetHwnd(), IDC_CHECK_BASE_PATH, m_bGrepOutputBaseFolder );
	CheckDlgButtonBool( GetHwnd(), IDC_CHECK_SEP_FOLDER, m_bGrepSeparateFolder );

	// �t�H���_�̏����l���J�����g�t�H���_�ɂ���
	::CheckDlgButton( GetHwnd(), IDC_CHK_DEFAULTFOLDER, m_pShareData->m_Common.m_sSearch.m_bGrepDefaultFolder );

	return;
}


/*!
	���ݕҏW���t�@�C�����猟���`�F�b�N�ł̐ݒ�
*/
void CDlgGrep::SetDataFromThisText( bool bChecked )
{
#if REI_MOD_GREP
  // Grep�t�H���_�̃`�F�b�N�����ׂĊO�����Ƃ��u�t�@�C���v�Ɓu�T�u�t�H���_�����v�������ɂȂ�
  //   ���u�ҏW���̃t�@�C�����猟���v�Ɠ��Ӌ`
  
  BOOL f = (0 != ::IsDlgButtonChecked(GetHwnd(), IDC_CHK_FROMTHISTEXT));
  m_bFromThisText = f;
  
	::EnableWindow(::GetDlgItem(GetHwnd(), IDC_CHK_FOLDER99), !f);
	::EnableWindow(::GetDlgItem(GetHwnd(), IDC_CHK_FOLDER2), !f);
	::EnableWindow(::GetDlgItem(GetHwnd(), IDC_CHK_FOLDER3), !f);
	::EnableWindow(::GetDlgItem(GetHwnd(), IDC_CHK_FOLDER4), !f);
	
	BOOL bFolder99 = !f && (0 != ::IsDlgButtonChecked(GetHwnd(), IDC_CHK_FOLDER99));
	BOOL bFolder2 = !f && (0 != ::IsDlgButtonChecked(GetHwnd(), IDC_CHK_FOLDER2));
	BOOL bFolder3 = !f && (0 != ::IsDlgButtonChecked(GetHwnd(), IDC_CHK_FOLDER3));
	BOOL bFolder4 = !f && (0 != ::IsDlgButtonChecked(GetHwnd(), IDC_CHK_FOLDER4));
	BOOL bThisText = f || (!bFolder99 && !bFolder2 && !bFolder3 && !bFolder4);
	
	::EnableWindow( ::GetDlgItem( GetHwnd(), IDC_COMBO_FOLDER ),  bFolder99 );
	::EnableWindow( ::GetDlgItem( GetHwnd(), IDC_COMBO_FOLDER2 ), bFolder2 );
	::EnableWindow( ::GetDlgItem( GetHwnd(), IDC_COMBO_FOLDER3 ), bFolder3 );
	::EnableWindow( ::GetDlgItem( GetHwnd(), IDC_COMBO_FOLDER4 ), bFolder4 );
	::EnableWindow( ::GetDlgItem( GetHwnd(), IDC_BUTTON_FOLDER ), bFolder99 );
	::EnableWindow( ::GetDlgItem( GetHwnd(), IDC_BUTTON_FOLDER2 ), bFolder2 );
	::EnableWindow( ::GetDlgItem( GetHwnd(), IDC_BUTTON_FOLDER3 ), bFolder3 );
	::EnableWindow( ::GetDlgItem( GetHwnd(), IDC_BUTTON_FOLDER4 ), bFolder4 );
	
	::EnableWindow( ::GetDlgItem( GetHwnd(), IDC_COMBO_FILE ),    !bThisText );
	::EnableWindow( ::GetDlgItem( GetHwnd(), IDC_CHK_SUBFOLDER ), !bThisText );
#else
	BOOL bEnableControls = TRUE;
	if( 0 != m_szCurrentFilePath[0] && bChecked ){
		TCHAR	szWorkFolder[MAX_PATH];
		TCHAR	szWorkFile[MAX_PATH];
		// 2003.08.01 Moca �t�@�C�����̓X�y�[�X�Ȃǂ͋�؂�L���ɂȂ�̂ŁA""�ň͂��A�G�X�P�[�v����
		szWorkFile[0] = _T('"');
		SplitPath_FolderAndFile( m_szCurrentFilePath, szWorkFolder, szWorkFile + 1 );
		_tcscat( szWorkFile, _T("\"") ); // 2003.08.01 Moca
		::DlgItem_SetText( GetHwnd(), IDC_COMBO_FILE, szWorkFile );
		
		SetGrepFolder( GetItemHwnd(IDC_COMBO_FOLDER), szWorkFolder );

		::CheckDlgButton( GetHwnd(), IDC_CHK_SUBFOLDER, BST_UNCHECKED );
		bEnableControls = FALSE;
	}
	::EnableWindow( ::GetDlgItem( GetHwnd(), IDC_COMBO_FILE ),    bEnableControls );
	::EnableWindow( ::GetDlgItem( GetHwnd(), IDC_COMBO_FOLDER ),  bEnableControls );
	::EnableWindow( ::GetDlgItem( GetHwnd(), IDC_BUTTON_FOLDER ), bEnableControls );
	::EnableWindow( ::GetDlgItem( GetHwnd(), IDC_CHK_SUBFOLDER ), bEnableControls );
#endif // rei_
	return;
}

/*! �_�C�A���O�f�[�^�̎擾
	@retval TRUE  ����
	@retval FALSE ���̓G���[
*/
int CDlgGrep::GetData( void )
{
	/* �T�u�t�H���_�������������*/
	m_bSubFolder = ::IsDlgButtonChecked( GetHwnd(), IDC_CHK_SUBFOLDER );

	/* ���̕ҏW���̃e�L�X�g���猟������ */
#if REI_MOD_GREP == 0
	m_bFromThisText = ::IsDlgButtonChecked( GetHwnd(), IDC_CHK_FROMTHISTEXT );
#endif  // rei_

	/* �p�啶���Ɖp����������ʂ��� */
	m_sSearchOption.bLoHiCase = (0!=::IsDlgButtonChecked( GetHwnd(), IDC_CHK_LOHICASE ));

	//2001/06/23 N.Nakatani
	/* �P��P�ʂŌ��� */
	m_sSearchOption.bWordOnly = (0!=::IsDlgButtonChecked( GetHwnd(), IDC_CHK_WORD ));

	/* ���K�\�� */
	m_sSearchOption.bRegularExp = (0!=::IsDlgButtonChecked( GetHwnd(), IDC_CHK_REGULAREXP ));

	/* �����R�[�h�������� */
//	m_bKanjiCode_AutoDetect = ::IsDlgButtonChecked( GetHwnd(), IDC_CHK_KANJICODEAUTODETECT );

	/* �����R�[�h�Z�b�g */
	{
		int		nIdx;
		HWND	hWndCombo = ::GetDlgItem( GetHwnd(), IDC_COMBO_CHARSET );
		nIdx = Combo_GetCurSel( hWndCombo );
		m_nGrepCharSet = (ECodeType)Combo_GetItemData( hWndCombo, nIdx );
	}


	/* �s���o��/�Y������/�ۃ}�b�`�s ���o�� */
	if( ::IsDlgButtonChecked( GetHwnd(), IDC_RADIO_OUTPUTLINE ) ){
		m_nGrepOutputLineType = 1;
	}else if( ::IsDlgButtonChecked( GetHwnd(), IDC_RADIO_NOHIT ) ){
		m_nGrepOutputLineType = 2;
	}else{
		m_nGrepOutputLineType = 0;
	}

	/* Grep: �o�͌`�� */
	if( FALSE != ::IsDlgButtonChecked( GetHwnd(), IDC_RADIO_OUTPUTSTYLE1 ) ){
		m_nGrepOutputStyle = 1;				/* Grep: �o�͌`�� */
	}
	if( FALSE != ::IsDlgButtonChecked( GetHwnd(), IDC_RADIO_OUTPUTSTYLE2 ) ){
		m_nGrepOutputStyle = 2;				/* Grep: �o�͌`�� */
	}
	if( FALSE != ::IsDlgButtonChecked( GetHwnd(), IDC_RADIO_OUTPUTSTYLE3 ) ){
		m_nGrepOutputStyle = 3;
	}

	m_bGrepOutputFileOnly = IsDlgButtonCheckedBool( GetHwnd(), IDC_CHECK_FILE_ONLY );
	m_bGrepOutputBaseFolder = IsDlgButtonCheckedBool( GetHwnd(), IDC_CHECK_BASE_PATH );
	m_bGrepSeparateFolder = IsDlgButtonCheckedBool( GetHwnd(), IDC_CHECK_SEP_FOLDER );


	/* ���������� */
	int nBufferSize = ::GetWindowTextLength( GetItemHwnd(IDC_COMBO_TEXT) ) + 1;
	std::vector<TCHAR> vText(nBufferSize);
	::DlgItem_GetText( GetHwnd(), IDC_COMBO_TEXT, &vText[0], nBufferSize);
	m_strText = to_wchar(&vText[0]);
	m_bSetText = true;
	/* �����t�@�C�� */
	::DlgItem_GetText( GetHwnd(), IDC_COMBO_FILE, m_szFile, _countof2(m_szFile) );
	/* �����t�H���_ */
	::DlgItem_GetText( GetHwnd(), IDC_COMBO_FOLDER, m_szFolder, _countof2(m_szFolder) );
#if REI_MOD_GREP
	m_bFolder99 = ::IsDlgButtonChecked( GetHwnd(), IDC_CHK_FOLDER99 );
	m_bFolder2 = ::IsDlgButtonChecked( GetHwnd(), IDC_CHK_FOLDER2 );
	m_bFolder3 = ::IsDlgButtonChecked( GetHwnd(), IDC_CHK_FOLDER3 );
	m_bFolder4 = ::IsDlgButtonChecked( GetHwnd(), IDC_CHK_FOLDER4 );
	::DlgItem_GetText( GetHwnd(), IDC_COMBO_FOLDER2, m_szFolder2, _countof2(m_szFolder2) );
	::DlgItem_GetText( GetHwnd(), IDC_COMBO_FOLDER3, m_szFolder3, _countof2(m_szFolder3) );
	::DlgItem_GetText( GetHwnd(), IDC_COMBO_FOLDER4, m_szFolder4, _countof2(m_szFolder4) );
	
	m_pShareData->m_sSearchKeywords.m_bGrepFolders99 = m_bFolder99;
	m_pShareData->m_sSearchKeywords.m_bGrepFolders2 = m_bFolder2;
	m_pShareData->m_sSearchKeywords.m_bGrepFolders3 = m_bFolder3;
	m_pShareData->m_sSearchKeywords.m_bGrepFolders4 = m_bFolder4;
	m_pShareData->m_sSearchKeywords.m_szGrepFolders2 = m_szFolder2;
	m_pShareData->m_sSearchKeywords.m_szGrepFolders3 = m_szFolder3;
	m_pShareData->m_sSearchKeywords.m_szGrepFolders4 = m_szFolder4;
#endif // rei_

	m_pShareData->m_Common.m_sSearch.m_nGrepCharSet = m_nGrepCharSet;			// �����R�[�h��������
	m_pShareData->m_Common.m_sSearch.m_nGrepOutputLineType = m_nGrepOutputLineType;	// �s���o��/�Y������/�ۃ}�b�`�s ���o��
	m_pShareData->m_Common.m_sSearch.m_nGrepOutputStyle = m_nGrepOutputStyle;	// Grep: �o�͌`��
	m_pShareData->m_Common.m_sSearch.m_bGrepOutputFileOnly = m_bGrepOutputFileOnly;
	m_pShareData->m_Common.m_sSearch.m_bGrepOutputBaseFolder = m_bGrepOutputBaseFolder;
	m_pShareData->m_Common.m_sSearch.m_bGrepSeparateFolder = m_bGrepSeparateFolder;

//��߂܂���
//	if( 0 == wcslen( m_szText ) ){
//		WarningMessage(	GetHwnd(), _T("�����̃L�[���[�h���w�肵�Ă��������B") );
//		return FALSE;
//	}
	if( 0 != auto_strlen( m_szFile ) ){
		CGrepEnumKeys enumKeys;
		int nErrorNo = enumKeys.SetFileKeys( m_szFile );
		if( 1 == nErrorNo ){
			WarningMessage(	GetHwnd(), LS(STR_DLGGREP2) );
			return FALSE;
		}else if( nErrorNo == 2 ){
			WarningMessage(	GetHwnd(), LS(STR_DLGGREP3) );
			return FALSE;
		}
	}
	/* ���̕ҏW���̃e�L�X�g���猟������ */
	if( m_szFile[0] == _T('\0') ){
		//	Jun. 16, 2003 Moca
		//	�����p�^�[�����w�肳��Ă��Ȃ��ꍇ�̃��b�Z�[�W�\������߁A
		//	�u*.*�v���w�肳�ꂽ���̂ƌ��Ȃ��D
		_tcscpy( m_szFile, _T("*.*") );
	}
	if( m_szFolder[0] == _T('\0') ){
		WarningMessage(	GetHwnd(), LS(STR_DLGGREP4) );
		return FALSE;
	}

	{
		//�J�����g�f�B���N�g����ۑ��B���̃u���b�N���甲����Ƃ��Ɏ����ŃJ�����g�f�B���N�g���͕��������B
		CCurrentDirectoryBackupPoint cCurDirBackup;

		// 2011.11.24 Moca �����t�H���_�w��
		std::vector<std::tstring> vPaths;
		CGrepAgent::CreateFolders( m_szFolder, vPaths );
		int nFolderLen = 0;
		TCHAR szFolder[_MAX_PATH];
		szFolder[0] = _T('\0');
		for( int i = 0 ; i < (int)vPaths.size(); i ++ ){
			// ���΃p�X����΃p�X
			if( !::SetCurrentDirectory( vPaths[i].c_str() ) ){
				WarningMessage(	GetHwnd(), LS(STR_DLGGREP5) );
				return FALSE;
			}
			TCHAR szFolderItem[_MAX_PATH];
			::GetCurrentDirectory( _MAX_PATH, szFolderItem );
			// ;���t�H���_���Ɋ܂܂�Ă�����""�ň͂�
			if( auto_strchr( szFolderItem, _T(';') ) ){
				szFolderItem[0] = _T('"');
				::GetCurrentDirectory( _MAX_PATH, szFolderItem + 1 );
				auto_strcat(szFolderItem, _T("\""));
			}
			int nFolderItemLen = auto_strlen( szFolderItem );
			if( _MAX_PATH < nFolderLen + nFolderItemLen + 1 ){
				WarningMessage(	GetHwnd(), LS(STR_DLGGREP6) );
				return FALSE;
			}
			if( i ){
				auto_strcat( szFolder, _T(";") );
			}
			auto_strcat( szFolder, szFolderItem );
			nFolderLen = auto_strlen( szFolder );
		}
		auto_strcpy( m_szFolder, szFolder );
	}

//@@@ 2002.2.2 YAZAKI CShareData.AddToSearchKeyArr()�ǉ��ɔ����ύX
	/* ���������� */
	if( 0 < m_strText.size() ){
		// From Here Jun. 26, 2001 genta
		//	���K�\�����C�u�����̍����ւ��ɔ��������̌�����
		int nFlag = 0;
		nFlag |= m_sSearchOption.bLoHiCase ? 0x01 : 0x00;
		if( m_sSearchOption.bRegularExp  && !CheckRegexpSyntax( m_strText.c_str(), GetHwnd(), true, nFlag) ){
			return FALSE;
		}
		// To Here Jun. 26, 2001 genta ���K�\�����C�u���������ւ�
		if( m_strText.size() < _MAX_PATH ){
			CSearchKeywordManager().AddToSearchKeyArr( m_strText.c_str() );
			m_pShareData->m_Common.m_sSearch.m_sSearchOption = m_sSearchOption;		// �����I�v�V����
		}
	}else{
		// 2014.07.01 ��L�[���o�^����
		CSearchKeywordManager().AddToSearchKeyArr( L"" );
	}

	// ���̕ҏW���̃e�L�X�g���猟������ꍇ�A�����Ɏc���Ȃ�	Uchi 2008/5/23
	// 2016.03.08 Moca �u���̃t�@�C�����猟���v�̏ꍇ�̓T�u�t�H���_���ʐݒ���X�V���Ȃ�
#if REI_MOD_GREP
  bool bFromThisText = m_bFromThisText;
	if (!m_bFolder99 && !m_bFolder4 && !m_bFolder3 && !m_bFolder2) {
		bFromThisText = true;
	}
	if (!bFromThisText) {
#else
	if (!m_bFromThisText) {
#endif  // rei_
		/* �����t�@�C�� */
		CSearchKeywordManager().AddToGrepFileArr( m_szFile );

		/* �����t�H���_ */
#if REI_MOD_GREP
		if (m_bFolder99 && m_szFolder[0] != '\0') {
			CSearchKeywordManager().AddToGrepFolderArr( m_szFolder );
		}
		if (m_bFolder4 && m_szFolder4[0] != '\0') {
			CSearchKeywordManager().AddToGrepFolderArr( m_szFolder4 );
		}
		if (m_bFolder3 && m_szFolder3[0] != '\0') {
			CSearchKeywordManager().AddToGrepFolderArr( m_szFolder3 );
		}
		if (m_bFolder2 && m_szFolder2[0] != '\0') {
			CSearchKeywordManager().AddToGrepFolderArr( m_szFolder2 );
		}
#else
		CSearchKeywordManager().AddToGrepFolderArr( m_szFolder );
#endif // rei_

		// Grep�F�T�u�t�H���_������
		m_pShareData->m_Common.m_sSearch.m_bGrepSubFolder = m_bSubFolder;
	}

	return TRUE;
}

//@@@ 2002.01.18 add start
LPVOID CDlgGrep::GetHelpIdTable(void)
{
	return (LPVOID)p_helpids;
}
//@@@ 2002.01.18 add end


static void SetGrepFolder( HWND hwndCtrl, LPCTSTR folder )
{
	if( auto_strchr( folder, _T(';')) ){
		TCHAR szQuoteFolder[MAX_PATH];
		szQuoteFolder[0] = _T('"');
		auto_strcpy( szQuoteFolder + 1, folder );
		auto_strcat( szQuoteFolder, _T("\"") );
		::SetWindowText( hwndCtrl, szQuoteFolder );
	}else{
		::SetWindowText( hwndCtrl, folder );
	}
}
