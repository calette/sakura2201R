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
#include <limits.h>
#include "CEditView.h"
#include "window/CEditWnd.h"
#include "parse/CWordParse.h"
#include "util/string_ex2.h"

const int STRNCMP_MAX = 100;	/* MAX�L�[���[�h���Fstrnicmp�������r�ő�l(CEditView::KeySearchCore) */	// 2006.04.10 fon

// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- //
//                           ����                              //
// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- //

/*! �L�[���[�h���������̑O������`�F�b�N�ƁA����

	@date 2006.04.10 fon OnTimer, CreatePopUpMenu_R���番��
*/
BOOL CEditView::KeyWordHelpSearchDict( LID_SKH nID, POINT* po, RECT* rc )
{
	CNativeW	cmemCurText;

	/* �L�[���[�h�w���v���g�p���邩�H */
	if( !m_pTypeData->m_bUseKeyWordHelp )	/* �L�[���[�h�w���v�@�\���g�p���� */	// 2006.04.10 fon
		goto end_of_search;
	/* �t�H�[�J�X�����邩�H */
	if( !GetCaret().ExistCaretFocus() ) 
		goto end_of_search;
	/* �E�B���h�E���Ƀ}�E�X�J�[�\�������邩�H */
	GetCursorPos( po );
	GetWindowRect( GetHwnd(), rc );
	if( !PtInRect( rc, *po ) )
		goto end_of_search;
	switch(nID){
	case LID_SKH_ONTIMER:
		/* �E�R�����g�̂P�`�R�łȂ��ꍇ */
		if(!( m_bInMenuLoop == FALSE	&&			/* �P�D���j���[ ���[�_�� ���[�v�ɓ����Ă��Ȃ� */
			0 != m_dwTipTimer			&&			/* �Q�D����Tip��\�����Ă��Ȃ� */
			300 < ::GetTickCount() - m_dwTipTimer	/* �R�D��莞�Ԉȏ�A�}�E�X���Œ肳��Ă��� */
		) )	goto end_of_search;
		break;
	case LID_SKH_POPUPMENU_R:
		if(!( m_bInMenuLoop == FALSE	//&&			/* �P�D���j���[ ���[�_�� ���[�v�ɓ����Ă��Ȃ� */
		//	0 != m_dwTipTimer			&&			/* �Q�D����Tip��\�����Ă��Ȃ� */
		//	1000 < ::GetTickCount() - m_dwTipTimer	/* �R�D��莞�Ԉȏ�A�}�E�X���Œ肳��Ă��� */
		) )	goto end_of_search;
		break;
	default:
		PleaseReportToAuthor( NULL, _T("CEditView::KeyWordHelpSearchDict\nnID=%d"), (int)nID );
	}
	/* �I��͈͂̃f�[�^���擾(�����s�I���̏ꍇ�͐擪�̍s�̂�) */
	if( GetSelectedDataOne( cmemCurText, STRNCMP_MAX + 1 ) ){
	}
	/* �L�����b�g�ʒu�̒P����擾���鏈�� */	// 2006.03.24 fon
	else if(GetDllShareData().m_Common.m_sSearch.m_bUseCaretKeyWord){
		if(!GetParser().GetCurrentWord(&cmemCurText))
			goto end_of_search;
	}
	else
		goto end_of_search;

	if( CNativeW::IsEqual( cmemCurText, m_cTipWnd.m_cKey ) &&	/* ���Ɍ����ς݂� */
		(!m_cTipWnd.m_KeyWasHit) )								/* �Y������L�[���Ȃ����� */
		goto end_of_search;
	m_cTipWnd.m_cKey = cmemCurText;

	/* �������s */
	if( !KeySearchCore(&m_cTipWnd.m_cKey) )
		goto end_of_search;
	m_dwTipTimer = 0;		/* ����Tip��\�����Ă��� */
	m_poTipCurPos = *po;	/* ���݂̃}�E�X�J�[�\���ʒu */
	return TRUE;			/* �����܂ŗ��Ă���΃q�b�g�E���[�h */

	/* �L�[���[�h�w���v�\�������I�� */
	end_of_search:
	return FALSE;
}

/*! �L�[���[�h���������������C��

	@date 2006.04.10 fon KeyWordHelpSearchDict���番��
*/
BOOL CEditView::KeySearchCore( const CNativeW* pcmemCurText )
{
	CNativeW*	pcmemRefKey;
	int			nCmpLen = STRNCMP_MAX; // 2006.04.10 fon
	int			nLine; // 2006.04.10 fon


	m_cTipWnd.m_cInfo.SetString( _T("") );	/* tooltip�o�b�t�@������ */
	/* 1�s�ڂɃL�[���[�h�\���̏ꍇ */
	if(m_pTypeData->m_bUseKeyHelpKeyDisp){	/* �L�[���[�h���\������ */	// 2006.04.10 fon
		m_cTipWnd.m_cInfo.AppendString( _T("[ ") );
		m_cTipWnd.m_cInfo.AppendString( pcmemCurText->GetStringT() );
		m_cTipWnd.m_cInfo.AppendString( _T(" ]") );
	}
	/* �r���܂ň�v���g���ꍇ */
	if(m_pTypeData->m_bUseKeyHelpPrefix)
		nCmpLen = wcslen( pcmemCurText->GetStringPtr() );	// 2006.04.10 fon
	m_cTipWnd.m_KeyWasHit = FALSE;
	for(int i =0 ; i < m_pTypeData->m_nKeyHelpNum; i++){	//�ő吔�FMAX_KEYHELP_FILE
		if( m_pTypeData->m_KeyHelpArr[i].m_bUse ){
			// 2006.04.10 fon (nCmpLen,pcmemRefKey,nSearchLine)������ǉ�
			CNativeW*	pcmemRefText;
			int nSearchResult=m_cDicMgr.CDicMgr::Search(
				pcmemCurText->GetStringPtr(),
				nCmpLen,
				&pcmemRefKey,
				&pcmemRefText,
				m_pTypeData->m_KeyHelpArr[i].m_szPath,
				&nLine
			);
			if(nSearchResult){
				/* �Y������L�[������ */
				LPWSTR		pszWork;
				pszWork = pcmemRefText->GetStringPtr();
				/* �L���ɂȂ��Ă��鎫����S���Ȃ߂āA�q�b�g�̓s�x�����̌p������ */
				if(m_pTypeData->m_bUseKeyHelpAllSearch){	/* �q�b�g�������̎��������� */	// 2006.04.10 fon
					/* �o�b�t�@�ɑO�̃f�[�^���l�܂��Ă�����separator�}�� */
					if(m_cTipWnd.m_cInfo.GetStringLength() != 0)
						m_cTipWnd.m_cInfo.AppendString( LS(STR_ERR_DLGEDITVW5) );
					else
						m_cTipWnd.m_cInfo.AppendString( LS(STR_ERR_DLGEDITVW6) );	/* �擪�̏ꍇ */
					/* �����̃p�X�}�� */
					{
						TCHAR szFile[MAX_PATH];
						// 2013.05.08 �\������̂̓t�@�C����(�g���q�Ȃ�)�݂̂ɂ���
						_tsplitpath( m_pTypeData->m_KeyHelpArr[i].m_szPath, NULL, NULL, szFile, NULL );
						m_cTipWnd.m_cInfo.AppendString( szFile );
					}
					m_cTipWnd.m_cInfo.AppendString( _T("\n") );
					/* �O����v�Ńq�b�g�����P���}�� */
					if(m_pTypeData->m_bUseKeyHelpPrefix){	/* �I��͈͂őO����v���� */
						m_cTipWnd.m_cInfo.AppendString( pcmemRefKey->GetStringT() );
						m_cTipWnd.m_cInfo.AppendString( _T(" >>\n") );
					}/* ���������u�Ӗ��v��}�� */
					m_cTipWnd.m_cInfo.AppendStringW( pszWork );
					delete pcmemRefText;
					delete pcmemRefKey;	// 2006.07.02 genta
					/* �^�O�W�����v�p�̏����c�� */
					if(!m_cTipWnd.m_KeyWasHit){
						m_cTipWnd.m_nSearchDict=i;	/* �������J���Ƃ��ŏ��Ƀq�b�g�����������J�� */
						m_cTipWnd.m_nSearchLine=nLine;
						m_cTipWnd.m_KeyWasHit = TRUE;
					}
				}
				else{	/* �ŏ��̃q�b�g���ڂ̂ݕԂ��ꍇ */
					/* �L�[���[�h�������Ă�����separator�}�� */
					if(m_cTipWnd.m_cInfo.GetStringLength() != 0)
						m_cTipWnd.m_cInfo.AppendString( _T("\n--------------------\n") );
					
					/* �O����v�Ńq�b�g�����P���}�� */
					if(m_pTypeData->m_bUseKeyHelpPrefix){	/* �I��͈͂őO����v���� */
						m_cTipWnd.m_cInfo.AppendString( pcmemRefKey->GetStringT() );
						m_cTipWnd.m_cInfo.AppendString( _T(" >>\n") );
					}
					
					/* ���������u�Ӗ��v��}�� */
					m_cTipWnd.m_cInfo.AppendStringW( pszWork );
					delete pcmemRefText;
					delete pcmemRefKey;	// 2006.07.02 genta
					/* �^�O�W�����v�p�̏����c�� */
					m_cTipWnd.m_nSearchDict=i;
					m_cTipWnd.m_nSearchLine=nLine;
					m_cTipWnd.m_KeyWasHit = TRUE;
					return TRUE;
				}
			}
		}
	}
	if( m_cTipWnd.m_KeyWasHit != FALSE ){
			return TRUE;
	}
	/* �Y������L�[���Ȃ������ꍇ */
	return FALSE;
}

bool CEditView::MiniMapCursorLineTip( POINT* po, RECT* rc, bool* pbHide )
{
	*pbHide = true;
	if( !m_bMiniMap ){
		return false;
	}
	// �E�B���h�E���Ƀ}�E�X�J�[�\�������邩�H
	GetCursorPos( po );
	GetWindowRect( GetHwnd(), rc );
	rc->right -= ::GetSystemMetrics(SM_CXVSCROLL);
	if( !PtInRect( rc, *po ) ){
		return false;
	}
	if(!( m_bInMenuLoop == FALSE	&&			/* �P�D���j���[ ���[�_�� ���[�v�ɓ����Ă��Ȃ� */
		300 < ::GetTickCount() - m_dwTipTimer	/* �Q�D��莞�Ԉȏ�A�}�E�X���Œ肳��Ă��� */
	) ){
		return false;
	}
	if( WindowFromPoint( *po ) != GetHwnd() ){
		return false;
	}

	CMyPoint ptClient(*po);
	ScreenToClient( GetHwnd(), &ptClient );
	CLayoutPoint ptNew;
	GetTextArea().ClientToLayout( ptClient, &ptNew );
	// �����s�Ȃ�Ȃɂ����Ȃ�
	if( 0 == m_dwTipTimer && m_cTipWnd.m_nSearchLine == (Int)ptNew.y ){
		*pbHide = false; // �\���p��
		return false;
	}
	CNativeW cmemCurText;
	CLayoutYInt nTipBeginLine = ptNew.y;
	CLayoutYInt nTipEndLine = ptNew.y + CLayoutYInt(4);
	for( CLayoutYInt nCurLine = nTipBeginLine; nCurLine < nTipEndLine; nCurLine++ ){
		const CLayout* pcLayout = NULL;
		if( 0 <= nCurLine ){
			pcLayout = GetDocument()->m_cLayoutMgr.SearchLineByLayoutY( nCurLine );
		}
		if( pcLayout ){
			CNativeW cmemCurLine;
			{
				CLogicInt nLineLen = pcLayout->GetLengthWithoutEOL();
				const wchar_t* pszData = pcLayout->GetPtr();
				int nLimitLength = 80;
				int pre = 0;
				int i = 0;
				int k = 0;
				int charSize = CNativeW::GetSizeOfChar( pszData, nLineLen, i );
				int charWidth = t_max(1, (int)(Int)CNativeW::GetKetaOfChar( pszData, nLineLen, i ));
				int charType = 0;
				// �A������"\t" " " �� " "1�ɂ���
				// ������nLimitLength�܂ł̕���؂���
				while( i + charSize <= (Int)nLineLen && k + charWidth <= nLimitLength ){
					if( pszData[i] == L'\t' || pszData[i] == L' ' ){
						if( charType == 0 ){
							cmemCurLine.AppendString( pszData + pre , i - pre );
							cmemCurLine.AppendString( L" " );
							charType = 1;
						}
						pre = i + charSize;
						k++;
					}else{
						k += charWidth;
						charType = 0;
					}
					i += charSize;
					charSize = CNativeW::GetSizeOfChar( pszData, nLineLen, i );
					charWidth = t_max(1, (int)(Int)CNativeW::GetKetaOfChar( pszData, nLineLen, i ));
				}
				cmemCurLine.AppendString( pszData + pre , i - pre );
			}
			if( nTipBeginLine != nCurLine ){
				cmemCurText.AppendString( L"\n" );
			}
			cmemCurLine.Replace( L"\\", L"\\\\" );
			cmemCurText.AppendNativeData( cmemCurLine );
		}
	}
	if( cmemCurText.GetStringLength() <= 0 ){
		return false;
	}
	m_cTipWnd.m_cKey = cmemCurText;
	m_cTipWnd.m_cInfo = cmemCurText.GetStringT();
	m_cTipWnd.m_nSearchLine = (Int)ptNew.y;
	m_dwTipTimer = 0;		// ����Tip��\�����Ă��� */
	m_poTipCurPos = *po;	// ���݂̃}�E�X�J�[�\���ʒu */
	return true;			// �����܂ŗ��Ă���΃q�b�g�E���[�h
}

/* ���݃J�[�\���ʒu�P��܂��͑I��͈͂�茟�����̃L�[���擾 */
#if REI_MOD_SEARCH_KEY_REGEXP_AUTO_QUOTE
void CEditView::GetCurrentTextForSearch( CNativeW& cmemCurText, bool bStripMaxPath /* = true */, bool bTrimSpaceTab /* = false */, bool bRegQuote /* = false */ )
#else
void CEditView::GetCurrentTextForSearch( CNativeW& cmemCurText, bool bStripMaxPath /* = true */, bool bTrimSpaceTab /* = false */ )
#endif // rei_
{

	int				i;
	CNativeW		cmemTopic = L"";
	const wchar_t*	pLine;
	CLogicInt		nLineLen;
	CLogicInt		nIdx;
	CLayoutRange	sRange;

	cmemCurText.SetString(L"");
	if( GetSelectionInfo().IsTextSelected() ){	/* �e�L�X�g���I������Ă��邩 */
		/* �I��͈͂̃f�[�^���擾 */
		if( GetSelectedDataOne( cmemCurText, INT_MAX ) ){
			/* ��������������݈ʒu�̒P��ŏ����� */
			if( bStripMaxPath ){
				LimitStringLengthW(cmemCurText.GetStringPtr(), cmemCurText.GetStringLength(), _MAX_PATH - 1, cmemTopic);
			}else{
				cmemTopic = cmemCurText;
			}
		}
	}else{
		const CLayout*	pcLayout;
		pLine = m_pcEditDoc->m_cLayoutMgr.GetLineStr( GetCaret().GetCaretLayoutPos().GetY2(), &nLineLen, &pcLayout );
		if( NULL != pLine ){
			/* �w�肳�ꂽ���ɑΉ�����s�̃f�[�^���̈ʒu�𒲂ׂ� */
			nIdx = LineColumnToIndex( pcLayout, GetCaret().GetCaretLayoutPos().GetX2() );

			/* ���݈ʒu�̒P��͈̔͂𒲂ׂ� */
			bool bWhere = m_pcEditDoc->m_cLayoutMgr.WhereCurrentWord(
				GetCaret().GetCaretLayoutPos().GetY2(),
				nIdx,
				&sRange,
				NULL,
				NULL
			);
			if( bWhere ){
				/* �I��͈͂̕ύX */
				GetSelectionInfo().m_sSelectBgn = sRange;
				GetSelectionInfo().m_sSelect    = sRange;

				/* �I��͈͂̃f�[�^���擾 */
				if( GetSelectedDataOne( cmemCurText, INT_MAX ) ){
					/* ��������������݈ʒu�̒P��ŏ����� */
					if( bStripMaxPath ){
						LimitStringLengthW(cmemCurText.GetStringPtr(), cmemCurText.GetStringLength(), _MAX_PATH - 1, cmemTopic);
					}else{
						cmemTopic = cmemCurText;
					}
				}
				/* ���݂̑I��͈͂��I����Ԃɖ߂� */
				GetSelectionInfo().DisableSelectArea( false );
			}
		}
	}

#if REI_MOD_SEARCH_KEY_REGEXP_AUTO_QUOTE
	// ���K�\���������N�H�[�g����
	// (PHP 4, PHP 5) string preg_quote ( string $str [, string $delimiter = NULL ] )
  auto preg_quote = [](const CNativeW &str) {
    const wchar_t *pStr = str.GetStringPtr();
    bool bEscape = false;

    CNativeW out = L"";

    while (L'\0' != *pStr) {
      if (bEscape) {
        bEscape = false;
      } else if (!bEscape && *pStr == L'\\') {
        bEscape = true;
      }
      // ���K�\���������N�H�[�g���� . \ + * ? [ ^ ] $ ( ) { } = ! < > | : -
      else if (*pStr == L'[' || *pStr == L']' || *pStr == L'(' || *pStr == L')' ||
               *pStr == L'{' || *pStr == L'}' || *pStr == L'<' || *pStr == L'>' ||
               *pStr == L'+' || *pStr == L'*' || *pStr == L'.' || *pStr == L'?' ||
               *pStr == L'^' || *pStr == L'$' || *pStr == L'=' ||
               *pStr == L'\\' || *pStr == L'!' || *pStr == L'|' ||
               *pStr == L':' || *pStr == L'-') {
        out += L"\\";
      }

      out += *pStr;
      pStr++;
    }

    return out;
  };
	
	{
    static int regexp_auto_quote = !!RegGetDword(L"RegexpAutoQuote", 1);
    
    if (bRegQuote) {
      bRegQuote = regexp_auto_quote;
    }
  }
	
	//if (GetDllShareData().m_Common.m_sSearch.m_sSearchOption.bRegularExp) {
	if (bRegQuote) {
		cmemTopic = preg_quote(cmemTopic);
	}
#endif // rei_

	wchar_t *pTopic2 = cmemTopic.GetStringPtr();
	if( bTrimSpaceTab ){
		// �O�̃X�y�[�X�E�^�u����菜��
		while( L'\0' != *pTopic2 && ( ' ' == *pTopic2 || '\t' == *pTopic2 ) ){
			pTopic2++;
		}
	}
	int nTopic2Len = (int)wcslen( pTopic2 );
	/* ����������͉��s�܂� */
	bool bExtEol = GetDllShareData().m_Common.m_sEdit.m_bEnableExtEol;
	for( i = 0; i < nTopic2Len; ++i ){
		if( WCODE::IsLineDelimiter(pTopic2[i], bExtEol) ){
			break;
		}
	}
	
	if( bTrimSpaceTab ){
		// ���̃X�y�[�X�E�^�u����菜��
		int m = i - 1;
		while( 0 <= m &&
		    ( L' ' == pTopic2[m] || L'\t' == pTopic2[m] ) ){
			m--;
		}
		if( 0 <= m ){
			i = m + 1;
		}
	}
	cmemCurText.SetString( pTopic2, i );
}


/*!	���݃J�[�\���ʒu�P��܂��͑I��͈͂�茟�����̃L�[���擾�i�_�C�A���O�p�j
	@return �l��ݒ肵����
	@date 2006.08.23 ryoji �V�K�쐬
	@date 2014.07.01 Moca bGetHistory�ǉ��A�߂�l��bool�ɕύX
*/
#if REI_MOD_SEARCH_KEY_REGEXP_AUTO_QUOTE
bool CEditView::GetCurrentTextForSearchDlg( CNativeW& cmemCurText, bool bGetHistory, bool bRegQuote /* = false */ )
#else
bool CEditView::GetCurrentTextForSearchDlg( CNativeW& cmemCurText, bool bGetHistory )
#endif // rei_
{
	bool bStripMaxPath = false;
	cmemCurText.SetString(L"");

	if( GetSelectionInfo().IsTextSelected() ){	// �e�L�X�g���I������Ă���
#if REI_MOD_SEARCH_KEY_REGEXP_AUTO_QUOTE
		GetCurrentTextForSearch( cmemCurText, bStripMaxPath, false, bRegQuote );
#else
		GetCurrentTextForSearch( cmemCurText, bStripMaxPath );
#endif // rei_
	}
	else{	// �e�L�X�g���I������Ă��Ȃ�
		bool bGet = false;
		if( GetDllShareData().m_Common.m_sSearch.m_bCaretTextForSearch ){
#if REI_MOD_SEARCH_KEY_REGEXP_AUTO_QUOTE
			GetCurrentTextForSearch( cmemCurText, bStripMaxPath, false, bRegQuote );	// �J�[�\���ʒu�P����擾
#else
			GetCurrentTextForSearch( cmemCurText, bStripMaxPath );	// �J�[�\���ʒu�P����擾
#endif // rei_
			if( cmemCurText.GetStringLength() == 0 && bGetHistory ){
				bGet = true;
			}
		}else{
			bGet = true;
		}
		if( bGet ){
			if( 0 < GetDllShareData().m_sSearchKeywords.m_aSearchKeys.size()
					&& m_nCurSearchKeySequence < GetDllShareData().m_Common.m_sSearch.m_nSearchKeySequence ){
				cmemCurText.SetString( GetDllShareData().m_sSearchKeywords.m_aSearchKeys[0] );	// ��������Ƃ��Ă���
				return true; // ""�ł�true
			}else{
				cmemCurText.SetString( m_strCurSearchKey.c_str() );
				return 0 <= m_nCurSearchKeySequence; // ""�ł�true.���ݒ�̂Ƃ���false
			}
		}
	}
	return 0 < cmemCurText.GetStringLength();
}



// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- //
//                        �`��p����                           //
// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- //

/* ���݈ʒu������������ɊY�����邩 */
//2002.02.08 hor
//���K�\���Ō��������Ƃ��̑��x���P�̂��߁A�}�b�`�擪�ʒu�������ɒǉ�
//Jun. 26, 2001 genta	���K�\�����C�u�����̍����ւ�
/*
	@retval 0
		(�p�^�[��������) �w��ʒu�ȍ~�Ƀ}�b�`�͂Ȃ��B
		(����ȊO) �w��ʒu�͌���������̎n�܂�ł͂Ȃ��B
	@retval 1,2,3,...
		(�p�^�[��������) �w��ʒu�ȍ~�Ƀ}�b�`�����������B
		(�P�ꌟ����) �w��ʒu������������Ɋ܂܂�鉽�Ԗڂ̒P��̎n�܂�ł��邩�B
		(����ȊO) �w��ʒu������������̎n�܂肾�����B
*/
int CEditView::IsSearchString(
	const CStringRef&	cStr,
	/*
	const wchar_t*	pszData,
	CLogicInt		nDataLen,
	*/
	CLogicInt		nPos,
	CLogicInt*		pnSearchStart,
	CLogicInt*		pnSearchEnd
) const
{
	*pnSearchStart = nPos;	// 2002.02.08 hor

	if( m_sCurSearchOption.bRegularExp ){
		/* �s���ł͂Ȃ�? */
		/* �s�������`�F�b�N�́ACBregexp�N���X�����Ŏ��{����̂ŕs�v 2003.11.01 ����� */

		/* �ʒu��0��MatchInfo�Ăяo���ƁA�s�������������ɁA�S�� true�@�ƂȂ�A
		** ��ʑS�̂����������񈵂��ɂȂ�s��C��
		** �΍�Ƃ��āA�s���� MacthInfo�ɋ����Ȃ��Ƃ����Ȃ��̂ŁA������̒����E�ʒu����^����`�ɕύX
		** 2003.05.04 �����
		*/
		if( m_CurRegexp.Match( cStr.GetPtr(), cStr.GetLength(), nPos ) ){
			*pnSearchStart = m_CurRegexp.GetIndex();	// 2002.02.08 hor
			*pnSearchEnd = m_CurRegexp.GetLastIndex();
			return 1;
		}
		else{
			return 0;
		}
	}
	else if( m_sCurSearchOption.bWordOnly ) { // �P�ꌟ��
		/* �w��ʒu�̒P��͈̔͂𒲂ׂ� */
		CLogicInt posWordHead, posWordEnd;
		if( ! CWordParse::WhereCurrentWord_2( cStr.GetPtr(), CLogicInt(cStr.GetLength()), nPos, &posWordHead, &posWordEnd, NULL, NULL ) ) {
			return 0; // �w��ʒu�ɒP�ꂪ������Ȃ������B
 		}
		if( nPos != posWordHead ) {
			return 0; // �w��ʒu�͒P��̎n�܂�ł͂Ȃ������B
		}
		const CLogicInt wordLength = posWordEnd - posWordHead;
		const wchar_t *const pWordHead = cStr.GetPtr() + posWordHead;

		// ��r�֐�
		int (*const fcmp)( const wchar_t*, const wchar_t*, size_t ) = m_sCurSearchOption.bLoHiCase ? wcsncmp : wcsnicmp;

		// �������P��ɕ������Ȃ���w��ʒu�̒P��Əƍ�����B
		int wordIndex = 0;
		const wchar_t* const searchKeyEnd = m_strCurSearchKey.data() + m_strCurSearchKey.size();
		for( const wchar_t* p = m_strCurSearchKey.data(); p < searchKeyEnd; ) {
			CLogicInt begin, end; // ������Ɋ܂܂��P��?�̈ʒu�BWhereCurrentWord_2()�̎d�l�ł͋󔒕�������P��Ɋ܂܂��B
			if( CWordParse::WhereCurrentWord_2( p, CLogicInt(searchKeyEnd - p), CLogicInt(0), &begin, &end, NULL, NULL )
				&& begin == 0 && begin < end
			) {
				if( ! WCODE::IsWordDelimiter( *p ) ) {
					++wordIndex;
					// p...(p + end) ��������Ɋ܂܂�� wordIndex�Ԗڂ̒P��B(wordIndex�̍ŏ��� 1)
					if( wordLength == end && 0 == fcmp( p, pWordHead, wordLength ) ) {
						*pnSearchStart = posWordHead;
						*pnSearchEnd = posWordEnd;
						return wordIndex;
					}
				}
				p += end;
			} else {
				p += CNativeW::GetSizeOfChar( p, searchKeyEnd - p, 0 );
			}
		}
		return 0; // �w��ʒu�̒P��ƌ���������Ɋ܂܂��P��͈�v���Ȃ������B
	}
	else {
		const wchar_t* pHit = CSearchAgent::SearchString(cStr.GetPtr(), cStr.GetLength(), nPos, m_sSearchPattern);
		if( pHit ){
			*pnSearchStart = pHit - cStr.GetPtr();
			*pnSearchEnd = *pnSearchStart + m_sSearchPattern.GetLen();
			return 1;
		}
		return 0; // ���̍s�̓q�b�g���Ȃ�����
	}
	return 0;
}