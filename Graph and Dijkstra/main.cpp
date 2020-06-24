/*
TO DO:
	1. add addVertex and removeVertex to Graph/Digraph
*/
#include <limits>
#include <set>
#include <iostream>
#include <vector>

using namespace std;

class Edge
{
public:
	Edge(long int toVert, long int wgt) { to = toVert; weight = wgt; }
	long int getTo() const { return to; }
	long int getWeight() const { return weight; }
	bool operator< (const Edge& edge2) const { return this->to < edge2.to; }		//to determine order of edges in adjList
	bool operator==	(const Edge& edge2) const { return this->to == edge2.to; }	//to determine duplicate edges in set
private:
	long int to;			//vertex number of destination vertex (for adjList)
	long int weight;		//weight of edge	
};

class Graph
{
public:
	Graph(long int numVerts) : adjList(numVerts) { }
	virtual void addEdge(long int start, long int end, long int weight) = 0;	//adds edge from start to end with given weight if no edge from start to end already exists
	virtual void removeEdge(long int start, long int end) = 0;			//removes edge from start to end if it exists
	virtual long int weight(long int start, long int end) = 0;			//returns the weight of the edge going from start to end
	friend long int* dijkstra(Graph*, long int, long int);
protected:
	vector<set<Edge>> adjList;
	


};

//Directed Graph
class DiGraph : public Graph
{
public:
	
	DiGraph(long int numVerts) : Graph(numVerts) { }      
	//initializes adjList to a vector (size num) of sets and 
	//size of vector (number of vertices) intialized to num

	virtual void addEdge(long int start, long int end, long int weight)
	{
		if (adjList[start].find(Edge(end, weight)) == adjList[start].end())
			adjList[start].insert(Edge(end, weight));
	}
	virtual void removeEdge(long int start, long int end)
	{
		auto it = adjList[start].find(Edge(end, 1));
		if (it != adjList[start].end())
			adjList[start].erase(it);
	}
	virtual long int weight(long int start, long int end)
		//returns weight of the edge from start vertex to end vertex if it exists, 0 if no edge exists		
	{
		auto it = adjList[start].find(Edge(end, 1));   //search the set at adjList[start] to see if it contains an edge to end
		if (it != adjList[start].end())
			return it->getWeight();
		return std::numeric_limits<long int>::max()-100;				//temporary error code - can replace with exception if value is important
	}


};

enum class Mark { UNVISITED, VISITED };

//helper function to get index of vertex with minimum distance
long int minDist(Graph* G, long int num, long int* D, Mark* visitArr)
//input : graph, number of vertices, distance array, mark array
//output: index of vertex with min distance
{
	long int i, v = -1;
	for (i = 0; i < num; ++i)
		if (visitArr[i] == Mark::UNVISITED) { v = i; break; }
	for (++i; i < num; ++i)
		if (visitArr[i] == Mark::UNVISITED && D[i] < D[v])
			v = i;
	return v;
}

//Dijkstra's algorithm

long int* dijkstra(Graph* G, long int num, long int S)
// input : a graph, the number of vertices, and the number of the start vertex (from 0 to n-1)
// output: array containing the shortest distance to each vertex from the starting vertex
// (index of the array indicates vertex number, i.e. number at index 0 is distance to vertex 0)
// output : dynamic array with the results stored where index = vertex number (starting with 0)
// For user: REMEMBER TO DELETE RESULT ARRAY AFTER FINISHED WITH IT
{
	long int v;
	constexpr long int INFIN = std::numeric_limits<long int>::max()-100;
	long int* distance = new long int[num];	//initialize distance values to "infinity"
	for (long int i = 0; i < num; ++i)
		distance[i] = INFIN;
	distance[S] = 0; 				//set distance from source to 0
	Mark* visit = new Mark[num]{ Mark::UNVISITED };

	for (long int i = 0; i < num; ++i)
	{
		/*if (visit[i] != VISITED && 		this seems to have been leftover but serves no purpose - saving until finished*/

		//Select the unvisited vertex with minimum distance from the source and mark it as visited
		v = minDist(G, num, distance, visit);
		visit[v] = Mark::VISITED;



		//Check all vertices and relax distance if necessary
		//for (int u = 0; u < num; ++u)

		//For all of v's adjacent vertices, check if adjacent vertex has been visited already, that graph isn't disconnected, 
		//and if distances need relaxed (while accounting for potential overflow)
		for (auto it = G->adjList[v].begin(); it != G->adjList[v].end(); ++it)
		{
			if (distance[it->getTo()] > distance[v] + G->weight(v, it->getTo()) && distance[v] != INFIN && visit[it->getTo()] != Mark::VISITED)
			{
				distance[it->getTo()] = distance[v] + G->weight(v, it->getTo());
			}
		}
		//Notes:
		//1. if adjacent vertices has already been visited a shorter path has already been found
		//2. if this node is disconnected it's neighboring vertices are also disconnected
		//3. what to do if shortest path is over maximum value for int?
	}


	//delete array for storing whether array was visited, but not distance array since that is the return value - must be deleted
	//by user of function
	delete[] visit;
	return distance;
}





/*
	1. initialize starting vertex to 0 distance and all other vertices to infinity (int max)
	2. recursively select unvisited vertex with shortest distance
	3. mark selected vertex as visited
	4. relax distances to vertices adjacent to vertex previously selected

	** problem occurs with negative edges when assuming a path doesn't need to be checked for relaxation if the total
		distance up to that point is already greater than the edge to the vertices
*/

int main()
{
	DiGraph myGraph(6);

	//tests for addEdge and removeEdge
	myGraph.addEdge(0, 1, 5);
	cout << myGraph.weight(0, 1) << endl;        //expected : 5
	myGraph.removeEdge(0, 1);
	myGraph.addEdge(0, 1, 7);
	cout << myGraph.weight(0, 1) << endl;        //expected: 7

	myGraph.removeEdge(0, 1);
	cout << myGraph.weight(0, 1) << endl;        //expected: max of long int

	//constructing a graph
	myGraph.addEdge(0, 1, 4);
	myGraph.addEdge(0, 2, 2);
	myGraph.addEdge(0, 3, 3);
	myGraph.addEdge(2, 4, 5);
	myGraph.addEdge(2, 5, 1);
	myGraph.addEdge(3, 4, 5);
	myGraph.addEdge(3, 5, 7);


	//testing dijkstra
	Graph* myGraphPtr = &myGraph;
	auto myResults = dijkstra(myGraphPtr, 6, 0);

	for (int i = 0; i < 6; ++i)
		cout << i << ": " << myResults[i] << endl;

	delete[] myResults;

	return 0;
}
