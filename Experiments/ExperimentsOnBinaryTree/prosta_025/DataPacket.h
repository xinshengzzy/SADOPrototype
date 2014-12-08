// DataPacket.h
#ifndef DATA_PACKET_H
#define DATA_PACKET_H

//#include <vld.h>

#include <string>
#include <algorithm>
#include <list>

#include "components.h"
using namespace std;
class DataPacket
{
	public:
	enum Type{normal, nack, nocache, unknow};
	DataPacket() :
		m_name (""),
		m_payload (""),
		m_currentRouterDist (0),
		m_cachingRouterDist (0),
		m_size (0),
		m_arrivalFace(-1),
		m_type(unknow),
		m_hopCount(0),
		m_relevantRouters(list<FaceMetric>()),
		m_cachingRouterId(-1),
		m_weight(0),
		m_reuseTime(0),
		m_id(-1)
	{
	}
	
	DataPacket(string name) :
		m_name(name),
		m_payload (""),
		m_currentRouterDist (0),
		m_cachingRouterDist (0),
		m_size (name.size()),
		m_arrivalFace(-1),
		m_type(unknow),
		m_hopCount(0),
		m_relevantRouters(list<FaceMetric>()),
		m_cachingRouterId(-1),
		m_weight(0),
		m_reuseTime(0),
		m_id(-1)
	{
	}	
	
	DataPacket(string name, string payload) :
		m_name(name),
		m_payload(payload),
		m_currentRouterDist(0),
		m_cachingRouterDist(0),
		m_size(name.size() + payload.size()),
		m_arrivalFace(-1),
		m_type(normal),
		m_hopCount(0),
		m_relevantRouters(list<FaceMetric>()),
		m_cachingRouterId(-1),
		m_weight(0),
		m_reuseTime(0),
		m_id(-1)
	{
	}
	
	DataPacket(const DataPacket& other)
	{
		m_name = other.m_name;
		m_currentRouterDist = other.m_currentRouterDist;
		m_cachingRouterDist = other.getCachingRouterDist();
		m_payload = other.m_payload;
		m_size = other.m_size;
		m_arrivalFace = other.m_arrivalFace;
		m_type = other.m_type;
		m_hopCount = other.m_hopCount;
		m_relevantRouters = list<FaceMetric>();
		other.getRelevantRouters(m_relevantRouters);
		m_cachingRouterId = other.m_cachingRouterId;
		m_weight = other.m_weight;
		m_reuseTime = other.m_reuseTime;
		m_id = other.m_id;
	}
	
	/**
	<@brief Set the name of the Data packet. Its name will be set by the way.
	*/
	void setName(string name)
	{
		if("" != m_name)
			m_size -= m_name.size();
		m_name = name;
		m_size += name.size();
	}
	
	string getName() const
	{
		return m_name;
	}
	
	void setCurrentRouterDist(int currentRouterDist)
	{
		m_currentRouterDist = currentRouterDist;
	}
	
	int getCurrentRouterDist() const
	{
		return m_currentRouterDist;
	}
	
	void increaseCurrentRouterDist()
	{
		++m_currentRouterDist;
	}
	
	void decreaseCurrentRouterDist()
	{
		--m_currentRouterDist;
	}
	
	void setCachingRouterDist(int cachingRouterDist)
	{
		m_cachingRouterDist = cachingRouterDist;
	}
	
	int getCachingRouterDist() const
	{
		return m_cachingRouterDist;
	}
	
	void increaseCachingRouterDist()
	{
		++m_cachingRouterDist;
	}
	
	void decreaseCachingRouterDist()
	{
		--m_cachingRouterDist;
	}
	
	/**
	<@brief Set the payload of the Data packet. Its size will be set.
	*/
	void setPayload(string payload)
	{
		if("" != m_payload)
			m_size -= m_payload.size();
		m_payload = payload;
		m_size += m_payload.size();
	}
	
	string getPayload() const
	{
		return m_payload;
	}
	
	void setArrivalFace(int arrivalFace)
	{
		m_arrivalFace = arrivalFace;
	}
	
	int getArrivalFace() const
	{
		return m_arrivalFace;
	}
	
	void setSize()
	{
		//m_size = m_payload.length() + m_name.length();
		m_size = 1024;
	}
	
	string::size_type getSize() const
	{
		return m_size;
	}
	
	void setType(Type type)
	{
		m_type = type;
	}
	
	Type getType() const
	{
		return m_type;
	}
	
	int getHopCount() const
	{
		return m_hopCount;
	}
	
	void setHopCount(int hopCount)
	{
		m_hopCount = hopCount;
	}
	
	void increaseHopCount()
	{
		++m_hopCount;
	}

	void setWeight(float weight)
	{
		m_weight = weight;
	}

	float getWeight() const
	{
		return m_weight;
	}

	/**
	<@function. increaseWeight
	<@brief. Increase the weight of the Data packet by a particular value.
	<@param. value, the value by which the weight of the Data packet will be increased.
	*/
	void increaseWeight(float value)
	{
		m_weight += value;
	}

	/**
	<@function. decreaseWeight
	<@brief. Decrease the weight of the function by a particular value.
	<@param. value, the value by which the weight of the Data packet will be decreased.
	*/
	void decreaseWeight(float value)
	{
		m_weight -= value;
	}
	
	void operator=(const DataPacket& other)
	{
		m_name = other.m_name;
		m_currentRouterDist = other.m_currentRouterDist;
		m_cachingRouterDist = other.getCachingRouterDist();
		m_payload = other.m_payload;
		m_size = other.m_size;
		m_arrivalFace = other.m_arrivalFace;
		m_type = other.m_type;
		m_hopCount = other.m_hopCount;
		m_relevantRouters = list<FaceMetric>();
		other.getRelevantRouters(m_relevantRouters);
		m_cachingRouterId = other.m_cachingRouterId;
		m_weight = other.m_weight;
		m_reuseTime = other.m_reuseTime;
		m_id = other.m_id;
	}
	
	bool operator==(const DataPacket& other) const
	{
		return m_name == other.getName();
	}

	bool operator<(const DataPacket& other) const
	{
		return m_name < other.getName();
	}
	
	/**
	<@function. getRelevantRouters
	<@brief. Pass a reference variable and store all the data in the node's relevant router list into the variable. Then other function will 
		be able to retrive the the datas through the reference variable.
	<@param. relevantRouters, the reference variable to be passed to the function.
	*/
	void getRelevantRouters(list<FaceMetric>& relevantRouters) const
	{
		relevantRouters.clear();
		for(list<FaceMetric>::const_iterator iter(m_relevantRouters.begin()), end(m_relevantRouters.end());
			iter != end; ++iter)
			relevantRouters.push_back(*iter);
	}

	/**
	<@function. insertRelevantRouters
	<@brief. Insert a router into the relevant router list of the Data packet.
	<@param. router, the id of the router constructing the FaceMetric information.
	<@param. face, the face associated with the dynamic FIB entry corresponding to the FaceMetric information.
	<@param. metric, the distance from the router where the Data packet will be cached to the current router.
	*/
	void insertRelevantRouter(int router, vector<int> faces, float metric)
	{
		FaceMetric faceMetric;
		faceMetric.router = router;
		faceMetric.faces = faces;
		faceMetric.metric = metric;
		list<FaceMetric>::iterator iter = find(m_relevantRouters.begin(), m_relevantRouters.end(), faceMetric);
		if(m_relevantRouters.end() == iter)
			m_relevantRouters.push_back(faceMetric);
	}
	
	/**
	<@function. addRelevantRouters
	<@brief. Add a list of FaceMetric in to the relevantRouters list.
	<@param. relevantRouters, a list of FaceMetric.
	*/
	void addRelevantRouters(list<FaceMetric> relevantRouters)
	{
		for(list<FaceMetric>::iterator iter(relevantRouters.begin()), end(relevantRouters.end());
			iter != end; ++iter)
			m_relevantRouters.push_back(*iter);
	}


	/**
	<@function. clearRelevantRouter
	<@brief. Clear the relevant router list of the Data packet.
	*/
	void clearRelevantRouters()
	{
		m_relevantRouters.clear();
	}
	
	void setCachingRouterId(int cachingRouterId)
	{
		m_cachingRouterId = cachingRouterId;
	}

	int getCachingRouterId() const
	{
		return m_cachingRouterId;
	}

	void setReuseTime(int reuseTime)
	{
		m_reuseTime = reuseTime;
	}

	int getReuseTime() const
	{
		return m_reuseTime;
	}

	void resetReuseTime()
	{
		m_reuseTime = 0;
	}

	void increaseReuseTime()
	{
		++m_reuseTime;
	}

	void setId(int id)
	{
		m_id = id;
	}

	int getId() const
	{
		return m_id;
	}

	/**
	<@function. print
	<@brief. print out the content of the Data packet.
	*/
	void print()
	{
		cout << "the property of Data packet:" << endl;
		cout << "name: " << m_name << endl;
		cout << "current router dist: " << m_currentRouterDist << endl;
		cout << "caching router dist: " << m_cachingRouterDist << endl;
		cout << "payload: " << m_payload << endl;
		cout << "size: " << m_size << endl;
		cout << "arrival face: " << m_arrivalFace << endl;
		cout << "type: ";
		if(normal == m_type) cout << "normal" << endl;
		else if(nack == m_type) cout << "nack" << endl;
		cout << "relevant routers:" << endl;
		for(list<FaceMetric>::iterator iter(m_relevantRouters.begin()), end(m_relevantRouters.end());
			iter != end; ++iter)
			iter->print();
		cout << "----------" << endl;
	}

	private:
	string m_name;	//<@brief The name of the Data packet.
	int m_currentRouterDist;	//<@brief The distance from the end user to the current router.
	int m_cachingRouterDist;	//<@brief The distance from the end user to the caching router.
	string m_payload;	//<@brief The payload of the Data packet.
	string::size_type m_size;	//<@brief The size of the Data packet. It's the sum of the payload's size and the name's size.
	int m_arrivalFace;	//<@brief. //<@brief. The arrival face of the Data packet. In the framework, 
	// we take a node's ID as the face corresponding to it. So when node A forwards the Data packet to another node, say node B, 
	// node A will set the arrival face of the Data packet as its own ID.
	Type m_type;	//<@brief. The type of the Data packet. When it is a normal Data packet, its type is normal, and when it is a
	// NACK packet, its type is nack.
	int m_hopCount;	//<@brief. The total hops the Data packet and the Interest packet requesting it have travels.
	list<FaceMetric> m_relevantRouters;	//<@brief. The list record the routers which maintains dynamic routing information for the Data packet.
		// When the Data packet is evicted, all the dynamic routing information about the packet will be updated. The mechanism is 
		// is hard to implement in real network. We do this to implement our frame work in a perfect condition where the dynamic routing information
		// is updated in time.
	int m_cachingRouterId;	//The id of the router the Data packet will be cached.
	float m_weight;	//<@brief. The member is used to measuer how important the Data packet is; the metric will be used when 
	// some Data packet need to be evicted from the content store in our probabilitic eviction version of the routing framework.
	int m_reuseTime;	//<@brief. The number of times a cached Data packet is reused.
	int m_id;	//<@brief. The id of the Data packet.
};

#endif
