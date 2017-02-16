/*!	@file
	@brief �v���Z�X�ԋ��L�f�[�^�ւ̃A�N�Z�X

	@author Norio Nakatani
	@date 1998/05/26  �V�K�쐬
*/
/*
	Copyright (C) 1998-2001, Norio Nakatani
	Copyright (C) 2000, jepro, genta
	Copyright (C) 2001, jepro, genta, asa-o, MIK, YAZAKI, hor
	Copyright (C) 2002, genta, aroka, Moca, MIK, YAZAKI, hor
	Copyright (C) 2003, Moca, aroka, MIK, genta
	Copyright (C) 2004, Moca, novice, genta
	Copyright (C) 2005, MIK, genta, ryoji, aroka, Moca
	Copyright (C) 2006, aroka, ryoji, D.S.Koba, fon
	Copyright (C) 2007, ryoji, maru
	Copyright (C) 2008, ryoji, Uchi
	Copyright (C) 2011, nasukoji

	This source code is designed for sakura editor.
	Please contact the copyright holder to use this code for other purpose.
*/
//2007.09.23 kobake m_nSEARCHKEYArrNum,      m_szSEARCHKEYArr      �� m_aSearchKeys      �ɂ܂Ƃ߂܂���
//2007.09.23 kobake m_nREPLACEKEYArrNum,     m_szREPLACEKEYArr     �� m_aReplaceKeys     �ɂ܂Ƃ߂܂���
//2007.09.23 kobake m_nGREPFILEArrNum,       m_szGREPFILEArr       �� m_aGrepFiles       �ɂ܂Ƃ߂܂���
//2007.09.23 kobake m_nGREPFOLDERArrNum,     m_szGREPFOLDERArr     �� m_aGrepFolders     �ɂ܂Ƃ߂܂���
//2007.09.23 kobake m_szCmdArr,              m_nCmdArrNum          �� m_aCommands        �ɂ܂Ƃ߂܂���
//2007.09.23 kobake m_nTagJumpKeywordArrNum, m_szTagJumpKeywordArr �� m_aTagJumpKeywords �ɂ܂Ƃ߂܂���
//2007.12.13 kobake DLLSHAREDATA�ւ̊ȈՃA�N�Z�T��p��


#ifndef SAKURA_ENV_CSHAREDATA_H_
#define SAKURA_ENV_CSHAREDATA_H_

#include "CSelectLang.h"		// 2011.04.10 nasukoji

class CShareData;

// 2010.04.19 Moca DLLSHAREDATA�֘A��DLLSHAREDATA.h���Œ���K�v�ȏꏊ�ֈړ�
// CShareData.h�́A������Interface�����񋟂��܂���B�ʂ�DLLSHAREDATA.h��include���邱�ƁB
struct DLLSHAREDATA;
struct STypeConfig;
class CMutex;

/*!	@brief ���L�f�[�^�̊Ǘ�

	CShareData��CProcess�̃����o�ł��邽�߁C���҂̎����͓���ł��D
	�{����CProcess�I�u�W�F�N�g��ʂ��ăA�N�Z�X����ׂ��ł����C
	CProcess���̃f�[�^�̈�ւ̃|�C���^��static�ϐ��ɕۑ����邱�Ƃ�
	Singleton�̂悤�ɂǂ�����ł��A�N�Z�X�ł���\���ɂȂ��Ă��܂��D

	���L�������ւ̃|�C���^��m_pShareData�ɕێ����܂��D���̃����o��
	���J����Ă��܂����CCShareData�ɂ����Map/Unmap����邽�߂�
	ChareData�̏��łɂ���ă|�C���^m_pShareData�������ɂȂ邱�Ƃ�
	���ӂ��Ă��������D

	@date 2002.01.03 YAZAKI m_tbMyButton�Ȃǂ�CShareData����CMenuDrawer�ֈړ��������Ƃɂ��C���B
*/
class CShareData : public TSingleton<CShareData>
{
	friend class TSingleton<CShareData>;
	CShareData();
	~CShareData();

public:
	/*
	||  Attributes & Operations
	*/
	bool InitShareData();	/* CShareData�N���X�̏��������� */
	void RefreshString();	/* ����I����ɋ��L���������̕�������X�V���� */
	
	//MRU�n
	BOOL IsPathOpened( const TCHAR* pszPath, HWND* phwndOwner ); /* �w��t�@�C�����J����Ă��邩���ׂ� */
	BOOL ActiveAlreadyOpenedWindow( const TCHAR* pszPath, HWND* phwndOwner, ECodeType nCharCode );/* �w��t�@�C�����J����Ă��邩���ׂA���d�I�[�v�����̕����R�[�h�Փ˂��m�F */	// 2007.03.16

	//�f�o�b�O  ���͎�Ƀ}�N���E�O���R�}���h���s�p
	void TraceOut( LPCTSTR lpFmt, ...);	/* �A�E�g�v�b�g�E�B���h�E�ɏo��(printf�t�H�[�}�b�g) */
	void TraceOutString( const wchar_t* pszStr, int len = -1);	/* �A�E�g�v�b�g�E�B���h�E�ɏo��(�����H������) */
	void SetTraceOutSource( HWND hwnd ){ m_hwndTraceOutSource = hwnd; }	/* TraceOut�N�����E�B���h�E�̐ݒ� */
	bool OpenDebugWindow( HWND hwnd, bool bAllwaysActive );	//!<  �f�o�b�O�E�B���h�E���J��

	BOOL IsPrivateSettings( void );


	//�}�N���֘A
	int			GetMacroFilename( int idx, TCHAR* pszPath, int nBufLen ); // idx�Ŏw�肵���}�N���t�@�C�����i�t���p�X�j���擾����	//	Jun. 14, 2003 genta �����ǉ��D�����ύX
	bool		BeReloadWhenExecuteMacro( int idx );	//	idx�Ŏw�肵���}�N���́A���s���邽�тɃt�@�C����ǂݍ��ސݒ肩�H

	//�^�C�v�ʐݒ�(�R���g���[���v���Z�X��p)
	void CreateTypeSettings();
	std::vector<STypeConfig*>& GetTypeSettings();

	// ���ۉ��Ή��̂��߂̕������ύX����(�R���g���[���v���Z�X��p)
	void ConvertLangValues(std::vector<std::wstring>& values, bool bSetValues);

	static CMutex& GetMutexShareWork();

	void InitKeyword(DLLSHAREDATA*, bool);

protected:
	/*
	||  �����w���p�֐�
	*/

	//	Jan. 30, 2005 genta �������֐��̕���
	bool InitKeyAssign(DLLSHAREDATA*); // 2007.11.04 genta �N�����~�̂��ߒl��Ԃ�
	void RefreshKeyAssignString(DLLSHAREDATA*);
	void InitToolButtons(DLLSHAREDATA*);
	void InitTypeConfigs(DLLSHAREDATA*, std::vector<STypeConfig*>&);
	void InitPopupMenu(DLLSHAREDATA*);

public:
	static void InitFileTree(SFileTree*);

private:
	CSelectLang m_cSelectLang;			// ���b�Z�[�W���\�[�XDLL�ǂݍ��ݗp�i�v���Z�X��1�j		// 2011.04.10 nasukoji
	HANDLE			m_hFileMap;
	DLLSHAREDATA*	m_pShareData;
	std::vector<STypeConfig*>* 	m_pvTypeSettings;	//	(�R���g���[���v���Z�X�̂�)
	HWND			m_hwndTraceOutSource;	// TraceOutA()�N�����E�B���h�E�i���������N�������w�肵�Ȃ��Ă��ނ悤�Ɂj

};



///////////////////////////////////////////////////////////////////////
#endif /* SAKURA_ENV_CSHAREDATA_H_ */


