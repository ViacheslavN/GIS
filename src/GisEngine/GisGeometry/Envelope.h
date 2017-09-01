#ifndef GIS_GEOMETRY_ENVELOPE_H
#define GIS_GEOMETRY_ENVELOPE_H

#include "Geometry.h"
 

namespace GisEngine
{
	namespace GisGeometry
	{

		class  CEnvelope : public IEnvelope
		{
	
		public:
			CEnvelope();
			CEnvelope(const GisBoundingBox& box, ISpatialReference* spatRef = NULL);
			virtual ~CEnvelope();
		public:
			// IEnvelope
			virtual const GisBoundingBox& GetBoundingBox() const;
			virtual ISpatialReferencePtr  GetSpatialReference() const;


	
			virtual GisBoundingBox& GetBoundingBox();
			virtual void SetBoundingBox(const GisBoundingBox& box);
			virtual void SetSpatialReference(ISpatialReference* spatRef);
			virtual void Expand(IEnvelope* envelope);
			virtual bool Intersect(IEnvelope* envelope);
			virtual void Project(ISpatialReference* spatRef);
			virtual CommonLib::shape_compress_params GetCompressParams() const;

			virtual IEnvelopePtr	clone() const; 
		private:
			GisBoundingBox m_box;
			ISpatialReferencePtr m_pSpatialRef;    
		};
	}
}

#endif