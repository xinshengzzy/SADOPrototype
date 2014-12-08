// DynamicFib.h
#ifndef DYNAMIC_FIB_H
#define DYNAMIC_FIB_H

//#include <vld.h>

#include <set>
#include <string>
#include <vector>

#include "DynamicFibEntry.h"
#include "FaceInfo.h"
using namespace std;

class DynamicFib
{
	public:
	DynamicFib()
	{
		m_entries = set<DynamicFibEntry>();
	}

	~DynamicFib()
	{
	}
	
	/**
	<@function. addRoutingInfo
	<@brief. This happens when we need to establish routing information for a Data packet.
			If there is no dynamic FIB entry for the given prefix, we will install a new entry for it. 
			If there is a entry for the prefix, the relevant information about the entry will be updated.
	<@param. prefix, the prefix for which we need to establish routing information.
	<@param. metric, the distance from the caching router to the current router.
	<@param. faces, the face list associated with the routing information.
	*/
	void addRoutingInfo(string prefix, vector<int> faces, float metric)
	{
		DynamicFibEntry entry(prefix);
		set<DynamicFibEntry>::iterator iter = m_entries.find(entry);
		if(m_entries.end() == iter)	//There is not a FIB entry for the prefix; install an entry for it.
		{
			entry.addRoutingInfo(faces, metric);
		}
		else
		{
			entry = *iter;
			entry.addRoutingInfo(faces, metric);
			m_entries.erase(iter);
		}
		m_entries.insert(entry);
	}
	
	/**
	<@function. eraseRoutingInfo
	<@brief. That the function is called means a Data packet for which the dynamic FIB maintains routing information has been dropped,
		so we need to update the relevant routing information in the dynamic FIB.
	<@param. prefix, the file-name-part prefix of the dropped Data packet.
	<@param. face, the ID of  router in which the dropped Data packet is located.
	<@param. metric, the distance from the router in which the dropped Data packet is located to the this router.
	*/
	void eraseRoutingInfo(string prefix, vector<int> faces, float metric)
	{
		DynamicFibEntry entry(prefix);
		set<DynamicFibEntry>::iterator iter = m_entries.find(entry);
		if(m_entries.end() == iter)
			return;	// No relevat dynamic FIB entry exists in the router.
		entry = *iter;
		m_entries.erase(iter);
		entry.eraseRoutingInfo(faces, metric);
		if(0 != entry.getFacesNum())
			m_entries.insert(entry);
	}

	/**
	<@function. timeElapse
	<@brief. As a span of time elapse, the lifetime of the FIB entries will be updated. The timed-out faces will be removed from their
				home FIB entries. The FIB entries with no faces will be dropped from the FIB.
	<@param. deviation, the amount of elapsed time.
	*/
	void timeElapse(int deviation)
	{
		std::set<DynamicFibEntry> entries;
		std::set<DynamicFibEntry>::iterator iter(m_entries.begin()), end(m_entries.end());
		for(; iter != end; ++iter)
		{
			DynamicFibEntry entry(*iter);
			entry.decreaseLifetime(deviation);
			if(entry.getFacesNum() > 0)
				entries.insert(entry);
		}
		m_entries.clear();
		iter = entries.begin(); 
		end = entries.end();
		for(; iter != end; ++iter)
			m_entries.insert(*iter);
	}
	
	/**
	<@function. matchingEntryExists
	<@brief. Check if there is a FIB entry matching the given prefix.
	<@param. prefix, the prefix to be checked against.
	<@return. Return true if a matching entry exist, return false otherwise.
	*/
	bool matchingEntryExists(std::string prefix)
	{
		DynamicFibEntry entry(prefix);
		std::set<DynamicFibEntry>::iterator iter = m_entries.find(entry);
		if(m_entries.end() != iter) return true;
		else return false;
	}
	
	/**
	<@function. getMatchingFaceInfos
	<@brief. Get the face infos in the matching entry, if any.
	<@param. prefix, the destination prefix whose relevant face infos are to be extracted.
	<@param. faceInfos, a vector container and a reference variable. The face infos in the matching FIB entry will be stored in it, if any.
			If there is no matching FIB entry or the face list in the FIB entry is empty, the container will be left empty.
	*/
	void getMatchingFacesMetrics(std::string prefix, set<FaceInfo>& faceInfos)
	{
		faceInfos.clear();
		DynamicFibEntry entry(prefix);
		std::set<DynamicFibEntry>::iterator iter = m_entries.find(entry);
		if(m_entries.end() == iter)
		{
			return;
		}
		entry = *iter;
		entry.getFaceInfos(faceInfos);
	}

	/**
	<@function. print
	<@brief. Print the content of the FIB
	*/
	void print()
	{
		cout << "dynamic FIB:" << endl;
		for(set<DynamicFibEntry>::iterator iter(m_entries.begin()), end(m_entries.end());
			iter != end; ++iter)
			iter->print();
	}

	/**
	<@function. getSize
	<@brief. Get the number of FIB entries in the dynamic FIB.
	*/
	int getSize() const
	{
		return m_entries.size();
	}

	/**
	<@function. getEntry
	<@brief. Given a prefix, the function will return the dynamic FIB entry corresponding to it.
	<@param. prefix, the prefix for which we will retrieve the dynamic FIB entry.
	<@param. entry, a reference variable, the returned dynamic FIB entry will be stored in it.
	*/
	void getEntry(string prefix, DynamicFibEntry& entry) const
	{
		DynamicFibEntry tempEntry(prefix);
		set<DynamicFibEntry>::const_iterator iter = m_entries.find(tempEntry);
		if(m_entries.end() == iter)
		{
			entry.setPrefix("<none>");
			entry.clearFaceInfos();
		}
		else entry = *iter;
	}
	
	private:
	std::set<DynamicFibEntry> m_entries;
};

#endif
