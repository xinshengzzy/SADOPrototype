//components.h
#ifndef COMPONENTS_H
#define COMPONENTS_H

//<brief. Given a particular node,the struct is used to record the metric from the origin node to the node,
// as well as through which node this given particular node could be reached.

//#include <vld.h>
#include <vector>
#include <iostream>

#define TOPO_CONFIG_FILE "./topology-config.txt"
#define PRODUCER_CONFIG_FILE "./producer-config.txt"
using namespace std;
typedef struct
{
	bool flag;
	int face;	//<@brief. Through which face the node identified by the struct could be reached. 
	//In fact, in this project, face is the synonyms of nexthop.
	int metric;	//<@brief. The distance to the node identified by this struct.
}PathInfo;

typedef struct FaceCost
{
	int face;
	float cost;
	bool operator<(const struct FaceCost& other) const
	{
		return cost < other.cost;
	}
} FaceCost;

typedef struct FaceMetric
{
	int router;	// In which router the FaceMetric information is constructed.
	vector<int> faces;	// The face associated with the dynamic FIB entry corresponding to the FaceMetric information.
	float metric;	// The metric associated with the dynamic FIB entry corresponding to the FaceMetric information.
	void print()
	{
		cout << "router = " << router << ", faces = (";
		for(vector<int>::iterator iter(faces.begin()), end(faces.end());
			iter != end; ++iter)
			cout << *iter << " ";
		cout << "), metric = " << metric << endl;
	}
	bool operator==(const struct FaceMetric& other) const
	{
		return router == other.router;
	}
}FaceMetric;

/**
<@brief. The struct is used to record how many types of Data packets in the content store, and how many Data packets there 
		are for each type.
*/
typedef struct ContentStoreStat
{
	string prefix;
	int count;
	bool operator==(const ContentStoreStat& other)
	{
		return prefix == other.prefix;
	}
} ContentStoreStat;

/**
<@brief. There will be a list of PitInfo in every PIT entry. Every instance of PitInfo corresponds to 
	a coming Interest packet.
*/
struct PitInfo
{
	int m_distance;
	int m_hopCount;
	int m_arrivalFace;
	int m_interestPacketId;
	bool operator == (const PitInfo& other) const
	{
		return m_arrivalFace == other.m_arrivalFace;
	}

	void operator = (const PitInfo& other)
	{
		m_arrivalFace = other.m_arrivalFace;
		m_hopCount = other.m_hopCount;
		m_distance = other.m_distance;
		m_interestPacketId = other.m_interestPacketId;
	}
	void print()
	{
		cout << "distance = " << m_distance << ", hopCount = " << m_hopCount << ", arrivalFace = " 
			<< m_arrivalFace << ", id = " << m_interestPacketId << endl;
	}
};
#endif
