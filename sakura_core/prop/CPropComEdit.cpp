/*!	@file
	@brief ���ʐݒ�_�C�A���O�{�b�N�X�A�u�ҏW�v�y�[�W

	@author Norio Nakatani
*/
/*
	Copyright (C) 1998-2001, Norio Nakatani
	Copyright (C) 2000, jepro
	Copyright (C) 2001, MIK, jepro, genta
	Copyright (C) 2002, YAZAKI, MIK, aroka
	Copyright (C) 2003, KEITA
	Copyright (C) 2006, ryoji
	Copyright (C) 2007, genta, ryoji

	This source code is designed for sakura editor.
	Please contact the copyright holders to use this code for other purpose.
*/

#include "StdAfx.h"
#include "prop/CPropCommon.h"
#include "env/DLLSHAREDATA.h"
#include "env/CFileNameManager.h"
#include "util/shell.h"
#include "util/window.h"
#include "sakura_rc.h"
#include "sakura.hh"

//@@@ 2001.02.04 Start by MIK: Popup Help
static const DWORD p_helpids[] = {	//10210
	IDC_CHECK_ADDCRLFWHENCOPY,			HIDC_CHECK_ADDCRLFWHENCOPY,				//�܂�Ԃ��s�ɉ��s��t���ăR�s�[
	IDC_CHECK_COPYnDISABLESELECTEDAREA,	HIDC_CHECK_COPYnDISABLESELECTEDAREA,	//�R�s�[������I������
	IDC_CHECK_bEnableNoSelectCopy,		HIDC_CHECK_bEnableNoSelectCopy,			//�I���Ȃ��ŃR�s�[���\�ɂ���	// 2007.11.18 ryoji
	IDC_CHECK_bEnableLineModePaste,		HIDC_CHECK_bEnableLineModePaste,		//���C�����[�h�\��t�����\�ɂ���	// 2007.10.08 ryoji
	IDC_CHECK_DRAGDROP,					HIDC_CHECK_DRAGDROP,					//Drag&Drop�ҏW����
	IDC_CHECK_DROPSOURCE,				HIDC_CHECK_DROPSOURCE,					//�h���b�v���ɂ���
	IDC_CHECK_bNotOverWriteCRLF,		HIDC_CHECK_bNotOverWriteCRLF,			//�㏑�����[�h
	IDC_CHECK_bOverWriteFixMode,		HIDC_CHECK_bOverWriteFixMode,			//�������ɍ��킹�ăX�y�[�X���l�߂�
	IDC_CHECK_bOverWriteBoxDelete,		HIDC_CHECK_bOverWriteBoxDelete,			//��`���͂őI��͈͂��폜����
	//	2007.02.11 genta �N���b�J�u��URL�����̃y�[�W�Ɉړ�
	IDC_CHECK_bSelectClickedURL,	HIDC_CHECK_bSelectClickedURL,	//�N���b�J�u��URL
	IDC_CHECK_CONVERTEOLPASTE,			HIDC_CHECK_CONVERTEOLPASTE,			//���s�R�[�h��ϊ����ē\��t����
	IDC_RADIO_CURDIR,					HIDC_RADIO_CURDIR,						//�J�����g�t�H���_
	IDC_RADIO_MRUDIR,					HIDC_RADIO_MRUDIR,						//�ŋߎg�����t�H���_
	IDC_RADIO_SELDIR,					HIDC_RADIO_SELDIR,						//�w��t�H���_
	IDC_EDIT_FILEOPENDIR,				HIDC_EDIT_FILEOPENDIR,					//�w��t�H���_�p�X
	IDC_BUTTON_FILEOPENDIR, 			HIDC_EDIT_FILEOPENDIR,					//�w��t�H���_�p�X
	IDC_CHECK_ENABLEEXTEOL,				HIDC_CHECK_ENABLEEXTEOL,				//���s�R�[�hNEL,PS,LS��L���ɂ���
	IDC_CHECK_BOXSELECTLOCK,			HIDC_CHECK_BOXSELECTLOCK,				//��`�I���ړ��őI�������b�N����
//	IDC_STATIC,							-1,
	0, 0
};
//@@@ 2001.02.04 End

//	From Here Jun. 2, 2001 genta
/*!
	@param hwndDlg �_�C�A���O�{�b�N�X��Window Handle
	@param uMsg ���b�Z�[�W
	@param wParam �p�����[�^1
	@param lParam �p�����[�^2
*/
INT_PTR CALLBACK CPropEdit::DlgProc_page(
	HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	return DlgProc( reinterpret_cast<pDispatchPage>(&CPropEdit::DispatchEvent), hwndDlg, uMsg, wParam, lParam );
}
//	To Here Jun. 2, 2001 genta

/* ���b�Z�[�W���� */
INT_PTR CPropEdit::DispatchEvent(
    HWND		hwndDlg,	// handle to dialog box
    UINT		uMsg,		// message
    WPARAM		wParam,		// first message parameter
    LPARAM		lParam 		// second message parameter
)
{
	WORD		wNotifyCode;
	WORD		wID;
	NMHDR*		pNMHDR;
//	int			nVal;
//	LPDRAWITEMSTRUCT pDis;

	switch( uMsg ){

	case WM_INITDIALOG:
		EditCtl_LimitText( ::GetDlgItem( hwndDlg, IDC_EDIT_FILEOPENDIR ), _MAX_PATH - 1 );
		/* �_�C�A���O�f�[�^�̐ݒ� Edit */
		SetData( hwndDlg );
		// Modified by KEITA for WIN64 2003.9.6
		::SetWindowLongPtr( hwndDlg, DWLP_USER, lParam );

		/* ���[�U�[���G�f�B�b�g �R���g���[���ɓ��͂ł���e�L�X�g�̒����𐧌����� */

		return TRUE;
	case WM_COMMAND:
		wNotifyCode	= HIWORD(wParam);	/* �ʒm�R�[�h */
		wID			= LOWORD(wParam);	/* ����ID� �R���g���[��ID� �܂��̓A�N�Z�����[�^ID */
		switch( wNotifyCode ){
		/* �{�^���^�`�F�b�N�{�b�N�X���N���b�N���ꂽ */
		case BN_CLICKED:
			switch( wID ){
			case IDC_CHECK_DRAGDROP:	/* �^�X�N�g���C���g�� */
				if( ::IsDlgButtonChecked( hwndDlg, IDC_CHECK_DRAGDROP ) ){
					::EnableWindow( ::GetDlgItem( hwndDlg, IDC_CHECK_DROPSOURCE ), TRUE );
				}
				else{
					::EnableWindow( ::GetDlgItem( hwndDlg, IDC_CHECK_DROPSOURCE ), FALSE );
				}
				return TRUE;
			case IDC_RADIO_CURDIR:
			case IDC_RADIO_MRUDIR:
			case IDC_RADIO_SELDIR:
				EnableEditPropInput( hwndDlg );
				return TRUE;
			case IDC_BUTTON_FILEOPENDIR:
				{
					TCHAR szMetaPath[_MAX_PATH];
					TCHAR szPath[_MAX_PATH];
					::DlgItem_GetText( hwndDlg, IDC_EDIT_FILEOPENDIR, szMetaPath, _countof(szMetaPath) );
					CFileNameManager::ExpandMetaToFolder( szMetaPath, szPath, _countof(szPath) );
					if( SelectDir( hwndDlg, LS(STR_PROPEDIT_SELECT_DIR), szPath, szPath ) ){
						CNativeT cmem(szPath);
						cmem.Replace(_T("%"), _T("%%"));
						::DlgItem_SetText( hwndDlg, IDC_EDIT_FILEOPENDIR, cmem.GetStringPtr() );
					}
				}
				return TRUE;
			}
			break;
		}
		break;

	case WM_NOTIFY:
		pNMHDR = (NMHDR*)lParam;
		switch( pNMHDR->code ){
		case PSN_HELP:
			OnHelp( hwndDlg, IDD_PROP_EDIT );
			return TRUE;
		case PSN_KILLACTIVE:
			DEBUG_TRACE( _T("Edit PSN_KILLACTIVE\n") );

			/* �_�C�A���O�f�[�^�̎擾 Edit */
			GetData( hwndDlg );
			return TRUE;

		case PSN_SETACTIVE: //@@@ 2002.01.03 YAZAKI �Ō�ɕ\�����Ă����V�[�g�𐳂����o���Ă��Ȃ��o�O�C��
			m_nPageNum = ID_PROPCOM_PAGENUM_EDIT;
			return TRUE;
		}
		break;	/* WM_NOTIFY */

//@@@ 2001.02.04 Start by MIK: Popup Help
	case WM_HELP:
		{
			HELPINFO *p = (HELPINFO *)lParam;
			MyWinHelp( (HWND)p->hItemHandle, HELP_WM_HELP, (ULONG_PTR)(LPVOID)p_helpids );	// 2006.10.10 ryoji MyWinHelp�ɕύX�ɕύX
		}
		return TRUE;
		/*NOTREACHED*/
		//break;
//@@@ 2001.02.04 End

//@@@ 2001.12.22 Start by MIK: Context Menu Help
	//Context Menu
	case WM_CONTEXTMENU:
		MyWinHelp( hwndDlg, HELP_CONTEXTMENU, (ULONG_PTR)(LPVOID)p_helpids );	// 2006.10.10 ryoji MyWinHelp�ɕύX�ɕύX
		return TRUE;
//@@@ 2001.12.22 End

	}
	return FALSE;
}


/* �_�C�A���O�f�[�^�̐ݒ� */
void CPropEdit::SetData( HWND hwndDlg )
{
	/* �h���b�O & �h���b�v�ҏW */
	::CheckDlgButton( hwndDlg, IDC_CHECK_DRAGDROP, m_Common.m_sEdit.m_bUseOLE_DragDrop );
	if( ::IsDlgButtonChecked( hwndDlg, IDC_CHECK_DRAGDROP ) ){
		::EnableWindow( ::GetDlgItem( hwndDlg, IDC_CHECK_DROPSOURCE ), TRUE );
	}
	else{
		::EnableWindow( ::GetDlgItem( hwndDlg, IDC_CHECK_DROPSOURCE ), FALSE );
	}

	/* DropSource */
	::CheckDlgButton( hwndDlg, IDC_CHECK_DROPSOURCE, m_Common.m_sEdit.m_bUseOLE_DropSource );

	/* �܂�Ԃ��s�ɉ��s��t���ăR�s�[ */
	::CheckDlgButton( hwndDlg, IDC_CHECK_ADDCRLFWHENCOPY, m_Common.m_sEdit.m_bAddCRLFWhenCopy ? BST_CHECKED : BST_UNCHECKED );

	/* �R�s�[������I������ */
	::CheckDlgButton( hwndDlg, IDC_CHECK_COPYnDISABLESELECTEDAREA, m_Common.m_sEdit.m_bCopyAndDisablSelection );

	/* �I���Ȃ��ŃR�s�[���\�ɂ��� */	// 2007.11.18 ryoji
	::CheckDlgButton( hwndDlg, IDC_CHECK_bEnableNoSelectCopy, m_Common.m_sEdit.m_bEnableNoSelectCopy );

	/* ���C�����[�h�\��t�����\�ɂ��� */	// 2007.10.08 ryoji
	::CheckDlgButton( hwndDlg, IDC_CHECK_bEnableLineModePaste, m_Common.m_sEdit.m_bEnableLineModePaste ? BST_CHECKED : BST_UNCHECKED );

	/* ���s�͏㏑�����Ȃ� */
	::CheckDlgButton( hwndDlg, IDC_CHECK_bNotOverWriteCRLF, m_Common.m_sEdit.m_bNotOverWriteCRLF );

	// �������ɍ��킹�ăX�y�[�X���l�߂�
	CheckDlgButtonBool( hwndDlg, IDC_CHECK_bOverWriteFixMode, m_Common.m_sEdit.m_bOverWriteFixMode );

	// ��`���͂őI��͈͂��폜����
	CheckDlgButtonBool( hwndDlg, IDC_CHECK_bOverWriteBoxDelete, m_Common.m_sEdit.m_bOverWriteFixMode );

	//	URL���N���b�N���ꂽ��I�����邩 */	// 2007.02.11 genta ���̃y�[�W�ֈړ�
	::CheckDlgButton( hwndDlg, IDC_CHECK_bSelectClickedURL, m_Common.m_sEdit.m_bSelectClickedURL );

	/*	���s�R�[�h��ϊ����ē\��t���� */	// 2009.02.28 salarm
	::CheckDlgButton( hwndDlg, IDC_CHECK_CONVERTEOLPASTE, m_Common.m_sEdit.m_bConvertEOLPaste ? BST_CHECKED : BST_UNCHECKED );

	// �t�@�C���_�C�A���O�̏����ʒu
	if( m_Common.m_sEdit.m_eOpenDialogDir == OPENDIALOGDIR_CUR ){
		::CheckDlgButton( hwndDlg, IDC_RADIO_CURDIR, TRUE );
	}
	if( m_Common.m_sEdit.m_eOpenDialogDir == OPENDIALOGDIR_MRU ){
		::CheckDlgButton( hwndDlg, IDC_RADIO_MRUDIR, TRUE );
	}
	if( m_Common.m_sEdit.m_eOpenDialogDir == OPENDIALOGDIR_SEL ){
		::CheckDlgButton( hwndDlg, IDC_RADIO_SELDIR, TRUE );
	}
	::DlgItem_SetText( hwndDlg, IDC_EDIT_FILEOPENDIR, m_Common.m_sEdit.m_OpenDialogSelDir );

	// ���s�R�[�hNEL,PS,LS��L���ɂ���
	CheckDlgButtonBool( hwndDlg, IDC_CHECK_ENABLEEXTEOL, m_Common.m_sEdit.m_bEnableExtEol );
	// ��`�I���ړ��őI�������b�N����
	CheckDlgButtonBool( hwndDlg, IDC_CHECK_BOXSELECTLOCK, m_Common.m_sEdit.m_bBoxSelectLock );

	EnableEditPropInput( hwndDlg );
}



/* �_�C�A���O�f�[�^�̎擾 */
int CPropEdit::GetData( HWND hwndDlg )
{
	/* �h���b�O & �h���b�v�ҏW */
	m_Common.m_sEdit.m_bUseOLE_DragDrop = ::IsDlgButtonChecked( hwndDlg, IDC_CHECK_DRAGDROP );
	/* DropSource */
	m_Common.m_sEdit.m_bUseOLE_DropSource = ::IsDlgButtonChecked( hwndDlg, IDC_CHECK_DROPSOURCE );

	/* �܂�Ԃ��s�ɉ��s��t���ăR�s�[ */
	m_Common.m_sEdit.m_bAddCRLFWhenCopy = (0 != ::IsDlgButtonChecked( hwndDlg, IDC_CHECK_ADDCRLFWHENCOPY ));

	/* �R�s�[������I������ */
	m_Common.m_sEdit.m_bCopyAndDisablSelection = ::IsDlgButtonChecked( hwndDlg, IDC_CHECK_COPYnDISABLESELECTEDAREA );

	/* �I���Ȃ��ŃR�s�[���\�ɂ��� */	// 2007.11.18 ryoji
	m_Common.m_sEdit.m_bEnableNoSelectCopy = ::IsDlgButtonChecked( hwndDlg, IDC_CHECK_bEnableNoSelectCopy );

	/* ���C�����[�h�\��t�����\�ɂ��� */	// 2007.10.08 ryoji
	m_Common.m_sEdit.m_bEnableLineModePaste = (0 != ::IsDlgButtonChecked( hwndDlg, IDC_CHECK_bEnableLineModePaste ));

	/* ���s�͏㏑�����Ȃ� */
	m_Common.m_sEdit.m_bNotOverWriteCRLF = ::IsDlgButtonChecked( hwndDlg, IDC_CHECK_bNotOverWriteCRLF );

	// �������ɍ��킹�ăX�y�[�X���l�߂�
	m_Common.m_sEdit.m_bOverWriteFixMode = IsDlgButtonCheckedBool( hwndDlg, IDC_CHECK_bOverWriteFixMode );

	// ��`���͂őI��͈͂��폜����
	m_Common.m_sEdit.m_bOverWriteBoxDelete = IsDlgButtonCheckedBool( hwndDlg, IDC_CHECK_bOverWriteBoxDelete );

	/* URL���N���b�N���ꂽ��I�����邩 */	// 2007.02.11 genta ���̃y�[�W�ֈړ�
	m_Common.m_sEdit.m_bSelectClickedURL = ::IsDlgButtonChecked( hwndDlg, IDC_CHECK_bSelectClickedURL );

	//	���s�R�[�h��ϊ����ē\��t���� */	// 2009.02.28 salarm
	m_Common.m_sEdit.m_bConvertEOLPaste = (0 != ::IsDlgButtonChecked( hwndDlg, IDC_CHECK_CONVERTEOLPASTE ));

	if( ::IsDlgButtonChecked(hwndDlg, IDC_RADIO_CURDIR) ){
		m_Common.m_sEdit.m_eOpenDialogDir = OPENDIALOGDIR_CUR;
	}
	if( ::IsDlgButtonChecked(hwndDlg, IDC_RADIO_MRUDIR) ){
		m_Common.m_sEdit.m_eOpenDialogDir = OPENDIALOGDIR_MRU;
	}
	if( ::IsDlgButtonChecked(hwndDlg, IDC_RADIO_SELDIR) ){
		m_Common.m_sEdit.m_eOpenDialogDir = OPENDIALOGDIR_SEL;
	}
	::DlgItem_GetText( hwndDlg, IDC_EDIT_FILEOPENDIR, m_Common.m_sEdit.m_OpenDialogSelDir, _countof2(m_Common.m_sEdit.m_OpenDialogSelDir) );

	// ���s�R�[�hNEL,PS,LS��L���ɂ���
	m_Common.m_sEdit.m_bEnableExtEol = IsDlgButtonCheckedBool( hwndDlg, IDC_CHECK_ENABLEEXTEOL );
	// ��`�I���ړ��őI�������b�N����
	m_Common.m_sEdit.m_bBoxSelectLock = IsDlgButtonCheckedBool( hwndDlg, IDC_CHECK_BOXSELECTLOCK );

	return TRUE;
}

/*!	�`�F�b�N��Ԃɉ����ă_�C�A���O�{�b�N�X�v�f��Enable/Disable��
	�K�؂ɐݒ肷��

	@param hwndDlg �v���p�e�B�V�[�g��Window Handle

	@date 2013.03.31 novice �V�K�쐬
*/
void CPropEdit::EnableEditPropInput( HWND hwndDlg )
{
	// �w��t�H���_
	if( ::IsDlgButtonChecked( hwndDlg, IDC_RADIO_SELDIR ) ){
		::EnableWindow( ::GetDlgItem( hwndDlg, IDC_EDIT_FILEOPENDIR ), TRUE );
		::EnableWindow( ::GetDlgItem( hwndDlg, IDC_BUTTON_FILEOPENDIR ), TRUE );
	}else{
		::EnableWindow( ::GetDlgItem( hwndDlg, IDC_EDIT_FILEOPENDIR ), FALSE );
		::EnableWindow( ::GetDlgItem( hwndDlg, IDC_BUTTON_FILEOPENDIR ), FALSE );
	}
}

