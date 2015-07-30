#include "stdafx.h"
#include "SpatialReferenceProj4.h"
#include "proj4_lib.h"
#include "CodeProj.h"
#include "CommonLibrary/File.h"
namespace GisEngine
{
	namespace Geometry
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

		CSpatialReferenceProj4::CSpatialReferenceProj4(const CommonLib::str_t& prj4Str, eSPRefParamType paramType) :
			m_prjCode(0)
			,m_hHandle(0)
		{
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
		CSpatialReferenceProj4::CSpatialReferenceProj4(int prjCode) :
		m_prjCode(prjCode)
			,m_hHandle(0)
		{
			CreateProjection();
		}
		CSpatialReferenceProj4::CSpatialReferenceProj4(Handle hHandle) :
		m_prjCode(0)
			,m_hHandle(hHandle)
		{

		}
		CSpatialReferenceProj4::CSpatialReferenceProj4(const GisBoundingBox& bbox): m_prjCode(0)
			,m_hHandle(0)
		{
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

		}



		bool  CSpatialReferenceProj4::IsValid()
		{
			 return m_hHandle != 0;
		}
		void*  CSpatialReferenceProj4::GetHandle()
		{
			return m_hHandle;
		}
		bool CSpatialReferenceProj4::Project(ISpatialReference* destSpatRef, CommonLib::CGeoShape* pShape)
		{
			return true;
		}
		bool CSpatialReferenceProj4::Project(ISpatialReference* destSpatRef, GisBoundingBox& bbox)
		{
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

			bool is_latlong = pj_is_latlong((PJ*)m_hHandle) != 0;
			double koef = is_latlong ? DEG_TO_RAD : 1.0;

			point_sp_x = pPoint->x*koef;
			point_sp_y = pPoint->y*koef;

			int ret = pj_transform((PJ*)m_hHandle, pj, 1, 1, &point_sp_x, &point_sp_y, 0);
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

		void CSpatialReferenceProj4::save(CommonLib::IWriteStream *pStream) const
		{
			pStream->write(m_prj4Str);
		}
		void CSpatialReferenceProj4::load(CommonLib::IReadStream *pStream)
		{
			pStream->read(m_prj4Str);
		}


		void CSpatialReferenceProj4::CreateProjection()
		{
			if (m_hHandle)
				pj_free((PJ*)m_hHandle);

			if(!m_prj4Str.isEmpty())
			{
				  m_hHandle = pj_init_plus(m_prj4Str.cstr());
				  if(m_hHandle != 0)
				  {
					  PrepareGeometries();
					  return;
				  }
			}

			if(m_prjCode != 0)
			{
				m_prj4Str = CodeToProj4Str(m_prjCode);
				m_hHandle = pj_init_plus(m_prj4Str.cstr());
				if(m_hHandle != 0)
				{
					PrepareGeometries();
				}
			}
		

		}


		void CSpatialReferenceProj4::PrepareGeometries()
		{
			if(pj_is_latlong((PJ*)m_hHandle))
				return;

			double cut_meridian = get_cut_meridian((PJ*)m_hHandle);
			if(cut_meridian == HUGE_VAL)
				return;

			double bottom_parallel;
			double top_parallel;

			get_parallel_range((PJ*)m_hHandle, &bottom_parallel, &top_parallel);

			PrepareGeometry(&m_LeftShp, cut_meridian - 359.9, bottom_parallel, cut_meridian - 0.01, top_parallel);
			PrepareGeometry(&m_RightShp, cut_meridian + 0.01, bottom_parallel, cut_meridian + 359.9, top_parallel);
			PrepareCutMeridian(cut_meridian, bottom_parallel, top_parallel);

			GisBoundingBox rbbox;
		//	m_RightShp.bbox(rbbox);
			PrepareBoundShape(rbbox);
			GisBoundingBox lbbox;
		//	m_LeftShp.bbox(lbbox);
			PrepareBoundShape(lbbox);
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
			/*for(int i = 0, sz = (int)shp.pointCount(); i < sz; i++)
			{
				projUV pnt;
				pnt.u = shp.ptX(i) * DEG_TO_RAD;
				pnt.v = shp.ptY(i) * DEG_TO_RAD;
				pnt = pj_fwd(pnt, (PJ*)handle_);
				shp.ptX(i) = pnt.u;
				shp.ptY(i) = pnt.v;
			}

			shp.calcBBox();
			shp.finishExternalChanges();*/
			m_BoundShape = shp;
		}
		bool CSpatialReferenceProj4::IsEqual(CSpatialReferenceProj4* ref) const
		{
			if(!ref)
				return false;

			std::vector<CommonLib::str_t> orgParams;
			std::vector<CommonLib::str_t> clnParams;

			PJ* orgPrj = (PJ*)m_hHandle;
			PJ* clnPrj = (PJ*)(ref->GetHandle());
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

			//*pShp = geom;
			//m_fullExtent.expand(geom.bbox());
		}

		void CSpatialReferenceProj4::DensifyBoundBox(CommonLib::CGeoShape *pShp, const GisBoundingBox &bbox, int precision) const
		{
			const int pntCount = precision * 4;

			double w = bbox.xMax - bbox.xMin;
			double h = bbox.yMax - bbox.yMin;

			w /= precision;
			h /= precision;

		/*	shp->create(decore::shape_type_polygon, pntCount + 1, 1);
			GisXYPoint* xy = shp->getXYs();

			for(int i = 0; i < precision; i++)
			{
				xy[i + 0].x = bbox.xMin + w * i;
				xy[i + 0].y = bbox.yMin;
				xy[i + precision].x = bbox.xMax;
				xy[i + precision].y = bbox.yMin + h * i;
				xy[i + 2*precision].x = bbox.xMax - w * i;
				xy[i + 2*precision].y = bbox.yMax;
				xy[i + 3*precision].x = bbox.xMin;
				xy[i + 3*precision].y = bbox.yMax - h * i;
			}
			xy[pntCount].x = bbox.xMin;
			xy[pntCount].y = bbox.yMin;

			shp->partStart(0) = 0;
			shp->calcBBox();
			shp->finishExternalChanges();*/
		}
	}
}