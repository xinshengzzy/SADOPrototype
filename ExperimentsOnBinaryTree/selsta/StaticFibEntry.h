// StaticFibEntry.h
/**
The static FIB entry records the routing information about the producer of Data packets. 
The static FIB entry is in the form of <prefix, face, metric>, where prefix corresponds to a particular producer, 
face is identifies the shortest path to the producer of the Data packets, 
and metric is the distance from the current router to the producer corresponding to the prefix. For the specifics, 
please refer to our paper "'Do My Best' Routing in Named Data Networking".
Attention: It's obvious that the metric in the static FIB should always be positive integer, 
but in the dynamic FIB, the metric could be float, so to be coherent with the dynamic FIB, we use float for metric.
*/
#ifndef STATIC_FIB_ENTRY_H
#define STATIC_FIB_ENTRY_H

//#include <vld.h>

#include <string>
using namespace std;

class StaticFibEntry
{
	public:
	StaticFibEntry()
	{
		m_prefix = "";
		m_face = 0;
		m_metric = 0;
	}
	
	StaticFibEntry(std::string prefix, int face, float metric)
	{
		m_prefix = prefix;
		m_face = face;
		m_metric = metric;
	}
	
	StaticFibEntry(std::string prefix)
	{
		m_prefix = prefix;
		m_face = 0;
		m_metric = 0;
	}
	
	StaticFibEntry(const StaticFibEntry& other)
	{
		m_prefix = other.getPrefix();
		m_face = other.getFace();
		m_metric = other.getMetric();
	}
	
	void operator=(const StaticFibEntry& other)
	{
		m_prefix = other.getPrefix();
		m_face = other.getFace();
		m_metric = other.getMetric();
	}

	bool operator<(const StaticFibEntry& other) const
	{
		return m_prefix < other.getPrefix();
	}
	
	void setPrefix(std::string prefix)
	{
		m_prefix = prefix;
	}
	
	string getPrefix() const
	{
		return m_prefix;
	}
	
	void setFace(int face)
	{
		m_face = face;
	}
	
	int getFace() const
	{
		return m_face;
	}
	
	void setMetric(float metric)
	{
		m_metric = metric;
	}
	
	float getMetric() const
	{
		return m_metric;
	}
	
	void increaseMetric(int deviation)
	{
		m_metric += deviation;
	}
	
	void decreaseMetric(int deviation)
	{
		m_metric -= deviation;
	}
	private:
	std::string m_prefix;	//<@brief The prefix corresponding to the FIB entry
	int m_face;		//<@brief The face associated with the FIB entry
	float m_metric;		//<@brief The metric associated with the FIB entry. 
};

#endif
