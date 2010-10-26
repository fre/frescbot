#include "Behavior.h"

#include "Common.h"
#include "BehaviorManager.h"

Behavior::Behavior(int parentId, std::map<const char*, double>* map):
	_parentId(parentId)
{
	if (map)
		this->_map = *map;
	behaviorManager->registerBehavior(this);
}

Behavior::Behavior(const Behavior& copy)
{
	(*this) = copy;
}

const Behavior& Behavior::operator=(const Behavior& right)
{
	if (this != &right)
	{
		this->_id = right._id;
		this->_parentId = right._parentId;
		this->_map = right._map;
	}
	return (right);
}

int
Behavior::getId() const
{
	return (this->_id);
}

void
Behavior::setId(int id)
{
	this->_id = id;
}

double
Behavior::getValue(const char* name)
{
	Behavior* parent;

	if (this->_map.count(name))
		return (this->_map[name]);
	parent = behaviorManager->getBehavior(_parentId);
	if (parent)
		return (parent->getValue(name));
	DEBUG("Value " << name << " not found, return 0.");
	return (0);
}

void
Behavior::setValue(const char* name, double value)
{
	this->_map[name] = value;
}

double
Behavior::operator[](const char* name)
{
	return (getValue(name));
}
