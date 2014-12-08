//prosta
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cstdio>
#include <utility>
#include <vector>
#include <sstream>
#include <map>
#include <cmath>
#include <set>

#include "Node.h"
#include "utility.h"
#include "components.h"
#include "DataPacket.h"
using namespace std;

typedef unsigned short int crc;

int fibFaceLifetime = 10;	//<@brief. The preset life time of a fib associated with a dynamic FIB entry.
int nodesNum;	//<@brief. The total number of nodes in the network.
vector<int> producers;	//<@brief. The IDs of the producers.
vector<int> routers;	//<@brief. The IDs of the routers.
vector<int> users;	//<@brief. The IDs of end users.
vector<pair<int, int> > links; //<@brief. The links in the network. Each pair represents a link in the network, with each element
		// in the pair represent the ID of a end point of the link.
map<int, string> idPrefix; //<@brief. The container is used to maintain the 
		//IDs of producers and the highest level prefix corresponding to each producer.
vector<Node> nodes;	//<@brief. All the nodes in the network, including the producers, routers, and end users will be stored in it.
int pitEntryLifetime;	//<@brief. The preset life time of a PIT entry.
vector<string> fileNames;	//<@brief. The vector contains the fileNames the network could supply. 
vector<float> fileRequestProbability;	//<@brief. The container defines the probability that a file in 
// fileNames will be requested. The probability follows the Zipf-like distribution with alpha = 0.85.
int responsePacketNum = 0;		//<@brief. The total number of responsee Data packets we have received.
long long cachedPacketNum = 0;	//<@brief. The total number of Data packets that has been cached in the network.
map<string, float> filenameAndProbability;	//<@brief. The filenames and the probability that each file would be accessed.
int cacheThreshold = 1;	//<@brief. The threshold to cache a Data packet. If the distance from the provider to the caching router is less 
	//than the threshold, the provider will tag the response Data packet as nocache. So the response won't be cache in the caching router.
ofstream reuseTime;	//<@brief. Output the reuse time of the Data packets in the content store of all the routers into the file.
ofstream PLCR;
crc crcLookupTable[256];
int packetId;	//<@brief. The variable is used to identify an Interest packet and its corresponding Data packet uniquely.
ofstream peekFile;	//<@brief. Some information need to be observed will be outputed into the file.
int requiredHopNum;	//<@brief. The number of hops needed to fetch the Data packets back when no in-router caching exists.
int measuredHopNum;	//<@brief. The number of hops needed to fetch the Data packets back when the in-router caching exists.
int lowerPacketNumLimit;	//<@brief. When the id of the Data or Interest packet is larger than or equal to this limit, 
	//record the hop information about the packet.
int upperPacketNumLimit;	//<@brief. When the id of the Data or Interest packet is less than or equal to this limit,
	//record the hop information about the packet.
int file_number;	//<brief. The number of files corresponding to a single prefix.
int capacity;	//<brief. The times of the total content store capacity over  the total file size in the network.
int spread_factor;	//<brief. How many clients are connected to a router.
string experiment;	//<brief. The experiment to be carried out.
int delegateRouterNumber;	//<brief. The number of delegate routers in the network. The variable is used in the real network simulations only.

struct Configuration
{
	string m_experiment;
	int m_file_number;
	int m_capacity;
	int m_spread_factor;
};

int main()
{
	ifstream fconfig("data/experiment_configuration.dt");
	string temp;
	getline(fconfig, temp);
	string dataset;
	while(fconfig >> experiment >> spread_factor >> file_number >> capacity >> dataset >> delegateRouterNumber)
	{
		packetId = 0;
		requiredHopNum = 0;
		measuredHopNum = 0;
		lowerPacketNumLimit = 400000;
		upperPacketNumLimit = 500000;
		//freopen("data/experiment17_persta.log", "w", stdout);
		//freopen("data/experiment17_persta.data", "w", stderr);
		//reuseTime.open("data/experiment17_persta_reuseTime.data");
		temp = "data/" + experiment + "_prosta_050.log";
		freopen(temp.c_str(), "w", stdout);
		temp = "data/" + experiment + "_prosta_hopRatio_050.dt";
		freopen(temp.c_str(), "w", stderr);
		temp = "data/" + experiment + "_prosta_reuseRatio_050.dt";
		reuseTime.open(temp.c_str());
		//initCRCLookupTable();
		srand(0);
		//int k = 2;	//The spread factor of the k-ary tree.
		//int h = 8;	// The height of the k-ary tree.
		//int m = 7;	// The number of users attached to each edge router.
		//constructNetworkTopologyHeavyEdge(k, h, m, nodesNum, producers, routers, users, links);	
		//constructNetworkTopologyKary(k, h, nodesNum, producers, routers, users, links);

		//Generate the possible file names set.
		//int k = spread_factor;	//The spread factor of the k-ary tree.
		//int h = height;	// The height of the k-ary tree.
		//int m = 7;	// The number of users attached to each edge router.
		//constructNetworkTopologyHeavyEdge(k, h, m, nodesNum, producers, routers, users, links);	
		//constructNetworkTopologyKary(k, h, nodesNum, producers, routers, users, links);
		string routersFileName = "data/routers@" + dataset + ".dt";
		string linksFileName = "data/links@" + dataset + ".dt";
		constructRealNetwork(routersFileName, linksFileName, nodesNum, producers, routers, users, links);
		//return 0;
		//Generate the possible file names set.
		//ifstream fprefixes("data/highestLevelPrefixes.data");
		idPrefix.clear();
		vector<string> prefixes;
		ifstream inFile;
		inFile.open("data/highestLevelPrefixes.dt");
		string tempPrefix;
		while(inFile >> tempPrefix)
		{
			prefixes.push_back(tempPrefix);
		}
		int producerNum = producers.size();
		for(int i = 0; i < producerNum; ++i)
		{
			idPrefix[producers[i]] = prefixes[i];
		}
		generateFileNames(prefixes, fileNames, fileRequestProbability);
		int contentStoreCapacity = fileNames.size()*100*1024*capacity/routers.size();	// The total content store capacity should be 
		nodes.clear();
		for(int i = 0; i < nodesNum; ++i)
		{
			nodes.push_back(Node(i, contentStoreCapacity));
		}
		//Initialise the links between nodes	
		for(vector<pair<int, int> >::iterator iter(links.begin()), end(links.end());
			iter != end; ++iter)
		{
			int node1 = iter->first;
			int node2 = iter->second;
			nodes[node1].addLink(node2);
			nodes[node2].addLink(node1);
		}
		//Define the types of the nodes
		for(vector<int>::iterator iter(producers.begin()), end(producers.end());
			iter != end; ++iter)
			nodes[*iter].setType(Node::producer);
		for(vector<int>::iterator iter(routers.begin()), end(routers.end());
			iter != end; ++iter)
			nodes[*iter].setType(Node::router);
		for(vector<int>::iterator iter(users.begin()), end(users.end());
			iter != end; ++iter)
			nodes[*iter].setType(Node::user);
		//Construct the static FIB for every router.
		//In our model, since every end user and every producer is connected to only a router,
		// we don't need to configure the static FIB for the producer nodes.
		//vector<PathInfo> pathInfos;
		//for(int i = 0; i < nodesNum; ++i)
		//{
		//	if(Node::producer == nodes[i].getType())
		//		continue;
		//	constructShortestPath(i, nodesNum, links, pathInfos);
		//	//cout << i << ":" << endl;
		//	for(vector<int>::iterator iter(producers.begin()), end(producers.end());
		//		iter != end; ++iter)
		//	{
		//		string prefix = idPrefix[*iter];
		//		float metric = pathInfos[*iter].metric;
		//		int face = pathInfos[*iter].face;
		//		nodes[i].insertStaticFibEntry(prefix, face, metric);
		//	}
		//}
		PathInfo** pathInfoMatrix = new PathInfo*[nodesNum];
		for(int i = 0; i < nodesNum; ++i)
		{
			pathInfoMatrix[i] = new PathInfo[nodesNum];
		}
		const int INF = 0x7fffffff;
		for(int i = 0; i < nodesNum; ++i)
		{
			for(int j = 0; j < nodesNum; ++j)
			{
				pathInfoMatrix[i][j].metric = INF;
				pathInfoMatrix[i][j].face = -1;
			}
		}
		for(int i = 0; i < nodesNum; ++i)
		{
			pathInfoMatrix[i][i].metric = 0;
			pathInfoMatrix[i][i].face = i;
		}
		for(int i = 0; i < links.size(); ++i)
		{
			int first = links[i].first;
			int second = links[i].second;
			pathInfoMatrix[first][second].face = second;
			pathInfoMatrix[first][second].metric = 1;
			pathInfoMatrix[second][first].face = first;
			pathInfoMatrix[second][first].metric = 1;
		}
		for(int k = 0; k < nodesNum; ++k)
		{
			for(int i = 0; i < nodesNum; ++i)
			{
				for(int j = 0; j < nodesNum; ++j)
				{
					if(INF != pathInfoMatrix[i][k].metric && INF != pathInfoMatrix[k][j].metric && 
						pathInfoMatrix[i][k].metric + pathInfoMatrix[k][j].metric < pathInfoMatrix[i][j].metric)
					{
						pathInfoMatrix[i][j].metric = pathInfoMatrix[i][k].metric + pathInfoMatrix[k][j].metric;
						pathInfoMatrix[i][j].face = pathInfoMatrix[i][k].face;
					}
				}
			}
		}
		for(int i = 0; i < nodesNum; ++i)
		{
			if(Node::producer == nodes[i].getType())
				continue;
			nodes[i].setWeight();
			for(vector<int>::iterator iter(producers.begin()), end(producers.end());
				iter != end; ++iter)
			{
				string prefix = idPrefix[*iter];
				float metric = pathInfoMatrix[i][*iter].metric;
				int face = pathInfoMatrix[i][*iter].face;
				nodes[i].insertStaticFibEntry(prefix, face, metric);
			}
		}
		for(int i = 0; i < nodesNum; ++i)
		{
			delete pathInfoMatrix[i];
		}
		delete pathInfoMatrix;
		
		vector<int> nodeIds;	//The container is used to maintain the IDs of nodes in the network.
		for(int i = 0; i < nodesNum; ++i)
			nodeIds.push_back(i);
		responsePacketNum = 0;
		while(true)
		{
			random_shuffle(nodeIds.begin(), nodeIds.end());
			for(vector<int>::iterator iter(nodeIds.begin()), end(nodeIds.end());
				iter != end; ++iter)
			{
				if(Node::producer == nodes[*iter].getType())
				{	
					nodes[*iter].producerOperation();
				}
				else if(Node::router == nodes[*iter].getType())
				{
					nodes[*iter].processInterestPacket();
					nodes[*iter].processDataPacket();
				}
				else if(Node::user == nodes[*iter].getType())
				{
					int processTime = rand()%4;
					for(int processIndex = 0; processIndex <= processTime; ++processIndex)
					{
						nodes[*iter].userOperation();
					}
				}
			}
			if(responsePacketNum >= 500000)
				break;

		}
		// Print out the reuse time of Data packets in the routers' content store.
		for(vector<int>::iterator iter(routers.begin()), end(routers.end());
			iter != end; ++iter)
		{
			nodes[*iter].printDataPacketsReuseTime();
			//nodes[*iter].countDataPackets(dataPacketsContainer);
		}
		
		cout << "measuredHopNum = " << measuredHopNum << endl;
		cout << "requiredHopNum = " << requiredHopNum << endl;
		cout << "measuredHopNum/requiredHopNum = " << (float)measuredHopNum/(float)requiredHopNum << endl;
		reuseTime.close();
		temp = "data/" + experiment + "_prosta_cachedPacketsNum.dt";
		ofstream fcachedPacketsNum(temp.c_str());
		fcachedPacketsNum << "#routerId	#linksNum	#cachedDataPacketsNum	#ratio" << endl;
		for(vector<int>::iterator iter(routers.begin()), end(routers.end());
				iter != end; ++iter)
		{
			int linksNum = nodes[*iter].getLinksNum();
			int cachedPacketsNum = nodes[*iter].getCachedDataPacketsNum();
			fcachedPacketsNum << *iter << "\t" << linksNum << "\t" << cachedPacketsNum << "\t" << (float)cachedPacketsNum*(float)linksNum << endl;
		}
	}
	fconfig.close();
	return 0;
}
