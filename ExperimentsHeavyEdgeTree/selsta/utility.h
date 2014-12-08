//utility.h
#ifndef UTILITY_H
#define UTILITY_H
#include <string>
#include <vector>
#include <map>

//#include <vld.h>

#include "components.h"
using namespace std;

typedef unsigned short int crc;


/**
<@function. splitString
<@brief. Split a string by the the given delimiter. And save every segment in a vector.
<@param. line, the string to be processed.
<@param. v, a reference variable. The resultant segments of the original string will be saved in it.
<@param. delimiter, the delimiter by which we will split the original string.
*/
void splitString(string line, vector<string>& v, char delimiter);

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
	vector<int>& routers, vector<int>& users, vector<pair<int, int> >& links);
	
/**
<@function. parseProducerConfigFile
<@brief. Parse the IDs of producers and the highest level prefixes corresponding to each producer from PRODUCER_CONFIG_FILE
<@param. pairs, a reference variable, a map container of <id, prefix> pair.
*/
void parseProducerConfigFile(map<int, string>& pairs);
	
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
void constructShortestPath(int origin, int nodesNum, vector<pair<int, int> > links, vector<PathInfo>& pathInfos);

/**
<@function. trimLastComponentFromName
<@brief. In our framework, the name of a Data packet is hierarchical, and the last component 
		of the name is the chunk sequence number of the Data packet. The function will trim the component.
<@param. name, the name of a Data packet to be processed.
<@return. The remainder part of the original name after getting rid of the last component.
*/
string trimLastComponentFromName(string name);

/**
<@function. computeBetweennessCentrality
<@brief. Compute the ego network betweenness centrality of a given node.
<@param. origin, the node for which we will compute the ego network centrality betweenness.
<@return. the ego network betweenness centrality of the given node.
*/
float computeBetweennssCentrality(int origin);

/**
<@function. generateRandomString
<@brief. given a number, the function will generate a string. The string consists of digits, upper case letters and lower case letters.
<@param. value, the number based which we will generate the random string.
<@param. length, the length of the resultant string.
<@return. The resultant string we will get finally.
*/
string generateRandomString(int value, int length);

/**
<@function. hashStringToNum
<@brief. Hash a string to a random data distributed in the range of [0, 86969)
<@param. A string to be hashed.
<@return. A random data distributed in the range of [0, 1009).
*/
float hashStringToNum(string str);

/**
<@function. generateFileNames
<@brief. Generate a set of file names that the network could supply as well as the probability the files are requested.
	The users will choose files from the file name set following probability generated here.
<@param. prefixes, the set of highest level prefixes in the network. For every highest prefix, the function will generate 100 file names for it.
<@param. fileNames, a reference variable, a container of vector, used to maintain the file names. 
<@param. fileRequestProbability, a vector defining the probability the files identified by the filenames in fileNames are requested.
	The probability the files will be requested follows zipf-like distribution with alpha equals 0.75
*/
void generateFileNames(vector<string> prefixes, vector<string>& fileNames, vector<float>& fileRequestProbability);

/**
<@function. vectorSubtraction
<@brief. Given two vectors, the function will remove the repitition elements from the former vector.
<@param. former, a reference variable, the first vector, the repitition elements in it will be removed.
<@param. later, the second vector
*/
void vectorSubtraction(vector<int>& former, vector<int> later);

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
	vector<int>& routers, vector<int>& users, vector<pair<int, int> >& links);

/**
<@function. power
<@brief. Compute the power. The built-in pow function can't function well (I don't know the cause), so I coded the function by myself.
<@param. base, the base of the pow computation.
<@param. exponent, the exponent of the pow computation.
<@param. The exponent must be an integer no less than 0.
*/
float power(float base, int exponent);

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
	vector<int>& routers, vector<int>& users, vector<pair<int, int> >& links);

/**
<@function. initCRCLookupTable
<@brief. Initialise the CRCLookupTable.
*/
void initCRCLookupTable();

/**
<@function. constructRealNetwork
<@brief. Utilizing the real network topology, the function will construct a network of 200 routers,
	50 data producers, and 100 users.
<@param. routerFile, the name of the file containing the routers.
<@param. linkFile, the name of the file containing the links.
<@param. nodesNum, reference variable, the number of nodes in the network will be stored in it.
<@param. producers, reference variable, the IDs of producers will be stored in it.
<@param. routers, reference variable, the IDs of routers will be stored in it.
<@param. users, reference variable, the IDs of end users will be stored in it.
<@param. links, reference variable, the IDs of end nodes pairs of every link will be stored in it.
*/

void constructRealNetwork(string routerFile, string linkFile,int& nodesNum, vector<int>& producers, 
	vector<int>& routers, vector<int>& users, vector<pair<int, int> >& links);
#endif
