#include "stdafx.h"
#include "Layers.h"

namespace GisEngine
{
	namespace Cartography
	{
		CLayers::CLayers()
		{

		}
		CLayers::~CLayers()
		{

		}

		void CLayers::SetOnRemoveAllLayers(OnRemoveAllLayers* pFunck, bool bAdd)
		{
			if(bAdd)
				OnContentsClearedEvent += pFunck;
			else
				OnContentsClearedEvent -= pFunck;
		}
		void CLayers::SetOnLayerAdded(OnLayerAdded* pFunck, bool bAdd)
		{
			if(bAdd)
				OnLayerAddedEvent += pFunck;
			else
				OnLayerAddedEvent -= pFunck;
		}
		void CLayers::SetOnLayerRemove(OnLayerRemove* pFunck, bool bAdd)
		{
			if(bAdd)
				OnLayerDeletedEvent += pFunck;
			else
				OnLayerDeletedEvent -= pFunck;
		}
		void CLayers::SetOnLayerMoved(OnLayerMoved* pFunck, bool bAdd)
		{
			if(bAdd)
				OnLayerMovedEvent += pFunck;
			else
				OnLayerMovedEvent -= pFunck;
		}
		// ILayers
		int CLayers::GetLayerCount() const
		{
			CommonLib::CSSection::scoped_lock lock(mutex_);

			return (int)layers_.size();
		}

		ILayerPtr CLayers::GetLayer(int index) const
		{
			CommonLib::CSSection::scoped_lock lock(mutex_);

			return layers_[index];
		}

		void CLayers::AddLayer(ILayer* layer)
		{
			if (!layer)
			 return;

			RemoveLayer(layer);  
			{
				CommonLib::CSSection::scoped_lock lock(mutex_);
				layers_.push_back(ILayerPtr(layer));
			}
			OnLayerAddedEvent.fire(this, layer);
		}

		void CLayers::InsertLayer(ILayer* layer, int index)
		{
			MoveLayer(layer, index);
		}

		void CLayers::RemoveLayer(ILayer* layer)
		{
			CommonLib::CSSection::scoped_lock  lock(mutex_);

			ILayerPtr layer2(layer);
			std::vector<ILayerPtr>::iterator it;

			for(it = layers_.begin(); it != layers_.end(); ++it)
			{
				if((*it) == layer2)
				{
					layers_.erase(it);
					lock.unlock();
					OnLayerDeletedEvent.fire(this,  layer2.get());
					return;
				}
			}

			
		}

		void CLayers::RemoveAllLayers()
		{
			{
				CommonLib::CSSection::scoped_lock  lock(mutex_);
				layers_.clear();
			}

			OnContentsClearedEvent.fire(this);
		}

		void CLayers::MoveLayer(ILayer* layer, int index)
		{
			if (!layer)
			{
				return;
			}

			RemoveLayer(layer);

			{
				CommonLib::CSSection::scoped_lock lock(mutex_);
	
				if(index > (int)layers_.size() - 1)
					layers_.push_back(ILayerPtr(layer));
				else
					layers_.insert(layers_.begin() + index, ILayerPtr(layer));
			}

			OnLayerMovedEvent.fire(this, layer, index);
		}
	}
}