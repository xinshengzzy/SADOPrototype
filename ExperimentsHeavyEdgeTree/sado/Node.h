//Node.h
//<@brief: The node could act as a client, a server, or a router. 
#ifndef NODE_H
#define NODE_H

//#include <vld.h>
//#include <boost/shared_ptr.hpp>

#include <set>
#include <queue>
#include <vector>
#include <algorithm>
#include <sstream>
#include <list>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <cmath>

#include "ContentStore.h"
#include "Pit.h"
#include "StaticFib.h"
#include "DynamicFib.h"
#include "DynamicFibEntry.h"
#include "DataPacket.h"
#include "InterestPacket.h"
//#include "NetworkConfig.h"
#include "FaceInfo.h"
#include "utility.h"
#include "components.h"
using namespace std;
class Node;

extern vector<Node> nodes;
extern int pitEntryLifetime;
extern vector<string> fileNames;
extern map<int, string> idPrefix;
extern vector<int> users;
extern int responsePacketNum;
extern vector<float> fileRequestProbability;
extern int cacheThreshold;
extern ofstream reuseTime;
extern int packetId;
extern int requiredHopNum;
extern int measuredHopNum;
extern int lowerPacketNumLimit;
extern int upperPacketNumLimit;

class Node
{
	public:
	Node(int id)
	{
		m_id = id;
		m_links = set<int>();
		m_contentStore = ContentStore(0);
		m_pit = Pit();
		m_staticFib = StaticFib();
		m_dynamicFib = DynamicFib();	
		m_type = unknow;
		m_dataList = list<DataPacket>();
		m_interestList = list<InterestPacket>();
		m_waitingInterestList = list<InterestPacket>();
		m_betweennessCentrality = -1;
		m_weight = -1;
		m_fileToRequest = "";
		m_dataPacketSeqNum = 100;
		m_userInterestCount = 0;
		m_userDataCount = 0;
		m_unmetInterestList = list<string>();
	}
	
	Node(int id, long long capacity)
	{
		m_id = id;
		m_links = set<int>();
		m_contentStore = ContentStore(capacity);
		m_pit = Pit();
		m_staticFib = StaticFib();
		m_dynamicFib = DynamicFib();
		m_type = unknow;
		m_dataList = list<DataPacket>();
		m_interestList = list<InterestPacket>();
		m_waitingInterestList = list<InterestPacket>();
		m_betweennessCentrality = -1;
		m_weight = -1;
		m_fileToRequest = "";
		m_dataPacketSeqNum = 100;
		m_userInterestCount = 0;
		m_userDataCount = 0;
		m_unmetInterestList = list<string>();
	}
	
	~Node()
	{
	}

	int getId()
	{
		return m_id;
	}
	
	void addLink(int link)
	{
//		cout << "id = " << m_id << endl;
		m_links.insert(link);
	}
	
	void eraseLink(int link)
	{
		set<int>::iterator iter = m_links.find(link);
		if(m_links.end() != iter)
			m_links.erase(iter);
	}
	
	set<int>::size_type getLinksNum()
	{
		return m_links.size();
	}
	
	
	enum Type{producer, router, user, unknow};
	
	void setType(Type type)
	{
		m_type = type;
	}
	
	Type getType() const
	{
		return m_type;
	}
	
	/**
	<@function. insertStaticFibEntry
	<@brief. Insert a static FIB entry into the static FIB of a node.
	<@param. prefix, the highest level prefix associated to producer.
	<@param. face, through which face the producer associated with the prefix could be reached.
	<@param. metric, the metric to the producer corresponding to the prefix through static FIB.
	*/
	void insertStaticFibEntry(string prefix, int face, float metric)
	{
		m_staticFib.insertEntry(prefix, face, metric);
	}
	
	/**
	<@function. queryStaticFib
	<@brief. Query the static FIB for the static routing information about a highest level prefix.
	<@param. prefix, target prefix to be queried about.
	<@param. flag, a reference variable. If the target information is available, it will be set to true,
			otherwise it will be set to false.
	<@param. face, a reference vairable, in the case where target information is available, it 
			will record through which face the destination producer could be reached.
	<@param. metric, a reference variable, in the case where target inforamtion is available it 
	will record the distance from the current node to the destination producer.
	*/
	void queryStaticFib(string prefix, bool& flag, int& face, float& metric)
	{
		m_staticFib.query(prefix, flag, face, metric);
	}
	
	/**
	<@function. pendInterestPacket
	<@brief. Pend an Interest packet to the Interest packet queue of the node.
	<@param. interestPacket, a reference variable, the Interest packet to be pended.
	*/
	void pendInterestPacket(InterestPacket interestPacket)
	{
			m_interestList.push_back(interestPacket);
	}
	
	/**
	<@function. pendDataPacket
	<@brief. Pend a Data packet to the Data packet queue of the node.
	<@param. dataPacket, a reference variable, the Data packet to be pended.
	*/
	void pendDataPacket(DataPacket dataPacket)
	{
		m_dataList.push_back(dataPacket);
	}
	
	/**
	<@function. processNormalDataPacket
	<@brief. When the Data packet to be processed is a normal Data packet, call the function.
	<@param. dataPacket, the Data packet to be processed.
	*/
/*	void processNormalDataPacket(DataPacket dataPacket)
	{
		string dataPacketName = dataPacket.getName();
		InterestPacket interestPacket(dataPacketName);
		list<InterestPacket>::iterator interestIter;
		int arrivalFace = dataPacket.getArrivalFace();
		
		dataPacket.setArrivalFace(m_id);
		dataPacket.increaseHopCount();

		list<int> arrivalFaces;
		list<PitInfo> pitInfos;
		m_pit.getPitInfos(dataPacketName, pitInfos);
		m_pit.dropEntry(dataPacketName);
		for(list<PitInfo>::iterator pitInfoIter(pitInfos.begin()), pitInfoEnd(pitInfos.end());
			pitInfoIter != pitInfoEnd; ++pitInfoIter)
		{
			arrivalFaces.push_back(pitInfoIter->m_arrivalFace);
		}

		//cout << "normal Data " << dataPacketName << " router " << m_id << "<---" << dataPacket.getArrivalFace() << endl;

		// This packet is a normal Data packet, so we consider caching it or setting up dynamic routing information for it, besides forward it.
		dataPacket.decreaseCurrentRouterDist();

		// Determine the router will cache it or set up routing information for it, and modify the relevant router list accordingly.
		if(dataPacket.getCurrentRouterDist() == dataPacket.getCachingRouterDist() || 
			(dataPacket.getCurrentRouterDist() > dataPacket.getCachingRouterDist() && arrivalFaces.empty()) ||
			(dataPacket.getCurrentRouterDist() > dataPacket.getCachingRouterDist() && 1 == arrivalFaces.size() && users.end() != find(users.begin(), users.end(), arrivalFaces.front())))
		{//if the currentRouterDist value is equal to the cachingRouterDist value, or there is nowhere to forward the Interest packet,
			// or there is only one next node and the node is a user node, cache the Data packet.
			//cout << "Cache the Data packet " << dataPacketName << endl;
			cacheDataPacket(dataPacket);	
			dataPacket.clearRelevantRouters();
			dataPacket.setCachingRouterId(m_id);
		}
		else if(dataPacket.getCurrentRouterDist() < dataPacket.getCachingRouterDist())
		{// The Data packe has been cached, so we will set up a dynamic fib entry for it.
			//cout << dataPacketName << " has been cached in some previous node." << endl;
			//cout << "Set up dynamic routing information for the Data packet " << dataPacketName << endl;
			vector<int> tempFaces;
			tempFaces.push_back(arrivalFace);
			string prefix = trimLastComponentFromName(dataPacket.getName());
			float metric = dataPacket.getCachingRouterDist() - dataPacket.getCurrentRouterDist();
			m_dynamicFib.addRoutingInfo(prefix, tempFaces, metric);
			dataPacket.insertRelevantRouter(m_id,tempFaces, metric);
		}
		else// The Data packet has not been cached in previous nodes, and will be cached in later nodes. So 
			// we set up a dynamic fib entry for it. 
		{
			//cout << dataPacketName << " has not been cached in previous nodes, and will be cached in later nodes." << endl;
			//cout << "Set up routing information for the Data packet " << dataPacketName << endl;
			string prefix = trimLastComponentFromName(dataPacket.getName());
			float metric = dataPacket.getCurrentRouterDist() - dataPacket.getCachingRouterDist();
			vector<int> tempFaces;
			for(list<int>::iterator iter(arrivalFaces.begin()), end(arrivalFaces.end());
				iter != end; ++iter)
				tempFaces.push_back(*iter);
			vectorSubtraction(tempFaces, users);
			m_dynamicFib.addRoutingInfo(prefix, tempFaces, metric);
			dataPacket.insertRelevantRouter(m_id, tempFaces, metric);
		}
		interestIter = find(m_waitingInterestList.begin(), m_waitingInterestList.end(), interestPacket);
		while(m_waitingInterestList.end() != interestIter)
		{
			m_waitingInterestList.erase(interestIter);
			interestIter = find(m_waitingInterestList.begin(), m_waitingInterestList.end(), interestPacket);
		}
		while(true)
		{
			bool flag = false;
			for(list<InterestPacket>::iterator intIter(m_interestList.begin()), intEnd(m_interestList.end());
				intIter != intEnd; ++intIter)
			{
				if(*intIter == interestPacket && InterestPacket::nack == intIter->getType())
				{
					//cout << "Erase an Interest packet from the Interest list." << endl;
					//intIter->print();
					m_interestList.erase(intIter);
					flag = true;
					break;
				}
			}
			if(false == flag) break;
		}
		
		//if(arrivalFaces.empty()) 
		//	cout << "There is no pit entry to forward Data packet " << dataPacket.getName() << endl;
		
		for(list<PitInfo>::iterator iter(pitInfos.begin()), end(pitInfos.end());
			iter != end; ++iter)
		{
			DataPacket tempDataPacket(dataPacket);
			tempDataPacket.setId(iter->m_interestPacketId);
			nodes[iter->m_arrivalFace].pendDataPacket(tempDataPacket);
			if(tempDataPacket.getId() >= lowerPacketNumLimit && tempDataPacket.getId() <= upperPacketNumLimit)
			{
				++measuredHopNum;
			}
			//cout << "normal Data " << dataPacket.getName() << " router " << m_id << "--->" << iter->m_arrivalFace << endl;
		}
	}
*/
	void processNormalDataPacket(DataPacket dataPacket)
	{
		string dataPacketName = dataPacket.getName();
		InterestPacket interestPacket(dataPacketName);
		list<InterestPacket>::iterator interestIter;
		int arrivalFace = dataPacket.getArrivalFace();
		
		dataPacket.setArrivalFace(m_id);
		dataPacket.increaseHopCount();

		list<int> arrivalFaces;
		list<PitInfo> pitInfos;
		m_pit.getPitInfos(dataPacketName, pitInfos);
		m_pit.dropEntry(dataPacketName);
		for(list<PitInfo>::iterator pitInfoIter(pitInfos.begin()), pitInfoEnd(pitInfos.end());
			pitInfoIter != pitInfoEnd; ++pitInfoIter)
		{
			arrivalFaces.push_back(pitInfoIter->m_arrivalFace);
		}

		//cout << "normal Data " << dataPacketName << " router " << m_id << "<---" << dataPacket.getArrivalFace() << endl;

		// This packet is a normal Data packet, so we consider caching it or setting up dynamic routing information for it, besides forward it.
		dataPacket.decreaseCurrentRouterDist();

		// Determine the router will cache it or set up routing information for it, and modify the relevant router list accordingly.
		if(dataPacket.getCurrentRouterDist() == dataPacket.getCachingRouterDist())
		{//if the currentRouterDist value is equal to the cachingRouterDist value, or there is nowhere to forward the Interest packet,
			// or there is only one next node and the node is a user node, cache the Data packet.
			//cout << "Cache the Data packet " << dataPacketName << endl;
			cacheDataPacket(dataPacket);	
			dataPacket.clearRelevantRouters();
			dataPacket.setCachingRouterId(m_id);
			for(list<PitInfo>::iterator iter(pitInfos.begin()), end(pitInfos.end());
				iter != end; ++iter)
			{
				DataPacket tempDataPacket(dataPacket);
				tempDataPacket.setId(iter->m_interestPacketId);
				nodes[iter->m_arrivalFace].pendDataPacket(tempDataPacket);
				if(tempDataPacket.getId() >= lowerPacketNumLimit && tempDataPacket.getId() <= upperPacketNumLimit)
				{
					++measuredHopNum;
				}
				//cout << "normal Data " << dataPacket.getName() << " router " << m_id << "--->" << iter->m_arrivalFace << endl;
			}
		}
		else if(dataPacket.getCurrentRouterDist() < dataPacket.getCachingRouterDist())
		{// The Data packe has been cached, so we will set up a dynamic fib entry for it.
			//cout << dataPacketName << " has been cached in some previous node." << endl;
			//cout << "Set up dynamic routing information for the Data packet " << dataPacketName << endl;
			vector<int> tempFaces;
			tempFaces.push_back(arrivalFace);
			string prefix = trimLastComponentFromName(dataPacket.getName());
			float metric = dataPacket.getCachingRouterDist() - dataPacket.getCurrentRouterDist();
			m_dynamicFib.addRoutingInfo(prefix, tempFaces, metric);
			dataPacket.insertRelevantRouter(m_id,tempFaces, metric);
			for(list<PitInfo>::iterator iter(pitInfos.begin()), end(pitInfos.end());
				iter != end; ++iter)
			{
				DataPacket tempDataPacket(dataPacket);
				tempDataPacket.setId(iter->m_interestPacketId);
				nodes[iter->m_arrivalFace].pendDataPacket(tempDataPacket);
				if(tempDataPacket.getId() >= lowerPacketNumLimit && tempDataPacket.getId() <= upperPacketNumLimit)
				{
					++measuredHopNum;
				}
				//cout << "normal Data " << dataPacket.getName() << " router " << m_id << "--->" << iter->m_arrivalFace << endl;
			}

		}
		else// The Data packet has not been cached in previous nodes, and will be cached in later nodes. So 
			// we set up a dynamic fib entry for it. 
		{
			//cout << dataPacketName << " has not been cached in previous nodes, and will be cached in later nodes." << endl;
			//cout << "Set up routing information for the Data packet " << dataPacketName << endl;
			string prefix = trimLastComponentFromName(dataPacket.getName());
			float metric = dataPacket.getCurrentRouterDist() - dataPacket.getCachingRouterDist();
			vector<int> tempFaces;
			for(list<PitInfo>::iterator iter(pitInfos.begin()), end(pitInfos.end());
				iter != end; ++iter)
			{
				tempFaces.clear();
				tempFaces.push_back(iter->m_arrivalFace);
				//vectorSubtraction(tempFaces, users);
				m_dynamicFib.addRoutingInfo(prefix, tempFaces, metric);
				DataPacket tempDataPacket(dataPacket);
				tempDataPacket.setId(iter->m_interestPacketId);
				tempDataPacket.insertRelevantRouter(m_id, tempFaces, metric);
				nodes[iter->m_arrivalFace].pendDataPacket(tempDataPacket);
				if(tempDataPacket.getId() >= lowerPacketNumLimit && tempDataPacket.getId() <= upperPacketNumLimit)
				{
					++measuredHopNum;
				}
			}
		}
		interestIter = find(m_waitingInterestList.begin(), m_waitingInterestList.end(), interestPacket);
		while(m_waitingInterestList.end() != interestIter)
		{
			m_waitingInterestList.erase(interestIter);
			interestIter = find(m_waitingInterestList.begin(), m_waitingInterestList.end(), interestPacket);
		}
		while(true)
		{
			bool flag = false;
			for(list<InterestPacket>::iterator intIter(m_interestList.begin()), intEnd(m_interestList.end());
				intIter != intEnd; ++intIter)
			{
				if(*intIter == interestPacket && InterestPacket::nack == intIter->getType())
				{
					//cout << "Erase an Interest packet from the Interest list." << endl;
					//intIter->print();
					m_interestList.erase(intIter);
					flag = true;
					break;
				}
			}
			if(false == flag) break;
		}
		
		//if(arrivalFaces.empty()) 
		//	cout << "There is no pit entry to forward Data packet " << dataPacket.getName() << endl;
		
		//for(list<PitInfo>::iterator iter(pitInfos.begin()), end(pitInfos.end());
		//	iter != end; ++iter)
		//{
		//	DataPacket tempDataPacket(dataPacket);
		//	tempDataPacket.setId(iter->m_interestPacketId);
		//	nodes[iter->m_arrivalFace].pendDataPacket(tempDataPacket);
		//	if(tempDataPacket.getId() >= lowerPacketNumLimit && tempDataPacket.getId() <= upperPacketNumLimit)
		//	{
		//		++measuredHopNum;
		//	}
		//	cout << "normal Data " << dataPacket.getName() << " router " << m_id << "--->" << iter->m_arrivalFace << endl;
		//}
	}

	
	/**
	<@function. processNocacheDataPacket
	<@brief. When the Data packet to be processed is a nocache Data packet, call the function.
	<@param. dataPacket, the Data packet to be processed.
	*/
	void processNocacheDataPacket(DataPacket dataPacket)
	{
		string dataPacketName = dataPacket.getName();
		InterestPacket interestPacket(dataPacketName);
		list<InterestPacket>::iterator interestIter;
		int arrivalFace = dataPacket.getArrivalFace();
		
		dataPacket.setArrivalFace(m_id);
		dataPacket.increaseHopCount();

		list<int> arrivalFaces;
		list<PitInfo> pitInfos;
		m_pit.getPitInfos(dataPacketName, pitInfos);
		m_pit.dropEntry(dataPacketName);
		for(list<PitInfo>::iterator pitInfoIter(pitInfos.begin()), pitInfoEnd(pitInfos.end());
			pitInfoIter != pitInfoEnd; ++pitInfoIter)
		{
			arrivalFaces.push_back(pitInfoIter->m_arrivalFace);
		}
		
		//cout << "nocache Data " << dataPacketName << " router " << m_id << "<---" << dataPacket.getArrivalFace() << endl;

		interestIter = find(m_waitingInterestList.begin(), m_waitingInterestList.end(), interestPacket);
		while(m_waitingInterestList.end() != interestIter)
		{
			m_waitingInterestList.erase(interestIter);
			interestIter = find(m_waitingInterestList.begin(), m_waitingInterestList.end(), interestPacket);
		}
		while(true)
		{
			bool flag = false;
			for(list<InterestPacket>::iterator intIter(m_interestList.begin()), intEnd(m_interestList.end());
				intIter != intEnd; ++intIter)
			{
				if(*intIter == interestPacket && InterestPacket::nack == intIter->getType())
				{
					m_interestList.erase(intIter);
					flag = true;
					break;
				}
			}
			if(false == flag) break;
		}
		
		//if(arrivalFaces.empty()) 
		//	cout << "There is no pit entry to forward Data packet " << dataPacket.getName() << endl;
		
		for(list<PitInfo>::iterator iter(pitInfos.begin()), end(pitInfos.end());
			iter != end; ++iter)
		{
			DataPacket tempDataPacket(dataPacket);
			tempDataPacket.setId(iter->m_interestPacketId);
			nodes[iter->m_arrivalFace].pendDataPacket(tempDataPacket);
			if(tempDataPacket.getId() >= lowerPacketNumLimit && tempDataPacket.getId() <= upperPacketNumLimit)
			{
				++measuredHopNum;
			}
			//cout << "nocache Data " << dataPacket.getName() << " router " << m_id << "--->" << iter->m_arrivalFace << endl;
		}
	}

	/**
	<@function. processNackDataPacket
	<@brief. When the Data packet to be processed is a nack Data packet, call the function.
	<@param. dataPacket, the Data packet to be processed.
	*/
	void processNackDataPacket(DataPacket dataPacket)
	{
		string dataPacketName = dataPacket.getName();
		InterestPacket interestPacket(dataPacketName);
		list<InterestPacket>::iterator interestIter;
		int arrivalFace = dataPacket.getArrivalFace();
		
		// This is a NACK packet. The relevant Interest packet will be waked up.
		//cout << "nack Data " << dataPacketName << " router " << m_id << "<---" << dataPacket.getArrivalFace() << endl;
		interestIter = find(m_waitingInterestList.begin(), m_waitingInterestList.end(), interestPacket);
		if(m_waitingInterestList.end() != interestIter)
		{
			//cout << "Wake up an Interest packet " << dataPacketName << " from the waiting Interest list." << endl;
			interestIter->setType(InterestPacket::nack);
			interestIter->setHopCount(dataPacket.getHopCount() + 1);
			//cout << "before assigning" << endl;
			interestPacket = *interestIter;
			//cout << "after assigning" << endl;
			m_waitingInterestList.erase(interestIter);
			m_interestList.push_back(interestPacket);
		}
	}



	/**
	<@function. processDataPacket
	<@brief. If the Data packe queue is empty, the function does nothing and returns. Or the function will process 
			a Data packet then return.
	<@attention. The process applies for the router nodes only.
	*/
	void processDataPacket()
	{
		while(!m_dataList.empty())
		{
			DataPacket dataPacket(m_dataList.front());
			m_dataList.pop_front(); 
			if(dataPacket.getType() == DataPacket::normal)
				processNormalDataPacket(dataPacket);
			else if(dataPacket.getType() == DataPacket::nocache)
				processNocacheDataPacket(dataPacket);
			else if(dataPacket.getType() == DataPacket::nack)
				processNackDataPacket(dataPacket);
		}
	}
	
	/**
	<@function. processDataPacketStatic
	<@brief. The function is used when dynamic routing is not considered. When a router receives a 
		Data packet, it will forward cache it (in case the router should cache the Data packet 
		according to our caching strategy) and forward it without setting up dynamic routing information
		for it.
	*/
	//void processDataPacketStatic()
	//{
	//	while(!m_dataList.empty())
	//	{
	//		//cout << "Router" << m_id << " enters processDataPacket()" << endl;
	//		DataPacket dataPacket(m_dataList.front());
	//		dataPacket.increaseHopCount();
	//		cout << "Data " << dataPacket.getName() << " router " << m_id << "<---" << dataPacket.getArrivalFace() << endl;
	//		cout << "dataPacket.hopSum = " << dataPacket.getHopCount() << endl;
	//		m_dataList.pop_front(); 
	//		//dataPacket.increaseHopCount();
	//		string dataPacketName = dataPacket.getName();
	//		dataPacket.setArrivalFace(m_id);
	//		dataPacket.decreaseCurrentRouterDist();

	//		// Determine the router will cache it or set up routing information for it, and modify the relevant router list accordingly.
	//		if(dataPacket.getCurrentRouterDist() == dataPacket.getCachingRouterDist())
	//		{//if the currentRouterDist value is equal to the cachingRouterDist value, or there is nowhere to forward the Interest packet,
	//			// or there is only one next node and the node is a user node, cache the Data packet.
	//			cacheDataPacket(dataPacket);	
	////			cout << "after caching." << endl;
	//		}

	//		list<int> arrivalFaces;
	//		m_pit.getArrivalFaces(dataPacketName, arrivalFaces);
	////		cout << "before drop()" << endl;
	//		m_pit.dropEntry(dataPacketName);
	//		
	//		if(arrivalFaces.empty()) 
	//			cout << "There is no pit entry to forward Data packet " << dataPacket.getName() << endl;

	//		for(list<int>::iterator iter(arrivalFaces.begin()), end(arrivalFaces.end());
	//			iter != end; ++iter)
	//		{
	//			nodes[*iter].pendDataPacket(dataPacket);
	//			cout << "Data " << dataPacket.getName() << " router " << m_id << "--->" << *iter << endl;
	//		}
	//	}
	//}


	/**
	<@function. processNormalInterestPacket
	<@brief. When the Interest packet to be processed is a normal Interest packet, call the function.
	<@param. interestPacket, the Interest packet to be processed.
	*/
	void processNormalInterestPacket(InterestPacket interestPacket)
	{
		string interestPacketName = interestPacket.getName();
		string trimedName = trimLastComponentFromName(interestPacketName);
		int arrivalFace = interestPacket.getArrivalFace();
		//cout << "normal Interest " << interestPacketName << " router " << m_id << "<---" << arrivalFace << endl;
		interestPacket.increaseHopCount();
		pair<bool, DataPacket> tempPair = m_contentStore.getDataPacket(interestPacketName);
		if(true == tempPair.first)	//The node can supply the requested Data packet.
		{
			//cout << "Caching hit!" << endl;
			DataPacket returnDataPacket(tempPair.second);
			returnDataPacket.setCurrentRouterDist(interestPacket.getCurrentRouterDist() + 1);
			returnDataPacket.setCachingRouterDist(interestPacket.getCachingRouterDist());
			returnDataPacket.setArrivalFace(m_id);
			returnDataPacket.setHopCount(interestPacket.getHopCount());
			returnDataPacket.clearRelevantRouters();
			float currentRouterDist = float(returnDataPacket.getCurrentRouterDist());
			float cachingRouterDist = float(returnDataPacket.getCachingRouterDist());
			if(currentRouterDist - cachingRouterDist > cacheThreshold && cachingRouterDist/currentRouterDist < 0.618)	// I choose the gold ratio as the 
			{																	// criterion; I don't know why, but I believe it will work well.
				returnDataPacket.setType(DataPacket::normal);
			}
			else
			{
				returnDataPacket.setType(DataPacket::nocache);
			}
			returnDataPacket.setId(interestPacket.getId());
			nodes[arrivalFace].pendDataPacket(returnDataPacket);
			if(returnDataPacket.getId() >= lowerPacketNumLimit & returnDataPacket.getId() <= upperPacketNumLimit)
			{
				++measuredHopNum;
			}
			//cout << "Data " << returnDataPacket.getName() << " router " << m_id << "--->" << arrivalFace << endl;
			return;
		}
		// The node cannot supply the requested Data packet.
		//If there is a matching PIT entry for the Interest packet.
		if(m_pit.matchingEntryExists(interestPacketName))
		{
			//We consider a case here where the arrival face is the same as the face through which the Interest packet correspoonding to the
			// Pit entry is forwarded. In the case, the router from which the Interest packet is transmitted has set up a PIT entry for this 
			// Interest packet and hope to receive the requested Data packet from this router. If we add this Interest packet to this router's 
			// Pit, and wait for the arrival of response Data packet from the previous router, we will fall into a dead lock. So we need to 
			// check if the arrival face of the Interest packet and the forwarding face associated with the Pit entry is the same.
			int forwardingFace = m_pit.getForwardingFace(interestPacketName);
			if(forwardingFace == arrivalFace)
			{// Wake up the Interest packet corresponding with the same name as this Interest packet from waiting Interest list
				// and put it into the interest packet list, with its type set to nack.
				//cout << "The arrival face of the Interest packet happens to be the forwarding face of the corresponding PIT entry" << endl;
				int staticFace;
				float staticMetric;
				bool flag;
				getStaticRoutingInfo(interestPacketName, flag, staticFace, staticMetric);
				if(arrivalFace != staticFace)
				{// This means that the router the Interest packet comes from is not the parent router of the current router in the routing information tree.
					// So it must be a child router of this router in routing information tree. So We just look for another face to forward the Interest packet,
					// while add that child router, i.e., the router the Interest packet is from, into the FIB entry. When we fetch the requested Data packet back,
					// we will send a copy to that child router. The child router don't need to do anything, as the face is the only face the child router could choose.
					m_pit.insertEntry(interestPacketName, arrivalFace, interestPacket.getHopCount(), 
						interestPacket.getCurrentRouterDist() + 1, interestPacket.getId());
					list<InterestPacket>::iterator iter = find(m_waitingInterestList.begin(), m_waitingInterestList.end(), interestPacket);
					if(m_waitingInterestList.end() != iter)
					{// Actually we should always find a matching Interest packet in waiting list; but in case we failed to find the 
						// matching Interest, we add the judgement statement.
						interestPacket = *iter;
						m_waitingInterestList.erase(iter);
						interestPacket.setType(InterestPacket::nack);
						m_interestList.push_back(interestPacket);
						//cout << "And the forwarding face is not the face towarding the producer." << endl;
						//cout << "Wake up the Interest packet from the waiting list" << endl;
					}
				}
				else
				{
					//cout << "But the forwarding face is the face towarding the producer." << endl;
					m_pit.insertEntry(interestPacketName, arrivalFace, interestPacket.getHopCount(), 
						interestPacket.getCurrentRouterDist() + 1, interestPacket.getId());
					//cout << "Add the Interest " << interestPacketName << " into the PIT entry" << endl;
				}
			}
			else
			{
				m_pit.insertEntry(interestPacketName, arrivalFace, interestPacket.getHopCount(), 
					interestPacket.getCurrentRouterDist() + 1, interestPacket.getId());
				//cout << "Add the Interest " << interestPacketName << " into the PIT entry" << endl;
			}
		}
		else// There is no matching PIT entry to forward the Interest packet.
		{
			set<int> unavailableFaces;
			interestPacket.getUnavailableFaces(unavailableFaces);
			int resultantFace;
			// Check if there are available faces.
			getAvailableFace(interestPacketName, unavailableFaces, resultantFace);
			if(-1 == resultantFace || resultantFace == arrivalFace)
			{// There is no availabe face to forward the Interest packet, so the node will report a Nack packet to its previous node.
				DataPacket nackDataPacket(interestPacketName);
				nackDataPacket.setType(DataPacket::nack);
				nackDataPacket.setHopCount(interestPacket.getHopCount());
				nackDataPacket.setArrivalFace(m_id);
				nackDataPacket.setId(interestPacket.getId());
				nodes[arrivalFace].pendDataPacket(nackDataPacket);
				if(nackDataPacket.getId() >= lowerPacketNumLimit && nackDataPacket.getId() <= upperPacketNumLimit)
				{
					++measuredHopNum;
				}
				//cout << "There are no available faces to forward the Interest packet " << interestPacketName << endl;
			}//End: There is no availabe face to forward the Interest packet, so the node will report a Nack packet to its previous node.
			else	//There is an available face to forward the Interest packet. Forward it.
			{
				interestPacket.insertUnavailableFace(resultantFace);
				interestPacket.increaseCurrentRouterDist();
				float hashValue1 = interestPacket.getHashValue();
				float weight1 = interestPacket.getWeight();
				//Combined the node's identifier and the file name part of the Interest packet's name into a new string.
				string combinedString = generateRandomString(m_id, 10);
				//cout << "Combined String: " << combinedString << endl;
				combinedString = combinedString + trimedName;
				float hashValue2 = hashStringToNum(combinedString);
				float weight2 = m_weight;
				float alpha1, alpha2;
				//For the logic of the following code, please refer to our paper "'Do My Best' Routing in Named Data Networking".
				if(weight1 < weight2)
				{
					alpha1 = 2*weight1/(weight1 + weight2);
					alpha2 = 1;
				}
				else
				{
					alpha1 = 1;
					alpha2 = 2*weight2/(weight1 + weight2);
				}
				if(alpha2*hashValue2 > alpha1*hashValue1)
				//if(hashValue2 > hashValue1)
				{
					//cout << "Update the weight, hash value and caching router value." << endl;
					interestPacket.setCachingRouterDist(interestPacket.getCurrentRouterDist());
					interestPacket.setWeight(weight2);
					interestPacket.setHashValue(hashValue2);
				}
				//interestPacket.setArrivalFace(m_id);
				InterestPacket tempInterestPacket(interestPacket);
				tempInterestPacket.setArrivalFace(m_id);
				tempInterestPacket.setType(InterestPacket::normal);
				nodes[resultantFace].pendInterestPacket(tempInterestPacket);
				if(tempInterestPacket.getId() >= lowerPacketNumLimit && tempInterestPacket.getId() <= upperPacketNumLimit)
				{
					++measuredHopNum;
				}
				//cout << "Interest " << interestPacket.getName() << " router " << m_id << "--->" << resultantFace << endl;
				m_pit.insertEntry(interestPacketName, arrivalFace, interestPacket.getHopCount(), interestPacket.getCurrentRouterDist(), 
					interestPacket.getId(), resultantFace); 
				m_waitingInterestList.push_back(interestPacket);
			}//End: There is an available face to forward the Interest packet. Forward it.
		}//End: There is no matching PIT entry to forward the Interest packet.
		//cout << "leave if" << endl;
	}

	/**
	<@function. processNackInterestPacket
	<@brief. When an Interest packet whose type is nack need to be processed, call the function.
	<@param. interestPacket, the Interest packet need to be processed.
	*/
	void processNackInterestPacket(InterestPacket interestPacket)
	{		
		string interestPacketName = interestPacket.getName();
		string trimedName = trimLastComponentFromName(interestPacketName);
		int arrivalFace = interestPacket.getArrivalFace();
		//cout << "nack Interest " << interestPacketName << " router " << m_id << "<---" << arrivalFace << endl;
		//Check if the router could supply the requested Data packet.
		pair<bool, DataPacket> tempPair = m_contentStore.getDataPacket(interestPacketName);
		if(true == tempPair.first)	//The node can supply the requested Data packet.
		{
			//cout << "Caching hit!" << endl;			
			DataPacket returnDataPacket(tempPair.second);
			returnDataPacket.setCurrentRouterDist(interestPacket.getCurrentRouterDist());
			returnDataPacket.setCachingRouterDist(interestPacket.getCachingRouterDist());
			returnDataPacket.setArrivalFace(m_id);
			returnDataPacket.setHopCount(interestPacket.getHopCount());
			float currentRouterDist = float(returnDataPacket.getCurrentRouterDist());
			float cachingRouterDist = float(returnDataPacket.getCachingRouterDist());
			if(currentRouterDist - cachingRouterDist > cacheThreshold && cachingRouterDist/currentRouterDist < 0.618)	// I choose the gold ratio as the 
			{																	// criterion; I don't know why, but I believe it will work well.
				returnDataPacket.setType(DataPacket::normal);
			}
			else
			{
				returnDataPacket.setType(DataPacket::nocache);
			}
			returnDataPacket.setId(interestPacket.getId());
			if(returnDataPacket.getId() >= lowerPacketNumLimit && returnDataPacket.getId() <= upperPacketNumLimit)
			{
				++measuredHopNum;
			}
			nodes[arrivalFace].pendDataPacket(returnDataPacket);
			//cout << "Data " << returnDataPacket.getName() << " router " << m_id << "--->" << arrivalFace << endl;
			return;
		}
		//The router could not supply the requested Data packet.
		//Consider forwarding the Interest packet.
		set<int> unavailableFaces;
		interestPacket.getUnavailableFaces(unavailableFaces);
		int resultantFace;
		// Check if there are available faces.
		getAvailableFace(interestPacketName, unavailableFaces, resultantFace);
		if(-1 == resultantFace || resultantFace == arrivalFace)
		{// There is no availabe face to forward the Interest packet, so the node will report a Nack packet to its previous node.
			DataPacket nackDataPacket(interestPacketName);
			nackDataPacket.setType(DataPacket::nack);
			nackDataPacket.setHopCount(interestPacket.getHopCount());
			nackDataPacket.setArrivalFace(m_id);
			nackDataPacket.setId(interestPacket.getId());
			nodes[arrivalFace].pendDataPacket(nackDataPacket);
			if(nackDataPacket.getId() >= lowerPacketNumLimit && nackDataPacket.getId() <= upperPacketNumLimit)
			{
				++measuredHopNum;
			}
			m_pit.dropEntry(interestPacketName);
			//cout << "There are no available faces to forward the Interest packet " << interestPacketName << endl;
			//cout << "nack Data " << interestPacketName << " router " << m_id << "--->" << arrivalFace << endl;
		}//End: There is no available face to forward the Interest packet, so the node will report a Nack packet to its previous node.
		else	//There is an available face to forward the Interest packet. Forward it.
		{
			interestPacket.insertUnavailableFace(resultantFace);
			InterestPacket tempInterestPacket(interestPacket);
			tempInterestPacket.setArrivalFace(m_id);
			tempInterestPacket.setType(InterestPacket::normal);
			nodes[resultantFace].pendInterestPacket(tempInterestPacket);
			if(tempInterestPacket.getId() >= lowerPacketNumLimit && tempInterestPacket.getId() <= upperPacketNumLimit)
			{
				++measuredHopNum;
			}
			m_waitingInterestList.push_back(interestPacket);
			m_pit.setForwardingFace(interestPacketName, resultantFace);
			//cout << "Interest " << interestPacket.getName() << " router " << m_id << "--->" << resultantFace << endl;
		}
	}

	

	void processInterestPacket()
	{		
		while(!m_interestList.empty())
		{
			InterestPacket interestPacket = m_interestList.front();
			m_interestList.pop_front();			
			if(InterestPacket::normal == interestPacket.getType())
			{
				processNormalInterestPacket(interestPacket);
			}
			else if(InterestPacket::nack == interestPacket.getType())
			{
				processNackInterestPacket(interestPacket);
			}
		}// End while
	}

	/**
	<@function. processInterestPacketStatic
	<@brief. The function is used when we don't consider the dynamic routing, i.e., every Interest packet will be forwarded 
		to the producer of requested Data packet. When the Interest packet pass across a router and the router happens to be able
		to supply the requested Data packet, the Interest packet will be consumed and the Data packet will be fetched back from the 
		router.
	*/
	//void processInterestPacketStatic()
	//{
	//	while(!m_interestList.empty())
	//	{
	//		InterestPacket interestPacket = m_interestList.front();
	//		m_interestList.pop_front();
	//		string interestPacketName = interestPacket.getName();
	//		//cout << "interestPacketName = " << interestPacketName << endl;
	//		string trimedName = trimLastComponentFromName(interestPacketName);
	//		//cout << "interestPacketName = " << interestPacketName << endl;
	//		int arrivalFace = interestPacket.getArrivalFace();
	//		//cout << "here" << endl;
	//		//if(InterestPacket::normal == interestPacket.getType() )
	//		//{
	//	//	cout << "enter if" << endl;
	//		//cout << "normal Interest:" << endl;
	//		cout << "Interest " << interestPacketName << " router " << m_id << "<---" << arrivalFace << endl;
	//		interestPacket.increaseHopCount();
	//		cout << "interestPacket.hopSum = " << interestPacket.getHopCount() << endl;
	//		interestPacket.increaseCurrentRouterDist();
	//		interestPacket.setArrivalFace(m_id);
	//		
	//		// Check if there are matching Data packet in content store.
	//		pair<bool, DataPacket> tempPair = m_contentStore.getDataPacket(interestPacketName);
	//		if(true == tempPair.first)	//The node can supply the requested Data packet.
	//		{
	//			cout << "Caching hit!" << endl;
	//			DataPacket returnDataPacket(tempPair.second);
	//			returnDataPacket.setCurrentRouterDist(interestPacket.getCurrentRouterDist());
	//			returnDataPacket.setCachingRouterDist(interestPacket.getCachingRouterDist());
	//			returnDataPacket.setArrivalFace(m_id);
	//			returnDataPacket.setHopCount(interestPacket.getHopCount());
	//			//returnDataPacket.clearRelevantRouters();
	//			nodes[arrivalFace].pendDataPacket(returnDataPacket);
	//			//Data www.youtube.com/TKWoQOBjvg/nwbK7iXmgF/LTmfLBIE4A/100/99 user 23<---2
	//			cout << "Data " << returnDataPacket.getName() << " router " << m_id << "--->" << arrivalFace << endl;
	//			//cout << "after2" << endl;
	//			continue;
	//		}
	//		// The node cannot supply the requested Data packet.
	//		//If there is a matching PIT entry for the Interest packet.
	//		if(m_pit.matchingEntryExists(interestPacketName))
	//		{
	//			m_pit.insertEntry(interestPacketName, arrivalFace, pitEntryLifetime);
	//		}
	//		else// There is no matching PIT entry to forward the Interest packet.
	//		{
	//			//interestPacket.increaseCurrentRouterDist();
	//			//Extract the hash value and weight from the Interest packet.
	//			float hashValue1 = interestPacket.getHashValue();
	//			float weight1 = interestPacket.getWeight();
	//			//Combined the node's identifier and the file name part of the Interest packet's name into a new string.
	//			string combinedString = generateRandomString(m_id, 10);
	//			combinedString = combinedString + trimedName;
	//			float hashValue2 = hashStringToNum(combinedString);
	//			float weight2 = m_weight;
	//			float alpha1, alpha2;
	//			//For the logic of the following code, please refer to our paper "'Do My Best' Routing in Named Data Networking".
	//			if(weight1 < weight2)
	//			{
	//				alpha1 = 2*weight1/(weight1 + weight2);
	//				alpha2 = 1;
	//			}
	//			else
	//			{
	//				alpha1 = 1;
	//				alpha2 = 2*weight2/(weight1 + weight2);
	//			}
	//			if(alpha2*hashValue2 > alpha1*hashValue1)
	//			{
	//				interestPacket.setCachingRouterDist(interestPacket.getCurrentRouterDist());
	//				interestPacket.setWeight(weight2);
	//				interestPacket.setHashValue(hashValue2);
	//			}
	//			int staticFace;
	//			float staticMetric;
	//			bool flag;
	//			getStaticRoutingInfo(interestPacketName, flag, staticFace, staticMetric);
	//			nodes[staticFace].pendInterestPacket(interestPacket);
	//			cout << "Interest " << interestPacket.getName() << " router " << m_id << "--->" << staticFace << endl;
	//			m_pit.insertEntry(interestPacketName, arrivalFace, pitEntryLifetime);
	//		}//End: There is no matching PIT entry to forward the Interest packet.
	//	}// End while
	//}


	/**
	<@function. userOperation
	<@brief. The operation of end users. The operation includes initiate Interest packets and 
		process the returned Data packets.
	<@attention. The function applies to end users only.
	*/
	void userOperation()
	{
		//cout << "in the userOperation" << endl;
		//cout << "User " << m_id << " enters userOperation()" << endl;
		if(100 == m_dataPacketSeqNum)
		{
			//srand((unsigned)time(0));
			float randomNum = float(rand())/RAND_MAX;
			int fileNameNum = fileNames.size();
			int i = 0;
			for(; i < fileNameNum; ++i)
			{
				if(randomNum <= fileRequestProbability[i])
					break;
			}
			//randomNum = randomNum%fileNameNum;
			m_fileToRequest = fileNames[i];
			m_dataPacketSeqNum = 0;
		}
		/*if(m_userInterestCount < 10000)
		{
		*/	//cout << "User " << m_id << " initiates an Interest packet." << endl;
		ostringstream convert;
		convert << m_dataPacketSeqNum++;
		string dataPacketSeqNumStr = convert.str();
		string dataPacketName = m_fileToRequest + "/" + dataPacketSeqNumStr;
		//cout << dataPacketName << endl;
		
		int forwardingFace = *(m_links.begin());
		
		// Forward the Interest packet.
		InterestPacket interestPacket(dataPacketName);
		interestPacket.setArrivalFace(m_id);
		interestPacket.insertUnavailableFace(forwardingFace);
		interestPacket.setId(++packetId);
		nodes[forwardingFace].pendInterestPacket(interestPacket);
		if(interestPacket.getId() >= lowerPacketNumLimit && interestPacket.getId() <= upperPacketNumLimit)
		{
			++measuredHopNum;
			vector<string> substrs;
			splitString(dataPacketName, substrs, '/');
			string highestLevelPrefix = substrs[0];
			// query the static FIB for the highest level prefix
			bool doesExist;
			int staticFace;
			float staticMetric;
			m_staticFib.query(highestLevelPrefix, doesExist, staticFace, staticMetric);
			requiredHopNum += 2*staticMetric;
		}
		++m_userInterestCount;
		m_unmetInterestList.push_back(dataPacketName);
		//cout << "Interest " << dataPacketName << " user " << m_id << "--->" << forwardingFace << endl;
		//}
		
		
		//record the log.
//		cout << "User " << m_id << " initiates  an Interest packet to " << forwardingFace << ": " << dataPacketName << endl; 
		
		// processing the arrival Data packets.
		//if(!m_dataList.empty())
		while(!m_dataList.empty())
		{
			//cout << "hello" << endl;
		/*	if(m_userDataCount < 10000)
			{
	*/		DataPacket dataPacket = m_dataList.front();
			//m_userDataCount++;
			m_dataList.pop_front();
			dataPacket.increaseHopCount();
			if(DataPacket::nack == dataPacket.getType())
			{
				//cout << "User " << m_id << " failed to receive the Data packet: " << dataPacket.getName() << endl;
				//cout << "User request failed: " << dataPacket.getName() << endl;
			}
			else
			{// cout << "User " << m_id << " receives the Data packet from node " << dataPacket.getArrivalFace() << ": " << dataPacket.getName() << endl;
				//cout << "Data " << dataPacket.getName() << " user " << m_id << "<---" << dataPacket.getArrivalFace()<< endl;
				++m_userDataCount;
				int cachingRouterId = dataPacket.getCachingRouterId();
				//cout << "cachingRouterId = " << cachingRouterId << endl;
				if(-1 != cachingRouterId)
				{
					nodes[cachingRouterId].cacheDataPacket(dataPacket);
				}
				list<string>::iterator iter = find(m_unmetInterestList.begin(), m_unmetInterestList.end(), dataPacket.getName());
				m_unmetInterestList.erase(iter);
				vector<string> nameComponents;
				splitString(dataPacket.getName(), nameComponents, '/');
				string prefix = nameComponents[0];
				bool flag;
				int staticFace;
				float staticMetric;
				m_staticFib.query(prefix, flag, staticFace, staticMetric);
				//cout << dataPacket.getName() << ": staticCost = " << 2*staticMetric << ", dynamicCost = " << dataPacket.getHopCount() << ", " ;
				cerr << dataPacket.getHopCount()/float(2*staticMetric) << endl;
				//cout << dataPacket.getHopCount()/float(2*staticMetric) << endl;
				++responsePacketNum;
				//cout << "responsePacketNum = " << responsePacketNum << endl;
				//cout << "responsePacketNum = " << responsePacketNum << endl;
				//cout << m_id <<  " dataCount = " << m_userDataCount << ", interestCount = " << m_userInterestCount << endl;
				
				//cout << m_id << ", userDataCount = " << m_userDataCount << endl; 
			}
			//}
		}
	}
	
	/**
	<@function. producerOperation
	<@brief. When a producer receives an Interest packet, it will determine if it could provide the requested Data packet. If it could provide the 
		requeted Data packet, it will return the requested Data packet. Or it will drop it silently.
	<@attention. The function applies to the producer nodes only.
	*/
	void producerOperation()
	{
//		cout << m_id << ": producerOperation()" << endl;
		//cout << "Producer" <<  m_id << " enters producerOperation()" << endl;
		//if(!m_interestList.empty())
		while(!m_interestList.empty())
		{
			InterestPacket interestPacket = m_interestList.front();
			m_interestList.pop_front();
			interestPacket.increaseHopCount();
			interestPacket.increaseCurrentRouterDist();
			
			string localPrefix = idPrefix[m_id];
			string interestPacketName = interestPacket.getName();
			vector<string> nameComponents;
			splitString(interestPacketName, nameComponents, '/');
			int arrivalFace = interestPacket.getArrivalFace();
			
			//cout << "Interest " << interestPacketName << " producer " << m_id << "<---" << arrivalFace << endl;

			if(localPrefix != nameComponents[0])
			{
				DataPacket dataPacket;
				dataPacket.setName(interestPacketName);
				dataPacket.setArrivalFace(m_id);
				dataPacket.setType(DataPacket::nack);
				dataPacket.setHopCount(dataPacket.getHopCount());
				dataPacket.setId(interestPacket.getId());
				nodes[arrivalFace].pendDataPacket(dataPacket);
				if(dataPacket.getId() >= lowerPacketNumLimit && dataPacket.getId() <= upperPacketNumLimit)
				{
					++measuredHopNum;
				}
				//cout << "Producer " << m_id << " receives the wrong Interest packet from node " << arrivalFace << " : " << interestPacketName << endl;
			}
			else
			{
				//string payload = generateRandomString(0, 1024);
				string payload = "";
				DataPacket dataPacket;
				dataPacket.setName(interestPacketName);
				dataPacket.setPayload(payload);
				dataPacket.setCurrentRouterDist(interestPacket.getCurrentRouterDist());
				dataPacket.setCachingRouterDist(interestPacket.getCachingRouterDist());
				dataPacket.setArrivalFace(m_id);
				//float currentRouterDist = float(dataPacket.getCurrentRouterDist());
				//float cachingRouterDist = float(dataPacket.getCachingRouterDist());
				//if(currentRouterDist - cachingRouterDist > cacheThreshold && cachingRouterDist/currentRouterDist < 0.618)	// I choose the gold ratio as the 
				//{																	// criterion; I don't know why, but I believe it will work well.
				//	dataPacket.setType(DataPacket::normal);
				//}
				//else
				//{
				//	dataPacket.setType(DataPacket::nocache);
				//}
				dataPacket.setType(DataPacket::normal);
				dataPacket.setSize();
				dataPacket.setHopCount(interestPacket.getHopCount());
				dataPacket.setId(interestPacket.getId());
				nodes[arrivalFace].pendDataPacket(dataPacket);
				if(dataPacket.getId() >= lowerPacketNumLimit && dataPacket.getId() <= upperPacketNumLimit)
				{
					++measuredHopNum;
				}
				//cout << "Data " << interestPacketName << " producer " << m_id << "--->" << arrivalFace << endl;
				//cout << "Producer " << m_id << " returns a Data packet to node" << arrivalFace << ": " << interestPacketName << endl;
			}
		}
//		cout << m_id << " leaves producerOperation()" << endl;
	}

	/**
	<@function. getAvailableFace
	<@brief. When an Interest packet needs to be forwarded, we will query the dynamic FIB and the static FIB to get the availabe faces.
		The function returns back the faces we can use to forward the Interest packet.
	<@param. interestPacketName, the name of the Interest packet need to be forwarded. 
	<@param. unavailableFaces, the set of unavailable faces in the Interest packet. The faces in the set won't be chose to forward the Interest packet.
	<@param. resultantFace, a reference variable. If there are some availbe face to forward the Interest packet, the face will be 
		recorded into it. Otherwise the variable will be set to -1;
	*/
	void getAvailableFace(string interestPacketName, set<int> unavailableFaces, int& resultantFace)
	{
		vector<string> substrs;
		splitString(interestPacketName, substrs, '/');
		if(substrs.size() < 3)
		{
			resultantFace = -1;
			return;
		}
		string highestLevelPrefix = substrs[0];
		vector<string>::reverse_iterator strIter = substrs.rbegin();
		++strIter;
		int dataPacketsNum;	//The number of Data packets the source file is devided into.
		istringstream(*strIter) >> dataPacketsNum;
		string trimedName = trimLastComponentFromName(interestPacketName);
		// query the static FIB for the highest level prefix
		bool doesExist;
		int staticFace;
		float staticMetric;
		m_staticFib.query(highestLevelPrefix, doesExist, staticFace, staticMetric); //attention. In the model we don't consider
		// the case where no matching static FIB entry exists temporarily.
		
		// query the dynamic FIB for the trimed name.
		set<FaceInfo> faceInfos;		
		m_dynamicFib.getMatchingFacesMetrics(trimedName, faceInfos);
		
		vector<FaceCost> faceCosts;
		FaceCost tempFaceCost;
		
		// Compute the cost to forward the Interest packet through 
		// the face associated with the matching static FIB entry.
		tempFaceCost.face = staticFace;
		tempFaceCost.cost = 2*staticMetric;
		faceCosts.push_back(tempFaceCost);
		
		// Compute the costs to forward the Interest packet through 
		// the faces associated with the matching dynamic FIB entry.
		for(set<FaceInfo>::iterator iter(faceInfos.begin()), end(faceInfos.end());
			iter != end; ++iter)
		{
			tempFaceCost.face = iter->getFace();
			tempFaceCost.cost = 2*iter->getMetric() + 2*staticMetric*(dataPacketsNum - iter->getNum())/float(dataPacketsNum);
			faceCosts.push_back(tempFaceCost);
		}
		// Rearrange the faces in the order of increasing cost.
		sort(faceCosts.begin(), faceCosts.end());

		// Search for the first face not in the unavailableFaces set.
		vector<FaceCost>::iterator faceCostIter(faceCosts.begin()), faceCostEnd(faceCosts.end());
		for(; faceCostIter != faceCostEnd; ++faceCostIter)
		{
			set<int>::iterator faceIter = unavailableFaces.find(faceCostIter->face);
			if(unavailableFaces.end() == faceIter)
				break;
		}

		if(faceCosts.end() == faceCostIter || faceCostIter->cost > 2*staticMetric)
			resultantFace = -1;
		else resultantFace = faceCostIter->face;
	}
	
	void setBetweennessCentrality(float betweennessCentrality)
	{
		m_betweennessCentrality = betweennessCentrality;
	}
	
	float getBetweennessCentrality() const
	{
		return m_betweennessCentrality;
	}
	
	void setWeight(float weight)
	{
		m_weight = weight;
	}
	
	float getWeight() const
	{
		return m_weight;
	}

	void setWeight()
	{
		float temp = log((double)m_links.size()) + 0.5;
		m_weight = m_contentStore.getCapacity()/temp;
	}
	
	/**
	<@function. setCapacity
	<@brief. Set the capacity of the node's content store.
	<@param. capacity, the value of capacity to be set.
	*/
	void setCapacity(long long capacity)
	{
		m_contentStore.setCapacity(capacity);
	}
	
	/**
	<@function. getCapacity 
	<@brief. Get the node's content store capacity.
	<@return. The node's content store capacity.
	*/
	long long getCapacity() const
	{
		return m_contentStore.getCapacity();
	}	

	/**
	<@function. dropDataPacket
	<@brief. Drop a Data packet from the node's content store and modify the dynamic FIBs of other node accordingly.
	*/
	void dropDataPacket()
	{
		//if(!m_pStore->empty())
		if(!m_contentStore.empty())
		{
			DataPacket dataPacket= m_contentStore.dropDataPacket();
			//cout << "Drop a Data packet: " << dataPacket.getName() << endl;
			//dataPacket.print();
			list<FaceMetric> relevantRouters;
			dataPacket.getRelevantRouters(relevantRouters);
			
			string dataPacketName = dataPacket.getName();
			string trimedName = trimLastComponentFromName(dataPacketName);

			for(list<FaceMetric>::iterator iter(relevantRouters.begin()), end(relevantRouters.end());
				iter != end; ++iter)
			{
				//cout << "Modify the dynamic FIB of router " << iter->router << endl;
				//cout << "Before modifying:" << endl;
				//nodes[iter->router].printDynamicFib();
				nodes[iter->router].m_dynamicFib.eraseRoutingInfo(trimedName, iter->faces, iter->metric);
				//cout << "After modifying: " << endl;
				//nodes[iter->router].printDynamicFib();
			}
		}
	}
	
	/**
	<@function. cacheDataPacket
	<@brief. Cache a Data packet in the content store.
	<@param. dataPacket, the Data packet to be cached.
	*/
	void cacheDataPacket(DataPacket dataPacket)
	{
		while(false == m_contentStore.cacheDataPacket(dataPacket))
		{
			dropDataPacket();
		}
	}

	int getUserInterestCount() const
	{
		return m_userInterestCount;
	}

	int getUserDataCount() const
	{
		return m_userDataCount;
	}

	/**
	<@funcion. printStaticFib
	<@brief. Print the node's static FIB.
	*/
	void printStaticFib()
	{
		m_staticFib.print(m_id);
	}
	
	/**
	<@function. printDynamicFib
	<@brief. Print the node's dynamic FIB.
	*/
	void printDynamicFib()
	{
		m_dynamicFib.print();
	}

	/**
	<@function. printUnmetInterests
	<@brief. Print the names of unmet Interest packets.
	*/
	void printUnmetInterests()
	{
		cout << "In node " << m_id << ":" << endl;
		for(list<string>::iterator iter(m_unmetInterestList.begin()), end(m_unmetInterestList.end());
			iter != end; ++iter)
			cout << *iter << endl;
	}
	
	/**
	<@function. printDataList
	<@brief. Print the names of Data packets in the m_pDataList.
	*/
	void printDataList()
	{
		cout << "Data List:" << endl;
		for(list<DataPacket>::iterator iter(m_dataList.begin()), end(m_dataList.end());
			iter != end; ++iter)
			cout << iter->getName() << endl;
	}

	/**
	<@function. printInterestList
	<@brief. Print the names of Interest packets in the m_pInterestList. 
	*/
	void printInterestList()
	{
		cout << "Interest List: " << endl;
		for(list<InterestPacket>::iterator iter(m_interestList.begin()), end(m_interestList.end());
			iter != end; ++iter)
			cout << iter->getName() << endl;
	}

	/**
	<@function. printWaitingInterestList
	<@brief. Print the names of m_pWaitingInterestList.
	*/
	void printWaitingInterestList()
	{
		cout << "Waiting Interest List:" << endl;
		for(list<InterestPacket>::iterator iter(m_waitingInterestList.begin()), end(m_waitingInterestList.end());
			iter != end; ++iter)
			cout << iter->getName() << endl;
	}
	
	/**
	<@function. printContentStore
	<@brief. Print the content of content store.
	*/
	void printContentStore()
	{
		m_contentStore.print();
	}

	/**
	<@function. printDataPacketsReuseTime
	<@brief. Print out the reuse time of the Data packets in the content store.
	*/
	void printDataPacketsReuseTime()
	{
		m_contentStore.printReuseTime();
	}

	/**
	<@function. dataListEmpty
	<@brief. Check if the node's Data packet list is empty.
	<@return. If the node's Data packet list is empty, the function returns true, or the function returns false.
	*/
	bool dataListEmpty()
	{
		return m_dataList.empty();
	}

	/**
	<@function. interestListEmpty
	<@brief. Check if the node's Interest packet list is empty.
	<@return. If the node's Interest packet list is empty, the function returns true, or the function returns false.
	*/
	bool interestListEmpty()
	{
		return m_interestList.empty();
	}

	/**
	<@function. waitingInterestListEmtpy
	<@brief. check if the node's waiting Interest packet list is empty.
	<@return. If the node's waiting Interest list is empty, the function returns true, or the function returns false.
	*/
	bool waitingInterestListEmpty()
	{
		return m_waitingInterestList.empty();
	}
	
	/**
	<@function. getStaticRoutingInfo
	<@brief. Given a Interest packet name, the function will return the static routing information corresponding to the Interest packet.
	<@param. interestName, the name of the Interest packet to be quried for.
	<@param. flag, a reference variable, if corresponding routing information exists, flag will be set to true, otherwise flag will be set to false.
	<@param. face, a reference variable, in the case where corresponding routing information exists, 
		face will record the face associated with the matching static FIB entry.
	<@param. metric, a reference variable, if corresponding routing information exists, metric will be set to the metric associated
		with the matching static FIB entry.
	*/
	void getStaticRoutingInfo(string interestName, bool& flag, int& face, float& metric)
	{
		vector<string> components;
		splitString(interestName, components, '/');
		string prefix = components[0];
		m_staticFib.query(prefix, flag, face, metric);
	}

	/**
	<@function. printContentStoreStat
	<@brief. Print out the statistic information about the Content Store.
	*/
	void printContentStoreStat()
	{
		list<ContentStoreStat> contentStoreStat;
		m_contentStore.getStat(contentStoreStat);
		cout << "capacity = " << m_contentStore.getCapacity() << endl;
		cout << "remainderCapacity = " << m_contentStore.getRemainderCapacity() << endl;
		for(list<ContentStoreStat>::iterator iter(contentStoreStat.begin()), end(contentStoreStat.end());
			iter != end; ++iter)
			cout << iter->prefix << "\t" << iter->count << endl;
	}
	
	/**
	<@function. peekStatus
	<@brief. Print the current status of the node, include the number of Data packets in its content store,
		Data packet list, number of Interest packets in its Interest list, and so on.
	*/
	void peekStatus() const
	{
		cout << "node " << m_id << ":" << endl;
		string strType;
		switch(m_type)
		{
			case producer: strType = "producer"; break;
			case user: strType = "user"; break;
			case router: strType = "router";
		}
		cout << "number of links is " << m_links.size() << endl;
		cout << "the number of Data packets in content store is " << m_contentStore.getSize() << endl;
		cout << "the number of PIT entry in PIT is " << m_pit.getSize() << endl;
		cout << "the number of static FIB entries is " << m_staticFib.getSize() << endl;
		cout << "the number of dynamic FIB entries is " << m_dynamicFib.getSize() << endl;
		cout << "the number of Data packets in Data list is " << m_dataList.size() << endl;
		cout << "the number of Interest packets in Interest list is " << m_interestList.size() << endl;
		cout << "the number of Interest packets in waiting list is " << m_waitingInterestList.size() << endl;
		cout << "the number of Interest packets in unmet Interest list is " << m_unmetInterestList.size() << endl;
	}
	
	/**
	<@function. print
	<@brief. Print the status of the node.
	*/
	void print()
	{
		cout << "the property of node:" << endl;
		cout << "id: " << m_id << endl;
		cout << "links: ";
		for(set<int>::iterator iter(m_links.begin()), end(m_links.end());
			iter != end; ++iter)
			cout << *iter << " ";
		cout << endl;
		m_contentStore.print();
		m_pit.print();
		m_staticFib.print();
		m_dynamicFib.print();
		printDataList();
		printInterestList();
		printWaitingInterestList();
		cout << "Betweenness Centrality: " << m_betweennessCentrality << endl;
		cout << "Weight: " << m_weight << endl;
		cout << "File to Request: " << m_fileToRequest << endl;
		cout << "Data packet Sequence Number: " << m_dataPacketSeqNum << endl;
		cout << "----------" << endl;
	}

	/**
	<@function. countDataPackets
	<@brief. In the function, the Data packets in this node's content store will be inserted into the given 
		container, while the duplicate Data packets against the existing Data packets will be removed. Thus the container could count 
		the number of unique Data packet there are.
	<@param. containter, the Data packets in the content store will	be inserted into the container, and as the container is a
		set, the duplicate Data packets will be removed automatically.
	*/
	void countDataPackets(set<DataPacket>& container)
	{
		m_contentStore.countDataPackets(container);
	}

	/**
	<@function. getStaticForwardingFaces
	<@brief. Get the forwarding faces in the static FIB of the node.
	<@param. forwardingFaces, a reference variable. The forwarding faces of the node's 
		static FIB will be inserted in it.
	*/
	void getStaticForwardingFaces(vector<int>& forwardingFaces)
	{
		m_staticFib.getForwardingFaces(forwardingFaces);
	}

	private:
	int m_id;	//<@brief The identifier of the node. Every node  in the network will has a unique identifier.
	set<int> m_links;	//<@brief The identifiers of the nodes to which the node is connected. 
	ContentStore m_contentStore;	//<@brief The node's content store.
	Pit m_pit;	//<@brief. Pointer to the Pending Interest Table of the node.
	StaticFib m_staticFib;	//<@brief. Pointer to the static FIB of the node.
	DynamicFib m_dynamicFib;	//<@brief. Pointer to the dynamic FIB of the node.
	Type m_type;	//<@brief. The type of the node, a producer node, a router node, a end user node, or some other kind of node.
	list<DataPacket> m_dataList;	//<@brief. The list for the Data packet need to be processed.
	list<InterestPacket> m_interestList;	//<@brief. The list for the Interest packet need to be processed.
	list<InterestPacket> m_waitingInterestList;	//<brief. For the Interet packets that has been forwarded but neither corresponding nack packet nor 
		// response Data packet has been received.
	float m_betweennessCentrality;	//<@brief. The ego network betweenness centrality of the node. For the reasonning for the ego network betweenness
		// centrality of a node, please refer to Martin Everett and Stephen P. Borgatti's "Ego network betweenness".
	float m_weight;	//<@brief. The weight of a node to cache a given Data packet. Its value depends on the node's ego network betweenness centrality
	// and its content store capacity.
	string m_fileToRequest;	//<@brief. Which file the user will request.
	int m_dataPacketSeqNum;	//<@brief. The sequence number of Data packets to be requested.
	int m_userInterestCount;	//<@brief. The number of Interest packets an end user has initiated.
	int m_userDataCount;	//<@brief. The number of Data packets an end user has received.
	list<string> m_unmetInterestList;	//<@brief. The list records the Interest packets whose 
		// response Data packets has not been received.
};
//bool Node::flag = true;
#endif
