// StaticFib.h
#ifndef STATIC_FIB_H
#define STATIC_FIB_H

//#include <vld.h>

#include "StaticFibEntry.h"
#include <set>
#include <utility>
using namespace std;

class StaticFib
{
	public:
	StaticFib()
	{
		m_entries = set<StaticFibEntry>();
	}
	
	~StaticFib()
	{
	}

	/**
	<@function insertEntry
	<@brief Insert a static FIB entry according to the given prefix, face, and metric
	<@param prefix, the prefix corresponding to the new entry
	<@param face, the face corresponding to the new entry
	<@param metric, the metric corresponding to the new metric
	<@attention If an entry with the given name has existed, the entry will be replaced by the new one.
	*/
	void insertEntry(string prefix, int face, float metric)
	{
		StaticFibEntry entry(prefix, face, metric);
		std::set<StaticFibEntry>::iterator iter = m_entries.find(entry);
		if(m_entries.end() != iter)
		{
			m_entries.erase(iter);
		}
		m_entries.insert(entry);
	}
	
	/**
	<@function matchingEntryExists
	<@brief Check if an static FIB entry with the given name has existed.
	<@param prefix, the prefix will be checked against.
	<@return Return true if matching entry does exist, otherwise return false.
	*/
	bool matchingEntryExists(std::string prefix)
	{
		StaticFibEntry entry(prefix);
		std::set<StaticFibEntry>::iterator iter = m_entries.find(entry);
		if(m_entries.end() == iter)
			return false;
		return true;
	}
	
	/**
	<@function. query
	<@brief. Extract the face and metric corresponding to a given prefix.
	<@param. prefix, the prefix corresponding to which the face and metric will be extracted.
	<@param. flag, a reference variable, if matching entry doesn't exist, flag will be set to false, otherwise it will be set to true.
	<@param. face, a reference variable, in the case where a matching entry exists, face is set to the face associated with the matching entry.
	<@param. metric, a reference variable, in the case where a matching entry exists, metric is set to the metric associated with the matching entry.
	*/
	void query(std::string prefix, bool& flag, int& face, float& metric)
	{
		StaticFibEntry entry(prefix);
		std::set<StaticFibEntry>::iterator iter = m_entries.find(entry);
		if(m_entries.end() == iter)
		{
			flag = false;
			return;
		}
		flag = true;
		face = iter->getFace();
		metric = iter->getMetric();
	}
	
	/**
	<@function dropEntry
	<@brief Drop the FIB entry corresponding to the given prefix.
	<@param prefix, the prefix corresponding to which the FIB entry will be dropped.
	*/
	void dropEntry(std::string prefix)
	{
		StaticFibEntry entry(prefix);
		m_entries.erase(entry);
	}
	
	/**
	<@function updateFace
	<@brief Update the face associated to a prefix
	<@param prefix, the prefix associated to which the face will be updated.
	<@param face, the new face as which the face associated with the prefix.
	<@attention The function is applied only when the FIB entry matched with the given prefix exists.
				In the case where matching FIB entry doesn't exist, there will be no operation.
	*/
	void updateFace(std::string prefix, int face)
	{
		StaticFibEntry entry(prefix);
		entry.setFace(face);
		std::set<StaticFibEntry>::iterator iter = m_entries.find(entry);
		if(m_entries.end() != iter)
		{
			entry.setMetric(iter->getMetric());
			m_entries.erase(iter);
		}
		m_entries.insert(entry);
	}
	
	/**
	<@function updateMetric
	<@brief UPdate the metric associated with a given prefix to a new value.
	<@param prefix, the prefix associated to which the metric will be updated.
	<@param metric, the new metric will be associated to the given prefix
	<@attenttion The function is applied only when a FIB entry matched with the given prefix exists.
				In the case where there is no matching entry exists, the function is of no effect.
	*/
	void updateMetric(std::string prefix, float metric)
	{
		StaticFibEntry entry(prefix);
		entry.setMetric(metric);
		std::set<StaticFibEntry>::iterator iter = m_entries.find(entry);
		if(m_entries.end() != iter)
		{
			entry.setFace(iter->getFace());
			m_entries.erase(iter);
		}
		m_entries.insert(entry);
	}

	/**
	<@function. print
	<@brief. Print the node's static FIB.
	*/
	void print()
	{
		cout << "the property of static FIB:" << endl;
		for(set<StaticFibEntry>::iterator iter(m_entries.begin()), end(m_entries.end());
			iter != end; ++iter)
		{
			cout << "prefix = " << iter->getPrefix() << ", face = " << iter->getFace() << ", metric = " << iter->getMetric() << endl;
		}
		cout << "----------" << endl;
	}

	/**
	<@function. print
	<@brief. Print the node's static FIB in the form of pairs.
	<@param. id, the id of the router the FIB is located.
	*/
	void print(int id)
	{
		for(set<StaticFibEntry>::iterator iter(m_entries.begin()), end(m_entries.end());
			iter != end; ++iter)
			cout << id << " " << iter->getFace() << endl;
	}

	/**
	<@function. getSize
	<@brief. Get the number of FIB entries in the static FIB.
	*/
	int getSize() const
	{
		return m_entries.size();
	}

	/**
	<@function. getForwardingFaces
	<@brief. Get the forwarding faces of the static FIB.
	<@param. forwardingFaces
	*/
	void getForwardingFaces(vector<int>& forwardingFaces)
	{
		forwardingFaces.clear();
		for(set<StaticFibEntry>::iterator iter(m_entries.begin()), end(m_entries.end());
			iter != end; ++iter)
		{
			forwardingFaces.push_back(iter->getFace());
		}
	}

	private:
	std::set<StaticFibEntry> m_entries;
};

#endif
