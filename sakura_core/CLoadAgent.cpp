/*
	Copyright (C) 2008, kobake

	This software is provided 'as-is', without any express or implied
	warranty. In no event will the authors be held liable for any damages
	arising from the use of this software.

	Permission is granted to anyone to use this software for any purpose,
	including commercial applications, and to alter it and redistribute it
	freely, subject to the following restrictions:

		1. The origin of this software must not be misrepresented;
		   you must not claim that you wrote the original software.
		   If you use this software in a product, an acknowledgment
		   in the product documentation would be appreciated but is
		   not required.

		2. Altered source versions must be plainly marked as such,
		   and must not be misrepresented as being the original software.

		3. This notice may not be removed or altered from any source
		   distribution.
*/
#include "StdAfx.h"
#include "CLoadAgent.h"
#include "CReadManager.h"
#include "_main/CAppMode.h"
#include "_main/CControlTray.h"
#include "CEditApp.h"
#include "env/CDocTypeManager.h"
#include "env/CShareData.h"
#include "doc/CEditDoc.h"
#include "view/CEditView.h"
#include "window/CEditWnd.h"
#include "uiparts/CVisualProgress.h"
#include "util/file.h"

ECallbackResult CLoadAgent::OnCheckLoad(SLoadInfo* pLoadInfo)
{
	CEditDoc* pcDoc = GetListeningDoc();

	// �����[�h�v���̏ꍇ�́A�p���B
	if(pLoadInfo->bRequestReload)goto next;

	//�t�H���_���w�肳�ꂽ�ꍇ�́u�t�@�C�����J���v�_�C�A���O��\�����A���ۂ̃t�@�C�����͂𑣂�
	if( IsDirectory(pLoadInfo->cFilePath) ){
		std::vector<std::tstring> files;
		SLoadInfo sLoadInfo(_T(""), CODE_AUTODETECT, false);
		bool bDlgResult = pcDoc->m_cDocFileOperation.OpenFileDialog(
			CEditWnd::getInstance()->GetHwnd(),
			pLoadInfo->cFilePath,	//�w�肳�ꂽ�t�H���_
			&sLoadInfo,
			files
		);
		if( !bDlgResult ){
			return CALLBACK_INTERRUPT; //�L�����Z�����ꂽ�ꍇ�͒��f
		}
		size_t nSize = files.size();
		if( 0 < nSize ){
			sLoadInfo.cFilePath = files[0].c_str();
			// ���̃t�@�C���͐V�K�E�B���h�E
			for( size_t i = 1; i < nSize; i++ ){
				SLoadInfo sFilesLoadInfo = sLoadInfo;
				sFilesLoadInfo.cFilePath = files[i].c_str();
				CControlTray::OpenNewEditor(
					G_AppInstance(),
					CEditWnd::getInstance()->GetHwnd(),
					sFilesLoadInfo,
					NULL,
					true
				);
			}
		}
		*pLoadInfo = sLoadInfo;
	}

	// ���̃E�B���h�E�Ŋ��ɊJ����Ă���ꍇ�́A������A�N�e�B�u�ɂ���
	HWND	hWndOwner;
	if( CShareData::getInstance()->ActiveAlreadyOpenedWindow(pLoadInfo->cFilePath, &hWndOwner, pLoadInfo->eCharCode) ){
		pLoadInfo->bOpened = true;
		return CALLBACK_INTERRUPT;
	}

	// ���݂̃E�B���h�E�ɑ΂��ăt�@�C����ǂݍ��߂Ȃ��ꍇ�́A�V���ȃE�B���h�E���J���A�����Ƀt�@�C����ǂݍ��܂���
	if(!pcDoc->IsAcceptLoad()){
		CControlTray::OpenNewEditor(
			G_AppInstance(),
			CEditWnd::getInstance()->GetHwnd(),
			*pLoadInfo
		);
		return CALLBACK_INTERRUPT;
	}

next:
	// �I�v�V�����F�J�����Ƃ����t�@�C�������݂��Ȃ��Ƃ��x������
	if( GetDllShareData().m_Common.m_sFile.GetAlertIfFileNotExist() ){
		if(!fexist(pLoadInfo->cFilePath)){
			InfoBeep();
			//	Feb. 15, 2003 genta Popup�E�B���h�E��\�����Ȃ��悤�ɁD
			//	�����ŃX�e�[�^�X���b�Z�[�W���g���Ă���ʂɕ\������Ȃ��D
			TopInfoMessage(
				CEditWnd::getInstance()->GetHwnd(),
				LS(STR_NOT_EXSIST_SAVE),	//Mar. 24, 2001 jepro �኱�C��
				pLoadInfo->cFilePath.GetBufferPointer()
			);
		}
	}

	// �ǂݎ��\�`�F�b�N
	do{
		CFile cFile(pLoadInfo->cFilePath.c_str());

		//�t�@�C�������݂��Ȃ��ꍇ�̓`�F�b�N�ȗ�
		if(!cFile.IsFileExist())break;

		// ���b�N�͈ꎞ�I�ɉ������ă`�F�b�N����i�`�F�b�N�����Ɍ�߂�ł��Ȃ��Ƃ���܂Ői�߂�����S�j
		// �� ���b�N���Ă��Ă��A�N�Z�X���̕ύX�ɂ���ēǂݎ��Ȃ��Ȃ��Ă��邱�Ƃ�����
		bool bLock = (pLoadInfo->IsSamePath(pcDoc->m_cDocFile.GetFilePath()) && pcDoc->m_cDocFile.IsFileLocking());
		if( bLock ) pcDoc->m_cDocFileOperation.DoFileUnlock();

		//�`�F�b�N
		if(!cFile.IsFileReadable()){
			if( bLock ) pcDoc->m_cDocFileOperation.DoFileLock(false);
			ErrorMessage(
				CEditWnd::getInstance()->GetHwnd(),
				LS(STR_LOADAGENT_ERR_OPEN),
				pLoadInfo->cFilePath.c_str()
			);
			return CALLBACK_INTERRUPT; //�t�@�C�������݂��Ă���̂ɓǂݎ��Ȃ��ꍇ�͒��f
		}
		if( bLock ) pcDoc->m_cDocFileOperation.DoFileLock(false);
	}
	while(false);	//	1�񂵂��ʂ�Ȃ�. break�ł����܂Ŕ��

	// �t�@�C���T�C�Y�`�F�b�N
	if( GetDllShareData().m_Common.m_sFile.m_bAlertIfLargeFile ){
		WIN32_FIND_DATA wfd;
		HANDLE nFind = ::FindFirstFile( pLoadInfo->cFilePath.c_str(), &wfd );
		if( nFind != INVALID_HANDLE_VALUE ){
			::FindClose( nFind );
			LARGE_INTEGER nFileSize;
			nFileSize.HighPart = wfd.nFileSizeHigh;
			nFileSize.LowPart = wfd.nFileSizeLow;
			// GetDllShareData().m_Common.m_sFile.m_nAlertFileSize ��MB�P��
			if( (nFileSize.QuadPart>>20) >= (GetDllShareData().m_Common.m_sFile.m_nAlertFileSize) ){
				int nRet = MYMESSAGEBOX( CEditWnd::getInstance()->GetHwnd(),
					MB_ICONQUESTION | MB_YESNO | MB_TOPMOST,
					GSTR_APPNAME,
					LS(STR_LOADAGENT_BIG_FILE),
					GetDllShareData().m_Common.m_sFile.m_nAlertFileSize );
				if( nRet != IDYES ){
					return CALLBACK_INTERRUPT;
				}
			}
		}
	}

	return CALLBACK_CONTINUE;
}

void CLoadAgent::OnBeforeLoad(SLoadInfo* pLoadInfo)
{
}

ELoadResult CLoadAgent::OnLoad(const SLoadInfo& sLoadInfo)
{
	ELoadResult eRet = LOADED_OK;
	CEditDoc* pcDoc = GetListeningDoc();

	/* �����f�[�^�̃N���A */
	pcDoc->InitDoc(); //$$

	// �p�X���m��
	pcDoc->SetFilePathAndIcon( sLoadInfo.cFilePath );

	// ������ʊm��
	pcDoc->m_cDocType.SetDocumentType( sLoadInfo.nType, true );
	pcDoc->m_pcEditWnd->m_pcViewFontMiniMap->UpdateFont(&pcDoc->m_pcEditWnd->GetLogfont());
	InitCharWidthCache( pcDoc->m_pcEditWnd->m_pcViewFontMiniMap->GetLogfont(), CWM_FONT_MINIMAP );
	SelectCharWidthCache( CWM_FONT_EDIT, pcDoc->m_pcEditWnd->GetLogfontCacheMode() );
	InitCharWidthCache( pcDoc->m_pcEditWnd->GetLogfont() );
	pcDoc->m_pcEditWnd->m_pcViewFont->UpdateFont(&pcDoc->m_pcEditWnd->GetLogfont());

	// �N���Ɠ����ɓǂޏꍇ�͗\�߃A�E�g���C����͉�ʂ�z�u���Ă���
	// �i�t�@�C���ǂݍ��݊J�n�ƂƂ��Ƀr���[���\�������̂ŁA���ƂŔz�u����Ɖ�ʂ̂�������傫���́j
	if( !pcDoc->m_pcEditWnd->m_cDlgFuncList.m_bEditWndReady ){
		pcDoc->m_pcEditWnd->m_cDlgFuncList.Refresh();
		HWND hEditWnd = pcDoc->m_pcEditWnd->GetHwnd();
		if( !::IsIconic( hEditWnd ) && pcDoc->m_pcEditWnd->m_cDlgFuncList.GetHwnd() ){
			RECT rc;
			::GetClientRect( hEditWnd, &rc );
			::SendMessageAny( hEditWnd, WM_SIZE, ::IsZoomed( hEditWnd )? SIZE_MAXIMIZED: SIZE_RESTORED, MAKELONG( rc.right - rc.left, rc.bottom - rc.top ) );
		}
	}

	//�t�@�C�������݂���ꍇ�̓t�@�C����ǂ�
	if(fexist(sLoadInfo.cFilePath)){
		//CDocLineMgr�̍\��
		CReadManager cReader;
		CProgressSubject* pOld = CEditApp::getInstance()->m_pcVisualProgress->CProgressListener::Listen(&cReader);
		EConvertResult eReadResult = cReader.ReadFile_To_CDocLineMgr(
			&pcDoc->m_cDocLineMgr,
			sLoadInfo,
			&pcDoc->m_cDocFile.m_sFileInfo
		);
		if(eReadResult==RESULT_LOSESOME){
			eRet = LOADED_LOSESOME;
		}
		CEditApp::getInstance()->m_pcVisualProgress->CProgressListener::Listen(pOld);
	}
	else{
		// ���݂��Ȃ��Ƃ����h�L�������g�ɕ����R�[�h�𔽉f����
		const STypeConfig& types = pcDoc->m_cDocType.GetDocumentAttribute();
		pcDoc->m_cDocFile.SetCodeSet( sLoadInfo.eCharCode, 
			( sLoadInfo.eCharCode == types.m_encoding.m_eDefaultCodetype ) ?
				types.m_encoding.m_bDefaultBom : CCodeTypeName( sLoadInfo.eCharCode ).IsBomDefOn() );
	}

	/* ���C�A�E�g���̕ύX */
	// 2008.06.07 nasukoji	�܂�Ԃ����@�̒ǉ��ɑΉ�
	// �u�w�茅�Ő܂�Ԃ��v�ȊO�̎��͐܂�Ԃ�����MAXLINEKETAS�ŏ���������
	// �u�E�[�Ő܂�Ԃ��v�́A���̌��OnSize()�ōĐݒ肳���
	const STypeConfig& ref = pcDoc->m_cDocType.GetDocumentAttribute();
	CLayoutInt nMaxLineKetas = ref.m_nMaxLineKetas;
	if( ref.m_nTextWrapMethod != WRAP_SETTING_WIDTH )
		nMaxLineKetas = MAXLINEKETAS;

	CProgressSubject* pOld = CEditApp::getInstance()->m_pcVisualProgress->CProgressListener::Listen(&pcDoc->m_cLayoutMgr);
	pcDoc->m_cLayoutMgr.SetLayoutInfo( true, ref, ref.m_nTabSpace, ref.m_nTsvMode, nMaxLineKetas );
	pcDoc->m_pcEditWnd->ClearViewCaretPosInfo();
	if (pcDoc->m_cLayoutMgr.m_tsvInfo.m_nTsvMode != TSV_MODE_NONE) {
		pcDoc->m_cLayoutMgr.m_tsvInfo.CalcTabLength(pcDoc->m_cLayoutMgr.m_pcDocLineMgr);
	}
	
	CEditApp::getInstance()->m_pcVisualProgress->CProgressListener::Listen(pOld);

	return eRet;
}

void CLoadAgent::OnAfterLoad(const SLoadInfo& sLoadInfo)
{
	CEditDoc* pcDoc = GetListeningDoc();

	/* �e�E�B���h�E�̃^�C�g�����X�V */
	pcDoc->m_pcEditWnd->UpdateCaption();

	// -- -- �� InitAllView�ł���Ă����� -- -- //	// 2009.08.28 nasukoji	CEditView::OnAfterLoad()���炱���Ɉړ�
	pcDoc->m_nCommandExecNum=0;

	// �e�L�X�g�̐܂�Ԃ����@��������
	pcDoc->m_nTextWrapMethodCur = pcDoc->m_cDocType.GetDocumentAttribute().m_nTextWrapMethod;	// �܂�Ԃ����@
	pcDoc->m_bTextWrapMethodCurTemp = false;													// �ꎞ�ݒ�K�p��������
	pcDoc->m_blfCurTemp = false;
	pcDoc->m_bTabSpaceCurTemp = false;

	// 2009.08.28 nasukoji	�u�܂�Ԃ��Ȃ��v�Ȃ�e�L�X�g�ő啝���Z�o�A����ȊO�͕ϐ����N���A
	if( pcDoc->m_nTextWrapMethodCur == WRAP_NO_TEXT_WRAP )
		pcDoc->m_cLayoutMgr.CalculateTextWidth();		// �e�L�X�g�ő啝���Z�o����
	else
		pcDoc->m_cLayoutMgr.ClearLayoutLineWidth();		// �e�s�̃��C�A�E�g�s���̋L�����N���A����
}


void CLoadAgent::OnFinalLoad(ELoadResult eLoadResult)
{
	CEditDoc* pcDoc = GetListeningDoc();

	if(eLoadResult==LOADED_FAILURE){
		pcDoc->SetFilePathAndIcon( _T("") );
		pcDoc->m_cDocFile.SetBomDefoult();
	}
	if(eLoadResult==LOADED_LOSESOME){
		CAppMode::getInstance()->SetViewMode(true);
	}

	//�ĕ`�� $$�s��
	// CEditWnd::getInstance()->GetActiveView().SetDrawSwitch(true);
	bool bDraw = CEditWnd::getInstance()->GetActiveView().GetDrawSwitch();
	if( bDraw ){
		CEditWnd::getInstance()->Views_RedrawAll(); //�r���[�ĕ`��
		InvalidateRect( CEditWnd::getInstance()->GetHwnd(), NULL, TRUE );
	}
	CCaret& cCaret = CEditWnd::getInstance()->GetActiveView().GetCaret();
	cCaret.MoveCursor(cCaret.GetCaretLayoutPos(),true);
	CEditWnd::getInstance()->GetActiveView().AdjustScrollBars();
}