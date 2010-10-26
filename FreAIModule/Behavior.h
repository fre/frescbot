#pragma once

#include <windows.h>

#include <map>

class Behavior
{
public:
	Behavior(int parentId, std::map<const char*, double>* map = 0);
	Behavior(const Behavior& copy);
	const Behavior& operator=(const Behavior& right);

	// Getters and setters
	int getId() const;
	void setId(int id);
	double getValue(const char* name);
	void setValue(const char* name, double value);
	double operator[](const char *name);

private:
	int _id;
	int _parentId;
	std::map<const char*, double>	_map;
};
