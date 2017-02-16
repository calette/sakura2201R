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
#include <time.h>
#include <io.h>	// access
#include "CBackupAgent.h"
#include "window/CEditWnd.h"
#include "util/format.h" //GetDateTimeFormat

/*! �Z�[�u�O���܂�����
	@param pSaveInfo [in] �ۑ��t�@�C�����

	@retval CALLBACK_CONTINUE ������
	@retval CALLBACK_INTERRUPT ���f
*/
ECallbackResult CBackupAgent::OnPreBeforeSave(SSaveInfo* pSaveInfo)
{
	CEditDoc* pcDoc = GetListeningDoc();

	//�V�����t�@�C�������ꍇ�͉������Ȃ�
	if(!fexist(pSaveInfo->cFilePath))return CALLBACK_CONTINUE;

	//���ʐݒ�F�ۑ����Ƀo�b�N�A�b�v���쐬����
	if( GetDllShareData().m_Common.m_sBackup.m_bBackUp ){
		//	Jun.  5, 2004 genta �t�@�C������^����悤�ɁD�߂�l�ɉ�����������ǉ��D
		// �t�@�C���ۑ��O�Ƀo�b�N�A�b�v����
		int nBackupResult = 0;
		{
			pcDoc->m_cDocFileOperation.DoFileUnlock();	//�o�b�N�A�b�v�쐬�O�Ƀ��b�N���������� #####�X�}�[�g����Ȃ���I
			nBackupResult = MakeBackUp( pSaveInfo->cFilePath );
			pcDoc->m_cDocFileOperation.DoFileLock();	//�o�b�N�A�b�v�쐬��Ƀ��b�N��߂� #####�X�}�[�g����Ȃ���I
		}
		switch( nBackupResult ){
		case 2:	//	���f�w��
			return CALLBACK_INTERRUPT;
		case 3: //	�t�@�C���G���[
			if( IDYES != ::MYMESSAGEBOX(
				CEditWnd::getInstance()->GetHwnd(),
				MB_YESNO | MB_ICONQUESTION | MB_TOPMOST,
				LS(STR_BACKUP_ERR_TITLE),
				LS(STR_BACKUP_ERR_MSG)
			)){
				return CALLBACK_INTERRUPT;
			}
			break;
		}
	}
	return CALLBACK_CONTINUE;
}





/*! �o�b�N�A�b�v�̍쐬
	@author genta
	@date 2001.06.12 asa-o
		�t�@�C���̎��������Ƀo�b�N�A�b�v�t�@�C�������쐬����@�\
	@date 2001.12.11 MIK �o�b�N�A�b�v�t�@�C�����S�~���ɓ����@�\
	@date 2004.06.05 genta �o�b�N�A�b�v�Ώۃt�@�C���������ŗ^����悤�ɁD
		���O��t���ĕۑ��̎��͎����̃o�b�N�A�b�v������Ă����Ӗ��Ȃ̂ŁD
		�܂��C�o�b�N�A�b�v���ۑ����s��Ȃ��I������ǉ��D
	@date 2005.11.26 aroka �t�@�C����������FormatBackUpPath�ɕ���

	@param target_file [in] �o�b�N�A�b�v���p�X��

	@retval 0 �o�b�N�A�b�v�쐬���s�D
	@retval 1 �o�b�N�A�b�v�쐬�����D
	@retval 2 �o�b�N�A�b�v�쐬���s�D�ۑ����f�w���D
	@retval 3 �t�@�C������G���[�ɂ��o�b�N�A�b�v�쐬���s�D

	@todo Advanced mode�ł̐���Ǘ�
*/
int CBackupAgent::MakeBackUp(
	const TCHAR* target_file
)
{
	int		nRet;

	/* �o�b�N�A�b�v�\�[�X�̑��݃`�F�b�N */
	//	Aug. 21, 2005 genta �������݃A�N�Z�X�����Ȃ��ꍇ��
	//	�t�@�C�����Ȃ��ꍇ�Ɠ��l�ɉ������Ȃ�
	if( (_taccess( target_file, 2 )) == -1 ){
		return 0;
	}

	const CommonSetting_Backup& bup_setting = GetDllShareData().m_Common.m_sBackup;

	TCHAR	szPath[_MAX_PATH]; // �o�b�N�A�b�v��p�X��
	if( !FormatBackUpPath( szPath, _countof(szPath), target_file ) ){
		int nMsgResult = ::TopConfirmMessage(
			CEditWnd::getInstance()->GetHwnd(),
			LS(STR_BACKUP_ERR_PATH_CRETE)
		);
		if( nMsgResult == IDYES ){
			return 0;//	�ۑ��p��
		}
		return 2;// �ۑ����f
	}

	//@@@ 2002.03.23 start �l�b�g���[�N�E�����[�o�u���h���C�u�̏ꍇ�͂��ݔ��ɕ��荞�܂Ȃ�
	bool dustflag = false;
	if( bup_setting.m_bBackUpDustBox ){
		dustflag = !IsLocalDrive( szPath );
	}
	//@@@ 2002.03.23 end

	if( bup_setting.m_bBackUpDialog ){	/* �o�b�N�A�b�v�̍쐬�O�Ɋm�F */
		ConfirmBeep();
		if( bup_setting.m_bBackUpDustBox && !dustflag ){	//���ʐݒ�F�o�b�N�A�b�v�t�@�C�������ݔ��ɕ��荞��	//@@@ 2001.12.11 add start MIK	//2002.03.23
			nRet = ::MYMESSAGEBOX(
				CEditWnd::getInstance()->GetHwnd(),
				MB_YESNO/*CANCEL*/ | MB_ICONQUESTION | MB_TOPMOST,
				LS(STR_BACKUP_CONFORM_TITLE1),
				LS(STR_BACKUP_CONFORM_MSG1),
				target_file,
				szPath
			);
		}
		else{	//@@@ 2001.12.11 add end MIK
			nRet = ::MYMESSAGEBOX(
				CEditWnd::getInstance()->GetHwnd(),
				MB_YESNOCANCEL | MB_ICONQUESTION | MB_TOPMOST,
				LS(STR_BACKUP_CONFORM_TITLE2),
				LS(STR_BACKUP_CONFORM_MSG2),
				target_file,
				szPath
			);	//Jul. 06, 2001 jepro [���O��t���ĕۑ�] �̏ꍇ������̂Ń��b�Z�[�W���C��
		}	//@@@ 2001.12.11 add MIK
		//	Jun.  5, 2005 genta �߂�l�ύX
		if( IDNO == nRet ){
			return 0;//	�ۑ��p��
		}else if( IDCANCEL == nRet ){
			return 2;// �ۑ����f
		}
	}

	//	From Here Aug. 16, 2000 genta
	//	Jun.  5, 2005 genta 1�̊g���q���c���ł�ǉ�
	if( bup_setting.GetBackupType() == 3 ||
		bup_setting.GetBackupType() == 6 ){
		//	���ɑ��݂���Backup�����炷����
		int				i;

		//	�t�@�C�������p
		HANDLE			hFind;
		WIN32_FIND_DATA	fData;

		TCHAR*	pBase = szPath + _tcslen( szPath ) - 2;	//	2: �g���q�̍Ō��2���̈Ӗ�

		//------------------------------------------------------------------
		//	1. �Y���f�B���N�g������backup�t�@�C����1���T��
		for( i = 0; i <= 99; i++ ){	//	�ő�l�Ɋւ�炸�C99�i2���̍ő�l�j�܂ŒT��
			//	�t�@�C�������Z�b�g
			auto_sprintf( pBase, _T("%02d"), i );

			hFind = ::FindFirstFile( szPath, &fData );
			if( hFind == INVALID_HANDLE_VALUE ){
				//	�����Ɏ��s���� == �t�@�C���͑��݂��Ȃ�
				break;
			}
			::FindClose( hFind );
			//	���������t�@�C���̑������`�F�b�N
			//	�͖ʓ|���������炵�Ȃ��D
			//	�������O�̃f�B���N�g������������ǂ��Ȃ�̂��낤...
		}
		--i;

		//------------------------------------------------------------------
		//	2. �ő�l���琧����-1�Ԃ܂ł��폜
		int boundary = bup_setting.GetBackupCount();
		boundary = boundary > 0 ? boundary - 1 : 0;	//	�ŏ��l��0

		for( ; i >= boundary; --i ){
			//	�t�@�C�������Z�b�g
			auto_sprintf( pBase, _T("%02d"), i );
			if( ::DeleteFile( szPath ) == 0 ){
				::MessageBox( CEditWnd::getInstance()->GetHwnd(), szPath, LS(STR_BACKUP_ERR_DELETE), MB_OK );
				//	Jun.  5, 2005 genta �߂�l�ύX
				//	���s���Ă��ۑ��͌p��
				return 0;
				//	���s�����ꍇ
				//	��ōl����
			}
		}

		//	���̈ʒu��i�͑��݂���o�b�N�A�b�v�t�@�C���̍ő�ԍ���\���Ă���D

		//	3. ��������0�Ԃ܂ł̓R�s�[���Ȃ���ړ�
		TCHAR szNewPath[MAX_PATH];
		TCHAR *pNewNrBase;

		_tcscpy( szNewPath, szPath );
		pNewNrBase = szNewPath + _tcslen( szNewPath ) - 2;

		for( ; i >= 0; --i ){
			//	�t�@�C�������Z�b�g
			auto_sprintf( pBase, _T("%02d"), i );
			auto_sprintf( pNewNrBase, _T("%02d"), i + 1 );

			//	�t�@�C���̈ړ�
			if( ::MoveFile( szPath, szNewPath ) == 0 ){
				//	���s�����ꍇ
				//	��ōl����
				::MessageBox( CEditWnd::getInstance()->GetHwnd(), szPath, LS(STR_BACKUP_ERR_MOVE), MB_OK );
				//	Jun.  5, 2005 genta �߂�l�ύX
				//	���s���Ă��ۑ��͌p��
				return 0;
			}
		}
	}
	//	To Here Aug. 16, 2000 genta

	/* �o�b�N�A�b�v�̍쐬 */
	//	Aug. 21, 2005 genta ���݂̃t�@�C���ł͂Ȃ��^�[�Q�b�g�t�@�C�����o�b�N�A�b�v����悤��
	TCHAR	szDrive[_MAX_DIR];
	TCHAR	szDir[_MAX_DIR];
	TCHAR	szFname[_MAX_FNAME];
	TCHAR	szExt[_MAX_EXT];
	_tsplitpath( szPath, szDrive, szDir, szFname, szExt );
	TCHAR	szPath2[MAX_PATH];
	auto_sprintf( szPath2, _T("%ts%ts"), szDrive, szDir );

	HANDLE			hFind;
	WIN32_FIND_DATA	fData;

	hFind = ::FindFirstFile( szPath2, &fData );
	if( hFind == INVALID_HANDLE_VALUE ){
		//	�����Ɏ��s���� == �t�@�C���͑��݂��Ȃ�
		::CreateDirectory( szPath2, NULL );
	}
	::FindClose( hFind );

	if( ::CopyFile( target_file, szPath, FALSE ) ){
		/* ����I�� */
		//@@@ 2001.12.11 start MIK
		if( bup_setting.m_bBackUpDustBox && !dustflag ){	//@@@ 2002.03.23 �l�b�g���[�N�E�����[�o�u���h���C�u�łȂ�
			TCHAR	szDustPath[_MAX_PATH+1];
			_tcscpy(szDustPath, szPath);
			szDustPath[_tcslen(szDustPath) + 1] = _T('\0');
			SHFILEOPSTRUCT	fos;
			fos.hwnd   = CEditWnd::getInstance()->GetHwnd();
			fos.wFunc  = FO_DELETE;
			fos.pFrom  = szDustPath;
			fos.pTo    = NULL;
			fos.fFlags = FOF_ALLOWUNDO | FOF_SIMPLEPROGRESS | FOF_NOCONFIRMATION;	//�_�C�A���O�Ȃ�
			fos.fAnyOperationsAborted = true; //false;
			fos.hNameMappings = NULL;
			fos.lpszProgressTitle = NULL; //"�o�b�N�A�b�v�t�@�C�������ݔ��Ɉړ����Ă��܂�...";
			if( ::SHFileOperation(&fos) == 0 ){
				/* ����I�� */
			}else{
				/* �G���[�I�� */
			}
		}
		//@@@ 2001.12.11 end MIK
	}else{
		/* �G���[�I�� */
		//	Jun.  5, 2005 genta �߂�l�ύX
		return 3;
	}
	//	Jun.  5, 2005 genta �߂�l�ύX
	return 1;
}




/*! �o�b�N�A�b�v�p�X�̍쐬

	@author aroka

	@param szNewPath [out] �o�b�N�A�b�v��p�X��
	@param newPathCount [in] szNewPath�̃T�C�Y
	@param target_file [in] �o�b�N�A�b�v���p�X��

	@retval true  ����
	@retval false �o�b�t�@�s��

	@date 2005.11.29 aroka
		MakeBackUp���番���D���������Ƀo�b�N�A�b�v�t�@�C�������쐬����@�\�ǉ�
	@date 2013.04.15 novice �w��t�H���_�̃��^������W�J�T�|�[�g

	@todo Advanced mode�ł̐���Ǘ�
*/
bool CBackupAgent::FormatBackUpPath(
	TCHAR*			szNewPath,
	size_t 			newPathCount,
	const TCHAR*	target_file
)
{
	TCHAR	szDrive[_MAX_DIR];
	TCHAR	szDir[_MAX_DIR];
	TCHAR	szFname[_MAX_FNAME];
	TCHAR	szExt[_MAX_EXT];
	TCHAR*	psNext;

	const CommonSetting_Backup& bup_setting = GetDllShareData().m_Common.m_sBackup;

	/* �p�X�̕��� */
	_tsplitpath( target_file, szDrive, szDir, szFname, szExt );

	if( bup_setting.m_bBackUpFolder
	  && (!bup_setting.m_bBackUpFolderRM || !IsLocalDrive( target_file ))) {	/* �w��t�H���_�Ƀo�b�N�A�b�v���쐬���� */	// m_bBackUpFolderRM �ǉ�	2010/5/27 Uchi
		TCHAR selDir[_MAX_PATH];
		CFileNameManager::ExpandMetaToFolder( bup_setting.m_szBackUpFolder, selDir, _countof(selDir) );
		if (GetFullPathName(selDir, _MAX_PATH, szNewPath, &psNext) == 0) {
			// ���܂����Ȃ�����
			_tcscpy( szNewPath, selDir );
		}
		/* �t�H���_�̍Ōオ���p����'\\'�łȂ��ꍇ�́A�t������ */
		AddLastYenFromDirectoryPath( szNewPath );
	}
	else{
		auto_sprintf( szNewPath, _T("%ts%ts"), szDrive, szDir );
	}

	/* ���΃t�H���_��}�� */
	if( !bup_setting.m_bBackUpPathAdvanced ){
		time_t	ltime;
		struct	tm *today;
		wchar_t	szTime[64];
		wchar_t	szForm[64];

		TCHAR*	pBase;
		int     nBaseCount;
		pBase = szNewPath + _tcslen( szNewPath );
		nBaseCount = newPathCount - _tcslen( szNewPath );

		/* �o�b�N�A�b�v�t�@�C�����̃^�C�v 1=(.bak) 2=*_���t.* */
		switch( bup_setting.GetBackupType() ){
		case 1:
			if( -1 == auto_snprintf_s( pBase, nBaseCount, _T("%ts.bak"), szFname ) ){
				return false;
			}
			break;
		case 5: //	Jun.  5, 2005 genta 1�̊g���q���c����
			if( -1 == auto_snprintf_s( pBase, nBaseCount, _T("%ts%ts.bak"), szFname, szExt ) ){
				return false;
			}
			break;
		case 2:	//	���t�C����
			_tzset();
			_wstrdate( szTime );
			time( &ltime );				/* �V�X�e�������𓾂܂� */
			today = localtime( &ltime );/* ���n���Ԃɕϊ����� */

			szForm[0] = L'\0';
			if( bup_setting.GetBackupOpt(BKUP_YEAR) ){	/* �o�b�N�A�b�v�t�@�C�����F���t�̔N */
				wcscat( szForm, L"%Y" );
			}
			if( bup_setting.GetBackupOpt(BKUP_MONTH) ){	/* �o�b�N�A�b�v�t�@�C�����F���t�̌� */
				wcscat( szForm, L"%m" );
			}
			if( bup_setting.GetBackupOpt(BKUP_DAY) ){	/* �o�b�N�A�b�v�t�@�C�����F���t�̓� */
				wcscat( szForm, L"%d" );
			}
			if( bup_setting.GetBackupOpt(BKUP_HOUR) ){	/* �o�b�N�A�b�v�t�@�C�����F���t�̎� */
				wcscat( szForm, L"%H" );
			}
			if( bup_setting.GetBackupOpt(BKUP_MIN) ){	/* �o�b�N�A�b�v�t�@�C�����F���t�̕� */
				wcscat( szForm, L"%M" );
			}
			if( bup_setting.GetBackupOpt(BKUP_SEC) ){	/* �o�b�N�A�b�v�t�@�C�����F���t�̕b */
				wcscat( szForm, L"%S" );
			}
			/* YYYYMMDD�����b �`���ɕϊ� */
			wcsftime( szTime, _countof( szTime ) - 1, szForm, today );
			if( -1 == auto_snprintf_s( pBase, nBaseCount, _T("%ts_%ls%ts"), szFname, szTime, szExt ) ){
				return false;
			}
			break;
	//	2001/06/12 Start by asa-o: �t�@�C���ɕt������t��O��̕ۑ���(�X�V����)�ɂ���
		case 4:	//	���t�C����
			{
				CFileTime ctimeLastWrite;
				GetLastWriteTimestamp( target_file, &ctimeLastWrite );

				szTime[0] = L'\0';
				if( bup_setting.GetBackupOpt(BKUP_YEAR) ){	/* �o�b�N�A�b�v�t�@�C�����F���t�̔N */
					auto_sprintf(szTime,L"%d",ctimeLastWrite->wYear);
				}
				if( bup_setting.GetBackupOpt(BKUP_MONTH) ){	/* �o�b�N�A�b�v�t�@�C�����F���t�̌� */
					auto_sprintf(szTime,L"%ls%02d",szTime,ctimeLastWrite->wMonth);
				}
				if( bup_setting.GetBackupOpt(BKUP_DAY) ){	/* �o�b�N�A�b�v�t�@�C�����F���t�̓� */
					auto_sprintf(szTime,L"%ls%02d",szTime,ctimeLastWrite->wDay);
				}
				if( bup_setting.GetBackupOpt(BKUP_HOUR) ){	/* �o�b�N�A�b�v�t�@�C�����F���t�̎� */
					auto_sprintf(szTime,L"%ls%02d",szTime,ctimeLastWrite->wHour);
				}
				if( bup_setting.GetBackupOpt(BKUP_MIN) ){	/* �o�b�N�A�b�v�t�@�C�����F���t�̕� */
					auto_sprintf(szTime,L"%ls%02d",szTime,ctimeLastWrite->wMinute);
				}
				if( bup_setting.GetBackupOpt(BKUP_SEC) ){	/* �o�b�N�A�b�v�t�@�C�����F���t�̕b */
					auto_sprintf(szTime,L"%ls%02d",szTime,ctimeLastWrite->wSecond);
				}
				if( -1 == auto_sprintf_s( pBase, nBaseCount, _T("%ts_%ls%ts"), szFname, szTime, szExt ) ){
					return false;
				}
			}
			break;
	// 2001/06/12 End

		case 3: //	?xx : xx = 00~99, ?�͔C�ӂ̕���
		case 6: //	Jun.  5, 2005 genta 3�̊g���q���c����
			//	Aug. 15, 2000 genta
			//	�����ł͍쐬����o�b�N�A�b�v�t�@�C�����̂ݐ�������D
			//	�t�@�C������Rotation�͊m�F�_�C�A���O�̌�ōs���D
			{
				//	Jun.  5, 2005 genta �g���q���c����悤�ɏ����N�_�𑀍삷��
				TCHAR* ptr;
				if( bup_setting.GetBackupType() == 3 ){
					ptr = szExt;
				}
				else {
					ptr = szExt + _tcslen( szExt );
				}
				*ptr   = _T('.');
				*++ptr = bup_setting.GetBackupExtChar();
				*++ptr = _T('0');
				*++ptr = _T('0');
				*++ptr = _T('\0');
			}
			if( -1 == auto_snprintf_s( pBase, nBaseCount, _T("%ts%ts"), szFname, szExt ) ){
				return false;
			}
			break;
		}

	}else{ // �ڍאݒ�g�p����
		TCHAR szFormat[1024];

		switch( bup_setting.GetBackupTypeAdv() ){
		case 4:	//	�t�@�C���̓��t�C����
			{
				// 2005.10.20 ryoji FindFirstFile���g���悤�ɕύX
				CFileTime ctimeLastWrite;
				GetLastWriteTimestamp( target_file, &ctimeLastWrite );
				if( !GetDateTimeFormat( szFormat, _countof(szFormat), bup_setting.m_szBackUpPathAdvanced , ctimeLastWrite.GetSYSTEMTIME() ) ){
					return false;
				}
			}
			break;
		case 2:	//	���݂̓��t�C����
		default:
			{
				// 2012.12.26 aroka	�ڍאݒ�̃t�@�C���ۑ������ƌ��ݎ����ŏ��������킹��
				SYSTEMTIME	SystemTime;
				// 2016.07.28 UTC�����[�J�������ɕύX
				::GetLocalTime(&SystemTime);			// ���ݎ������擾

				if( !GetDateTimeFormat( szFormat, _countof(szFormat), bup_setting.m_szBackUpPathAdvanced , SystemTime ) ){
					return false;
				}
			}
			break;
		}

		{
			// make keys
			// $0-$9�ɑΉ�����t�H���_����؂�o��
			TCHAR keybuff[1024];
			_tcscpy( keybuff, szDir );
			CutLastYenFromDirectoryPath( keybuff );

			TCHAR *folders[10];
			{
				//	Jan. 9, 2006 genta VC6�΍�
				int idx;
				for( idx=0; idx<10; ++idx ){
					folders[idx] = 0;
				}
				folders[0] = szFname;

				for( idx=1; idx<10; ++idx ){
					TCHAR *cp;
					cp = _tcsrchr(keybuff, _T('\\'));
					if( cp != NULL ){
						folders[idx] = cp+1;
						*cp = _T('\0');
					}
					else{
						break;
					}
				}
			}
			{
				// $0-$9��u��
				//wcscpy( szNewPath, L"" );
				TCHAR *q= szFormat;
				TCHAR *q2 = szFormat;
				while( *q ){
					if( *q==_T('$') ){
						++q;
						if( isdigit(*q) ){
							q[-1] = _T('\0');
							_tcscat( szNewPath, q2 );
//							if( newPathCount <  auto_strlcat( szNewPath, q2, newPathCount ) ){
//								return false;
//							}
							if( folders[*q-_T('0')] != 0 ){
								_tcscat( szNewPath, folders[*q-_T('0')] );
//								if( newPathCount < auto_strlcat( szNewPath, folders[*q-_T('0')], newPathCount ) ){
//									return false;
//								}
							}
							q2 = q+1;
						}
					}
					++q;
				}
				_tcscat( szNewPath, q2 );
//				if( newPathCount < auto_strlcat( szNewPath, q2, newPathCount ) ){
//					return false;
//				}
			}
		}
		{
			TCHAR temp[1024];
			TCHAR *cp;
			//	2006.03.25 Aroka szExt[0] == '\0'�̂Ƃ��̃I�[�o���������C��
			TCHAR *ep = (szExt[0]!=0) ? &szExt[1] : &szExt[0];
			assert( newPathCount <= _countof(temp) );

			// * ���g���q�ɂ���
			while( _tcschr( szNewPath, _T('*') ) ){
				_tcscpy( temp, szNewPath );
				cp = _tcschr( temp, _T('*') );
				*cp = 0;
				if( -1 == auto_snprintf_s( szNewPath, newPathCount, _T("%ts%ts%ts"), temp, ep, cp+1 ) ){
					return false;
				}
			}
			//	??�̓o�b�N�A�b�v�A�Ԃɂ������Ƃ���ł͂��邪�C
			//	�A�ԏ����͖�����2���ɂ����Ή����Ă��Ȃ��̂�
			//	�g�p�ł��Ȃ�����?��_�ɕϊ����Ă��������
			while(( cp = _tcschr( szNewPath, _T('?') ) ) != NULL){
				*cp = _T('_');
			}
		}
	}
	return true;
}


