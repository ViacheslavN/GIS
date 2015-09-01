#ifndef GIS_ENGINE_LAYERS_H_
#define GIS_ENGINE_LAYERS_H_

#include "Cartography.h"

namespace GisEngine
{
	namespace Cartography
	{
		class   CLayers : public ILayers
		{
		public:
			CLayers();
			virtual ~CLayers();

		private:
			CLayers(const CLayers&);
			CLayers& operator=(const CLayers&);

		public:
			// ILayers
			virtual int       GetLayerCount() const;
			virtual ILayerPtr GetLayer(int index) const;
			virtual void      AddLayer(ILayer* layer);
			virtual void      InsertLayer(ILayer* layer, int index);
			virtual void      RemoveLayer(ILayer* layer);
			virtual void      RemoveAllLayers();
			virtual void      MoveLayer(ILayer* layer, int index);

			virtual void SetOnRemoveAllLayers(OnRemoveAllLayers* pFunck, bool bAdd);
			virtual void SetOnLayerAdded(OnLayerAdded* pFunck, bool bAdd);
			virtual void SetOnLayerRemove(OnLayerRemove* pFunck, bool bAdd);
			virtual void SetOnLayerMoved(OnLayerMoved* pFunck, bool bAdd);

		private:
			std::vector<ILayerPtr> layers_;

			mutable CommonLib::CSSection mutex_;

			CommonLib::Event1<ILayers*>					OnContentsClearedEvent;
			CommonLib::Event2<ILayers*, ILayer*>        OnLayerAddedEvent;
			CommonLib::Event2<ILayers*, ILayer*>        OnLayerDeletedEvent;
			CommonLib::Event3<ILayers*, ILayer*, int>   OnLayerMovedEvent;
		};
	}
}

#endif