#include <set>
#include <vector>
#include <map>
#include <queue>
#include <string>
#include <iostream>

// DEEL 1 VAN DE PUZZEL
// =================================================================================================================================
void Infi1()
{	// aoc.infi.nl 2019, deel 1
	// input
	std::vector<std::pair<int, int>> flats = { {2,5},{5,6},{8,8},{13,4},{17,5},{21,5},{24,7},{26,8},{27,9},{32,3},{37,3},{39,4},{43,2},{46,3},{47,2},{50,3},{53,5},{57,6},{59,3},{60,6} };
	std::vector < std::pair<int, int>> sprongen = { {2,1},{2,2},{4,0},{3,1},{0,0},{2,2},{1,1},{0,1},{4,0},{4,0},{1,1},{3,0},{2,1},{0,0},{2,1},{2,2},{3,1},{1,0},{0,3} };

	// Maak map van alle hoogtes van de flats: KEY= positie x van flat, VALUE = hoogte
	std::map<int, int> hoogtes;
	for (auto p : flats) hoogtes[p.first] = p.second;

	// Initialiseer
	std::pair<int, int> pos = flats[0];
	int nSprongen = 0;

	// Main loop: voer sprong uit tot [vallen] of tot [successvol landen na laatste sprong]
	std::cout << "Santa begint met springen, hou je hart vast:";
	std::cout << "\n== START ===\n";
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
	std::cout << "\n== END ===\n";
	std::cout << "\nDruk [Enter] om door te gaan..";
	std::cin.get();
}

// DEEL 2 VAN DE PUZZEL
// =================================================================================================================================
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
	// Deze hash wordt gebruikt om unieke landingsposities te vertalen naar een string die zal fungeren als unieke 
	// naam van de Nodes
	std::string posName;
	posName.append("(");
	posName.append(std::to_string(pos.first));
	posName.append(",");
	posName.append(std::to_string(pos.second));
	posName.append(")");
	return posName;
}
struct Arc; // Forward declaration nodig want Node en Arc zijn gekoppeld
struct Node 
{
	// Deze datastructuur vormt de basis voor onze graph (zie bestaat uit nodes en arcs). In dit voorbeeld zijn de
	//Nodes de dakposities
	Node(std::pair<int, int> p)
		:
		name(Hash(p)),
		position(p)
	{}
	// Basisvelden t.b.v. graph functionaliteit
	std::string name;
	std::pair<int,int> position;
	std::set<Arc*> arcs;
	friend bool operator==(const Node& p1, const Node& p2)
	{
		return (p1.name == p2.name);
	}
};
struct Arc 
{
	// Deze datastructuur definieert de arcs (verbindingen tussen de nodes, dus in dit voorbeeld: de sprongen)
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
int KostenVanPad(const std::vector<Arc*>& path)
{
	// Itereert over alle arcs (sprongen van dak-tot-dak in het pad) en telt de energie op van iedere sprong
	int cost = 0;
	for (Arc* arc : path)
	{
		cost += arc->cost;
	}
	return cost;
}
struct GroterePadKosten
{
	// Dit is een compare function die gebruikt wordt voor het sorteren (prioriteren) in de priority_map class voor de toepassing 
	// in het Dijkstra algorithme
	bool operator()(const std::vector<Arc*>& lhs, const std::vector<Arc*>& rhs) const
	{
		return KostenVanPad(lhs) > KostenVanPad(rhs);
	}
};
bool NodeBevatArcNaarPositie(Node* node, std::string landingNodeName)
{
	// Deze functie test of een sprong al is toegevoegd aan een gegeven positie; nodig om te voorkomen dat meerdere sprongen worden 
	// geregistreerd (we willen alleen de kleinste sprong)
	for (auto a : node->arcs)
	{
		if (a->finish->name == landingNodeName)
		{
			return true;
		}
	}
	return false;
}
void VindAlleBereikbareSchoorstenen(Node* start, std::map<int, int>& hoogtes)
{
	// Voor een gegeven positie (start node) zoekt deze functie alle mogelijke nieuwe landingsposities (schoorstenen), en de 
	// minimale sprongen die hiervoor genomen moeten worden.
	// Deze nieuwe posities en bijbehorende sprongen worden toegevoegd aan de graph als respectievelijk nieuwe Node* en Arc*
	// De vector basisSprongen van alle mogelijke sprongen is gesorteerd van lage energie naar hoge energie, dit helpt om optimale sprongen te vinden
	std::vector < std::pair<int, int>> basisSprongen = { {0,0},{0,1},{0,2},{0,3},{0,4},{1,0},{1,1},{1,2},{1,3},{2,0},{2,1},{2,2},{3,0},{3,1},{4,0} };
	for (auto s : basisSprongen) // loop door alle basissprongen
	{
		std::pair<int, int> currentPos = { start->position.first,start->position.second };
		if (kanVooruitLanden(currentPos, s, hoogtes))
		{
			// creeer nieuwe landingspositie
			std::pair<int, int> landingPos;
			landingPos.first = currentPos.first + s.first + 1;
			landingPos.second = hoogtes.find(landingPos.first)->second;
			int energy = s.first + s.second;
			std::string landingNodeName = Hash(landingPos);
			// Als de landingspositie nog niet in de graph zit, voeg deze dan toe aan de graph die we aan het construeren zijn
			// zo niet, dan negeren we de sprong (we weten al dat de eerdere geregistreerde sprong een lagere energie heeft, omdat
			// de vector basisSprongen gesorteerd is naar toenemende energie)
			if (!NodeBevatArcNaarPositie(start, landingNodeName))
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
	// Inits van support data structures
	std::vector<Arc*> path;
	std::priority_queue< std::vector<Arc*>, std::vector<std::vector<Arc*>>, GroterePadKosten> queue;
	std::map<std::string, int> fixed;
	
	// Main loop
	while (start->name != finish->name)
	{
		if (fixed.find(start->name) == fixed.end()) 
		{
			fixed.insert({ start->name,KostenVanPad(path) });
			VindAlleBereikbareSchoorstenen(start, hoogtes); 
			for (Arc* a : start->arcs)
			{
				{
					path.push_back(a);
					queue.push(path);
					path.erase(path.end() - 1);
				}
			}
		}
		if (queue.empty())
		{
			path.clear();
			return path;
		}
		path = queue.top(); queue.pop();
		start = path.back()->finish;
	}

	return path;
}
void Delete(Node* node, int& nArcs, int& nNodes)
{
	std::queue<Node*> queue;
	queue.push(node);
	std::set<std::string> visited;
	while (!queue.empty())
	{
		Node* currentNode = queue.front(); queue.pop();
		if (visited.find(Hash(currentNode->position)) == visited.end())
		{
			for (Arc* arc : currentNode->arcs)
			{
				Node* nextNode = arc->finish;
				if (visited.find(Hash(nextNode->position)) == visited.end())
				{				
					queue.push(nextNode);
					delete arc;
					nArcs++;
					arc = nullptr;
				}
			}
			visited.insert(Hash(currentNode->position));
			delete currentNode;
			nNodes++;
			currentNode = nullptr;
		}
	}
	return;
}

void Infi2()
{
	// aoc.infi.nl 2019, deel 2
	// Input (kan json load doen, maar dit was even sneller en is triviaal)
	std::vector<std::pair<int, int>> flats = { {2,5},{5,6},{8,8},{13,4},{17,5},{21,5},{24,7},{26,8},{27,9},{32,3},{37,3},{39,4},{43,2},{46,3},{47,2},{50,3},{53,5},{57,6},{59,3},{60,6} };
	// Maak map van alle hoogtes van de flats: KEY= positie x van flat, VALUE = hoogte
	std::map<int, int> hoogtes;
	for (auto p : flats) hoogtes[p.first] = p.second;

	// Definieer start en eind Nodes (schoorstenen)
	Node* start = new Node(flats.front());
	Node* finish = new Node(flats.back());

	// Pas Dijkstra algoritme toe
	auto kortstePad = VindKortstePad(start, finish, hoogtes);
	int minEnergie = KostenVanPad(kortstePad);

	// Presenteer resultaten
	std::cout << "\nMinimale Energie om van "<<start->name<<" naar "<<finish->name<<" te komen: " << minEnergie << '\n' ;
	std::cout << "\nOptimale route:\n";
	std::cout << "\n== START ===\n";
	for (auto a : kortstePad)
	{
		std::cout << a->start->name << " -cost[" << a->cost << "]->" << a->finish->name << '\n' ;
	}
	std::cout << "== END ===\n";
	std::cout << "\nDruk [Enter] om te sluiten..";
	std::cin.get();

	// Ruim geheugen op (old school garbage collection, of ik moet smart pointers (c++14) gebruiken
	int nArcs = 0; int nNodes = 0;
	Delete(start, nArcs, nNodes); std::cout << nNodes << " Nodes and " << nArcs << " Arcs deleted. End.";
	std::cin.get();
}

int main()
{
	// https://aoc.infi.nl 2019 / Oplossing #3
	// Deze oplossing maakt gebruik Dijkstra's shortest path algorithm, mooi om onze Nederlandse trots toe te passen. 
	// 
	// In tegenstelling tot oplossingen #1 en #2 is nu een aanpak geimplementeerd waarin niet eerste de volledige graph
	// van alle mogelijke routes wordt gemapt. Dit is het briljante van Dijkstra: je blijft gedurende het algo steeds 
	// de kortste uitbreiding van de route verkennen totdat je je doel vindt: dit is dan per definitie via de korste weg. 
	//
	// Ik gebruik alleen C++ STL data structuren. Een paar interessante features (zoals de std::priority_queue<...>) worden
	// gebruikt. Commentaar in de code spreekt hopelijk voor zich
	//
	// Nogmaals complimenten voor de puzzel, het was voor mij een perfecte manier om deze materie van allerlei kanten te
	// benaderen.
	// groeten! Rogier (rogerscode@gmail.com)

	Infi1(); // deel 1 van de puzzel
	Infi2(); // deel 2
	return 0;
}