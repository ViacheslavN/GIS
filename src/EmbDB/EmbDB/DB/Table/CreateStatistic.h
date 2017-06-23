#pragma once
namespace embDB
{
	IFieldStatisticHolderPtr CreateStatisticHolder(eDataTypes dataType, const SStatisticInfo& si, CDatabase* pDB)
	{
		IFieldStatisticHolderPtr pStatistic;
		if (si.m_Statistic == stFullStatistic)
		{
			switch (dataType)
			{
			case dtInteger8:
				pStatistic = new TFieldStatisticCharHolder(pDB->getCommonAlloc(), si, pDB->getCheckCRC());
				break;
			case dtUInteger8:
				pStatistic = new TFieldStatisticByteHolder(pDB->getCommonAlloc(), si, pDB->getCheckCRC());
				break;
			case dtUInteger16:
				pStatistic = new TFieldStatisticUint16Holder(pDB->getCommonAlloc(), si, pDB->getCheckCRC());
				break;
			case dtInteger16:
				pStatistic = new TFieldStatisticInt16Holder (pDB->getCommonAlloc(), si, pDB->getCheckCRC());
				break;
			case dtInteger32:
				pStatistic = new TFieldStatisticInt32Holder(pDB->getCommonAlloc(), si, pDB->getCheckCRC());
				break;
			case dtUInteger32:
				pStatistic = new TFieldStatisticUint32Holder(pDB->getCommonAlloc(), si, pDB->getCheckCRC());
				break;
			case dtInteger64:
				pStatistic = new TFieldStatisticInt64Holder(pDB->getCommonAlloc(), si, pDB->getCheckCRC());
				break;
			case dtUInteger64:
				pStatistic = new TFieldStatisticUint64Holder(pDB->getCommonAlloc(), si, pDB->getCheckCRC());
				break;
			}
		}


		return pStatistic;
	}
}