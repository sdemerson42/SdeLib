#include "sde.h"
#include <algorithm>

namespace sde
{

	std::map<std::type_index, std::vector<EventHandler *>> EventHandler::m_receiverMap;

	EventHandler::~EventHandler()
	{
		for (auto &p : m_receiverMap)
		{
			auto rp = std::find(begin(p.second), end(p.second), this);
			if (rp != end(p.second)) p.second.erase(rp);
		}
	}

	void EventHandler::handleEvent(EventBase *evnt)
	{
		auto p = m_funcMap.find(std::type_index{ typeid(*evnt) });
		if (p != end(m_funcMap)) p->second->call(evnt);
	}

	void EventHandler::broadcast(EventBase *evnt)
	{
		std::type_index ti{ typeid(*evnt) };
		auto p = m_receiverMap.find(ti);
		if (p != end(m_receiverMap))
		{
			for (auto rp : p->second)
			{
				if (rp != this) rp->handleEvent(evnt);
			}
		}
	}

}