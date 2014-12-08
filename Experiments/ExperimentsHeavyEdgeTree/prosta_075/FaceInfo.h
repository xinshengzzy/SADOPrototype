// FaceInfo.h
#ifndef FACEINFO_H
#define FACEINFO_H

//#include <vld.h>

class FaceInfo	//<@brief Record the relevant information about a face associated to a particular prefix.
{	
	public:
	FaceInfo(int face, float metric, long long num, int lifetime)
	{
		m_face = face;
		m_metric = metric;
		m_num = num;
		m_lifetime = lifetime;
	}
	
	FaceInfo(int face)
	{
		m_face = face;
		m_metric =0;
		m_num = 0;
		m_lifetime = 0;
	}
	
	FaceInfo(const FaceInfo& other)
	{
		m_face = other.getFace();
		m_metric = other.getMetric();
		m_num = other.getNum();
		m_lifetime = other.getLifetime();
	}
	
	bool operator<(const FaceInfo& other) const
	{
		return m_face < other.m_face;
	}
	
	void operator=(const FaceInfo& other)
	{
		m_face = other.getFace();
		m_metric = other.getMetric();
		m_num = other.getNum();
		m_lifetime = other.getLifetime();
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
	
	void setNum(long long num)
	{
		m_num = num;
	}
	
	long long getNum() const
	{
		return m_num;
	}
	
	void setLifetime(int lifetime)
	{
		m_lifetime = lifetime;
	}
	
	int getLifetime() const
	{
		return m_lifetime;
	}

	/**
	<@function. print
	<@brief. Print the content of the class.
	*/
	void print() const
	{
		cout << "face = " << m_face << ", metric = " << m_metric << ", num = " << m_num << ", lifetime = " << m_lifetime << endl;
	}
	
	private:
	int m_face;	  //<@brief Throught which the caching Data packets could be reacched.
	float m_metric;    //<@brief The distance from the current router to the caching router.
	long long m_num;       //<@brief The number of Data packets cached at the caching router.
	int m_lifetime;	//<@brief The lifetime of the face.
};
#endif
