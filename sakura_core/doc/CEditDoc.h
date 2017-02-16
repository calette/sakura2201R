/*!	@file
	@brief �����֘A���̊Ǘ�

	@author Norio Nakatani
	@date	1998/03/13 �쐬
*/
/*
	Copyright (C) 1998-2001, Norio Nakatani
	Copyright (C) 2000, genta
	Copyright (C) 2001, genta, MIK, hor
	Copyright (C) 2002, genta, Moca, YAZAKI
	Copyright (C) 2003, genta, ryoji, zenryaku, naoh
	Copyright (C) 2004, Moca, novice, genta
	Copyright (C) 2005, Moca, genta, aroka
	Copyright (C) 2006, ryoji
	Copyright (C) 2007, ryoji, maru
	Copyright (C) 2008, ryoji, nasukoji

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

#ifndef SAKURA_CEDITDOC_CE42530D_FEC1_4B51_9CA3_470856295FEF8_H_
#define SAKURA_CEDITDOC_CE42530D_FEC1_4B51_9CA3_470856295FEF8_H_


#include "_main/global.h"
#include "_main/CAppMode.h"
#include "CDocEditor.h"
#include "CDocFile.h"
#include "CDocFileOperation.h"
#include "CDocType.h"
#include "CDocOutline.h"
#include "CDocLocker.h"
#include "layout/CLayoutMgr.h"
#include "logic/CDocLineMgr.h"
#include "CBackupAgent.h"
#include "CAutoSaveAgent.h"
#include "CAutoReloadAgent.h"
#include "func/CFuncLookup.h"
#include "CEol.h"
#include "macro/CCookieManager.h"
#include "util/design_template.h"

class CSMacroMgr; // 2002/2/10 aroka
class CEditWnd; // Sep. 10, 2002 genta
struct EditInfo; // 20050705 aroka
class CFuncInfoArr;
class CEditApp;

/*!
	�����֘A���̊Ǘ�

	@date 2002.02.17 YAZAKI CShareData�̃C���X�^���X�́ACProcess�ɂЂƂ���̂݁B
	@date 2007.12.13 kobake GetDocumentEncoding�쐬
	@date 2007.12.13 kobake SetDocumentEncoding�쐬
	@date 2007.12.13 kobake IsViewMode�쐬
*/
class CEditDoc
: public CDocSubject
, public TInstanceHolder<CEditDoc>
{
public:
	//�R���X�g���N�^�E�f�X�g���N�^
	CEditDoc(CEditApp* pcApp);
	~CEditDoc();

	//������
	BOOL Create( CEditWnd* pcEditWnd );
	void InitDoc();	/* �����f�[�^�̃N���A */
	void InitAllView();	/* �S�r���[�̏������F�t�@�C���I�[�v��/�N���[�Y�����ɁA�r���[������������ */
	void Clear();

	//�ݒ�
	void SetFilePathAndIcon(const TCHAR* szFile);	// Sep. 9, 2002 genta

	//����
	ECodeType	GetDocumentEncoding() const;				//!< �h�L�������g�̕����R�[�h���擾
	bool		GetDocumentBomExist() const;				//!< �h�L�������g��BOM�t�����擾
	void		SetDocumentEncoding(ECodeType eCharCode, bool bBom);	//!< �h�L�������g�̕����R�[�h��ݒ�
	bool IsModificationForbidden( EFunctionCode nCommand ) const;	//!< �w��R�}���h�ɂ�鏑���������֎~����Ă��邩�ǂ���	//Aug. 14, 2000 genta
	bool IsEditable() const { return !CAppMode::getInstance()->IsViewMode() && !(!m_cDocLocker.IsDocWritable() && GetDllShareData().m_Common.m_sFile.m_bUneditableIfUnwritable); }	//!< �ҏW�\���ǂ���
	void GetSaveInfo(SSaveInfo* pSaveInfo) const;			//!< �Z�[�u�����擾

	//���
	void GetEditInfo( EditInfo* ) const;	//!< �ҏW�t�@�C�������擾 //2007.10.24 kobake �֐����ύX: SetFileInfo��GetEditInfo
	bool IsAcceptLoad() const;				//!< ���̃E�B���h�E��(�V�����E�B���h�E���J������)�V�����t�@�C�����J���邩

	//�C�x���g
	BOOL HandleCommand( EFunctionCode );
	void OnChangeType();
	void OnChangeSetting(bool bDoLayout = true);		// �r���[�ɐݒ�ύX�𔽉f������
	BOOL OnFileClose(bool);			/* �t�@�C�������Ƃ���MRU�o�^ & �ۑ��m�F �� �ۑ����s */

	void RunAutoMacro( int idx, LPCTSTR pszSaveFilePath = NULL );	// 2006.09.01 ryoji �}�N���������s

	void SetBackgroundImage();

	void SetCurDirNotitle();

	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- //
	//                       �����o�ϐ��Q                          //
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- //
public:
	//�Q��
	CEditWnd*		m_pcEditWnd;	//	Sep. 10, 2002

	//�f�[�^�\��
	CDocLineMgr		m_cDocLineMgr;
	CLayoutMgr		m_cLayoutMgr;

	//�e��@�\
public:
	CDocFile			m_cDocFile;
	CDocFileOperation	m_cDocFileOperation;
	CDocEditor			m_cDocEditor;
	CDocType			m_cDocType;
	CCookieManager		m_cCookie;

	//�w���p
public:
	CBackupAgent		m_cBackupAgent;
	CAutoSaveAgent		m_cAutoSaveAgent;		//!< �����ۑ��Ǘ�
	CAutoReloadAgent	m_cAutoReloadAgent;
	CDocOutline			m_cDocOutline;
	CDocLocker			m_cDocLocker;

	//���I���
public:
	int				m_nCommandExecNum;			//!< �R�}���h���s��

	//�����
public:
	CFuncLookup		m_cFuncLookup;				//!< �@�\���C�@�\�ԍ��Ȃǂ�resolve

	//�������ϐ�
public:
	int				m_nTextWrapMethodCur;		// �܂�Ԃ����@					// 2008.05.30 nasukoji
	bool			m_bTextWrapMethodCurTemp;	// �܂�Ԃ����@�ꎞ�ݒ�K�p��	// 2008.05.30 nasukoji
	LOGFONT			m_lfCur;					// �ꎞ�ݒ�t�H���g
	int				m_nPointSizeCur;			// �ꎞ�ݒ�t�H���g�T�C�Y
	bool			m_blfCurTemp;				// �t�H���g�ݒ�K�p��
	int				m_nPointSizeOrg;			// ���̃t�H���g�T�C�Y
	bool			m_bTabSpaceCurTemp;			// �^�u���ꎞ�ݒ�K�p��			// 2013.05.30 Moca

	HBITMAP			m_hBackImg;
	int				m_nBackImgWidth;
	int				m_nBackImgHeight;
};



///////////////////////////////////////////////////////////////////////
#endif /* SAKURA_CEDITDOC_CE42530D_FEC1_4B51_9CA3_470856295FEF8_H_ */