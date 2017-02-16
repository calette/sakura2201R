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
#include "CType.h"
#include "types/CTypeInit.h"
#include "view/Colors/EColorIndexType.h"
#include "env/CDocTypeManager.h"
#include "env/CShareData.h"
#include "env/DLLSHAREDATA.h"
#include "typeprop/CImpExpManager.h"

void _DefaultConfig(STypeConfig* pType);


// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- //
//                          CType                              //
// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- //
void CType::InitTypeConfig(int nIdx, STypeConfig& type)
{
	//�K��l���R�s�[
	static STypeConfig sDefault;
	static bool bLoadedDefault = false;
	if(!bLoadedDefault){
		_DefaultConfig(&sDefault);
		bLoadedDefault=true;
	}
	type = sDefault;

	//�C���f�b�N�X��ݒ�
	type.m_nIdx = nIdx;
	type.m_id = nIdx;

	//�ʐݒ�
	InitTypeConfigImp(&type);
}


// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- //
//                        CShareData                           //
// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- //

/*!	@brief ���L������������/�^�C�v�ʐݒ�

	�^�C�v�ʐݒ�̏���������

	@date 2005.01.30 genta CShareData::Init()���番���D
*/
void CShareData::InitTypeConfigs(DLLSHAREDATA* pShareData, std::vector<STypeConfig*>& types )
{
	CType* table[] = {
		new CType_Basis(),	//��{
		new CType_Text(),	//�e�L�X�g
		new CType_Cpp(),	//C/C++
		new CType_Html(),	//HTML
		new CType_Css(),	//CSS
		new CType_JavaScript(),	//JavaScript
		new CType_Sql(),	//PL/SQL
		new CType_Cobol(),	//COBOL
		new CType_Java(),	//Java
		new CType_Asm(),	//�A�Z���u��
		new CType_Awk(),	//awk
		new CType_Dos(),	//MS-DOS�o�b�`�t�@�C��
		new CType_Pascal(),	//Pascal
		new CType_Tex(),	//TeX
		new CType_Perl(),	//Perl
		new CType_Php(),	//PHP
		new CType_Python(),	//Python
		new CType_Ruby(),	//Ruby
		new CType_Vb(),		//Visual Basic
		new CType_Csharp(),	//C#
		new CType_Xml(),	//XML
		new CType_Rich(),	//���b�`�e�L�X�g
		new CType_Ini(),	//�ݒ�t�@�C��
	};
	types.clear();
	assert( _countof(table) <= MAX_TYPES );
	for(int i = 0; i < _countof(table) && i < MAX_TYPES; i++){
		STypeConfig* type = new STypeConfig;
		types.push_back(type);
		table[i]->InitTypeConfig(i, *type);
		auto_strcpy(pShareData->m_TypeMini[i].m_szTypeExts, type->m_szTypeExts);
		auto_strcpy(pShareData->m_TypeMini[i].m_szTypeName, type->m_szTypeName);
		pShareData->m_TypeMini[i].m_encoding = type->m_encoding;
		pShareData->m_TypeMini[i].m_id = type->m_id;
		SAFE_DELETE(table[i]);
	}
	pShareData->m_TypeBasis = *types[0];
	pShareData->m_nTypesCount = (int)types.size();
}

/*!	@brief ���L������������/�����L�[���[�h

	�����L�[���[�h�֘A�̏���������

	@param[in] bInit false=��������, true=sakura.ini���Ȃ������Ƃ�

	@date 2005.01.30 genta CShareData::Init()���番���D
		�L�[���[�h��`���֐��̊O�ɏo���C�o�^���}�N�������ĊȌ��ɁD
	@date 2013.12.22 Moca �L�[���[�h���C���|�[�g����悤��
*/
void CShareData::InitKeyword(DLLSHAREDATA* pShareData, bool bInit)
{
	/* �����L�[���[�h�̃e�X�g�f�[�^ */
	pShareData->m_Common.m_sSpecialKeyword.m_CKeyWordSetMgr.m_nCurrentKeyWordSetIdx = 0;
	CKeyWordSetMgr& cKeyWordSetMgr = pShareData->m_Common.m_sSpecialKeyword.m_CKeyWordSetMgr;
	int nSetCount = -1;
	TCHAR szKeywordDir[_MAX_PATH];
	GetExedir( szKeywordDir, _T("Keyword\\") );

#define PopulateKeyword1(name,case_sensitive,aryname, filename) \
	extern const wchar_t* g_ppszKeywords##aryname[]; \
	extern int g_nKeywords##aryname; \
	if( bInit ){ \
		++nSetCount; \
	}else{ \
		extern int g_nKeywordsIdx_##aryname; \
		g_nKeywordsIdx_##aryname = ++nSetCount; \
		cKeyWordSetMgr.AddKeyWordSet( (name), (case_sensitive) );	\
		cKeyWordSetMgr.SetKeyWordArr( nSetCount, g_nKeywords##aryname, g_ppszKeywords##aryname ); \
	} \

#define PopulateKeyword2(name,case_sensitive, aryname, filename) \
	if( bInit ){ \
		++nSetCount; \
		bool bCase = cKeyWordSetMgr.GetKeyWordCase( nSetCount ); \
		CImpExpKeyWord impKeyword( pShareData->m_Common, nSetCount, bCase ); \
		std::wstring sKeywordPath; \
		std::wstring TmpMsg; \
		sKeywordPath = to_wchar(szKeywordDir); \
		sKeywordPath += filename; \
		impKeyword.Import( sKeywordPath, TmpMsg ); \
	}else{ \
		extern int g_nKeywordsIdx_##aryname; \
		g_nKeywordsIdx_##aryname = ++nSetCount; \
		pShareData->m_Common.m_sSpecialKeyword.m_CKeyWordSetMgr.AddKeyWordSet( (name), (case_sensitive) ); \
	} \

#ifdef BUILD_OPT_IMPKEYWORD
#define PopulateKeyword PopulateKeyword1
#else
#define PopulateKeyword PopulateKeyword2
#endif
	PopulateKeyword1( L"C/C++",			true,	CPP,   L"cpp.kwd" );
	PopulateKeyword( L"HTML",			false,	HTML,  L"html5.kwd" );
	PopulateKeyword( L"PL/SQL",			false,	PLSQL, L"plsql.kwd" );
	PopulateKeyword( L"COBOL",			true,	COBOL, L"COBOL.kwd" );
	PopulateKeyword( L"Java",			true,	JAVA,  L"java.kwd" );
	PopulateKeyword( L"CORBA IDL",		true,	CORBA_IDL, L"corba.kwd" );
	PopulateKeyword( L"AWK",			true,	AWK,   L"awk.kwd" );
	PopulateKeyword( L"MS-DOS batch",	false,	BAT,   L"batch.kwd" );		//Oct. 31, 2000 JEPRO '�o�b�`�t�@�C��'��'batch' �ɒZ�k
	PopulateKeyword( L"Pascal",			false,	PASCAL, L"pascal.kwd" );	//Nov. 5, 2000 JEPRO ��E�������̋�ʂ�'���Ȃ�'�ɕύX
	PopulateKeyword( L"TeX",			true,	TEX,   L"tex1.kwd" );		//Sept. 2, 2000 jepro Tex ��TeX �ɏC�� Bool�l�͑�E�������̋��
	PopulateKeyword( L"TeX2",			true,	TEX2,  L"tex2.kwd" );		//Jan. 19, 2001 JEPRO �ǉ�
	PopulateKeyword( L"Perl",			true,	PERL,  L"perl.kwd" );
	PopulateKeyword( L"Perl2",			true,	PERL2, L"perlvar.kwd" );	//Jul. 10, 2001 JEPRO Perl����ϐ��𕪗��E�Ɨ�
	PopulateKeyword( L"Visual Basic",	false,	VB,    L"vb.kwd" );			//Jul. 10, 2001 JEPRO
	PopulateKeyword( L"Visual Basic2",	false,	VB2,   L"vb2.kwd" );		//Jul. 10, 2001 JEPRO
	PopulateKeyword( L"Rich Text",		true,	RTF,   L"rtf.kwd" );		//Jul. 10, 2001 JEPRO
	// 2013.12.22 Moca �ȉ�ruby4�܂Œǉ�
	PopulateKeyword2( L"C#",			true,	CSHARP,  L"csharp.kwd" );
	PopulateKeyword2( L"C# content",	true,	CSHARP2, L"csharp-context.kwd" );
	PopulateKeyword2( L"CSS",			true,	CSS,     L"css2.1.kwd" );
	PopulateKeyword2( L"JavaScript",	true,	JS,      L"ecmascript_sys.kwd" );
	PopulateKeyword2( L"JavaScript2",	true,	JS2,     L"javascript.kwd" );
	PopulateKeyword2( L"PHP",			true,	PHP,     L"php_reserved.kwd" );
	PopulateKeyword2( L"PHP2",			true,	PHP2,    L"php.kwd" );
	PopulateKeyword2( L"python",		true,	PYTHON,  L"python_2.5.kwd" );
	PopulateKeyword2( L"Ruby1",			true,	RUBY,    L"ruby1.kwd" );
	PopulateKeyword2( L"Ruby2",			true,	RUBY2,   L"ruby2.kwd" );
	PopulateKeyword2( L"Ruby3",			true,	RUBY3,   L"ruby3.kwd" );
	PopulateKeyword2( L"Ruby4",			true,	RUBY4,   L"ruby4.kwd" );

#undef PopulateKeyword1
#undef PopulateKeyword2
#undef PopulateKeyword
}


// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- //
//                        �f�t�H���g                           //
// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- //

void _DefaultConfig(STypeConfig* pType)
{
//�L�[���[�h�F�f�t�H���g�J���[�ݒ�
/************************/
/* �^�C�v�ʐݒ�̋K��l */
/************************/

	pType->m_nTextWrapMethod = WRAP_SETTING_WIDTH;	// �e�L�X�g�̐܂�Ԃ����@		// 2008.05.30 nasukoji
	pType->m_nMaxLineKetas = CLayoutInt(MAXLINEKETAS);	/* �܂�Ԃ����� */
	pType->m_nColumnSpace = 0;					/* �����ƕ����̌��� */
	pType->m_nLineSpace = 1;					/* �s�Ԃ̂����� */
	pType->m_nTabSpace = CLayoutInt(4);					/* TAB�̕����� */
	pType->m_nTsvMode = 0;						/* TSV���[�h */
	for( int i = 0; i < MAX_KEYWORDSET_PER_TYPE; i++ ){
		pType->m_nKeyWordSetIdx[i] = -1;
	}
	wcscpy( pType->m_szTabViewString, _EDITL("^       ") );	/* TAB�\�������� */
	pType->m_bTabArrow = TABARROW_STRING;	/* �^�u���\�� */	// 2001.12.03 hor	// default on 2013/4/11 Uchi
	pType->m_bInsSpace = false;				/* �X�y�[�X�̑}�� */	// 2001.12.03 hor
	
	//@@@ 2002.09.22 YAZAKI �ȉ��Am_cLineComment��m_cBlockComments���g���悤�ɏC��
	pType->m_cLineComment.CopyTo(0, L"", -1);	/* �s�R�����g�f���~�^ */
	pType->m_cLineComment.CopyTo(1, L"", -1);	/* �s�R�����g�f���~�^2 */
	pType->m_cLineComment.CopyTo(2, L"", -1);	/* �s�R�����g�f���~�^3 */	//Jun. 01, 2001 JEPRO �ǉ�
	pType->m_cBlockComments[0].SetBlockCommentRule(L"", L"");	/* �u���b�N�R�����g�f���~�^ */
	pType->m_cBlockComments[1].SetBlockCommentRule(L"", L"");	/* �u���b�N�R�����g�f���~�^2 */

	pType->m_nStringType = STRING_LITERAL_CPP;					/* �������؂�L���G�X�P�[�v���@ 0=[\"][\'] 1=[""][''] */
	pType->m_bStringLineOnly = false;
	pType->m_bStringEndLine  = false;
	pType->m_nHeredocType = HEREDOC_PHP;
	pType->m_szIndentChars[0] = L'\0';		/* ���̑��̃C���f���g�Ώە��� */

	pType->m_nColorInfoArrNum = COLORIDX_LAST;

	// 2001/06/14 Start by asa-o
	_tcscpy( pType->m_szHokanFile, _T("") );		/* ���͕⊮ �P��t�@�C�� */
	// 2001/06/14 End

	pType->m_nHokanType = 0;

	// 2001/06/19 asa-o
	pType->m_bHokanLoHiCase = false;			// ���͕⊮�@�\�F�p�啶���������𓯈ꎋ����

	//	2003.06.23 Moca �t�@�C��������̓��͕⊮�@�\
	pType->m_bUseHokanByFile = true;			//! ���͕⊮ �J���Ă���t�@�C�����������T��
	pType->m_bUseHokanByKeyword = true;			// �����L�[���[�h������͕⊮

	// �����R�[�h�ݒ�
	pType->m_encoding.m_bPriorCesu8 = false;
	pType->m_encoding.m_eDefaultCodetype = CODE_SJIS;
	pType->m_encoding.m_eDefaultEoltype = EOL_CRLF;
	pType->m_encoding.m_bDefaultBom = false;

	//@@@2002.2.4 YAZAKI
	pType->m_szExtHelp[0] = L'\0';
	pType->m_szExtHtmlHelp[0] = L'\0';
	pType->m_bHtmlHelpIsSingle = true;

	pType->m_bAutoIndent = true;			/* �I�[�g�C���f���g */
	pType->m_bAutoIndent_ZENSPACE = true;	/* ���{��󔒂��C���f���g */
	pType->m_bRTrimPrevLine = false;		// 2005.10.11 ryoji ���s���ɖ����̋󔒂��폜

	pType->m_nIndentLayout = 0;	/* �܂�Ԃ���2�s�ڈȍ~���������\�� */


	assert( COLORIDX_LAST <= _countof(pType->m_ColorInfoArr) );
	for( int i = 0; i < COLORIDX_LAST; ++i ){
		GetDefaultColorInfo(&pType->m_ColorInfoArr[i],i);
	}
	pType->m_szBackImgPath[0] = '\0';
	pType->m_backImgPos = BGIMAGE_TOP_LEFT;
	pType->m_backImgRepeatX = true;
	pType->m_backImgRepeatY = true;
	pType->m_backImgScrollX = true;
	pType->m_backImgScrollY = true;
	{
		POINT pt ={0,0};
		pType->m_backImgPosOffset = pt;
	}
	pType->m_bLineNumIsCRLF = true;					// �s�ԍ��̕\�� false=�܂�Ԃ��P�ʁ^true=���s�P��
	pType->m_nLineTermType = 1;						// �s�ԍ���؂� 0=�Ȃ� 1=�c�� 2=�C��
	pType->m_cLineTermChar = L':';					// �s�ԍ���؂蕶��
	pType->m_bWordWrap = false;						// �p�����[�h���b�v������
	pType->m_nCurrentPrintSetting = 0;				// ���ݑI�����Ă������ݒ�
	pType->m_bOutlineDockDisp = false;				// �A�E�g���C����͕\���̗L��
	pType->m_eOutlineDockSide = DOCKSIDE_FLOAT;		// �A�E�g���C����̓h�b�L���O�z�u
	pType->m_cxOutlineDockLeft = 0;					// �A�E�g���C���̍��h�b�L���O��
	pType->m_cyOutlineDockTop = 0;					// �A�E�g���C���̏�h�b�L���O��
	pType->m_cxOutlineDockRight = 0;				// �A�E�g���C���̉E�h�b�L���O��
	pType->m_cyOutlineDockBottom = 0;				// �A�E�g���C���̉��h�b�L���O��
	pType->m_eDefaultOutline = OUTLINE_TEXT;		/* �A�E�g���C����͕��@ */
	pType->m_nOutlineSortCol = 0;					/* �A�E�g���C����̓\�[�g��ԍ� */
	pType->m_bOutlineSortDesc = false;				// �A�E�g���C����̓\�[�g�~��
	pType->m_nOutlineSortType = 0;					/* �A�E�g���C����̓\�[�g� */
	CShareData::InitFileTree( &pType->m_sFileTree );
	pType->m_eSmartIndent = SMARTINDENT_NONE;		/* �X�}�[�g�C���f���g��� */
	pType->m_bIndentCppStringIgnore = true;
	pType->m_bIndentCppCommentIgnore = true;
	pType->m_bIndentCppUndoSep = false;
	pType->m_nImeState = IME_CMODE_NOCONVERSION;	/* IME���� */

	pType->m_szOutlineRuleFilename[0] = L'\0';		//Dec. 4, 2000 MIK
	pType->m_bKinsokuHead = false;					// �s���֑�				//@@@ 2002.04.08 MIK
	pType->m_bKinsokuTail = false;					// �s���֑�				//@@@ 2002.04.08 MIK
	pType->m_bKinsokuRet  = false;					// ���s�������Ԃ牺����	//@@@ 2002.04.13 MIK
	pType->m_bKinsokuKuto = false;					// ��Ǔ_���Ԃ牺����	//@@@ 2002.04.17 MIK
	pType->m_szKinsokuHead[0] = L'\0';				// �s���֑�				//@@@ 2002.04.08 MIK
	pType->m_szKinsokuTail[0] = L'\0';				// �s���֑�				//@@@ 2002.04.08 MIK
	wcscpy( pType->m_szKinsokuKuto, L"�A�B�C�D��,." );	// ��Ǔ_�Ԃ牺������	// 2009.08.07 ryoji

	pType->m_bUseDocumentIcon = false;				// �����Ɋ֘A�Â���ꂽ�A�C�R�����g��

//@@@ 2001.11.17 add start MIK
	for(int i = 0; i < _countof(pType->m_RegexKeywordArr); i++)
	{
		pType->m_RegexKeywordArr[i].m_nColorIndex = COLORIDX_REGEX1;
	}
	pType->m_RegexKeywordList[0] = L'\0';
	pType->m_bUseRegexKeyword = false;
//@@@ 2001.11.17 add end MIK
	pType->m_nRegexKeyMagicNumber = 0;

//@@@ 2006.04.10 fon ADD-start
	for(int i = 0; i < MAX_KEYHELP_FILE; i++){
		pType->m_KeyHelpArr[i].m_bUse = false;
		pType->m_KeyHelpArr[i].m_szAbout[0] = _T('\0');
		pType->m_KeyHelpArr[i].m_szPath[0] = _T('\0');
	}
	pType->m_bUseKeyWordHelp = false;		// �����I���@�\�̎g�p��
	pType->m_nKeyHelpNum = 0;				// �o�^������
	pType->m_bUseKeyHelpAllSearch = false;	// �q�b�g�������̎���������(&A)
	pType->m_bUseKeyHelpKeyDisp = false;	// 1�s�ڂɃL�[���[�h���\������(&W)
	pType->m_bUseKeyHelpPrefix = false;		// �I��͈͂őO����v����(&P)
//@@@ 2006.04.10 fon ADD-end

	// 2005.11.08 Moca �w��ʒu�c���̐ݒ�
	for(int i = 0; i < MAX_VERTLINES; i++ ){
		pType->m_nVertLineIdx[i] = CLayoutInt(0);
	}
	pType->m_nNoteLineOffset = 0;

	//  �ۑ����ɉ��s�R�[�h�̍��݂��x������	2013/4/14 Uchi
	pType->m_bChkEnterAtEnd = true;

	pType->m_bUseTypeFont = false;			//!< �^�C�v�ʃt�H���g�̎g�p

	pType->m_nLineNumWidth = LINENUMWIDTH_MIN;	//!< �s�ԍ��ŏ����� 2014.08.02 katze
}

void RegexAdd(STypeConfig* pType, int& keywordPos, int idx, int colorIdx, const wchar_t* keyword )
{
	wchar_t* pKeyword = pType->m_RegexKeywordList;
	pType->m_RegexKeywordArr[idx].m_nColorIndex = colorIdx;
	wcscpyn( &pKeyword[keywordPos], keyword, _countof(pType->m_RegexKeywordList) - keywordPos - 1 );
	keywordPos += auto_strlen(&pKeyword[keywordPos]) + 1;
	pKeyword[keywordPos] = L'\0';
}

void SetColorInfoBC(STypeConfig* pType, int index, bool bBold, COLORREF color)
{
	pType->m_ColorInfoArr[index].m_bDisp = true;
	pType->m_ColorInfoArr[index].m_sFontAttr.m_bBoldFont = bBold;
	pType->m_ColorInfoArr[index].m_sColorAttr.m_cTEXT = color;
}