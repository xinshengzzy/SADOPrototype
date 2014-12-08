//InterestPacket.h
#ifndef INTEREST_PACKET_H
#define INTEREST_PACKET_H

//#include <vld.h>

#include <set>
#include <string>
using namespace std;

class InterestPacket
{
	public:
	InterestPacket(string name)
	{
		m_name = name;
		m_ttl = 20;
		m_currentRouterDist = 0;
		m_cachingRouterDist = 0;
		m_hashValue = 0;
		m_weight = 0;
		m_arrivalFace = -1;
		m_type = normal;
		m_hopCount = 0;
		m_unavailableFaces = set<int>();
		m_id = -1;
	}
	
	InterestPacket(string name, int ttl)
	{
		m_name = name;
		m_ttl = ttl;
		m_currentRouterDist = 0;
		m_cachingRouterDist = 0;
		m_hashValue = 0;
		m_weight = 0;
		m_arrivalFace = -1;
		m_type = normal;
		m_hopCount = 0;
		m_unavailableFaces = set<int>();
		m_id = -1;
	}
	
	InterestPacket(const InterestPacket& other)
	{
		m_name = other.getName ();
		m_ttl = other.getTtl();
		m_currentRouterDist = other.m_currentRouterDist;
		m_cachingRouterDist = other.m_cachingRouterDist;
		m_hashValue = other.getHashValue();
		m_weight = other.getWeight();
		m_arrivalFace = other.getArrivalFace();
		m_unavailableFaces = set<int>();
		other.getUnavailableFaces(m_unavailableFaces);
		m_type = other.getType();
		m_hopCount = other.m_hopCount;
		m_id = other.m_id;
	}

	void operator=(const InterestPacket& other)
	{
		m_name = other.getName ();
		m_ttl = other.getTtl();
		m_currentRouterDist = other.m_currentRouterDist;
		m_cachingRouterDist = other.m_cachingRouterDist;
		m_hashValue = other.getHashValue();
		m_weight = other.getWeight();
		m_arrivalFace = other.getArrivalFace();
		m_unavailableFaces = set<int>();
		other.getUnavailableFaces(m_unavailableFaces);
		m_type = other.getType();
		m_hopCount = other.m_hopCount;
		m_id = other.m_id;
	}
	
	bool operator==(const InterestPacket& other) const
	{
		return m_name == other.getName();
	}
	
	void setName(string name)
	{
		m_name = name;
	}
	
	string getName() const
	{
		return m_name;
	}
	
	void setTtl(int ttl)
	{
		m_ttl = ttl;
	}
	
	int getTtl() const
	{
		return m_ttl;
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
	
	void setHashValue(float hashValue)
	{
		m_hashValue = hashValue;
	}
	
	float getHashValue() const
	{
		return m_hashValue;
	}
	
	void setWeight(float weight)
	{
		m_weight = weight;
	}
	
	float getWeight() const
	{
		return m_weight;
	}
	
	void setArrivalFace(int arrivalFace)
	{
		m_arrivalFace = arrivalFace;
	}
	
	int getArrivalFace() const
	{
		return m_arrivalFace;
	}
	
	void getUnavailableFaces(set<int>& unavailableFaces) const
	{
		unavailableFaces.clear();
		for(set<int>::const_iterator iter(m_unavailableFaces.begin()), end(m_unavailableFaces.end());
			iter != end; ++iter)
			unavailableFaces.insert(*iter);
	}	
	
	/**
	<@function. insertUnavailableFace
	<@brief. When an Interest packet is forwarded through some face, the face will be added to its 
		unavailable face list, which means the Interest packet won't be forwarded through the face again.
	<@brief. face, the face to be inserted into its unavailable face list.
	*/	
	void insertUnavailableFace(int face)
	{
		m_unavailableFaces.insert(face);
	}
	
	enum Type{normal, nack, unknow};
	
	void setType(Type type)
	{
		m_type = type;
	}
	
	Type getType() const
	{
		return m_type;
	}
	
	/**
	<@function. isFaceAvailable
	<@brief. Test if a given face is available, i.e., if the given face not in the Interest packet's unavailable face list.
	<@param. face, the face to be tested.
	<@return. If the given face is not in the Interest packet's unavailable face list, the ruturn value is true. 
		Otherwise the function returns false.
	*/
	bool isFaceAvailable(int face)
	{
		set<int>::iterator iter = m_unavailableFaces.find(face);
		if(m_unavailableFaces.end() == iter) return true;
		else return false;
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
	<@brief. Print out the properties of the Interest packet.
	*/
	void print()
	{
		cout << "the property of Interest packet:" << endl;
		cout << "name: " << m_name << endl;
		cout << "TTL: " << m_ttl << endl;
		cout << "current router distance: " << m_currentRouterDist << endl;
		cout << "caching router distance: " << m_cachingRouterDist << endl;
		cout << "hashValue: " << m_hashValue << endl;
		cout << "weight: " << m_weight << endl;
		cout << "arrival face: " << m_arrivalFace << endl;
		cout << "unavailable faces: ";
		for(set<int>::iterator iter(m_unavailableFaces.begin()), end(m_unavailableFaces.end());
			iter != end; ++iter)
			cout << *iter << " ";
		cout << endl;
		cout << "type: " ;
		if(normal == m_type) cout << "normal" << endl;
		else if(nack == m_type) cout << "nack" << endl;
		else if(unknow == m_type) cout << "unknow" << endl;
		cout << "hopCount: " << m_hopCount << endl;
		cout << "----------" << endl;
	}

	private:
	string m_name;	//<@brief. The name of the Interest packet.
	int m_ttl;	//<@brief. The time-to-live (in hops) of the Data packet.
	int m_currentRouterDist;	//<@brief. The distance from the end user to the current router.
	int m_cachingRouterDist;	//<@brief. The distance from the end user to the caching router.
	float m_hashValue;	//<@brief. The hash value of the caching router's identifier and the Interest packet's name.
	float m_weight;	//<@brief. The caching router's weight to cahe a given Data packet.
	int m_arrivalFace;	//<@brief. The arrival face of the Interest packet. In the framework, 
	// we take a node's ID as the face corresponding to it. So when node A forwards the Interest packet to another node, say node B, 
	// node A will set the arrival face of the Interest packet as its own ID.
	set<int> m_unavailableFaces;	//<@brief. When a face is proved that the requested Data packet cannot be reached through it, 
	// the face will be added to the face set.
	Type m_type;	// The type of the Interest packet. When the node receives a NACK packet corresponding to the Interest packet, 
	// the type field of the Interest packet will be set to nack, which means even if PIT entry corresponds to the Interest packet
	// exists, the Interest packet will be sent again.
	int m_hopCount;	// The total hops the Interest packet has travels, including the hops its copies has travelled.
	int m_id;	//<brief. The id of the Interest packet.
};

#endif
