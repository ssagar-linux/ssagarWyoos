
#include "driver.h"


Driver::Driver()
{
}

Driver::~Driver()
{
}

void Driver::Activate()
{
}

int Driver::Reset()
{
	return 0;
}

void Driver::Deactivate()
{
}





DriverManager::DriverManager()
{
	numDrivers = 0;
}

void DriverManager::AddDriver(Driver* pDriver)
{
	ppDrivers[numDrivers] = pDriver;
	numDrivers++;
}

void DriverManager::ActivateAll()
{
	for(int i = 0; i < numDrivers; i++)
		ppDrivers[i]->Activate();
}
