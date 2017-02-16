/*!	@file
	@brief �ҏW����v�f

	@author Norio Nakatani
	@date 1998/06/09 �V�K�쐬
*/
/*
	Copyright (C) 1998-2001, Norio Nakatani

	This source code is designed for sakura editor.
	Please contact the copyright holder to use this code for other purpose.
*/

#include "StdAfx.h"
#include "COpe.h"
#include "mem/CMemory.h"// 2002/2/10 aroka


// COpe�N���X�\�z
COpe::COpe(EOpeCode eCode)
{
	assert( eCode != OPE_UNKNOWN );
	m_nOpe = eCode;					// ������

	m_ptCaretPos_PHY_Before.Set(CLogicInt(-1),CLogicInt(-1));	//�J�[�\���ʒu
	m_ptCaretPos_PHY_After.Set(CLogicInt(-1),CLogicInt(-1));	//�J�[�\���ʒu

}




/* COpe�N���X���� */
COpe::~COpe()
{
}

/* �ҏW����v�f�̃_���v */
void COpe::DUMP( void )
{
	DEBUG_TRACE( _T("\t\tm_nOpe                  = [%d]\n"), m_nOpe               );
	DEBUG_TRACE( _T("\t\tm_ptCaretPos_PHY_Before = [%d,%d]\n"), m_ptCaretPos_PHY_Before.x, m_ptCaretPos_PHY_Before.y   );
	DEBUG_TRACE( _T("\t\tm_ptCaretPos_PHY_After  = [%d,%d]\n"), m_ptCaretPos_PHY_After.x, m_ptCaretPos_PHY_After.y   );
	return;
}

/* �ҏW����v�f�̃_���v */
void CDeleteOpe::DUMP( void )
{
	COpe::DUMP();
	DEBUG_TRACE( _T("\t\tm_ptCaretPos_PHY_To     = [%d,%d]\n"), m_ptCaretPos_PHY_To.x, m_ptCaretPos_PHY_To.y );
	DEBUG_TRACE( _T("\t\tm_cOpeLineData.size         = [%d]\n"), m_cOpeLineData.size() );
	for( size_t i = 0; i < m_cOpeLineData.size(); i++ ){
		DEBUG_TRACE( _T("\t\tm_cOpeLineData[%d].nSeq         = [%d]\n"), m_cOpeLineData[i].nSeq );
		DEBUG_TRACE( _T("\t\tm_cOpeLineData[%d].cmemLine     = [%ls]\n"), m_cOpeLineData[i].cmemLine.GetStringPtr() );		
	}
	return;
}

/* �ҏW����v�f�̃_���v */
void CInsertOpe::DUMP( void )
{
	COpe::DUMP();
	DEBUG_TRACE( _T("\t\tm_cOpeLineData.size         = [%d]\n"), m_cOpeLineData.size() );
	for( size_t i = 0; i < m_cOpeLineData.size(); i++ ){
		DEBUG_TRACE( _T("\t\tm_cOpeLineData[%d].nSeq         = [%d]\n"), m_cOpeLineData[i].nSeq );
		DEBUG_TRACE( _T("\t\tm_cOpeLineData[%d].cmemLine     = [%ls]\n"), m_cOpeLineData[i].cmemLine.GetStringPtr() );		
	}
	return;
}

