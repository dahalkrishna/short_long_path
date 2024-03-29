#include <iostream>
#include <fstream>
#include <vector>
#include <queue>
#include <limits.h>
#include <string>
#include <sstream>
#include <stack>

using namespace std;

const int N = 10001; // Maximum number of nodes

vector<pair<int, int> > adj[N]; // Adjacency list: {destination, weight}
vector<int> input_list;
vector<int> output_list;
int short_delay_list[10000][10000];
int long_delay_list[10000][10000];
// vector <int , int> short_delay;
// vector <int , int> long_delay;
void topologicalSortUtil(int v, bool visited[], stack<int>& Stack) {
	visited[v] = true;
	for (auto& edge : adj[v]) {
		int u = edge.first;
		if (!visited[u])
			topologicalSortUtil(u, visited, Stack);
	}
	Stack.push(v);
}


void longestPath(int src, int n) {
	stack<int> Stack;
	vector<int> dist(n, INT_MIN); // Initialize distances to negative infinity
	bool visited[N] = {false};
	for (int i = 0; i < n; ++i) {
		if (!visited[i]){
			topologicalSortUtil(i, visited, Stack);
		}
	}
	dist[src] = 0; // Distance from source to itself is 0

	while (!Stack.empty()) {
		int u = Stack.top();
		Stack.pop();

		for (auto& edge : adj[u]) {
			int v = edge.first;
			int weight = edge.second;

			if (dist[u] != INT_MIN && dist[v] < dist[u] + weight) {
				dist[v] = dist[u] + weight;
			}
		}
	}

	// Print longest distances from the source
	for (int i = 0; i < n; ++i) {
		if(dist[i] != INT_MIN){
			for (int j : output_list) {
				if (i == j) {
					// cout << "LD Node " <<src<<" "<< i << ": " << dist[i] << "\n";
					long_delay_list[src][i]=dist[i];
				}
			}
		}
	}
}


void dijkstra(int src, int n) {
	vector<int> dist(n, INT_MAX);
	dist[src] = 0;

	priority_queue<pair<int, int>, vector<pair<int, int> >, greater<pair<int, int> > > pq;
	pq.push({0, src});

	while (!pq.empty()) {
		int u = pq.top().second;
		pq.pop();

		for (auto& edge : adj[u]) {
			int v = edge.first;
			int weight = edge.second;

			if (dist[u] + weight < dist[v]) {
				dist[v] = dist[u] + weight;
				pq.push({dist[v], v});
			}
		}
	}

	// Print shortest distances from the source
	for (int i = 0; i < n; ++i) {
		if(dist[i] != INT_MAX){
			for (int j : output_list) {
				if (i == j) {
					// cout << "SD Node " <<src<<" "<< i << ": " << dist[i] << "\n";
					short_delay_list[src][i]=dist[i];
				}
			}
		}
	}
}

void ReadFile(ifstream& eventfile) {
	string line, token;
	int source,delay,dest, i;
	while (getline(eventfile, line)!=NULL) {
		//skip lines that begin with C, I and O
		if(line[0] == 'C') {
			continue;
		}
		if(line[0] == 'I'){
			stringstream ss(line);
			//skip INPUT
			ss >> token;
			//extract the input node and convert to int
			ss >> token;
			input_list.push_back(atoi(token.c_str()));
			continue;
		}
		else if(line[0] == 'O'){
			stringstream ss(line);
			//skip INPUT
			ss >> token;
			//extract the output node and convert to int
			ss >> token;
			output_list.push_back(atoi(token.c_str()));
			continue;
		}
		//getting the destination node
		i = line.size()-1;
		dest =0;
		while (line[i] != ' '){
			dest = atoi(&line[i]);
			i--;
		}
		// cout << "Destination: " << dest << endl;
		//getting delays

		stringstream ss(line);
		// skip the gate name
		ss >> token;
		cout<< token<<endl;        
		//extract the gate delay and convert to int
		ss >> token;
		token.erase(token.find("ns"));
		if (std::stringstream(token) >> delay ) {
		delay = atoi(token.c_str());
		}
		ss >> token;
		while (dest != atoi(token.c_str())){
			source = atoi(token.c_str());
			adj[source].push_back({dest, delay});
			ss >> token;
		}
	}
}
	

int main(int argc, char *argv[]) {
	int mindelay[500];
	int maxdelay[500];
	for (int i = 0; i < 500; i++) {	mindelay[i] = INT_MAX;	maxdelay[i] = INT_MIN;	}
	ifstream eventfile;
	string file_path, file_num, benchmark;
	file_num = argv[1];
	benchmark = argv[2];
	input_list.clear();
	output_list.clear();
	adj->clear();
	input_list.shrink_to_fit();
	output_list.shrink_to_fit();
	adj->shrink_to_fit();
	file_path = "./healthy_inst/"+ string(benchmark)+"_"+string(file_num)+"_event.txt";
	eventfile.open(file_path);
	ReadFile(eventfile);
	eventfile.close();
	int n = 10000; // Number of nodes
	n++;
	int sourceNode;
	//initialize short and long delay matrix
	for (int j : output_list) {
			for (int i : input_list) {
				short_delay_list[i][j] = INT_MAX;
			long_delay_list[i][j] = INT_MIN;
		}
	}	
	for (int i : input_list) {
		sourceNode = i;
		cout<< i<< "\t";
		dijkstra(sourceNode,n);
		// longestPath(sourceNode,n);
	}
	int k = 0;
	ofstream mindelayfile;
	mindelayfile.open("mindelay.csv", ios::app);
	for (int j : output_list) {
		int temp_short = INT_MAX;
		for (int i : input_list) {
			if(short_delay_list[i][j] != 0)
				if(short_delay_list[i][j] < temp_short){
					temp_short = short_delay_list[i][j];
					cout<<"Short delay from "<<i<<" to "<<j<<": "<<temp_short<<endl;
				}
		}
		// cout << "Min short delay "<<j<<": "<< temp_short/3 << endl;
		mindelayfile << temp_short/3 <<",";
		if (mindelay[k] > temp_short/3){
			mindelay[k] = temp_short/3;
		}
		k++;
		// cout << endl;
	}
	mindelayfile << endl;
	mindelayfile.close();
	k = 0;
	ofstream maxdelayfile;
	// maxdelayfile.open("maxdelay.csv", ios::app);
	// for (int j : output_list) {
	// 	int temp_long = INT_MIN;
	// 	for (int i : input_list) {
	// 		if(long_delay_list[i][j] != 0)
	// 			if(long_delay_list[i][j] > temp_long){
	// 				temp_long = long_delay_list[i][j];
	// 				cout<<"Long delay from "<<i<<" to "<<j<<": "<<temp_long<<endl;
	// 			}
	// 	}
	// 	// cout << "Max long delay "<<j<<": "<< temp_long/3 << endl;
	// 	maxdelayfile << temp_long/3 <<",";
	// 	if (maxdelay[k] < temp_long/3){
	// 		maxdelay[k] = temp_long/3;
	// 	}
	// 	k++;
	// 	// cout << endl;
	// }
	// maxdelayfile << endl;
	// maxdelayfile.close();
	return 0;
}