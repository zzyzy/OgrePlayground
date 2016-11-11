#ifndef __PathFinding_h_
#define __PathFinding_h_

#include <vector>
#include <algorithm>

#include "OgreRoot.h"

#include "Graph.h"
#include "NodeRecord.h"
#include "Connection.h"

#define VERY_LARGE_VALUE 1000000

class PathFinding
{
private:
	NodeRecord	mNodeRecords[TOTAL_NODES];
	bool		mOpenList[TOTAL_NODES];
	bool		mClosedList[TOTAL_NODES];
	Connection	mConnections[8];

public:
	PathFinding();
	~PathFinding();

	void reset();
	void clearConnections();

	bool Dijkstra(int startNode, int goalNode, Graph& pathFindingGraph, std::vector<int> &path);
	bool AStar(int startNode, int goalNode, Graph& pathFindingGraph, std::deque<int> &path);
};

#endif