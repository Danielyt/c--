#include <iostream>
#include <fstream>
#include <sstream>
#include <stdlib.h>
#include <algorithm>
#include "AVLTree.cpp"

//#include "Alphabet.cpp"
//#include "hash-table.cpp"
#include "stuff.cpp"
//#include "hash.cpp"
//#include "sha1-hash.cpp"
//#include "sha1.h"
//#include <windows.h>
//#include <time.h>

//struct node;
//int alphabet_max_size = 128;
//AVLTree tree;

Alphabet azbuka ("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz'.[\\]^_`");
//Alphabet azbuka ("abcdefghijklmnopqrstuvwxyz'.");
int difference = 0;
long long memory = 0;
long long string_capacity = 0;
int leak = 0;
struct node {
	Transition_Hash * neighbours;
	int index;
	bool final;
	node()
		:neighbours(new Transition_Hash()), index(-1), final(false){}
	node(const node& nSource){
		index = nSource.index;
		final = nSource.final;
    	if (nSource.neighbours) {   		
        	neighbours = new Transition_Hash(nSource.neighbours->size, nSource.neighbours->capacity, nSource.neighbours->p);        	
        	for (int i = 0; i < nSource.neighbours->capacity; i++) {
        		neighbours->array[i].ch = nSource.neighbours->array[i].ch;   		
        		neighbours->array[i].dest = nSource.neighbours->array[i].dest;
        	}
    	}
    	else
        	neighbours = 0;
		}

	node& operator=(const node& nSource) {		
		if (this == &nSource)
        	return *this;
 
    	delete neighbours;
    	
    	index = nSource.index;
		final = nSource.final; 
    	if (nSource.neighbours) {
           	neighbours = new Transition_Hash(nSource.neighbours->size, nSource.neighbours->capacity, nSource.neighbours->p);
           	for (int i = 0; i < nSource.neighbours->capacity; i++) {
        		neighbours->array[i].ch = nSource.neighbours->array[i].ch;
        		neighbours->array[i].dest = nSource.neighbours->array[i].dest;
        	}
    	}
    	else
        	neighbours = 0; 		
    	return *this;
    }

	~node(){
		delete neighbours;
    	neighbours = 0;
	}
};

struct NLev_node {	
	int index;
	char row;
	NLev_node()
		:index(1), row(0) {}
	NLev_node(int i, char r)
		:index(i), row(r){
	}
	~NLev_node(){}
};

class Graph {
public:
	node * nodes;
	int size;
	int capacity;
	Graph(){
		nodes = (node *) calloc (2, sizeof(node));		
		size = 0;
		capacity = 2;
	}
	Graph (int s, int c)
		:size(s), capacity(c){}
	void resize_Graph(){
		Graph g2(0, 2*capacity);
		g2.nodes = (node *) calloc (2 * capacity, sizeof(node));
		
		for (int k = 0; k < size; k++) {
			g2.nodes[k] = nodes[k];
			if (nodes[k].neighbours)
				delete nodes[k].neighbours;
			nodes[k].neighbours = 0;
		}

		free(nodes);		
		nodes = g2.nodes;
		capacity = g2.capacity;
		g2.nodes = 0;
	}

	void push_back(node n){
		if ((double)size / capacity > 0.9)
			resize_Graph();
		nodes[size] = n;
		size++;
	}

	void pop_back(){
		size--;
		delete nodes[size].neighbours;
		nodes[size].neighbours = 0;
	}

	node back(){
		/*std::cout << "in back\n";
		std::cout << "size: " << size;
		std::cout << "\ninfo on back\n";
		std::cout << "index: " << nodes[size - 1].index;
		std::cout << "\nsize: " << nodes[size - 1].neighbours->size;
		std::cout << "\ncapacity: " << nodes[size - 1].neighbours->capacity;*/
		return nodes[size - 1];
	}
	~Graph(){
		for (int i = 0; i < size; i++)
			delete nodes[i].neighbours;
		free(nodes);
	}
};

class Set_of_Nlev_nodes {
public:	
	NLev_node * nodes;
	int size;
	int capacity;
	int index;
	char p;
	bool final;

	Set_of_Nlev_nodes()
		:nodes ((NLev_node *) calloc (2, sizeof(NLev_node))),
		 size(0), capacity(2), index(-1), final(false), p(53){
		 	leak++;
	}

	Set_of_Nlev_nodes(int s, int c, int ind, char prime, bool f)
		:size(s), capacity(c), index(ind), p(prime), final(f) {}

	Set_of_Nlev_nodes(const Set_of_Nlev_nodes& setSource){
		size = setSource.size;
		capacity = setSource.capacity;
		index = setSource.index;
		p = setSource.p;
		final = setSource.final;
		if(setSource.nodes){
			nodes = (NLev_node *) calloc (capacity, sizeof(NLev_node));
			leak++;
			for(int i = 0; i < capacity; i++) {
				nodes[i].index = setSource.nodes[i].index;
				nodes[i].row = setSource.nodes[i].row;
			}
		}
		else
			nodes = 0;
	}

	Set_of_Nlev_nodes& operator=(const Set_of_Nlev_nodes& setSource){
		if (this == &setSource)
			return *this;
		
		free(nodes);
		
		size = setSource.size;
		capacity = setSource.capacity;
		index = setSource.index;
		p = setSource.p;
		final = setSource.final;

		if(setSource.nodes){
			nodes = (NLev_node *) calloc (capacity, sizeof(NLev_node));
			leak++;
			for(int i = 0; i < capacity; i++) {
				nodes[i].index = setSource.nodes[i].index;
				nodes[i].row = setSource.nodes[i].row;
			}
		}
		else
			nodes = 0;
		return *this;
	}

	void resize_Set(){
		Set_of_Nlev_nodes set2(0, 0, -1, 53, false);
		
		set2.nodes = (NLev_node *) calloc (2 * capacity, sizeof(NLev_node));
		set2.capacity = 2*capacity;
		for (int k = 0; k < capacity; k++)
			if(nodes[k].row != 0)
				set2.insert(nodes[k]);		
		free(nodes);		
		nodes = set2.nodes;
		capacity = set2.capacity;
		p = set2.p;
		set2.nodes = 0;
	}

	void insert(NLev_node n){
		if (search(n) == -1) {
			if ((double)size / capacity > 0.9)
				resize_Set();
			
			int k = hash1(n.index, n.row);
			while(nodes[k].row != 0)
				k = hash2(k);
			nodes[k].index = n.index;
			nodes[k].row = n.row;
			size++;
		}
	}

	int search(NLev_node n) {
		int k = hash1(n.index, n.row);
		int i = 0;
		while (nodes[k].row != 0
			&& nodes[k].index != n.index
			&& nodes[k].row != n.row 
			&& i < size) {								
				i++;		
				k = hash2(k);				
			}

			if (nodes[k].index == n.index && nodes[k].row == n.row)
				return k;
			return -1;
	}

	int hash1(int a, char b) {
		return (a*2 + b) % capacity;
	}

	int hash2(int n) {
		return (n + p) % capacity;
	}

	~Set_of_Nlev_nodes(){
		free(nodes);
	}
};


struct Vector {
	Set_of_Nlev_nodes * sets;
	int size;
	int capacity;
	Vector(){
		sets = (Set_of_Nlev_nodes *) calloc (2, sizeof(Set_of_Nlev_nodes));
		size = 0;
		capacity = 2;
	}
	Vector(int s, int c)
		:size(s), capacity(c){}
	
	void resize_Vector(){
		Vector v2(0, 2 * capacity);
		//free(v2.sets);
		v2.sets = (Set_of_Nlev_nodes *) calloc (2 * capacity, sizeof(Set_of_Nlev_nodes));
		//v2.capacity = 2*capacity;
		
		for (int k = 0; k < size; k++)
			v2.sets[k] = sets[k];
		
		free(sets);
		sets = v2.sets;
		capacity = v2.capacity;
		v2.sets = 0;
	}

	void push_back(Set_of_Nlev_nodes n){
		if ((double)size / capacity > 0.9)
			resize_Vector();
		sets[size] = Set_of_Nlev_nodes(n);
		size++;
	}

	void pop_back(){
		size--;
		free(sets[size].nodes);
		sets[size].nodes = 0;		
	}

	Set_of_Nlev_nodes back(){
		return sets[size - 1];
	}
};


std::string stringify (node n){
	std::stringstream ss;
	ss << n.final << ',';
	for (int i = 0; i < azbuka.get_num_of_symbols(); i++) {
		int k = n.neighbours->search(i + 1);
		if ( k != -1)
			ss << n.neighbours->array[k].ch << '>' << n.neighbours->array[k].dest << ',';
	}
	/*for (int i = 0; i < n.neighbours->capacity; i++)
		if (n.neighbours->array[i].ch != 0)
			ss << azbuka.get_symbol_at_index(n.neighbours->array[i].ch -1) << '>' << n.neighbours->array[i].dest << ',';*/
	return ss.str();
}

bool myfunction (NLev_node i, NLev_node j) {
	if (i.row != j.row)
		return (i.row < j.row);
	return (i.index < j.index);
}

std::string stringify(Set_of_Nlev_nodes s){
	std::sort(s.nodes, &s.nodes[s.capacity], myfunction);
	std::stringstream ss;
	//ss << (int)s.nodes[0].row << ", ";
	for (int i = 0; i < s.capacity; i++)
		ss << (int)s.nodes[i].row << ',' << s.nodes[i].index << ",";
	return ss.str();
}

std::string read(std::ifstream& sourceFile) {
	char * symbol = new char[1];
	int startPos = sourceFile.tellg();
	std::string str;	
	if (sourceFile.good())
		getline(sourceFile, str);
	return str;
}

//int find_minimized(Graph& g, Sha1_Hash& sh_hash, node t){
int find_minimized(Graph& g, AVLTree& tree, node t){
	std::string t_str = stringify (t);
	//int index = sh_hash.search(t_str);
	int index = tree.findString(t_str);
	if (index < 0){
		t.index = g.size;// + 1;
		g.push_back(t);
		tree.insert(t.index, t_str);
		//sh_hash.insert(t_str, t.index);
		difference += t.neighbours->capacity - t.neighbours->size;
		return t.index;
	}
	else {		
		//return sh_hash.array[index].index;
		return index;
	}
}

void graphviz(Graph& g, std::string file_name){
	std::ofstream output_file;
	output_file.open (file_name.c_str());
	output_file << "diGraph g{\n";
	for (int i = 0; i < g.size; i++){
		node q = g.nodes[i];
		for (int j = 0; j < q.neighbours->capacity; j++){
			if (q.neighbours->array[j].ch != 0)
				output_file << "\t" << q.index << " -> " << q.neighbours->array[j].dest << " [ label=\"" << azbuka.get_symbol_at_index(q.neighbours->array[j].ch - 1) << "\" ]\n";
		}
		if (q.final)
			output_file << "\t" << q.index << " [fillcolor=\"gray\", style=\"filled\"];\n";
	}
	output_file << "}";
	output_file.close();
}

std::pair <int,int> info(Graph& g){
	//memory = 0;
	int count = 0;
	int finals = 0;
	for (int i = 0; i < g.size; i++){
		count += g.nodes[i].neighbours->size;
		//difference+= g.nodes[i].neighbours->capacity - g.nodes[i].neighbours->size;
		//memory+= g.nodes[i].neighbours->capacity;
		if (g.nodes[i].final)
			finals++;
	}
	return std::make_pair (count, finals);
}

Set_of_Nlev_nodes delta(Graph& g, NLev_node n, int c){
	 Set_of_Nlev_nodes result;
	 	 
	 //check if there is a transition with c from n
	 int next = g.nodes[n.index].neighbours->search(c);

	 //if so add the corresponding node
	 if (next != -1) {
	 	int dest = g.nodes[n.index].neighbours->array[next].dest;
	 	result.insert(NLev_node(dest, n.row));
	 	
	 	if (g.nodes[dest].final) {

	 		result.final = true;
	 	}
	 	else {
	 		if (n.row == 1)
	 			for (int i = 0; i < g.nodes[dest].neighbours->capacity; i++)
	 				if (g.nodes[dest].neighbours->array[i].ch != 0 && g.nodes[g.nodes[dest].neighbours->array[i].dest].final) {
	 					result.final = true;
	 					break;
	 				}
		}
	 	//return result;
	 }

	 //then there is no immediate transition with c from n.index;
	 //if we are on the second row, return an empty set
	 if (n.row == 2)
	 	return result;
	 //we are on the first row

	 //add the directly above node	 
	 result.insert(NLev_node(n.index, 2));
	 if (g.nodes[n.index].final)
	 	result.final = true;
	 //if n has neighbours, add them
	 if (g.nodes[n.index].neighbours->size > 0) {	 		 	
	 	for (int i = 0; i < g.nodes[n.index].neighbours->capacity; i++)
	 		if (g.nodes[n.index].neighbours->array[i].ch != 0) {
	 			
	 			int next_ind = g.nodes[n.index].neighbours->array[i].dest;
	 			int next_next = g.nodes[next_ind].neighbours->search(c);
	 			result.insert(NLev_node(next_ind, 2));
	 			if (g.nodes[next_ind].final)
	 				result.final = true;
	 			if (next_next != -1) {
	 				result.insert(NLev_node(g.nodes[next_ind].neighbours->array[next_next].dest, 2));
	 				if (g.nodes[g.nodes[next_ind].neighbours->array[next_next].dest].final)
	 					result.final = true;
	 			}	 			
	 		}
	 	}

	 return result;
}

Set_of_Nlev_nodes delta(Graph& g, Set_of_Nlev_nodes s, int c) {	
	Set_of_Nlev_nodes result;
	for (int i = 0; i < s.capacity; i++) {
		if (s.nodes[i].row != 0) {					
			Set_of_Nlev_nodes from_delta = delta(g, s.nodes[i], c);		
			for (int j = 0; j < from_delta.capacity; j++) {
				if (from_delta.nodes[j].row != 0)
					result.insert(from_delta.nodes[j]);			
			}
		
			if (from_delta.final)
				result.final = true;
		}
	}
	return result;
}

bool accepts (Graph& g, std::string w){
	int q = g.nodes[0].index;
	int position = 0;
	while (position < w.length() && q != -1) {
		int next = g.nodes[q].neighbours->search(azbuka.get_index_of_symbol(w.at(position)) + 1);
			if (next != -1)
				q = g.nodes[q].neighbours->array[next].dest;
			else
				q = -1;
			position++;
	}
	if (q == -1)
		return false;
	if (position == w.length() && g.nodes[q].final)
		return true;
	return false;
}

void build_madfa(char* filename, Graph& Q_T) {
	std::ifstream sourceFile;
	sourceFile.open (filename, std::ifstream::binary);
	//Graph Q_T;
	Graph T;
	//Sha1_Hash sh_hash;
	AVLTree tree;

	T.push_back(node());
	Q_T.push_back(node());
	
	std::string alphaN;
	int len_alphaN = 0;
	
	if (sourceFile) {
		int wn = 0;
		std::cout << "Reading from file\n";
		while(sourceFile.good()){			
			std::string word = read(sourceFile);
			int len_word;			
			if (word.length() > 0 && (int)word.at(word.length() - 1) == 13)
				len_word = word.length() - 1;
			else
				len_word = word.length();			
			if (len_word > 0) {	
							
				wn++;				
				if (wn % 5000 == 0)
					std::cout << wn << ": " << word << "\n";								
				int i = 0;								
				while (i < len_word && i < len_alphaN && word.at(i) == alphaN.at(i))
					i++;
				
				for (int j = len_alphaN; j > i; j--) {
					node tj = T.back();
					T.pop_back();										
					int c = azbuka.get_index_of_symbol(alphaN.at(j-1)) + 1;					
					int ind = find_minimized(Q_T, tree, tj);					
					T.nodes[T.size - 1].neighbours->change(c, ind);					
				}
								
				for(int j = i; j < len_word; j++){
					T.push_back(node());
					if ( !azbuka.is_in_alphabet(word.at(j)) ) {
						std::cout << "Error: character not in alphabet in word: " << word;
						return;
						//return Graph();
					}
					//T.nodes[T.size - 2].neighbours->change(azbuka.get_index_of_symbol(word.at(j)) + 1, (-1)*T.size);
					T.nodes[T.size - 2].neighbours->insert(azbuka.get_index_of_symbol(word.at(j)) + 1, (-1)*T.size);
				}				
				T.nodes[T.size - 1].final = true;				
				alphaN = word;
				len_alphaN = len_word;
			}
		}
		sourceFile.close();

		for(int j = len_alphaN; j > 0; j--) {
			node tj = T.back();
			T.pop_back();			
			int c = azbuka.get_index_of_symbol(alphaN.at(j-1)) + 1;
			int ind = find_minimized(Q_T, tree, tj);
			T.nodes[T.size - 1].neighbours->change(c, ind);
		}

		Q_T.nodes[0] = T.back();
		T.pop_back();		
		Q_T.nodes[0].index = 0;
	}
	//return Q_T;
}

Graph build_levenshtein(Graph& gr){

		//Sha1_Hash sh_hash2;
		AVLTree tree;
		Graph levenshtein;
		
		Vector states_to_check;
		states_to_check.push_back(Set_of_Nlev_nodes());
		states_to_check.sets[states_to_check.size - 1].insert(NLev_node(0,1));
		states_to_check.sets[states_to_check.size - 1].index = 0;
		if (gr.nodes[0].final)
			states_to_check.sets[states_to_check.size - 1].final = true;
		else
			for(int i = 0; i < gr.nodes[0].neighbours->capacity; i++)
				if (gr.nodes[0].neighbours->array[i].ch != 0
					&& gr.nodes[gr.nodes[0].neighbours->array[i].dest].final) {
					states_to_check.sets[states_to_check.size - 1].final = true;
					break;
				}
		
		levenshtein.push_back(node());
		levenshtein.nodes[0].index = states_to_check.back().index;
		levenshtein.nodes[0].final = states_to_check.back().final;		
		while (states_to_check.size > 0){			

			Set_of_Nlev_nodes substate = states_to_check.back();
			states_to_check.pop_back();
			
			for (int c = 0; c < azbuka.get_num_of_symbols(); c++) {				
				Set_of_Nlev_nodes candidate = delta(gr, substate, c + 1);				
				if (candidate.size > 0) {
					std::string candidate_str = stringify(candidate);
					
					//int search = sh_hash2.search(candidate_str);
					int search = tree.findString(candidate_str);
					if (search < 0) {
						node new_node;
						new_node.index = levenshtein.size;
						new_node.final = candidate.final;
						//sh_hash2.insert(candidate_str, new_node.index);
						tree.insert(new_node.index, candidate_str);
						candidate.index = new_node.index;
						levenshtein.nodes[substate.index].neighbours->change(c + 1, new_node.index);
						levenshtein.push_back(new_node);
						if (levenshtein.size % 5000 == 0)
							std::cout << "state: " << levenshtein.size << "\n";
						states_to_check.push_back(candidate);
					}
					else
						//levenshtein.nodes[substate.index].neighbours->change(c + 1, sh_hash2.array[search].index);
						levenshtein.nodes[substate.index].neighbours->change(c + 1, search);
					//free(candidate_str);
				}
			}
		}
		return levenshtein;
}

int main (int argc, char* argv[]){
	
	Graph Q_T;
	//Graph T;

	if (argc > 1)
		//sourceFile.open (argv[1], std::ifstream::binary);
		build_madfa(argv[1], Q_T);

	
		//std::cout << "printing to file\n";
		//graphviz(Q_T, "result for madfa.txt");
		
		std::cout << "Number of nodes: " << Q_T.size << "\n";

		std::pair <int, int> inf = info(Q_T);
		std::cout << "Number of edges: " << inf.first << "\n";
		std::cout << "Number of final nodes: " << inf.second << "\n";
		//std::cout << "size of node: " << sizeof(node) << "\n";
		//std::cout << "size of Transition_Hash: " << sizeof(Transition_Hash) << "\n";
		//std::cout << "Q_T capacity: " << Q_T.capacity << "\n";
		//std::cout << "memory: " << memory << "\n";
		
		std::cout << "Building Levenshtein...\n";

		Graph levenshtein = build_levenshtein(Q_T);

		
		std::cout << "Info on levenshtein\n";
		//graphviz(levenshtein, "result from levenshtein-13.txt");
		
		std::cout << "Number of nodes: " << levenshtein.size << "\n";

		inf = info(levenshtein);
		std::cout << "Number of edges: " << inf.first << "\n";
		std::cout << "Number of final nodes: " << inf.second << "\n";
		//std::cout << "difference: " << difference << "\n";
		//std::cout << "Size of levenshtein: " << levenshtein.capacity * (sizeof(node) + 16) + memory * 8 << "\n";
		std::string check = "";
		while (check != "exit") {
			std::cout << "enter a word: ";
			std::cin >> check;
			if (accepts(levenshtein, check))
				std::cout << "true\n";
			else
				std::cout << "false\n";
		}		
	return 0;
}