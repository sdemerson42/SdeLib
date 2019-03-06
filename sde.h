#pragma once
#include <vector>
#include <memory>
#include <algorithm>
#include <typeindex>
#include <map>

namespace sde
{

	/* EventBase - A base struct that Events must inherit from. These Events
	can be handled by any clas that inherits from EventHandler.
	*/

	struct EventBase
	{
		virtual ~EventBase()
		{}
	};

	/* ISystem - Interface class for simulation systems.
	*/

	class ISystem
	{
	public:
		virtual void execute() = 0;
	};

	/* ComponentBase - Base class for Components to be held by Entities.
	*/

	class ComponentBase
	{
	public:
		virtual ~ComponentBase()
		{}
		inline void setActive(bool b)
		{
			m_active = b;
		}
		inline bool active() const
		{
			return m_active;
		}
	private:
		bool m_active;
	};

	/* AutoList - A base class template to simplify iteration through
	objects of the same type by allowing them to add a reference
	to a static vector at construction time.
	*/

	template<typename T>
	class AutoList
	{
	public:
		AutoList()
		{
			m_ref.push_back(static_cast<T *>(this));
		}
		virtual ~AutoList()
		{
			auto p = static_cast<T *>(this);
			auto it = std::find(std::begin(m_ref), std::end(m_ref), p);
			if (it != std::end(m_ref)) m_ref.erase(it);
		}
		static auto size()
		{
			return m_ref.size();
		}
		static T *get(int index)
		{
			return m_ref[index];
		}
	private:
		static std::vector<T *> m_ref;
	};

	template<typename T>
	std::vector<T *> AutoList<T>::m_ref;

	/* Entity - Basic Component-holding class. Components should be
	worked on by systems inheriting from ISystem.
	*/

	class Entity : public AutoList<Entity>
	{
	public:
		Entity()
		{}
		~Entity()
		{}
		Entity(const Entity &other) = delete;
		Entity(Entity &&other) = delete;
		Entity &operator=(const Entity &other) = delete;
		Entity &operator=(Entity &&other) = delete;

		template<typename T, typename ...Args>
		void addComponent(const Args &...args)
		{
			m_component.push_back(std::make_unique<T>(args...));
		}
		template<typename T>
		T *getComponent() const
		{
			std::type_index ti{ typeid(T) };

			auto it = std::find_if(std::begin(m_component), std::end(m_component), [&](const std::unique_ptr<ComponentBase> &up)
			{
				return std::type_index{ typeid(*up.get()) } == ti;
			});

			if (it != std::end(m_component)) return static_cast<T *>(it->get());
			return nullptr;
		}
		template<typename T>
		void removeComponent()
		{
			std::type_index ti{ typeid(T) };

			auto it = std::find_if(std::begin(m_component), std::end(m_component), [&](const std::unique_ptr<ComponentBase> &up)
			{
				return std::type_index{ typeid(*up.get()) } == ti;
			});

			if (it != std::end(m_component)) m_component.erase(it);
		}
	private:
		std::vector<std::unique_ptr<ComponentBase>> m_component;
	};

	/* EventSystem - A group of classes to assist in simple event passing from one
	instance to another. Events must inherit from the EventBase struct.
	*/

	template<typename T, typename ET>
	using MFunc = void(T::*)(ET *);

	class IFuncWrapper
	{
	public:
		virtual void call(const EventBase *) = 0;
	};

	template<typename T, typename ET>
	class FuncWrapper : public IFuncWrapper
	{
	public:
		FuncWrapper(T *caller, MFunc<T, ET> func) :
			m_caller{ caller }, m_func{ func }
		{}
		void call(const EventBase *evnt) override
		{
			(m_caller->*m_func)(static_cast<const ET*>(evnt));
		}
	private:
		T *m_caller;
		MFunc<T, ET> m_func;
	};

	class EventHandler
	{
	public:
		virtual ~EventHandler();
		template<typename T, typename ET>
		void registerFunc(T *caller, MFunc<T, ET> func)
		{
			std::type_index ti{ typeid(ET) };
			m_funcMap[ti] = std::make_shared<FuncWrapper<T, ET>>(caller, func);
			m_receiverMap[ti].emplace_back(caller);
		}
		void handleEvent(EventBase *evnt);
		void broadcast(EventBase *evnt);
	private:
		std::map<std::type_index, std::shared_ptr<IFuncWrapper>> m_funcMap;
		static std::map<std::type_index, std::vector<EventHandler *>> m_receiverMap;
	};
}