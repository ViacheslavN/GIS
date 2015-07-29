#ifndef _LIB_GIS_ENGINE_COMMON_SIMPLE_ENUM_
#define _LIB_GIS_ENGINE_COMMON_SIMPLE_ENUM_

#include "Common.h"

namespace GisEngine
{
	namespace Common
	{
			template <class TDataType, class TInterface>
			class CSimpleEnum : public TInterface
			{
				public:
					CSimpleEnum() : m_nIdx(0){}
					~CSimpleEnum(){}

					virtual void reset()
					{
						m_nIdx = 0;
					}
					virtual bool next(TDataType** pObj)
					{
						if(!pObj)
							return false;
						if(m_nIdx == m_objects.size())
						{
							*pObj = NULL;
							return false;
						}
						*pObj = m_objects[m_nIdx];
						m_nIdx++;
						return true;
					}
					virtual void add(TDataType *pObj)
					{
						m_objects.push_back(pObj);
					}
					virtual void clear()
					{
						m_objects.clear();
						m_nIdx = 0;
					}
				private:
					std::vector<TDataType*> m_objects;
					uint32 m_nIdx;
			};
	}
}
#endif