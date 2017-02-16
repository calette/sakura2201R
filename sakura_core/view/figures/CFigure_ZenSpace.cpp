#include "StdAfx.h"
#include "view/CEditView.h" // SColorStrategyInfo
#include "CFigure_ZenSpace.h"
#include "types/CTypeSupport.h"


// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- //
//                      CFigure_ZenSpace                         //
// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- //

bool CFigure_ZenSpace::Match(const wchar_t* pText, int nTextLen) const
{
	if( pText[0] == L'�@' ){
		return true;
	}
	return false;
}


// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- //
//                         �`�����                            //
// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- //

//! �S�p�X�y�[�X�`��
void CFigure_ZenSpace::DispSpace( CGraphics& gr, DispPos* pDispPos, CEditView* pcView, bool bTrans ) const
{
	//�N���b�s���O��`���v�Z�B��ʊO�Ȃ�`�悵�Ȃ�
	RECT rc;
	if(pcView->GetTextArea().GenerateClipRect(&rc,*pDispPos,2))
	{
		//�`��
		const wchar_t* szZenSpace =
			CTypeSupport(pcView,COLORIDX_ZENSPACE).IsDisp()?L"��":L"�@";
		::ExtTextOutW_AnyBuild(
			gr,
			pDispPos->GetDrawPos().x,
#if REI_LINE_CENTERING
			(pcView->m_pTypeData->m_nLineSpace/2) +
#endif  // rei_
			pDispPos->GetDrawPos().y,
			ExtTextOutOption() & ~(bTrans? ETO_OPAQUE: 0),
			&rc,
			szZenSpace,
			wcslen(szZenSpace),
			pcView->GetTextMetrics().GetDxArray_AllZenkaku()
		);
	}

	//�ʒu�i�߂�
	pDispPos->ForwardDrawCol(2);
}