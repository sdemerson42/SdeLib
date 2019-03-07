#include "sde.h"
#include <algorithm>

namespace sde
{
	void Entity::addTag(const std::string &tag)
	{
		m_tag.emplace_back(tag);
	}

	bool Entity::hasTag(const std::string &tag) const
	{
		auto it = std::find(std::begin(m_tag), std::end(m_tag), tag);
		if (it != std::end(m_tag)) return true;
		return false;
	}

	void Entity::removeTag(const std::string &tag)
	{
		auto it = std::find(std::begin(m_tag), std::end(m_tag), tag);
		if (it != std::end(m_tag)) m_tag.erase(it);
	}

	const std::vector<std::string> &Entity::getTags()
	{
		return m_tag;
	}
}