// DynamicFibEntry.h
#ifndef DYNAMIC_FIB_ENTRY_H
#define DYNAMIC_FIB_ENTRY_H

//#include <vld.h>

#include <string>
#include <set>
#include <vector>

#include "FaceInfo.h"
using namespace std;

extern int fibFaceLifetime;

class DynamicFibEntry
{
	public:
	DynamicFibEntry(std::string prefix)
	{
		m_prefix = prefix;
		m_faceInfos = set<FaceInfo>();
	}
		
	DynamicFibEntry(const DynamicFibEntry& other)
	{
		m_prefix = other.getPrefix();
		m_faceInfos = set<FaceInfo>();
		other.getFaceInfos(m_faceInfos);
	}
	DynamicFibEntry()
	{
		m_prefix = "";
		m_faceInfos = set<FaceInfo>();
	}
	
	DynamicFibEntry(string prefix, vector<int> faces, float metric)
	{
		m_prefix = prefix;
		m_faceInfos = set<FaceInfo>();
		addRoutingInfo(faces, metric);
	}

	~DynamicFibEntry()
	{
	}

	void setPrefix(std::string prefix)
	{
		m_prefix = prefix;
	}
		
	std::string getPrefix() const
	{
		return m_prefix;
	}
	
	/**
	<@function. decreaseLifetime
	<@brief. Decrease the lifetime of every face by a given value.
	<@param. deviation, the value by which the lifetime of faces will be decreased.
	<@besides. If some face's resultant lifetime is less than 0, the infomation relevant with the face will be removed.
	*/
	void decreaseLifetime(int deviation)
	{
		std::set<FaceInfo> faceInfos;
		std::set<FaceInfo>::iterator iter = m_faceInfos.begin();
		for(; iter != m_faceInfos.end(); ++iter)
		{
			if(iter->getLifetime() < deviation) continue;
			FaceInfo faceInfo(*iter);
			faceInfo.setLifetime(faceInfo.getLifetime() - deviation);
			faceInfos.insert(faceInfo);
		}	
		m_faceInfos.clear();
		iter = faceInfos.begin();
		for(; iter != faceInfos.end(); ++iter)
			m_faceInfos.insert(*iter);
	}
	
	/**
	<@function getFacesNum
	<@brief Get the number of available faces
	<@return The number of faces which could be used to forward Interest packet.
	*/
	int getFacesNum() const
	{
		return m_faceInfos.size();
	}
	
	/**
	<@function. addRoutingInfo
	<@brief. When the function is called, that means that we need to install dynamic routing information for a Data packet.
			For the reasons of the operations here please refer our paper "'Do My Best' Routing in Named Data Networking". 
			For every relevant face, its life time will be updated to a given value, 
			its metric will be updated to a new value, and its value of num will be increased by 1.
	<@param. faces, the faces need to be updated.
	<@param. metric, the distance from caching router to current router.
	*/
	void addRoutingInfo(std::vector<int> faces, float metric)
	{
		std::vector<int>::iterator iter(faces.begin()), end(faces.end());
		for(; iter != end; ++iter)
		{
			FaceInfo faceInfo(*iter);
			std::set<FaceInfo>::iterator infoIter = m_faceInfos.find(faceInfo);
			if(m_faceInfos.end() == infoIter)	//It's a new face, create a quadruple for it.
			{
				faceInfo.setMetric(metric);
				faceInfo.setNum(1);
				faceInfo.setLifetime(fibFaceLifetime);
				m_faceInfos.insert(faceInfo);
			}
			else //A quadruple corresponding to the face has existed, update the quadruple.
			{
				faceInfo = *infoIter;
				if(metric != faceInfo.getMetric())
				{
					float newMetric = (faceInfo.getMetric()*faceInfo.getNum() + metric)/(faceInfo.getNum() + 1);
					faceInfo.setMetric(newMetric);
				}
				faceInfo.setNum(faceInfo.getNum() + 1);
				faceInfo.setLifetime(fibFaceLifetime);
				m_faceInfos.erase(infoIter);
				m_faceInfos.insert(faceInfo);
			}
		}		
	}
	
	/**
	<@function. eraseRoutingInfo
	<@brief. When the function is called, it means a Data packet for which we have maintained dynamic routing information has 
		been dropped, so we need to update the dynamic FIB entry accordingly.
	<@param. face, where the dropped Data packet is located.
	<@param. metric, the distance from the router in which the dropped Data packet is located and the current router.
	*/
	void eraseRoutingInfo(vector<int> faces, float metric)
	{
		for(vector<int>::iterator iter(faces.begin()), end(faces.end());
			iter != end; ++iter)
		{
			FaceInfo faceInfo(*iter);
			set<FaceInfo>::iterator iterFaceInfo = m_faceInfos.find(faceInfo);
			if(m_faceInfos.end() == iterFaceInfo)
				continue;	// The corresponding face doesn't exist, do nothing.
			if(1== iterFaceInfo->getNum())
			{
				m_faceInfos.erase(iterFaceInfo);
			}
			else
			{
				faceInfo = *iterFaceInfo;
				m_faceInfos.erase(iterFaceInfo);
				float newMetric = faceInfo.getMetric()*faceInfo.getNum()-metric;
				newMetric = newMetric/(faceInfo.getNum() - 1);
				faceInfo.setMetric(newMetric);
				faceInfo.setNum(faceInfo.getNum() - 1);
				faceInfo.setLifetime(fibFaceLifetime);
				m_faceInfos.insert(faceInfo);
			}
		}
	}

	/**
	<@function. clearFaceInfos
	<@brief. Clear the face infos stored in the dynamic FIB entry.
	*/
	void clearFaceInfos()
	{
		m_faceInfos.clear();
	}

	/**
	<@function. dropFace
	<@brief. Drop a face as well as its relevant information from the FIB entry.
	<@param. face, the face to be dropped.
	*/
	void dropFace(int face)
	{
		FaceInfo faceInfo(face);
		m_faceInfos.erase(faceInfo);
	}
	
	/**
	<@function. getFaceInfos
	<@brief. Get the face infos associated with the FIB entry
	<@param. faceInfos, it is a vector container, a reference variable. The face infos associated with the FIB entry will be 
			stored in the container, if any. If there is no face infos, the container will be left empty. 
	*/
	void getFaceInfos(set<FaceInfo>& faceInfos) const
	{
//		return std::make_pair(m_pfacesInfo->begin(), m_pfacesInfo->end());
		faceInfos.clear();
		set<FaceInfo>::const_iterator iter(m_faceInfos.begin()), end(m_faceInfos.end());
		for(; iter != end; ++iter)
			faceInfos.insert(*iter);
	}
		
	bool operator<(const DynamicFibEntry& other) const
	{
		return m_prefix < other.getPrefix();
	}
	
	void operator=(DynamicFibEntry other)
	{
		m_prefix = other.getPrefix();
		m_faceInfos.clear();
		other.getFaceInfos(m_faceInfos);
	}

	/**
	<@function. print
	<@brief. Print the content of the FIB entry.
	*/
	void print() const
	{
		cout << m_prefix << endl;
		for(set<FaceInfo>::const_iterator iter(m_faceInfos.begin()), end(m_faceInfos.end());
			iter != end; ++iter)
			iter->print();
	}
	
	private:
	std::string m_prefix;	//<@brief The prefix associated with the FIB entry.
	std::set<FaceInfo> m_faceInfos;	//<@brief The relevant information of every face associated with the FIB entry.
};

#endif
