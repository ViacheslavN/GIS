#ifndef GIS_ENGINE_GEOMETRY_I_SPATIAL_REFERENCE_PROJ4_H_
#define GIS_ENGINE_GEOMETRY_I_SPATIAL_REFERENCE_PROJ4_H_

#include "Geometry.h"
#include "CommonLibrary/alloc_t.h"

namespace GisEngine
{
	namespace GisGeometry
	{
		enum eSPRefParamType
		{
			eSPRefTypePRJFilePath = 1,
			eSPRefTypePRJ4String = 2
		};


		class CSpatialReferenceProj4 : public ISpatialReference
		{
		public:
			 typedef void* Handle;

			CSpatialReferenceProj4(const CommonLib::str_t& prj4Str, eSPRefParamType paramType= eSPRefTypePRJ4String, CommonLib::alloc_t *pAlloc = NULL);
			CSpatialReferenceProj4(int prjCode, CommonLib::alloc_t *pAlloc = NULL);
			CSpatialReferenceProj4(Handle hHandle, CommonLib::alloc_t *pAlloc = NULL);
			CSpatialReferenceProj4(const GisBoundingBox& bbox, CommonLib::alloc_t *pAlloc = NULL);
			 
			virtual ~CSpatialReferenceProj4();

			virtual bool  IsValid();
			virtual void*  GetHandle();
			virtual bool Project(ISpatialReference* destSpatRef, CommonLib::CGeoShape* pShape);
			virtual bool Project(ISpatialReference* destSpatRef, GisBoundingBox& bbox);
			virtual bool Project(ISpatialReference *destSpatRef, GisXYPoint* pPoint);

			virtual const CommonLib::str_t& GetProjectionString() const;
			virtual int   GetProjectionCode() const;
			virtual bool  IsProjection() const;
			virtual bool IsEqual(ISpatialReference* pSpatRef) const;

			virtual bool save(CommonLib::IWriteStream *pStream) const;
			virtual bool load(CommonLib::IReadStream *pStream);

			virtual bool save(GisCommon::IXMLNode* pXmlNode) const;
			virtual bool load(GisCommon::IXMLNode* pXmlNode);
		private:
			void CreateProjection();
			void PrepareGeometries();
			void PreTransform(CommonLib::CGeoShape *shp) const;
			bool IsEqual(CSpatialReferenceProj4* pSpRef) const;
			void TestBounds(CommonLib::CGeoShape *geom) const;
			void PrepareGeometry(CommonLib::CGeoShape *pShp, double left_meridian, double bottom_parallel, double right_meridian, double top_parallel);
			void PrepareCutMeridian(double cut_meridian, double bottom_parallel, double top_parallel);
			void PrepareBoundShape(const GisBoundingBox& bbox);
			void DensifyBoundBox(CommonLib::CGeoShape *shp, const GisBoundingBox &bbox, int precision = 10) const;
		private:
			Handle m_prjHandle;
			int m_prjCode;
			CommonLib::str_t m_prj4Str;

			CommonLib::CGeoShape m_LeftShp;
			CommonLib::CGeoShape m_RightShp; 
			CommonLib::CGeoShape m_BoundShape; 
			GisBoundingBox m_cutMeridian;
			GisBoundingBox m_fullExtent;
			
			mutable size_t m_nBufferSize;
			mutable double *m_pBufferX;
			mutable double *m_pBufferY;
			mutable double *m_pBufferZ;

			CommonLib::alloc_t* m_pAlloc;
			CommonLib::simple_alloc_t m_alloc;
			ITopologicalOperatorPtr m_pTopoOp;
		};
	}
}
#endif