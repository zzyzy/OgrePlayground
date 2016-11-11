#ifndef __NodeRecord_h_
#define __NodeRecord_h_

class NodeRecord
{
private:
	int mFromNode;
    float mCostSoFar;
    float mEstimatedTotalCost;

public:
    NodeRecord();
    ~NodeRecord();

	void initNodeRecord();

	int getFromNode();
	float getCostSoFar();
	float getEstTotalCost();

	void setFromNode(int node);
	void setCostSoFar(float cost);
	void setEstTotalCost(float cost);
};

#endif