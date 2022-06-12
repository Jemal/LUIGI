#include <stdafx.hpp>

#include "layer.hpp"

namespace uie
{
	layer::layer(const std::string& name) : debug_name_(name)
	{

	}

	layer_stack::~layer_stack()
	{
		for (layer* layer : layers_)
		{
			layer->on_detach();
			delete layer;
		}
	}

	void layer_stack::push_layer(layer* layer)
	{
		layers_.emplace(layers_.begin() + layer_insert_index_, layer);
		layer_insert_index_++;
	}

	void layer_stack::push_overlay(layer* overlay)
	{
		layers_.emplace_back(overlay);
	}

	void layer_stack::pop_layer(layer* layer)
	{
		auto it = std::find(layers_.begin(), layers_.begin() + layer_insert_index_, layer);
		if (it != layers_.begin() + layer_insert_index_)
		{
			layer->on_detach();
			layers_.erase(it);
			layer_insert_index_--;
		}
	}

	void layer_stack::pop_overlay(layer* overlay)
	{
		auto it = std::find(layers_.begin() + layer_insert_index_, layers_.end(), overlay);
		if (it != layers_.end())
		{
			overlay->on_detach();
			layers_.erase(it);
		}
	}
}