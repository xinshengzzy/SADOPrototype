// PIT.h
#ifndef PIT_H
#define PIT_H

//#include <vld.h>

#include <set>
#include <vector>
#include <list>
#include "PitEntry.h"
using namespace std;

class Pit
{
	public:
	Pit()
	{
		m_pitEntries = set<PitEntry>();
	}

	/**
	<@function. insertEntry
	<@brief. Insert an Pit entry into the Pit due to an Interest packet arrival.
	<@param. name, the name of the Interest packet.
	<@param. arrivalFace, the arrival face of the Interest packet.
	<@param. hopCount, the hopCount of the arrival Interest packet.
	<@param. currentRouterDist, the currentRouterDist property of the Interest packet.
	<@besides. The function applies whether there is a PIT entry for the Interest packet or not. 
	*/
	void insertEntry(string name, int arrivalFace, int hopCount, int currentRouterDist, int interestPacketId)
	{
		PitInfo pitInfo;
		pitInfo.m_arrivalFace = arrivalFace;
		pitInfo.m_hopCount = hopCount;
		pitInfo.m_distance = currentRouterDist;
		pitInfo.m_interestPacketId = interestPacketId;
		PitEntry pitEntry(name, pitInfo);
		std::set<PitEntry>::iterator iter = m_pitEntries.find(pitEntry);
		if(m_pitEntries.end() == iter)
		{
			m_pitEntries.insert(pitEntry);
		}
		else
		{
			pitEntry = *iter;
			pitEntry.addPitInfo(pitInfo);
			m_pitEntries.erase(iter);
			m_pitEntries.insert(pitEntry);
		}
	}

	/**
	<@function. insertEntry
	<@brief. Insert an PIT entry into the PIT due to an Interest packet arrival.
	<@param. name, the name of the Interest packet.
	<@param. arrivalFace, the arrival face of the Interest packet.
	<@param. hopCount, the hopCount of the arrival Interest packet.
	<@param. currentRouterDist, the currentRouterDist property of the Interest packet.
	<@param. forwardingFace, the forwarding face associated with the pit entry.
	<@besides. The function applies whether there is a PIT entry for the Interest packet or not. 
	*/
	void insertEntry(string name, int arrivalFace, int hopCount, int currentRouterDist, int interestPacketId, int forwardingFace)
	{
		PitInfo pitInfo;
		pitInfo.m_arrivalFace = arrivalFace;
		pitInfo.m_hopCount = hopCount;
		pitInfo.m_distance = currentRouterDist;
		pitInfo.m_interestPacketId = interestPacketId;
		PitEntry pitEntry(name, pitInfo, forwardingFace);
		set<PitEntry>::iterator iter = m_pitEntries.find(pitEntry);
		if(m_pitEntries.end() == iter)
		{
			m_pitEntries.insert(pitEntry);
		}
		else
		{
			pitEntry = *iter;
			pitEntry.addPitInfo(pitInfo);
			pitEntry.setForwardingFace(forwardingFace);
			m_pitEntries.erase(iter);
			m_pitEntries.insert(pitEntry);
		}
	}

	/**
	<@function. getForwardingFace
	<@brief. get the forwarding face of the PIT entry with which the given name is associated.
	<@param. interestName, the name of the Interest packet of which we want to get the forwarding face
	<@return. The function returns the forwarding face associated with the given name if a PIT entry corresponding to
		the given name exists. Otherwise the function will return -1.
	*/
	int getForwardingFace(string interestName)
	{
		PitEntry pitEntry(interestName);
		set<PitEntry>::iterator iter = m_pitEntries.find(pitEntry);
		if(m_pitEntries.end() == iter)
			return -1;
		return iter->getForwardingFace();
	}

	/**
	<@function. setForwardingFace
	<@brief. Set the forwarding face of some PIT entry.
	<@param. name, the Interest packet name corresponding to pit entry for which the forwarding face will be set.
	<@param. forwardingFace, the forwarding face that will be set to.
	<@return. If a pit entry corresponding to the given name, the function will set the forwarding face and return true, otherwise the function will return false.
	*/
	bool setForwardingFace(string name, int forwardingFace)
	{
		PitEntry pitEntry(name);
		set<PitEntry>::iterator iter = m_pitEntries.find(pitEntry);
		if(m_pitEntries.end() == iter)
			return false;
		pitEntry = *iter;
		pitEntry.setForwardingFace(forwardingFace);
		m_pitEntries.erase(iter);
		m_pitEntries.insert(pitEntry);
		return true;
	}
	
	/**
	<@function. matchingEntryExists
	<@brief. Check if there is a PIT entry matching with the given name.
	<@param. name, the name will be checked against.
	<@return. Return true if matching entry exists, otherwise return false
	<@attention. Even if the face list of the matching entry is empty, the function will return true. (although I don't know how this would happen.) 
	*/
	bool matchingEntryExists(string name)
	{
		PitEntry pitEntry(name);
		set<PitEntry>::iterator iter = m_pitEntries.find(pitEntry);
		if(iter == m_pitEntries.end()) return false;
		return true;
	}
	
	/**
	<@function. getPitInfos
	<@brief. Get the PitInfo list corresponding to a given Data packet's name.
	<@param. name, the name of a Data packet
	<@param. pitInfos, a reference variable, if there is an entry for the Data packet name, and the PitInfo list of the
			 entry is not empty, the PitInfos in the list will be stored in it. Otherwise the container will be left empty. 
	*/
	void getPitInfos(string name, list<PitInfo>& pitInfos)
	{
		pitInfos.clear();
		PitEntry pitEntry(name);
		set<PitEntry>::iterator iter = m_pitEntries.find(pitEntry);
		if(m_pitEntries.end() != iter)
		{
			iter->getPitInfos(pitInfos);
		}
	}
	
	/**
	<@function dropEntry
	<@brief Drop the Pit entry to which the given name is associated.
	<@param name, the name associated to the PIT entry to be dropped.
	*/
	void dropEntry(string name)
	{
		PitEntry pitEntry(name);
		set<PitEntry>::iterator iter = m_pitEntries.find(pitEntry);
		if(iter != m_pitEntries.end())
			m_pitEntries.erase(iter);
	}

	/**
	<@function. getSize
	<@brief. Get the number of PIT entries in the PIT.
	*/
	int getSize() const
	{
		return m_pitEntries.size();
	}

	/**
	<@function. print
	<@brief. Print out the status of the PIT.
	*/
	void print()
	{
		cout << "the property of PIT:" << endl;
		for(set<PitEntry>::iterator iter(m_pitEntries.begin()), end(m_pitEntries.end());
			iter != end; ++iter)
			iter->print();
		cout << "----------" << endl;
	}

	private:
	set<PitEntry> m_pitEntries;
};

#endif
