#pragma once

namespace uie
{
	class layer
	{
	public:
		layer(const std::string& name = "Layer");
		virtual ~layer() = default;

		virtual void on_attach() {}
		virtual void on_detach() {}
		virtual void on_update() {}
		virtual void on_render() {}

		const std::string& GetName() const { return debug_name_; }

	protected:
		std::string debug_name_;

	};

	class layer_stack
	{
	public:
		layer_stack() = default;
		~layer_stack();

		void push_layer(layer* layer);
		void push_overlay(layer* overlay);
		void pop_layer(layer* layer);
		void pop_overlay(layer* overlay);

		std::vector<layer*>::iterator begin() { return layers_.begin(); }
		std::vector<layer*>::iterator end() { return layers_.end(); }
		std::vector<layer*>::reverse_iterator rbegin() { return layers_.rbegin(); }
		std::vector<layer*>::reverse_iterator rend() { return layers_.rend(); }

		std::vector<layer*>::const_iterator begin() const { return layers_.begin(); }
		std::vector<layer*>::const_iterator end()	const { return layers_.end(); }
		std::vector<layer*>::const_reverse_iterator rbegin() const { return layers_.rbegin(); }
		std::vector<layer*>::const_reverse_iterator rend() const { return layers_.rend(); }
	private:
		std::vector<layer*> layers_;
		unsigned int layer_insert_index_ = 0;
	};
}