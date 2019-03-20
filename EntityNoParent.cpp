#include "sde.h"
#include <algorithm>

namespace sde
{
	void EntityNoParent::setAllComponentsActive(bool b)
	{
		for (auto &up : m_component)
			up->setActive(b);
	}

	void EntityNoParent::initializeAllComponents()
	{
		for (auto &up : m_component)
			up->initialize();
	}

	void EntityNoParent::addTag(const std::string &tag)
	{
		m_tag.emplace_back(tag);
	}

	bool EntityNoParent::hasTag(const std::string &tag) const
	{
		auto it = std::find(std::begin(m_tag), std::end(m_tag), tag);
		if (it != std::end(m_tag)) return true;
		return false;
	}

	void EntityNoParent::removeTag(const std::string &tag)
	{
		auto it = std::find(std::begin(m_tag), std::end(m_tag), tag);
		if (it != std::end(m_tag)) m_tag.erase(it);
	}

	const std::vector<std::string> &EntityNoParent::getTags()
	{
		return m_tag;
	}
}