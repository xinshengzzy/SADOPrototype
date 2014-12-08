// ContentStore.h
#ifndef CONTENT_STORE_H
#define CONTENT_STORE_H
//#include <vld.h>

#include <list>
#include <utility>
#include <algorithm>
#include <fstream>

#include "DataPacket.h"
#include "components.h"
#include "utility.h"
#include "FaceMetric.h"
using namespace std;

//extern map<string, float> filenameAndProbability;
extern map<string, float> filenameAndProbability;
extern ofstream reuseTime;
extern int responsePacketNum;
extern long long cachedPacketNum;

bool compareDataPackets(const DataPacket& left, const DataPacket& right)
{
	return left.getWeight() > right.getWeight();
}

class ContentStore
{
	public:
	ContentStore(long long capacity)
	{
		m_capacity = capacity;
		m_remainderCapacity = capacity;
		m_store = list<DataPacket>();
		m_stat = list<ContentStoreStat>();
	}
	ContentStore()
	{
		m_capacity = 0;
		m_remainderCapacity = 0;
		m_store = list<DataPacket>();
		m_stat = list<ContentStoreStat>();
	}

	~ContentStore()
	{
	}

	/**
	<@function. setCapacity
	<@bref. Set the capacity of content store.
	<@param. capacity, the capacity of the content store that we will set it to be.
	<@caution. If the reduction in the capacity is greater than the remainder capacity, 
				the remainder capacity will have a minus value, which is an error.
	*/
	void setCapacity(long long capacity)
	{
		m_remainderCapacity += capacity - m_capacity;
		m_capacity = capacity;
		//if(capacity > m_pStore->max_size())
		//	m_pStore->resize(capacity);
	}
	
	/**
	<@function getCapacity
	<@brief Check the capacity of the content store.
	*/
	long long getCapacity() const
	{
		return m_capacity;
	}
	
	/**
	<@function getRemainderCapacity
	<@brief Check the remaider capacity of the content store.
	*/
	long long getRemainderCapacity()
	{
		return m_remainderCapacity;
	}
	
	/**
	<@function doesExist
	<@brief Check if there is a Data packet in the Content store with the given name
	<@param The name will be checked aginst.
	<@return true if does exist, or false if doesn't exist.
	*/
	bool DataPacketExist(string name)
	{
		DataPacket dataPacket(name);
		list<DataPacket>::iterator iter = find(m_store.begin(), m_store.end(), dataPacket);
		if(m_store.end() != iter)
			return true;
		return false;
	} 
	
	/**
	<@function getDataPacket
	<@brief Extract a Data packet matching the given name from the content store.
	<@param The name to be matched.
	<@return Return the <true, matching Data packet> if it exists, otherwise return <false, default Data packet> 
	<@besides If matching Data packet exist, move the matching packet to the font of the list.
	*/
	/*pair<bool, DataPacket> getDataPacket(string name)
	{
		DataPacket ret(name);
		list<DataPacket>::iterator iter = find(m_store.begin(), m_store.end(), ret);
		if(m_store.end() == iter)
		{
			return make_pair(false, ret);
		}
		string prefix = trimLastComponentFromName(name);
		float weight = filenameAndProbability[prefix];
		ret = *iter;
		ret.increaseWeight(weight);
		m_store.erase(iter);
		m_store.push_front(ret);
		m_store.sort(compareDataPackets);
		return make_pair(true, ret);
	}*/

	pair<bool, DataPacket> getDataPacket(string name)
	{
		DataPacket ret(name);
		list<DataPacket>::iterator iter = find(m_store.begin(), m_store.end(), ret);
		if(m_store.end() == iter)
		{
			return make_pair(false, ret);
		}
		//string prefix = trimLastComponentFromName(name);
		//float weight = filenameAndProbability[prefix];
		ret = *iter;
		ret.increaseReuseTime();
		//ret.increaseWeight(weight);
		m_store.erase(iter);
		m_store.push_front(ret);
		//m_store.sort(compareDataPackets);
		return make_pair(true, ret);
	}
	
	/**
	<@function dropDataPacket
	<@brief Drop the tail Data packet of the content store. The remaider capacity of the content store will be scaled.
	*/
	DataPacket dropDataPacket()
	{
		if(!m_store.empty())
		{
			DataPacket dataPacket= m_store.back();
			//DataPacket dataPacket = *iter;
			m_remainderCapacity += dataPacket.getSize();
			m_store.pop_back();
			// update the m_stat
			string dataPacketName = dataPacket.getName();
			if(responsePacketNum > 400000)
				reuseTime << dataPacket.getReuseTime() << endl;
			ContentStoreStat statItem;
			statItem.prefix = trimLastComponentFromName(dataPacketName);
			list<ContentStoreStat>::iterator iter= find(m_stat.begin(), m_stat.end(), statItem);
			iter->count = iter->count - 1;
			if(0 == iter->count) m_stat.erase(iter);
			return dataPacket;
		}
		else return DataPacket("");	// In fact the function returns nothing.
	}
	
	/**
	<@function. cacheDataPacket
	<@brief. Cache a given Data packet.
	<@param. dataPacket, the Data packet to be cached.
	<@return. If the Data packet is cached successfully or there has been a Data packet in the content store, the function will retrun true.
		Otherwise, the function will return false.
	<@attention. If there is a Data packet with the same name in the content store, 
				the original Data packet will be take to the head of the Data packet list.
	*/
	void cacheDataPacket(DataPacket dataPacket)
	{
		dataPacket.setReuseTime(0);
		list<DataPacket>::iterator iter = find(m_store.begin(), m_store.end(), dataPacket);
		//The Data packet to be cached has been in the content store.
		if(m_store.end() != iter)
		{
			DataPacket tempDataPacket(*iter);
			m_store.erase(iter);
			m_store.push_front(tempDataPacket);
			return;
		}
		// There are enough space to cache the Data packet.
		while(m_remainderCapacity < dataPacket.getSize())
		{
			DataPacket tempDataPacket(m_store.back());
			if(responsePacketNum > 400000)
				reuseTime << tempDataPacket.getReuseTime() << endl;
			m_store.pop_back();
			m_remainderCapacity += tempDataPacket.getSize();
		}
		m_store.push_back(dataPacket);
		m_remainderCapacity -= dataPacket.getSize();
	}


	//bool cacheDataPacket(DataPacket dataPacket)
	//{
	//	string prefix = trimLastComponentFromName(dataPacket.getName());
	//	//cout << "prefix = " << prefix << endl;
	//	float weight = filenameAndProbability[prefix];
	//	//cout << "weight = " << weight << endl;

	//	dataPacket.setCurrentRouterDist(0);
	//	dataPacket.setCachingRouterDist(0);
	//	dataPacket.setArrivalFace(-1);
	//	dataPacket.setHopCount(0);
	//	dataPacket.setCachingRouterId(-1);
	//	dataPacket.setWeight(weight);
	//	dataPacket.setReuseTime(0);
	//	list<DataPacket>::iterator iter = find(m_store.begin(), m_store.end(), dataPacket);
	//	//The Data packet to be cached has been in the content store.
	//	if(m_store.end() != iter)
	//	{
	//		//cout << "They has already a Data packet " << dataPacket.getName() << " in the content store." << endl;
	//		//iter->print();
	//		DataPacket tempDataPacket = *iter;
	//		list<FaceMetric> faceMetricList;
	//		dataPacket.getRelevantRouters(faceMetricList);
	//		tempDataPacket.addRelevantRouters(faceMetricList);
	//		//tempDataPacket.increaseWeight(weight);
	//		m_store.erase(iter);
	//		m_store.push_front(tempDataPacket);
	//		return true;
	//	}
	//	// There are enough space to cache the Data packet.
	//	if(m_remainderCapacity >= dataPacket.getSize())
	//	{
	//		m_store.push_front(dataPacket);
	//		m_remainderCapacity -= dataPacket.getSize();
	//		ContentStoreStat statItem;
	//		statItem.prefix = prefix;
	//		list<ContentStoreStat>::iterator iter = find(m_stat.begin(), m_stat.end(), statItem);
	//		if(m_stat.end() == iter)
	//		{
	//			statItem.count = 1;
	//			m_stat.push_back(statItem);
	//		}
	//		else
	//		{
	//			iter->count = iter->count + 1;
	//		}
	//		if(responsePacketNum > 400000)
	//		{
	//			++cachedPacketNum;
	//		}
	//		return true;
	//	}
	//	return false;
	//}


	/**
	<@function. getStat
	<@brief. Retrieve the statics for the Data packets in the content store.
	<@param. stat, a reference variable, the Data packet statistics will be stored in it.
	*/
	void getStat(list<ContentStoreStat>& stat) const
	{
		stat.clear();
		for(list<ContentStoreStat>::const_iterator iter(m_stat.begin()), end(m_stat.end());
			iter != end; ++iter)
			stat.push_back(*iter);
	}

	/**
	<@function. empty
	<@brief. Check if the content store is empty. If the content store is empty, the function returns true. 
		Otherwise the function returns false.
	*/
	bool empty()
	{
		return m_store.empty();
	}
	
	/**
	<@function. getLastPacket
	<@brief. Get the last Data packet of the Content store.
	*/
	DataPacket getLastPacket()
	{
		return m_store.back();
	}
	
	/**
	<@function. getSize
	<@bref. Get the number of Data packets in the content store.
	*/
	int getSize() const
	{
		return m_store.size();
	}
	
	/**
	<@function. print
	<@brief. Print out the status of the router.
	*/
	void print()
	{
		cout << "Content Store:" << endl;
		cout << "capacity: " << m_capacity << endl;
		cout << "remaider capacity: " << m_remainderCapacity << endl;
		cout << "Data packets:" << endl;
		for(list<DataPacket>::iterator iter(m_store.begin()), end(m_store.end());
			iter != end; ++iter)
			cout << iter->getName() << endl;
	}

	/**
	<@function. printReuseTime
	<@brief. Print the reuse time of all the Data packets in the content store.
	*/
	void printReuseTime() const
	{
		for(list<DataPacket>::const_iterator iter(m_store.begin()), end(m_store.end());
			iter != end; ++iter)
			reuseTime << iter->getReuseTime() << endl;
	}

	/**
	<@function. countDataPackets
	<@brief. In the function, the Data packets in the content store will be inserted into the given 
		container, while the duplicate Data packets against the existing Data packets will be removed. Thus the container could count 
		the number of unique Data packet there are.
	<@param. containter, the Data packets in the content store will	be inserted into the container, and as the container is a
		set, the duplicate Data packets will be removed automatically.
	*/
	void countDataPackets(set<DataPacket>& container)
	{
		for(list<DataPacket>::iterator iter(m_store.begin()), end(m_store.end());
			iter != end; ++iter)
			container.insert(*iter);
	}

	private:
	long long m_capacity;	//<@brief The size of the content store 
	list<DataPacket> m_store;	//<@brief The list to store the Data packets
	list<ContentStoreStat> m_stat;
	long long m_remainderCapacity;	//<@brief The remaider capacity of the content store that could be used to store the Data packets
};

#endif
