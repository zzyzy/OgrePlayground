#include "Connection.h"

Connection::Connection()
{}

Connection::~Connection()
{}

void Connection::set(float c, int to, int from)
{
	mCost = c;	
	mToNode = to;	
	mFromNode = from;
}

void Connection::clear()
{
	mCost = -1;		// cannot have negative cost in path finding
	mToNode = -1;	// no such thing as -1 node
	mFromNode = -1;
}

float Connection::getCost()
{
	return mCost;
}

int Connection::getFromNode()
{
	return mFromNode;
}

int Connection::getToNode()
{
	return mToNode;
}
