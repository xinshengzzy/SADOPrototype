//utility.cpp

//#include <vld.h>

#include <string>
#include <vector>
#include <queue>
#include <sstream>
#include <iostream>
#include <cstdlib>
#include <fstream>
#include <map>
#include <ctime>
#include <cmath>
#include <algorithm>
#include <utility>

#include "components.h"
#include "utility.h"
//#include "Node.h"
//#include "NetworkConfig.h"
using namespace std;

extern int nodesNum;
extern int file_number;
extern vector<pair<int, int> > links;

typedef unsigned short int crc;
extern crc crcLookupTable[256];
//extern vector<Node> nodes;

/**
<@function. splitString
<@brief. Split a string by the the given delimiter. And save every segment in a vector.
<@param. line, the string to be processed.
<@param. v, a reference variable. The resultant segments of the original string will be saved in it.
<@param. delimiter, the delimiter by which we will split the original string.
*/
void splitString(string line, vector<string>& v, char delimiter)
{
	v.clear();
	istringstream iss(line);
//	string delimiter = " \t,.;:'\"";
	string substr;
//	while(getline(iss, substr, delimiter))
	while(getline(iss, substr, delimiter))
	{
//		cout << substr << endl;
		v.push_back(substr);
	}
}


/**
<@function. parseProducerConfigFile
<@brief. Parse the IDs of producers and the highest level prefixes corresponding to each producer from PRODUCER_CONFIG_FILE
<@param. pairs, a reference variable, a map container of <id, prefix> pair.
*/
void parseProducerConfigFile(map<int, string>& pairs)
{
	ifstream inFile(PRODUCER_CONFIG_FILE);
	if(!inFile)
	{
		cerr << "Unable to open file: " << PRODUCER_CONFIG_FILE << endl;
		exit(1);
	}
	string line;
	while(getline(inFile, line))
	{
//		cout << line << endl;
		if('#' == line[0]) continue;
		vector<string> tempVector;
		splitString(line, tempVector, ' ');
		int tempId;
		istringstream(tempVector[0]) >> tempId;
		//pairs.push_back(make_pair(tempId, tempVector[1]));
		pairs[tempId] = tempVector[1];
	}
}



/**
<@function. constructShortestPath
<@brief. Construct the shortest path for a given node to any other nodes in the network.
<@param. origin, the node for which we will construct the shortest pathes.
<@param. nodesNum, the number of nodes in the network.
<@param. links, the links between nodes in the network.
<@param. pathInfos, a reference variable. It record the nexthop and metric corresponding to the shortest path to any other node.
		The ith element of the container records the information for the ith nodes.
<@attention. The function applies to a simplified model where the metric between any two joint nodes is 1.
*/
void constructShortestPath(int origin, int nodesNum, vector<pair<int, int> > links, vector<PathInfo>& pathInfos)
{
	pathInfos.clear();
	//int metric[nodesNum][nodesNum];
	//int** metric = new int[nodesNum][nodesNum];
	int** metric = new int*[nodesNum];
	for(int i = 0; i < nodesNum; ++i)
		metric[i] = new int[nodesNum];
	for(int i = 0; i < nodesNum; ++i)
		for(int j = 0; j < nodesNum; ++j)
			metric[i][j] = -1;

	vector<pair<int, int> >::iterator iter(links.begin()), end(links.end());
	for(; iter != end; ++iter)
	{
		int node1 = iter->first;
		int node2 = iter->second;
		metric[node1][node2] = 1;
		metric[node2][node1] = 1;
	}

	for(int i = 0; i < nodesNum; ++i)
	{
		PathInfo pathInfo;
		pathInfo.flag = false;
		pathInfo.metric = -1;
		pathInfo.face = -1;
		pathInfos.push_back(pathInfo);
	}

	pathInfos[origin].metric = 0;
	pathInfos[origin].flag = true;
	pathInfos[origin].face = origin;
	queue<int> que;
	que.push(origin);
	while(!que.empty())
	{
		int temp = que.front();
		que.pop();
		for(int i = 0; i < nodesNum; ++i)
		{
			if(false == pathInfos[i].flag && 1 == metric[temp][i])
			{
				pathInfos[i].metric = pathInfos[temp].metric + 1;
				pathInfos[i].flag = true;
				if(temp == origin)
				{
					pathInfos[i].face = i;
				}
				else
				{
					pathInfos[i].face = pathInfos[temp].face;
				}
				que.push(i);
			}
		}
	}

	//delete [] *metric;
	//delete [] metric;
	if(NULL != metric)
	{
		for(int i = 0; i < nodesNum; ++i)
			delete [] metric[i];
		delete [] metric;
		metric = NULL;
	}
}

/**
<@function. trimLastComponentFromName
<@brief. In our framework, the name of a Data packet is hierarchical, and the last component 
		of the name is the chunk sequence number of the Data packet. The function will trim the component.
<@param. name, the name of a Data packet to be processed.
<@return. The remainder part of the original name after getting rid of the last component.
*/
string trimLastComponentFromName(string name)
{
	vector<string> v;
	splitString(name, v, '/');
	v.pop_back();
	string ret;
	for(vector<string>::iterator iter(v.begin()), end(v.end());
		iter != end; ++iter)
	{
		ret = ret + *iter + "/";
	}
	//ret.pop_back();
	ret = ret.substr(0, ret.length() - 1);
	return ret;
}

/**
<@function. computeBetweennessCentrality
<@brief. Compute the ego network betweenness centrality of a given node.
<@param. origin, the node for which we will compute the ego network centrality betweenness.
<@return. the ego network betweenness centrality of the given node.
*/
float computeBetweennssCentrality(int origin)
{
//	int& nodesNum = NetworkConfig::m_nodesNum;
//	vector<pair<int, int> >& links = NetworkConfig::m_links;
//	int metric[nodesNum][nodesNum];
	int** metric = new int*[nodesNum];
	for(int i = 0; i < nodesNum; ++i)
		metric[i] = new int[nodesNum];

	for(int i = 0; i < nodesNum; ++i)
		for(int j = 0; j < nodesNum; ++j)
			metric[i][j] = 0;
	
	for(vector<pair<int, int> >::iterator iter(links.begin()), end(links.end());
		iter != end; ++iter)
	{
		int node1 = iter->first;
		int node2 = iter->second;
		metric[node1][node2] = 1;
		metric[node2][node1] = 1;	
	}
	
	vector<int> alters;
	alters.push_back(origin);
	for(int i = 0; i < nodesNum; ++i)
	{
		if(1 == metric[origin][i])
		{
			alters.push_back(i);
		}
	}
	
	int matrixLength = alters.size();
	//int adjacencyMatrix[matrixLength][matrixLength];
	int** adjacencyMatrix = new int*[matrixLength];
	for(int i = 0; i < matrixLength; ++i)
		adjacencyMatrix[i] = new int[matrixLength];

	for(int i = 0; i < matrixLength; ++i)
		for(int j = 0; j < matrixLength; ++j)
		{
			int node1 = alters[i];
			int node2 = alters[j];
			adjacencyMatrix[i][j] = metric[node1][node2];
		}
//			adjacencyMatrix[i][j] = 0;
	
	//int resultMatrix[matrixLength][matrixLength];
	int** resultMatrix = new int*[matrixLength];
	for(int i = 0; i < matrixLength; ++i)
		resultMatrix[i] = new int[matrixLength];

	for(int i = 0; i < matrixLength; ++i)
		for(int j = 0; j < matrixLength; ++j)
		{
			int temp = 0;
			for(int k = 0; k < matrixLength; ++k)
				temp += adjacencyMatrix[i][k]*adjacencyMatrix[k][j];
				resultMatrix[i][j] = temp;
		}
	
	for(int i = 0; i < matrixLength; ++i)
		for(int j = 0; j < matrixLength; ++j)
			adjacencyMatrix[i][j] = 1 - adjacencyMatrix[i][j];
	
	for(int i = 0; i < matrixLength; ++i)
		for(int j = 0; j < matrixLength; ++j)
			resultMatrix[i][j] = resultMatrix[i][j]*adjacencyMatrix[i][j];

	float ret = 0;
	for(int i = 0; i < matrixLength; ++i)
		for(int j = i + 1; j < matrixLength; ++j)
			if(0 != resultMatrix[i][j])
			{
				ret +=  1/float(resultMatrix[i][j]);
			}
	//delete [] *metric;
	//delete [] metric;
	if(NULL != metric)
	{
		for(int i = 0; i < nodesNum; ++i)
			delete [] metric[i];
		delete [] metric;
		metric = NULL;
	}

	//delete [] *adjacencyMatrix;
	//delete [] adjacencyMatrix;
	if(NULL != adjacencyMatrix)
	{
		for(int i = 0; i < matrixLength; ++i)
			delete [] adjacencyMatrix[i];
		delete [] adjacencyMatrix;
		adjacencyMatrix = NULL;
	}

	//delete [] *resultMatrix;
	//delete [] resultMatrix;
	if(NULL != resultMatrix)
	{
		for(int i = 0; i < matrixLength; ++i)
			delete [] resultMatrix[i];
		delete [] resultMatrix;
		resultMatrix = NULL;
	}
	return ret;
}

/**
<@function. generateRandomString
<@brief. given a number, the function will generate a string. The string consists of digits, upper case letters and lower case letters.
<@param. value, the number based which we will generate the random string.
<@param. length, the length of the resultant string.
<@return. The resultant string we will get finally.
*/
string generateRandomString(int value, int length)
{
	string source = "1234567890qwertyuioplkjhgfdsazxcvbnmQWERTYUIOPLKJHGFDSAZXCVBNM";
	srand(value);
	string ret = "";
	for(int i = 0; i < length; ++i)
	{
		int index = rand()%62;
		ret = ret + source[index];
	}
	return ret;
}

/**
<@function. hashStringToNum
<@brief. Hash a string to a random data distributed in the range of [0, 86969)
<@param. A string to be hashed.
<@return. A random data distributed in the range of [0, 1009).
*/
float hashStringToNum(string str)
{	
	#define A 54059
	#define B 76963
	#define C 1009
	
	unsigned ret = 31;
	int length = str.length();
	for(int i = 0; i < length; ++i)
	{
		unsigned temp = str[i];
		ret = (ret * A) ^ (temp * B);
	}	
//	return float(ret%C);
	return float(ret);
}

/**
<@function. generateFileNames
<@brief. Generate a set of file names that the network could supply as well as the probability the files are requested.
	The users will choose files from the file name set following probability generated here.
<@param. prefixes, the set of highest level prefixes in the network. For every highest prefix, the function will generate 100 file names for it.
<@param. fileNames, a reference variable, a container of vector, used to maintain the file names. 
<@param. fileRequestProbability, a vector defining the probability the files identified by the filenames in fileNames are requested.
	The probability the files will be requested follows zipf-like distribution with alpha equals 0.75
*/
void generateFileNames(vector<string> prefixes, vector<string>& fileNames, vector<float>& fileRequestProbability)
{
	fileNames.clear();
	fileRequestProbability.clear();
	for(vector<string>::iterator prefixIter(prefixes.begin()), prefixEnd(prefixes.end());
		prefixIter != prefixEnd; ++prefixIter)
	{
		for(int i = 0; i < file_number; ++i)
		{
			string filename = *prefixIter + "/";
			//srand((unsigned)time(0));
			
			int randomNum = rand();
			string temp = generateRandomString(randomNum, 10);
			filename = filename + temp + "/" + "100";
			
			/*randomNum = rand();
			temp = generateRandomString(randomNum, 10);
			filename = filename + temp + "/";
			
			randomNum = rand();
			temp = generateRandomString(randomNum, 10);
			filename = filename + temp + "/" + "100";*/
			
			fileNames.push_back(filename);
		}
		random_shuffle(fileNames.begin(), fileNames.end());
	}	
	int length = fileNames.size();
	float sum = 0;
	for(int i = 0; i < length; ++i)
	{
		float floatI = i + 1;
		float exponent = 0.75;
		float temp = 1/pow(floatI, exponent);
		sum += temp;
		fileRequestProbability.push_back(sum);
	}
	for(int i = 0; i < length; ++i)
		fileRequestProbability[i] = fileRequestProbability[i]/sum;
}


/**
<@function. vectorSubtraction
<@brief. Given two vectors, the function will remove the repitition elements from the former vector.
<@param. former, a reference variable, the first vector, the repitition elements in it will be removed.
<@param. later, the second vector
*/
void vectorSubtraction(vector<int>& former, vector<int> later)
{
	vector<int> temp = former;
	former.clear();
	for(vector<int>::iterator vIter(temp.begin()), vEnd(temp.end());
		vIter != vEnd; ++vIter)
	{
		int temp = *vIter;
		vector<int>::iterator iter = find(later.begin(), later.end(), temp);
		if(later.end() == iter)
			former.push_back(*vIter);
	}
}

/**
<@function. power
<@brief. Compute the power. The built-in pow function can't function well (I don't know the cause), so I coded the function by myself.
<@param. base, the base of the pow computation.
<@param. exponent, the exponent of the pow computation.
<@param. The exponent must be an integer no less than 0.
*/
float power(float base, int exponent)
{
	float ret = 1;
	for(int i = 0; i < exponent; ++i)
		ret = ret*base;
	return ret;
}


/**
<@function. parseTopoConfigFile
<@brief. Read the topology configuration file and extract information from it.
<@param. nodesNum, reference variable, the number of nodes in the network will be stored in it.
<@param. producers, reference variable, the IDs of producers will be stored in it.
<@param. routers, reference variable, the IDs of routers will be stored in it.
<@param. users, reference variable, the IDs of end users will be stored in it.
<@param. links, reference variable, the IDs of end nodes pairs of every link will be stored in it.
*/
void parseTopoConfigFile(int& nodesNum, vector<int>& producers, 
	vector<int>& routers, vector<int>& users, vector<pair<int, int> >& links)
{
//	cout << TOPO_CONFIG_FILE << endl;
	producers.clear();
	routers.clear();
	users.clear();
	links.clear();
	ifstream inFile;
	inFile.open(TOPO_CONFIG_FILE);
	if(!inFile)
	{
		cerr << "Unable to open file: " << TOPO_CONFIG_FILE << endl;
		exit(1);
	}
	string line;
//	while(inFile >> line)
//	vector<int> users;
//	vector<int> producers;
//	vector<int> routers;
//	vector<pair<int, int> > links;
	vector<string> temp;
	vector<string>::iterator iter, end;
//	int nodesNum;
	istringstream iss;
	while(getline(inFile, line))
	{
//		cout << line << endl;
		switch(line[0])
		{
			case('#'): break;
			case('N'):
//				vector<string> temp;
				splitString(line, temp, ' ');
//				istringstream iss(temp[1]);
				//iss = istringstream(temp[1]);
//				cout << "temp[1] = " << temp[1] << endl;
				istringstream(temp[1]) >> nodesNum;
//				cout << nodesNum + 1 << endl;
				break;
			case('P'):
				splitString(line, temp, ' ');
				temp.erase(temp.begin());
				int producer;
				iter = temp.begin();
				end = temp.end();
				for(; iter != end; ++iter)
				{
					istringstream(*iter) >> producer;
					producers.push_back(producer);
				}
				break;
			case('R'):
				splitString(line, temp, ' ');
				temp.erase(temp.begin());
				int router;
				iter = temp.begin();
				end = temp.end();
				for(; iter != end; ++iter)
				{
					istringstream(*iter) >> router;
					routers.push_back(router);
				}
				break;
			case('U'):
				splitString(line, temp, ' ');
				temp.erase(temp.begin());
				iter = temp.begin();
				end = temp.end();
				int user;
				for(; iter != end; ++iter)
				{
					istringstream(*iter) >> user;
					users.push_back(user);
				}
				break;
			case('L'):
				splitString(line, temp, ' ');
				int node1, node2;
				iter = temp.begin();
				istringstream(*(++iter)) >> node1;
				istringstream(*(++iter)) >> node2;
				links.push_back(make_pair(node1, node2));
				break;
			default:
				break;
		}
	}
//	cout << "nodesNum = " << nodesNum << endl;
//	cout << "producers.size() = " << producers.size() << endl;
//	cout << "routers.size() = " << routers.size() << endl;
//	cout << "users.size() = " << users.size() << endl;
//	cout << "links.size() = " << links.size() << endl;
}



/**
<@function. constructNetworkTopologyKary
<@brief. Construct a network whose topology is of a k-ary tree.
<@param. k, the spread factor of the k-ary tree. 
<@param. h, the height of the k-ary tree.
<@param. nodesNum, reference variable, the number of nodes in the network will be stored in it.
<@param. producers, reference variable, the IDs of producers will be stored in it.
<@param. routers, reference variable, the IDs of routers will be stored in it.
<@param. users, reference variable, the IDs of end users will be stored in it.
<@param. links, reference variable, the IDs of end nodes pairs of every link will be stored in it.
*/
void constructNetworkTopologyKary(int k, int h, int& nodesNum, vector<int>& producers, 
	vector<int>& routers, vector<int>& users, vector<pair<int, int> >& links)
{
	producers.clear();
	routers.clear();
	users.clear();
	links.clear();
	//cout << pow(2, 3) << endl;
	float floatK = k;	// As the function pow require the base must be a float, we make a float version of k here.
	nodesNum = pow(floatK, h) - 1;
	//nodesNum -= 1;
	int usersNum = pow(floatK, h - 1);
	nodesNum = nodesNum/(k - 1);
	producers.push_back(0);
	for(int i = 1; i < nodesNum - usersNum; ++i)
		routers.push_back(i);
	for(int i = nodesNum - usersNum; i < nodesNum; ++i)
		users.push_back(i);
	//Construct the links between any pair of nodes.
	for(int level = 1; level < h; ++level)
	{
		int num = pow(floatK, level - 1);	//the number of nodes in the this level
		for(int index = 1; index <= num; ++index)
		{
				int parentId;	// the id of upstream node of this node pair
				if(1 == level)
					parentId = 0;
				else parentId = (pow(floatK, level - 1) - 1)/(k - 1) + index - 1;
				for(int childIndex = 1; childIndex <= k; ++childIndex)
				{
					int childId;	// the id of the downstream node of this node pair
					childId = (pow(floatK, level) - 1)/(k - 1) + (index - 1)*k + childIndex - 1;
					links.push_back(make_pair(parentId, childId));
				}
		}
	}
}

/**
<@function. constructNetworkTopologyHeavyEdge
<@brief. The function struct a new type of network topology. The constructed network is divided into two separate
	parts. The upper part consists of a k-ary tree. The root of the k-ary tree is the producer. The inner nodes and 
	the edge nodes of the k-ary tree is the routers. Then for every router which is a edge node on the k-ary tree, 
	a given number of nodes will be attached to it. The thus attached nodes are acted as the end users.
<@param. k, the spread factor of the upper k-ary tree. 
<@param. h, the height of the upper k-ary tree.
<@param. m, the number of end users attached to each edge router.
<@param. nodesNum, reference variable, the number of nodes in the network will be stored in it.
<@param. producers, reference variable, the IDs of producers will be stored in it.
<@param. routers, reference variable, the IDs of routers will be stored in it.
<@param. users, reference variable, the IDs of end users will be stored in it.
<@param. links, reference variable, the IDs of end nodes pairs of every link will be stored in it.

*/
void constructNetworkTopologyHeavyEdge(int k, int h, int m, int& nodesNum, vector<int>& producers, 
	vector<int>& routers, vector<int>& users, vector<pair<int, int> >& links)
{
	producers.clear();
	routers.clear();
	users.clear();
	links.clear();
	
	nodesNum = (pow(float(k), h) - 1)/(k - 1) + pow(float(k), h - 1)*m;
	
	producers.push_back(0);
	int upperNodesNum = (pow(float(k), h) - 1)/(k - 1);
	for(int i = 1; i < upperNodesNum; ++i)
		routers.push_back(i);
	for(int i = upperNodesNum; i < nodesNum; ++i)
		users.push_back(i);

	//Construct the links between any pair of nodes in the upper part.
	for(int level = 1; level < h; ++level)
	{
		int num = pow(float(k), level - 1);	//the number of nodes in the this level
		for(int index = 1; index <= num; ++index)
		{
				int parentId;	// the id of upstream node of this node pair
				if(1 == level)
					parentId = 0;
				else parentId = (pow(float(k), level - 1) - 1)/(k - 1) + index - 1;
				for(int childIndex = 1; childIndex <= k; ++childIndex)
				{
					int childId;	// the id of the downstream node of this node pair
					childId = (pow(float(k), level) - 1)/(k - 1) + (index - 1)*k + childIndex - 1;
					links.push_back(make_pair(parentId, childId));
				}
		}
	}

	int lowestLevelNodesNum = pow(float(k), h - 1);	//The number of nodes in the lowest level of the upper k-ary tree.
	for(int parentIndex = 1; parentIndex <= lowestLevelNodesNum; ++parentIndex)
	{
		int parentId = (pow(float(k), h - 1) - 1)/(k - 1) + parentIndex - 1;
		for(int childIndex = 1; childIndex <= m; ++childIndex)
		{
			int childId = (pow(float(k), h) - 1)/(k - 1) + (parentIndex - 1)*m + childIndex - 1;
			links.push_back(make_pair(parentId, childId));
		}
	}
}


/**
<@function. initCRCLookupTable
<@brief. Initialise the CRCLookupTable.
*/
void initCRCLookupTable()
{
	const crc polynomial = 0x1021;
	for(int index = 0; index < 256; ++index)
	{
		crc remainder = index;
		for(int i = 0; i < 8; ++i)
		{
			if(remainder & (1 << 7))
			{
				remainder = (remainder << 1) ^ polynomial;
			}
			else remainder = remainder << 1;
		}
		crcLookupTable[index] = remainder;
	}
}


/**
<@function. constructRealNetwork
<@brief. Utilizing the real network topology, the function will construct a network of 200 routers,
	5 servers, and 100 users.
<@param. routerFile, the name of the file containing the routers.
<@param. linkFile, the name of the file containing the links.
<@param. nodesNum, reference variable, the number of nodes in the network will be stored in it.
<@param. producers, reference variable, the IDs of producers will be stored in it.
<@param. routers, reference variable, the IDs of routers will be stored in it.
<@param. users, reference variable, the IDs of end users will be stored in it.
<@param. links, reference variable, the IDs of end nodes pairs of every link will be stored in it.
*/

void constructRealNetwork(string routerFile, string linkFile,int& nodesNum, vector<int>& producers, 
	vector<int>& routers, vector<int>& users, vector<pair<int, int> >& links)
{
	producers.clear();
	routers.clear();
	users.clear();
	links.clear();

	ifstream frouters(routerFile.c_str());
	ifstream flinks(linkFile.c_str());
	string line;
	int count = 0;
	map<string, int> routerNameId;
	while(getline(frouters, line))
	{
		routerNameId[line] = count++;
	}
	
	nodesNum = count + 5 + 1000;

	for(int i = 0; i < count; ++i)
		routers.push_back(i);

	 //parse the link file.
	while(getline(flinks, line))
	{
		istringstream iss(line);
		string first, second;
		iss >> first;
		iss >> second;
		int firstId = routerNameId[first];
		int secondId = routerNameId[second];
		links.push_back(make_pair(firstId, secondId));
	}
	//cout << "hello" << endl;
	
	vector<int> tempRouters;
	for(int i = 0; i < count; ++i)
		tempRouters.push_back(i);
	random_shuffle(tempRouters.begin(), tempRouters.end());

	//Add the producers and their links to the routers.
	for(int i = 0; i < 5; ++i)
	{
		producers.push_back(count + i);
		links.push_back(make_pair(tempRouters[i], count + i));
	}

	//Add the end users and their links to the routers
	for(int i = 0, userId = count + 5; i < 100; ++i)
	{
		for(int j = 0; j < 10; ++j, ++userId)
		{
			users.push_back(userId);
			links.push_back(make_pair(tempRouters[5 + i], userId));
		}
	}
}