#include "NodeRecord.h"

NodeRecord::NodeRecord()
{
	initNodeRecord();
}

NodeRecord::~NodeRecord()
{}

void NodeRecord::initNodeRecord()
{
	mFromNode = -1;
	mCostSoFar = -1.0;
	mEstimatedTotalCost = -1.0;
}

int NodeRecord::getFromNode()
{
	return mFromNode;
}

float NodeRecord::getCostSoFar()
{
	return mCostSoFar;
}

float NodeRecord::getEstTotalCost()
{
	return mEstimatedTotalCost;
}

void NodeRecord::setFromNode(int node)
{
	mFromNode = node;
}

void NodeRecord::setCostSoFar(float cost)
{
	mCostSoFar = cost;
}

void NodeRecord::setEstTotalCost(float cost)
{
	mEstimatedTotalCost = cost;
}