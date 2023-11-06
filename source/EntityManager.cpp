#include "EntityManager.h"
#include <algorithm>

EntityManager::EntityManager()
{
}

void EntityManager::removeDeadEntities(EntityVec& vec)
{	
	vec.erase(std::remove_if(vec.begin(), vec.end(), [](std::shared_ptr<Entity> e) { return !e->isActive(); }), vec.end());
}

void EntityManager::update()
{
	// Add entities from m_entitiesToAd the proper location(s)
	for (auto& e : m_entitiesToAdd)
	{
		m_entities.push_back(e);
		m_entityMap[e->m_tag].push_back(e);
	}

	m_entitiesToAdd.clear();
	
	// Remove dead entities from the vector of all entities
	removeDeadEntities(m_entities);

	// Remove dead entities from each vector in the entity map
	// C++17 way of iterating through key, value pair in a map
	for (auto & pair : m_entityMap)
	{
		removeDeadEntities(pair.second);
	}
}

std::shared_ptr<Entity> EntityManager::addEntity(const std::string& tag)
{
	auto entity = std::shared_ptr<Entity>(new Entity(m_totalEntities, tag));
	//auto entity = std::make_shared<Entity>(m_totalEntities, tag);
	m_entitiesToAdd.push_back(entity);
	return entity;
}
	

const EntityVec& EntityManager::getEntities()
{
	return m_entities;
}

const EntityVec& EntityManager::getEntities(const std::string& tag)
{
	return m_entityMap[tag];
}