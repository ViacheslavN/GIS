#include "stdafx.h"
#include "SpatialReferenceProj4.h"
#include "proj4_lib.h"
#include "CodeProj.h"
#include "CommonLibrary/File.h"


#include "ogr/ogr_spatialref.h"
#include "cpl/cpl_conv.h"
#include "CommonLibrary/GeoShape.h"

namespace GisEngine
{
	namespace GisGeometry
	{
		const wchar_t c_WGS84_ESRI_STRING[] = L"GEOGCS[\"GCS_WGS_1984\",DATUM[\"D_WGS_1984\",SPHEROID[\"WGS_1984\",6378137,298.257223563]],PRIMEM[\"Greenwich\",0],UNIT[\"Degree\",0.0174532925199433]]";
		const char c_WGS84_PROJ4_STRING[] = "+proj=longlat +ellps=WGS84 +datum=WGS84";

		double get_cut_meridian(PJ* pj)
		{
			// search for lon_0 param
			for(paralist *pl = pj->params; pl; pl = pl->next)
			{

				CommonLib::str_t param = pl->param;
				if(param.find(L"lon_0") >= 0)
				{
					int n = param.find(L"=");
					if(n > 0)
					{
						param = param.right(param.length() - n - 1);
						double val = atof(param.cstr());
						val += (val > 0) ? -180 : 180;
						return val;
					}
				}
			}
			return HUGE_VAL;
		}

		void get_parallel_range(PJ* pj, double* bottom_parallel, double* top_parallel)
		{
			// search for "proj" param
			CommonLib::str_t param;
			for(paralist *pl = pj->params; pl; pl = pl->next)
			{

				CommonLib::str_t val = pl->param;
				if(val.find(L"proj") >= 0)
				{
					int n = val.find(L"=");
					if(n > 0)
					{
						param = val.right(val.length() - n - 1);
						break;
					}
				}
			}
			if(!param.isEmpty())
			{
				if(param.equals(L"aea", false))
				{
					*bottom_parallel = -90.0;
					*top_parallel = 89.9;
				}
				else if(param.equals(L"lcc", false))
				{
					*bottom_parallel = -59.9;
					*top_parallel = 89.9;
				}
				else
				{
					*bottom_parallel = -89.9;
					*top_parallel = 89.9;
				}
			}
			else
			{
				*bottom_parallel = -89.9;
				*top_parallel = 89.9;
			}
		}

		CSpatialReferenceProj4::CSpatialReferenceProj4(const CommonLib::str_t& prj4Str, eSPRefParamType paramType, CommonLib::alloc_t *pAlloc) :
			m_prjCode(0)
			,m_prjHandle(0)
			,m_pAlloc(pAlloc)
			, m_pBufferX(0)
			, m_pBufferY(0)
			, m_pBufferZ(0)
		{

			if(!m_pAlloc)
				m_pAlloc = &m_alloc;
			if(paramType == eSPRefTypePRJ4String)
			{
				m_prj4Str = prj4Str;
				CreateProjection();
			}
			else
			{
				CommonLib::CFile file;
				file.openFile(prj4Str.cwstr(), CommonLib::ofmOpenExisting, CommonLib::arRead, CommonLib::smNoMode);
				if(file.isValid())
				{
					uint32 fsize = (uint32)file.getFileSize();
					std::vector<char> buf(fsize + 1);
					uint32 res = file.readFile(&buf[0], fsize); 
					file.closeFile();
					buf[fsize] = 0;

					if (res == fsize)
						m_prj4Str = &buf[0];
					CreateProjection();
				}
			}
			
		}
		CSpatialReferenceProj4::CSpatialReferenceProj4(int prjCode, CommonLib::alloc_t *pAlloc) :
		m_prjCode(prjCode)
			,m_prjHandle(0)
			,m_pAlloc(pAlloc)
			, m_pBufferX(0)
			, m_pBufferY(0)
			, m_pBufferZ(0)
		{
			if(!m_pAlloc)
				m_pAlloc = &m_alloc;
			CreateProjection();
		}
		CSpatialReferenceProj4::CSpatialReferenceProj4(Handle hHandle, CommonLib::alloc_t *pAlloc) :
		m_prjCode(0)
			,m_prjHandle(hHandle)
			,m_pAlloc(pAlloc)
			, m_pBufferX(0)
			, m_pBufferY(0)
			, m_pBufferZ(0)
		{
			if(!m_pAlloc)
				m_pAlloc = &m_alloc;
		}
		CSpatialReferenceProj4::CSpatialReferenceProj4(const GisBoundingBox& bbox, CommonLib::alloc_t *pAlloc): m_prjCode(0)
			,m_prjHandle(0)
			,m_pAlloc(pAlloc)
			, m_pBufferX(0)
			, m_pBufferY(0)
			, m_pBufferZ(0)
		{

			if(!m_pAlloc)
				m_pAlloc = &m_alloc;

			bool degrees = true;

			if(bbox.xMin < -1000.0 || bbox.xMin > 1000.0)
				degrees = false;
			if(bbox.xMax < -1000.0 || bbox.xMax > 1000.0)
				degrees = false;
			if(bbox.yMin < -1000.0 || bbox.yMin > 1000.0)
				degrees = false;
			if(bbox.yMax < -1000.0 || bbox.yMax > 1000.0)
				degrees = false;

			if(degrees)
			{
				m_prj4Str = c_WGS84_PROJ4_STRING;
				CreateProjection();
			}
		}
		CSpatialReferenceProj4::~CSpatialReferenceProj4()
		{
			if (m_prjHandle)
				pj_free((PJ*)m_prjHandle);
			if (m_pBufferX)
				m_pAlloc->free(m_pBufferX);
			if (m_pBufferY)
				m_pAlloc->free(m_pBufferY);
			if (m_pBufferZ)
				m_pAlloc->free(m_pBufferZ);
		}



		bool  CSpatialReferenceProj4::IsValid()
		{
			 return m_prjHandle != 0;
		}
		void*  CSpatialReferenceProj4::GetHandle()
		{
			return m_prjHandle;
		}
		bool CSpatialReferenceProj4::Project(ISpatialReference* destSpatRef, CommonLib::CGeoShape* pShape)
		{
			if (destSpatRef == NULL)
				return false;

			PJ *pj = (PJ*)destSpatRef->GetHandle();
			if (!pj)
				return false;

			if(pj_is_latlong((PJ*)m_prjHandle) && !pj_is_latlong((PJ*)pj))
				((CSpatialReferenceProj4*)destSpatRef)->PreTransform(pShape);

			if(!pj_is_latlong((PJ*)m_prjHandle) && pj_is_latlong((PJ*)pj))
				TestBounds(pShape);

			size_t pointCount = pShape->getPointCnt();
			double *pZs = pShape->getZs();
			CommonLib::GisXYPoint* pPt = pShape->getPoints();
			if (pointCount > m_nBufferSize)
			{

				m_pAlloc->free(m_pBufferX);
				m_pBufferX = (double*)m_pAlloc->alloc(sizeof(double) * pointCount);

	
				m_pAlloc->free(m_pBufferY);
				m_pBufferY = (double*)m_pAlloc->alloc(sizeof(double) * pointCount);

				m_nBufferSize = pointCount;
				if (pZs)
				{
					m_pAlloc->free(m_pBufferZ);
					m_pBufferZ = (double*)m_pAlloc->alloc(sizeof(double) * pointCount);
				}
			}

			if (!pZs && m_pBufferZ)
			{
				m_pAlloc->free(m_pBufferZ);
				m_pBufferZ = NULL;
			}

			
			bool is_latlong = pj_is_latlong((PJ*)m_prjHandle) != 0;
			double koef = is_latlong ? DEG_TO_RAD : 1.0;
			for (size_t idx = 0; idx < pointCount; ++idx)
			{
				m_pBufferX[idx] = pPt[idx].x*koef;
				m_pBufferY[idx] = pPt[idx].y*koef;
			
				if (m_pBufferZ && pZs)
					m_pBufferZ[idx] = pZs[idx];
			}

			int ret = pj_transform((PJ*)m_prjHandle, pj, (long)pointCount, 0, m_pBufferX, m_pBufferY, m_pBufferZ);
			if (ret == 0)
			{
				koef = pj_is_latlong(pj) ? RAD_TO_DEG : 1.0;
				std::set<int> badPoints;

				for (size_t idx = 0; idx < pointCount; ++idx)
				{
					if(m_pBufferX[idx] == HUGE_VAL || m_pBufferY[idx] == HUGE_VAL)
						badPoints.insert((int)idx);
				}

				if(badPoints.size() == pointCount)
				{
					pShape->create(CommonLib::shape_type_null);
					return false;
				}

				if(badPoints.size() == 0)
				{
					for (size_t idx = 0; idx < pointCount; ++idx)
					{
						pPt[idx].x = m_pBufferX[idx]*koef;
						pPt[idx].y = m_pBufferY[idx]*koef;
						if (pZs && m_pBufferZ)
							pZs[idx] = m_pBufferZ[idx];
					}
				}
				else
				{
					std::vector<long> newPartsStarts;
					std::vector<GisXYPoint> newPoints;
					std::vector<double> newZs;

					int nparts = (int)pShape->getPartCount();

					if(nparts != 0)
						newPartsStarts.reserve(nparts);
					newPoints.reserve(pointCount);
					newZs.reserve(pointCount);

					

					int npointall = 0;
					if(nparts == 0)
					{
						for(size_t npoint = 0; npoint < pointCount; ++npoint, ++npointall)
						{
							if(badPoints.find(npointall) != badPoints.end())
								continue;

							GisXYPoint p;
							p.x = m_pBufferX[npointall] * koef;
							p.y = m_pBufferY[npointall] * koef;
							newPoints.push_back(p);
							if(pZs && m_pBufferZ)
								newZs.push_back(m_pBufferZ[npointall]);
						}
					}
					else
					{
						uint32* npartsstarts = pShape->getParts();
						for(int npart = 0; npart < nparts; ++npart)
						{
							int npartpointcount;
							if(npart == nparts - 1)
								npartpointcount = (int)pointCount - npartsstarts[npart];
							else
								npartpointcount = npartsstarts[npart + 1] - npartsstarts[npart];

							bool pointIsFirstInPart = true;
						
						}
					}
					

					pShape->create(pShape->type(), newPoints.size(), newPartsStarts.size());
					pShape->setPoints((double*)&newPoints[0]);
					if(!newPartsStarts.empty())
						memcpy(pShape->getParts(), &newPartsStarts[0], newPartsStarts.size() * sizeof(long));
					if(pZs)
						pShape->setZs(&newZs[0]);
				}

				pShape->calcBB();
				return true;
			}

			return false;
		}
		bool CSpatialReferenceProj4::Project(ISpatialReference* destSpatRef, GisBoundingBox& bbox)
		{
			if(!destSpatRef)
				return false;
			CommonLib::CGeoShape geom;
			DensifyBoundBox(&geom, bbox);
			geom.calcBB();
 

			if(!CSpatialReferenceProj4::Project(destSpatRef, &geom))
				return false;

			geom.calcBB();
			bbox = geom.getBB();
			return true;

		}
		bool CSpatialReferenceProj4::Project(ISpatialReference *pDestSpatRef, GisXYPoint* pPoint)
		{
			if (!pDestSpatRef)
				return false;

			CSpatialReferenceProj4 *pDestSpatProj4 = dynamic_cast<CSpatialReferenceProj4*>(pDestSpatRef);
			if(!pDestSpatProj4)
				return false;

			if(IsEqual(pDestSpatProj4))
				return true;

			PJ *pj = (PJ*)((CSpatialReferenceProj4*)pDestSpatRef)->GetHandle();
			if (!pj)
				return false;

			double point_sp_x;
			double point_sp_y;

			bool is_latlong = pj_is_latlong((PJ*)m_prjHandle) != 0;
			double koef = is_latlong ? DEG_TO_RAD : 1.0;

			point_sp_x = pPoint->x*koef;
			point_sp_y = pPoint->y*koef;

			int ret = pj_transform((PJ*)m_prjHandle, pj, 1, 1, &point_sp_x, &point_sp_y, 0);
			if (ret)
				return false;

			koef = pj_is_latlong(pj) != 0 ? RAD_TO_DEG : 1.0;
			pPoint->x = point_sp_x * koef;
			pPoint->y = point_sp_y * koef;
			 return true;

		}

		const CommonLib::str_t& CSpatialReferenceProj4::GetProjectionString() const
		{
			return m_prj4Str;
		}
		int   CSpatialReferenceProj4::GetProjectionCode() const
		{
			return m_prjCode;
		}
		bool  CSpatialReferenceProj4::IsProjection() const
		{
			return true;
		}

		bool CSpatialReferenceProj4::save(CommonLib::IWriteStream *pStream) const
		{
			pStream->write(m_prj4Str);
			return true;
		}
		bool CSpatialReferenceProj4::load(CommonLib::IReadStream *pStream)
		{
			SAFE_READ_RES(pStream, m_prj4Str, 1)
			return true;
		}

		bool CSpatialReferenceProj4::save(GisCommon::IXMLNode* pXmlNode) const
		{
			pXmlNode->AddProperty(L"proj", )
			return true;
		}
		bool CSpatialReferenceProj4::load(GisCommon::IXMLNode* pXmlNode)
		{
			return true;
		}

		void CSpatialReferenceProj4::CreateProjection()
		{
			if (m_prjHandle)
				pj_free((PJ*)m_prjHandle);


			if(!m_prj4Str.isEmpty())
			{
				  m_prjHandle = pj_init_plus(m_prj4Str.cstr());
				  if(m_prjHandle != 0)
				  {
					  PrepareGeometries();
					  return;
				  }
				  else
				  {

					  OGRSpatialReference spatRef(m_prj4Str.cstr());
					  char* prj4String;
					  
					  if(spatRef.morphToESRI() != 0)
						   return;

					  if(spatRef.exportToProj4(&prj4String) != 0)
						   return;

					  m_prj4Str = prj4String;
					  CPLFree(prj4String);
					  m_prjHandle = pj_init_plus(m_prj4Str.cstr());
					  if(m_prjHandle != 0)
					  {
						  PrepareGeometries();
						  return;
					  }

				  }
			}

			if(m_prjCode != 0)
			{
				m_prj4Str = CodeToProj4Str(m_prjCode);
				m_prjHandle = pj_init_plus(m_prj4Str.cstr());
				if(m_prjHandle != 0)
				{
					PrepareGeometries();
				}
			}
		

		}
		bool CSpatialReferenceProj4::IsEqual(ISpatialReference* pSpRef) const
		{
			if(!pSpRef)
				return false;

			std::vector<CommonLib::str_t> orgParams;
			std::vector<CommonLib::str_t> clnParams;

			PJ* orgPrj = (PJ*)m_prjHandle;
			PJ* clnPrj = (PJ*)(pSpRef->GetHandle());
			if(orgPrj == clnPrj)
				return true;

			paralist* pl;
			pl = orgPrj->params;
			while(pl)
			{
				if(pl->used && strcmp(pl->param, "no_defs") != 0)
					orgParams.push_back(pl->param);
				pl = pl->next;
			}
			std::sort(orgParams.begin(), orgParams.end());

			pl = clnPrj->params;
			while(pl)
			{
				if(pl->used && strcmp(pl->param, "no_defs") != 0)
					clnParams.push_back(pl->param);
				pl = pl->next;
			}
			std::sort(clnParams.begin(), clnParams.end());
			if(orgParams.size() != clnParams.size())
				return false;

			for(int ind = 0; ind < (int)orgParams.size(); ++ind)
				if(orgParams[ind] != clnParams[ind])
					return false;

			return true;
		}

		void CSpatialReferenceProj4::PrepareGeometries()
		{
			if(pj_is_latlong((PJ*)m_prjHandle))
				return;

			double cut_meridian = get_cut_meridian((PJ*)m_prjHandle);
			if(cut_meridian == HUGE_VAL)
				return;

			double bottom_parallel;
			double top_parallel;

			get_parallel_range((PJ*)m_prjHandle, &bottom_parallel, &top_parallel);

			PrepareGeometry(&m_LeftShp, cut_meridian - 359.9, bottom_parallel, cut_meridian - 0.01, top_parallel);
			PrepareGeometry(&m_RightShp, cut_meridian + 0.01, bottom_parallel, cut_meridian + 359.9, top_parallel);
			PrepareCutMeridian(cut_meridian, bottom_parallel, top_parallel);

					
			PrepareBoundShape(m_RightShp.getBB());
			PrepareBoundShape(m_LeftShp.getBB());

			if(m_pTopoOp.get())
			{
				m_pTopoOp->ClearIntersect();
				m_pTopoOp->AddShapeToIntersect(&m_BoundShape);
			}

		}
		void CSpatialReferenceProj4::PrepareCutMeridian(double cut_meridian, double bottom_parallel, double top_parallel)
		{
			m_cutMeridian.type = CommonLib::bbox_type_normal;
			m_cutMeridian.xMin = cut_meridian - 0.01;
			m_cutMeridian.yMin = bottom_parallel;
			m_cutMeridian.xMax = cut_meridian + 0.01;
			m_cutMeridian.yMax = top_parallel;
		}
		void CSpatialReferenceProj4::PrepareBoundShape(const GisBoundingBox& bbox)
		{
			CommonLib::CGeoShape shp;
			GisBoundingBox bbox_ = bbox;

			const int precision = 20;
			if(bbox_.xMin < -180) bbox_.xMin = -180;
			if(bbox_.xMax > 180) bbox_.xMax = 180;
			bbox_.yMax -= 1;
			bbox_.yMin += 1;
			bbox_.xMax -= 0.01;
			bbox_.xMin += 0.01;


			DensifyBoundBox(&shp, bbox_, precision);

			GisXYPoint* pPt = shp.getPoints();
			for (size_t i = 0, sz = shp.getPointCnt(); i < sz; ++i)
			{
				projUV pnt;
				pnt.u = pPt[i].x * DEG_TO_RAD;
				pnt.v = pPt[i].y * DEG_TO_RAD;
				pnt = pj_fwd(pnt, (PJ*)m_prjHandle);

				pPt[i].x  = pnt.u;
				pPt[i].y = pnt.v;
			}
			//shp.calcBB();
			if(m_BoundShape.type() == CommonLib::shape_type_null)
			{
				m_BoundShape = shp;
				m_BoundShape.AddRef();
			}
			else
				m_BoundShape += shp;

			m_BoundShape.calcBB();
		}

		void CSpatialReferenceProj4::TestBounds(CommonLib::CGeoShape *pShape) const
		{
			if(m_BoundShape.type() == CommonLib::shape_type_null || m_pTopoOp.get() == NULL)
				return;

			CommonLib::IGeoShapePtr pRes = m_pTopoOp->Intersect(pShape);
			if(pRes.get())
				*pShape = *(CommonLib::CGeoShape*)pRes.get();

		}
		bool CSpatialReferenceProj4::IsEqual(CSpatialReferenceProj4* pSpRef) const
		{
			if(!pSpRef)
				return false;

			std::vector<CommonLib::str_t> orgParams;
			std::vector<CommonLib::str_t> clnParams;

			PJ* orgPrj = (PJ*)m_prjHandle;
			PJ* clnPrj = (PJ*)(pSpRef->GetHandle());
			if(orgPrj == clnPrj)
				return true;

			paralist* pl;
			pl = orgPrj->params;
			while(pl)
			{
				if(pl->used && strcmp(pl->param, "no_defs") != 0)
					orgParams.push_back(pl->param);
				pl = pl->next;
			}
			std::sort(orgParams.begin(), orgParams.end());

			pl = clnPrj->params;
			while(pl)
			{
				if(pl->used && strcmp(pl->param, "no_defs") != 0)
					clnParams.push_back(pl->param);
				pl = pl->next;
			}
			std::sort(clnParams.begin(), clnParams.end());
			if(orgParams.size() != clnParams.size())
				return false;

			for(int ind = 0; ind < (int)orgParams.size(); ++ind)
				if(orgParams[ind] != clnParams[ind])
					return false;

			return true;
		}

		void CSpatialReferenceProj4::PrepareGeometry(CommonLib::CGeoShape *pShp, double left_meridian, double bottom_parallel, double right_meridian, double top_parallel)
		{
			GisBoundingBox bbox;
			bbox.xMin = left_meridian;
			bbox.xMax = right_meridian;
			bbox.yMin = bottom_parallel;
			bbox.yMax = top_parallel;

			CommonLib::CGeoShape geom;
			DensifyBoundBox(&geom, bbox, 20);

			*pShp = geom;
			m_fullExtent.expand(geom.getBB());
		}

		void CSpatialReferenceProj4::DensifyBoundBox(CommonLib::CGeoShape *pShp, const GisBoundingBox &bbox, int precision) const
		{
			const int pntCount = precision * 4;

			double w = bbox.xMax - bbox.xMin;
			double h = bbox.yMax - bbox.yMin;

			w /= precision;
			h /= precision;

			pShp->create(CommonLib::shape_type_polygon, pntCount + 1, 0);
			GisXYPoint* pPt = pShp->getPoints();

			for(int i = 0; i < precision; i++)
			{
				pPt[i + 0].x = bbox.xMin + w * i;
				pPt[i + 0].y = bbox.yMin;
				pPt[i + precision].x = bbox.xMax;
				pPt[i + precision].y = bbox.yMin + h * i;
				pPt[i + 2*precision].x = bbox.xMax - w * i;
				pPt[i + 2*precision].y = bbox.yMax;
				pPt[i + 3*precision].x = bbox.xMin;
				pPt[i + 3*precision].y = bbox.yMax - h * i;
			}
			pPt[pntCount].x = bbox.xMin;
			pPt[pntCount].y = bbox.yMin;
		
			pShp->calcBB();
		
		}
	}
}