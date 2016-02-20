#include "stdafx.h"
#include "BPTreeStatistics.h"

namespace embDB
{
	CBPTreeStatistics::CBPTreeStatistics()
	{
		Clear();
	}
	CBPTreeStatistics::~CBPTreeStatistics()
	{

	}

	void CBPTreeStatistics::CreateNode(bool bLeaf)
	{
		if(bLeaf)
			m_nCreateLeafNodes++;
		else
			m_nCreateInnerNodes++;

	}
	void CBPTreeStatistics::LoadNode(bool bLeaf)
	{
		if(bLeaf)
			m_nLoadLeafNodes++;
		else
			m_nLoadInnerNodes++;
	}
	void CBPTreeStatistics::DeleteNode(bool bLeaf)
	{
		if(bLeaf)
			m_nDeleteLeafNodes++;
		else
			m_nDeleteInnerNodes++;
	}

	void CBPTreeStatistics::SaveNode(bool bLeaf)
	{
		if(bLeaf)
			m_nSaveLeafNodes++;
		else
			m_nSaveInnerNodes++;

	}

	void CBPTreeStatistics::Clear()
	{
		m_nCreateLeafNodes = 0;
		m_nCreateInnerNodes = 0;
		m_nLoadLeafNodes = 0;
		m_nLoadInnerNodes = 0;
		m_nDeleteLeafNodes = 0;
		m_nDeleteInnerNodes = 0;
		m_nSaveLeafNodes = 0;
		m_nSaveInnerNodes = 0;
	}

	uint32 CBPTreeStatistics::GetCreateNode(bool bLeaf) const
	{
		if(bLeaf)
			return m_nCreateLeafNodes;
		else
			return m_nCreateInnerNodes;
	}
	uint32 CBPTreeStatistics::GetLoadNode(bool bLeaf) const
	{
		if(bLeaf)
			return m_nLoadLeafNodes;
		else
			return m_nLoadInnerNodes;
	}
	uint32 CBPTreeStatistics::GetDeleteNode(bool bLeaf) const
	{
		if(bLeaf)
			return m_nDeleteLeafNodes;
		else
			return m_nDeleteInnerNodes;
	}

	uint32 CBPTreeStatistics::GetSaveNode(bool bLeaf) const
	{
		if(bLeaf)
			return m_nSaveLeafNodes;
		else
			return m_nSaveInnerNodes;
	}
}