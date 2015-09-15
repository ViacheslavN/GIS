#ifndef GIS_ENGINE_GEO_DATABASE_DATASET_BASE_H
#define GIS_ENGINE_GEO_DATABASE_DATASET_BASE_H

#include "GeoDatabase.h"
namespace GisEngine
{
	namespace GeoDatabase
	{
		template< class I>
		class IDataSetBase : public I
		{
			public:
				IDataSetBase(IWorkspace *pWks) : m_DatasetType(dtUndefined), m_pWorkspace(pWks)
				{

				}

				virtual ~IDataSetBase()
				{

				}
				virtual eDatasetType  GetDatasetType() const
				{
					return m_DatasetType;
				}
				virtual IWorkspacePtr   GetWorkspace() const
				{
					return m_pWorkspace;
				}
				const CommonLib::CString&   GetDatasetName() const
				{
					return m_sDatasetName;
				}
				const CommonLib::CString&   GetDatasetViewName() const
				{
					return m_sDatasetViewName;
				}
			protected:
				IWorkspacePtr m_pWorkspace;
				eDatasetType m_DatasetType;
				CommonLib::CString m_sDatasetName;
				CommonLib::CString m_sDatasetViewName;
		};
	}

}

#endif