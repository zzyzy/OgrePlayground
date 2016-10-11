#include "PathFinding.h"

PathFinding::PathFinding()
{}

PathFinding::~PathFinding()
{}

void PathFinding::reset()
{
	for(int counter = 0; counter < TOTAL_NODES; counter++)
	{
		mNodeRecords[counter].initNodeRecord();
		mOpenList[counter] = false;
		mClosedList[counter] = false;

		clearConnections();
	}
}

void PathFinding::clearConnections()
{
	for(int counter = 0; counter < 8; counter++)
	{
		mConnections[counter].clear();
	}
}

bool PathFinding::Dijkstra(int startNode, int goalNode, Graph& pathFindingGraph, std::vector<int>& path)
{
	reset();
	path.clear();

	int nodesProcessed = 0;

	// set current node to start node
    int currentNode = startNode;
    int toNode;
    float lowestCostSoFar;

	mNodeRecords[startNode].setCostSoFar(0);
    //put start node in open list
    mOpenList[startNode] = true;

	while (currentNode != goalNode)
    {
		//process current node, get outgoing connections
		clearConnections();
        pathFindingGraph.getConnections(currentNode, mConnections);

		//trace all outgoing connections
        for (int counter = 0; counter < 8; counter++)
		{
			//check if valid connection, if not ignore
			if(mConnections[counter].getToNode() != -1)
			{
				toNode = mConnections[counter].getToNode();

                //just update node record if node is unvisited
                if(mNodeRecords[toNode].getFromNode() == -1 && !mClosedList[toNode])
                {
					//set the node record's connection
                    mNodeRecords[toNode].setFromNode(currentNode);
                    mNodeRecords[toNode].setCostSoFar(mNodeRecords[currentNode].getCostSoFar() + mConnections[counter].getCost());

                    mOpenList[toNode] = true;
                }

				//if node previously visited (either on open or closed list) check if better path found
                else
                {
					//if better path found, then update record, else leave it
                    if((mNodeRecords[currentNode].getCostSoFar() + mConnections[counter].getCost()) < mNodeRecords[toNode].getCostSoFar())
					{
						//change the node record's connection
						mNodeRecords[toNode].setFromNode(currentNode);
                        //change the node record's cost-so-far
                        mNodeRecords[toNode].setCostSoFar(mNodeRecords[currentNode].getCostSoFar() + mConnections[counter].getCost());

                        //if not found on open list, then must be in closed list
                        if(mOpenList[toNode] == false)
                        {
							//remove from closed list
                            mClosedList[toNode] = false;
                            //put node into open list
                            mOpenList[toNode] = true;
						}
					}
				}
			}
		}

		//finished processing current node, so remove from open list and put in closed list
        mOpenList[currentNode] = false;
        mClosedList[currentNode] = true;
        nodesProcessed++;

        //select next node to process
        lowestCostSoFar = VERY_LARGE_VALUE; // set to very large impossible value

		//go through open list
        for (int counter = 0; counter < TOTAL_NODES; counter++)
		{
			//if node is on open list
            if(mOpenList[counter] == true)
            {
				//check whether this node has lower estimated total cost
				if(mNodeRecords[counter].getCostSoFar() < lowestCostSoFar)
				{
					//set this as the current node and update current lowest estimated total cost
					currentNode = counter;
					lowestCostSoFar = mNodeRecords[counter].getCostSoFar();
				}
			}
		}

		//if nothing left in open list, this means goal node was not reachable
		if (lowestCostSoFar == VERY_LARGE_VALUE)
		{
			return false;
		}
	}

	currentNode = goalNode;
	path.push_back(currentNode);
	
	while(currentNode != startNode)
    {
		currentNode = mNodeRecords[currentNode].getFromNode();
        
		path.push_back(currentNode);
	}

	std::reverse(path.begin(), path.end());

	return true;
}

bool PathFinding::AStar(int startNode, int goalNode, Graph& pathFindingGraph, std::deque<int> &path)
{
	reset();
	path.clear();

	int nodesProcessed = 0;

	// set current node to start node
    int currentNode = startNode;
    int toNode;
	float heuristic;
    float lowestEstimatedTotalCost;

	mNodeRecords[startNode].setCostSoFar(0);
    //put start node in open list
    mOpenList[startNode] = true;

	while (currentNode != goalNode)
    {
		//process current node, get outgoing connections
		clearConnections();
        pathFindingGraph.getConnections(currentNode, mConnections);

		//trace all outgoing connections
        for (int counter = 0; counter < 8; counter++)
		{
			//check if valid connection, if not ignore
			if(mConnections[counter].getToNode() != -1)
			{
				toNode = mConnections[counter].getToNode();

                //just update node record if node is unvisited
                if(mNodeRecords[toNode].getFromNode() == -1 && !mClosedList[toNode])
                {
					//set the node record's connection
                    mNodeRecords[toNode].setFromNode(currentNode);
                    mNodeRecords[toNode].setCostSoFar(mNodeRecords[currentNode].getCostSoFar() + mConnections[counter].getCost());

					Ogre::Vector3 position = pathFindingGraph.getPosition(goalNode);

					heuristic = position.distance(pathFindingGraph.getPosition(toNode));

					mNodeRecords[toNode].setEstTotalCost(mNodeRecords[toNode].getCostSoFar() + heuristic);

                    mOpenList[toNode] = true;
                }

				//if node previously visited (either on open or closed list) check if better path found
                else
                {
					//if better path found, then update record, else leave it
                    if((mNodeRecords[currentNode].getCostSoFar() + mConnections[counter].getCost()) < mNodeRecords[toNode].getCostSoFar())
					{
						//change the node record's connection
						mNodeRecords[toNode].setFromNode(currentNode);
                        //change the node record's cost-so-far
                        mNodeRecords[toNode].setCostSoFar(mNodeRecords[currentNode].getCostSoFar() + mConnections[counter].getCost());

						Ogre::Vector3 position = pathFindingGraph.getPosition(goalNode);

						heuristic = position.distance(pathFindingGraph.getPosition(toNode));

						mNodeRecords[toNode].setEstTotalCost(mNodeRecords[toNode].getCostSoFar() + heuristic);
						
                        //if not found on open list, then must be in closed list
                        if(mOpenList[toNode] == false)
                        {
							//remove from closed list
                            mClosedList[toNode] = false;
                            //put node into open list
                            mOpenList[toNode] = true;
						}
					}
				}
			}
		}

		//finished processing current node, so remove from open list and put in closed list
        mOpenList[currentNode] = false;
        mClosedList[currentNode] = true;
        nodesProcessed++;

        //select next node to process
		lowestEstimatedTotalCost = VERY_LARGE_VALUE; // set to very large impossible value

		//go through open list
        for (int counter = 0; counter < TOTAL_NODES; counter++)
		{
			//if node is on open list
            if(mOpenList[counter] == true)
            {
				//check whether this node has lower estimated total cost
				if(mNodeRecords[counter].getEstTotalCost() < lowestEstimatedTotalCost)
				{
					//set this as the current node and update current lowest estimated total cost
					currentNode = counter;
					lowestEstimatedTotalCost = mNodeRecords[counter].getEstTotalCost();
				}
			}
		}

		//if nothing left in open list, this means goal node was not reachable
		if (lowestEstimatedTotalCost == VERY_LARGE_VALUE)
		{
			return false;
		}
	}

	currentNode = goalNode;
	path.push_back(currentNode);
	
	while(currentNode != startNode)
    {
		currentNode = mNodeRecords[currentNode].getFromNode();
        
		path.push_back(currentNode);
	}

	std::reverse(path.begin(), path.end());

	return true;
}