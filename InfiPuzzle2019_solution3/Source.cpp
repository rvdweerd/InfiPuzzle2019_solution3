#include <set>
#include <vector>
#include <map>
#include <queue>
#include <string>
#include <iostream>

void Infi1()
{	// aoc.infi.nl 2019, deel 1
	// input
	std::vector<std::pair<int, int>> flats = { {3,7},{5,9},{8,5},{10,6},{14,7},{16,10},{21,9},{25,10},{30,6},{33,8},{34,9},{37,10},{42,8},{45,5},{47,6},{49,9},{50,10},{54,8},{57,9},{58,10},{61,8} };
	std::vector < std::pair<int, int>> sprongen = { {1,2},{2,0},{1,1},{3,1},{1,3},{4,0},{3,1},{4,0},{2,2},{0,1},{2,1},{4,0},{2,0},{1,1},{0,0},{0,1},{3,0},{2,1},{0,1},{2,0} };

	// Maak map van alle hoogtes van de flats: KEY= positie x van flat, VALUE = hoogte
	std::map<int, int> hoogtes;
	for (auto p : flats) hoogtes[p.first] = p.second;

	// Initialiseer
	std::pair<int, int> pos = flats[0];
	int nSprongen = 0;

	// Main loop: voer sprong uit tot [vallen] of tot [successvol landen na laatste sprong]
	for (int i = 0; i < sprongen.size(); i++)
	{
		std::cout << "Positie: (" << pos.first << "," << pos.second << "). ";
		pos.first += sprongen[i].first + 1;
		pos.second += sprongen[i].second;
		auto landingsHoogte = hoogtes.find(pos.first);
		if (landingsHoogte != hoogtes.end() && landingsHoogte->second <= pos.second)
		{
			pos.second = landingsHoogte->second;
			nSprongen++;
			std::cout << "Succesvolle " << nSprongen << "e sprong naar (" << pos.first << "," << pos.second << ")" << std::endl;
		}
		else break;
	}

	// Resultaat tonen
	if (nSprongen == sprongen.size()) std::cout << "success, nSprongen = " << nSprongen; else std::cout << "Fail na sprong: " << nSprongen << ", de kerstman valt dus bij sprong " << nSprongen + 1;
	std::cout << "\nDruk [Enter] om door te gaan..";
	std::cin.get();
}

bool kanVooruitLanden(const std::pair<int, int> startPos, const std::pair<int, int> sprong, const std::map<int, int>& hoogtes)
{
	// Test of Santa vanaf startpositie {x,y} op een nieuw dak kan landen voor een gegeven sprong {sprong_x,sprong_y}
	std::pair<int, int> endPos = { startPos.first + sprong.first + 1, startPos.second + sprong.second };
	auto landingsHoogteIt = hoogtes.find(endPos.first);  // de STL find() functie voor maps werkt met een iterator class voor de container (zie bijv. cppreference.com)
	if (landingsHoogteIt != hoogtes.end() && landingsHoogteIt->second <= endPos.second)
	{
		return true;
	}
	else return false;
}
std::string Hash(std::pair<int, int> pos)
{
	std::string posName;
	posName.append("(");
	posName.append(std::to_string(pos.first));
	posName.append(",");
	posName.append(std::to_string(pos.second));
	posName.append(")");
	return posName;
}
struct Arc; // Forward declaration nodig want Node en Arc zijn gekoppeld
struct Node // Deze datastructuur vormt de basis voor onze graph (zie bestaat uit nodes en arcs)
{
	Node(std::pair<int, int> p)
		:
		x(p.first),
		y(p.second),
		name(Hash(p))
	{}
	// Basisvelden t.b.v. graph functionaliteit
	std::string name;
	int x;
	int y;
	std::set<Arc*> arcs;
	friend bool operator==(const Node& p1, const Node& p2)
	{
		return (p1.name == p2.name);
	}
};
struct Arc
{
	Arc(Node* s, Node* f, int c)
		:
		start(s),
		finish(f),
		cost(c)
	{
	}
	Node* start;
	Node* finish;
	int cost;
};

int GetPathCost(const std::vector<Arc*>& path)
{
	int cost = 0;
	for (Arc* arc : path)
	{
		cost += arc->cost;
	}
	return cost;
}

struct GreaterPathLength
{
	bool operator()(const std::vector<Arc*>& lhs, const std::vector<Arc*>& rhs) const
	{
		return GetPathCost(lhs) > GetPathCost(rhs);
	}
};
bool NodeHasArcToPosition(Node* node, std::string landingNodeName)
{
	for (auto a : node->arcs)
	{
		if (a->finish->name == landingNodeName)
		{
			return true;
		}
	}
	return false;
}

void FillWithAllPossibleArcs(Node* start, std::map<int, int>& hoogtes)
{
	std::vector < std::pair<int, int>> basisSprongen = { {0,0},{0,1},{0,2 },{0,3},{0,4},{1,0},{1,1},{1,2},{1,3},{2,0},{2,1},{2,2},{3,0},{3,1},{4,0} };
	for (auto s : basisSprongen)
	{
		std::pair<int, int> currentPos = { start->x,start->y };
		if (kanVooruitLanden(currentPos, s, hoogtes))
		{
			// creeer nieuwe landingspositie
			std::pair<int, int> landingPos;
			landingPos.first = currentPos.first + s.first + 1;
			landingPos.second = hoogtes.find(landingPos.first)->second;
			int energy = s.first + s.second;
			// als de landingspositie nog niet in de graph zit, voeg deze toe aan de queue
			std::string landingNodeName = Hash(landingPos);
			if (!NodeHasArcToPosition(start, landingNodeName))
			{
				Node* newNode = new Node(landingPos);
				Arc* newArc = new Arc(start, newNode, energy);
				start->arcs.insert(newArc);
			}
		}
	}
}
std::vector<Arc*> VindKortstePad(Node* start, Node* finish, std::map<int, int>& hoogtes)
{
	std::vector<Arc*> path;
	std::priority_queue< std::vector<Arc*>, std::vector<std::vector<Arc*>>, GreaterPathLength> queue;
	//std::priority_queue<Arc*,std::vector<Arc*>, GreaterPathLength> queue;
	std::map<std::string, int> fixed;
	while (start != finish)
	{
		if (fixed.find(start->name) == fixed.end()) // als het startpunt niet eerder is vastgepind, dan weten we dat deze nu de kortste route is
		{
			fixed[start->name] = GetPathCost(path);
			FillWithAllPossibleArcs(start, hoogtes);
			for (Arc* a : start->arcs)
			{
				if (fixed.find(a->finish->name) == fixed.end())
				{
					path.push_back(a);
					queue.push(path);
					path.erase(path.end() - 1); // CHECK
				}
			}
		}
		if (queue.empty())
		{
			//path.clear();
			return path;
		}
		path = queue.top(); queue.pop();
		start = path[path.size() - 1]->finish;
	}
	return path;
}

void Infi2()
{
	// aoc.infi.nl 2019, deel 2
	// Input (kan json load doen, maar dit was even sneller en is triviaal)
	//std::vector<std::pair<int, int>> flats = { {3,3},{5,5},{9,6},{13,7},{15,9},{16,10},{18,9},{21,10},{23,3},{25,4},{28,5},{29,7},{30,8},{34,7},{37,9},{39,7},{41,8},{42,10},{43,9},{47,10},{48,10},{50,6},{52,7},{53,10},{56,6},{57,4},{58,6},{59,2},{62,2} };
	std::vector<std::pair<int, int>> flats = { {3,7},{5,9},{8,5},{10,6},{14,7},{16,10},{21,9},{25,10},{30,6},{33,8},{34,9},{37,10},{42,8},{45,5},{47,6},{49,9},{50,10},{54,8},{57,9},{58,10},{61,8} };
	// Maak map van alle hoogtes van de flats: KEY= positie x van flat, VALUE = hoogte
	std::map<int, int> hoogtes;
	for (auto p : flats) hoogtes[p.first] = p.second;
	Node* start = new Node(flats.front());
	Node* finish = new Node(flats.back());
	Node* test = new Node({ 58, 10 });
	FillWithAllPossibleArcs(test, hoogtes);
	auto kortstePad = VindKortstePad(start, finish, hoogtes);
	int minEnergy = GetPathCost(kortstePad);

	// Presenteer resultaten
	//std::cout << "\nShortest path with Dijkstra: " << SimpleGraph::getPathCost(shortestPath)<<std::endl;
	//std::cout << "\nOptimale route:\n";
	//std::cout << "\n== START ===\n";
	//for (auto a : shortestPath)
	//{
	//	std::cout << a->start->name << " -cost[" << a->cost << "]->" << a->finish->name << std::endl;
	//}
	//std::cout << "== END ===\n";

	//graph.BFS(startNode);
	//std::cout << "nNodes: " << graph.nNodes;




	std::cout << "\nDruk [Enter] om te sluiten..";
	std::cin.get();
}

int main()
{
	// https://aoc.infi.nl 2019
	// Deze oplossing maakt gebruik Dijkstra's shortest path algorithm, mooi om onze Nederlandse trots toe te passen. 
	// 
	// Naast de C++ Standard Library (STL) includes voor standaard container classes heb ik eigen "SimpleGraph" class 
	// gebouwd en included.
	//
	// Basisgedachte: Vertaal de sprongen in een Directed Graph:
	// > iedere positie is een Vertex (/Node)
	// > iedere mogelijke sprong is een Edge (/Arc) naar een nieuwe positie
	// > de kosten van iedere sprongen is een dataveld van de Edge
	//
	// Als de graph eenmaal goed geconstrueerd is, is het eenvoudig om Dijkstra toe te passen.
	// groeten! Rogier (rogerscode@gmail.com)

	//Infi1(); // deel 1 van de puzzel
	Infi2(); // deel 2
	return 0;
}