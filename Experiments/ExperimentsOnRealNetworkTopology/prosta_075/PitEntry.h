// PITEntry.h
#ifndef PIT_ENTRY_H
#define PIT_ENTRY_H

//#include <vld.h>

#include <string>
#include <set>
#include <utility>
#include <vector>
#include <list>
#include <algorithm>

#include "components.h"
using namespace std;

class PitEntry
{
	public:
	PitEntry(std::string name, PitInfo pitInfo)
	{
		m_name = name;
		m_pitInfos = list<PitInfo>();
		m_pitInfos.push_back(pitInfo);
		m_forwardingFace = -1;
	}

	PitEntry(std::string name, PitInfo pitInfo, int forwardingFace)
	{
		m_name = name;
		m_pitInfos = list<PitInfo>();
		m_pitInfos.push_back(pitInfo);
		m_forwardingFace = forwardingFace;
	}

	PitEntry(std::string name)
	{
		m_name = name;
		m_forwardingFace = -1;
		m_pitInfos = list<PitInfo>();
	}
	
	PitEntry(const PitEntry& other)
	{
		m_name = other.m_name;
		m_pitInfos = list<PitInfo>();
		for(list<PitInfo>::const_iterator iter(other.m_pitInfos.begin()), end(other.m_pitInfos.end());
			iter != end; ++iter)
		{
			m_pitInfos.push_back(*iter);
		}
		m_forwardingFace = other.m_forwardingFace;
	}
	
	bool operator<(const PitEntry & other) const
	{
		return m_name < other.m_name;
	}
	
	void operator=(const PitEntry& other)
	{
		m_name = other.m_name;
		m_pitInfos = list<PitInfo>();
		for(list<PitInfo>::const_iterator iter(other.m_pitInfos.begin()), end(other.m_pitInfos.end());
			iter != end; ++iter)
		{
			m_pitInfos.push_back(*iter);
		}
		m_forwardingFace = other.m_forwardingFace;
	}

	void setName(string name)
	{
		m_name = name;
	}
	
	std::string getName() const
	{
		return m_name;
	}
			
	void addPitInfo(PitInfo pitInfo)
	{
		m_pitInfos.push_back(pitInfo);
	}
	
	void removePitInfo(PitInfo pitInfo)
	{
		m_pitInfos.remove(pitInfo);
	}	
	/**
	<@function. getPitInfos
	<@brief. Get the PitInfo list associated with the Pit entry
	<@param. pitInfos, reference variable, if the PitInfo list is not empty, the PitInfos 
			associated with the entry will be stored in it, otherwise the container will be left empty. 
	*/
	void getPitInfos(list<PitInfo>& pitInfos) const
	{
		pitInfos.clear();
		list<PitInfo>::const_iterator iter(m_pitInfos.begin()), end(m_pitInfos.end());
		while(iter != end)
		{
			pitInfos.push_back(*iter);
			++iter;
		}
	}
	
	//<@function arrivalFacesEmpty
	//<@brief Test if the face list associated with the PIT entry empty.
	//<@return Return true is there is no face in the face list, return false otherwise.
	bool pitInfosEmpty()
	{
		return m_pitInfos.empty();
	}
	
	void setForwardingFace(int forwardingFace)
	{
		m_forwardingFace = forwardingFace;
	}

	int getForwardingFace() const
	{
		return m_forwardingFace;
	}
	
	/**
	<@function. print
	<@brief. Print out the content of the PIT entry.
	*/
	void print()
	{
		cout << "PIT entry:" << endl;
		cout << "name: " << m_name << endl;
		cout << "forwarding face: " << m_forwardingFace << endl;
		cout << "arrival faces: ";
		for(list<PitInfo>::iterator iter(m_pitInfos.begin()), end(m_pitInfos.end());
			iter != end; ++iter)
			iter->print();
	}

	private:
	std::string m_name;	//<@brief The Interest packet's name corresponding to the PIT entry.
	list<PitInfo> m_pitInfos;	//<@brief Record the information about the faces through which the response Data packet will be forwarded.
	int m_forwardingFace;	//<@brief. The face through which the Interest packet corresponding to the PIT entry has been forwarded.
};


#endif
