#include<iostream>
#include<vector>
#include<fstream>
#include<list>
using namespace std;


vector<vector<int>> AdjencyMatrix(int rows, int columns) {
	ifstream adjacency_matrix_data("data.txt");
	if (adjacency_matrix_data) {
		cout << "Plik otwarty\n";
		vector<vector<int>> adjacency_matrix(rows, vector<int>(columns, 0));
		//Insert Values
		for (int i = 0; i < rows; i++) {
			for (int j = 0; j < columns; j++) {
				adjacency_matrix_data >> adjacency_matrix[i][j];
				if (adjacency_matrix[i][j] != 0) {
					//cout << "from: " << i +1 << " to: " << j + 1<< " weight: " << adjacency_matrix[i][j] << endl;

				}
				
			}
			
		}
		
		return adjacency_matrix;
	}
	else {
		cout << "Nie udalo sie otworzyc pliku\n";
	}
}

struct Edge {
	int from, to, weight;
};


vector<vector<int>> InsertValIntoColor(Edge edge, vector<vector<int>> color_matrix, int forest_counter, int rows = 8, int columns = 8) {
	int from = edge.from - 1;
	int to = edge.to - 1;
	//DLA KOLORU
	// Jeœli oba wierzcho³ki s¹ bia³e
	if (color_matrix[forest_counter - 1][from] == 0 && color_matrix[forest_counter - 1][to] == 0) {
		color_matrix[forest_counter - 1][from] = 1;
		color_matrix[forest_counter - 1][to] = 1;
	}

	// Jeœli jeden wierzcho³ek jest szary
	else if (color_matrix[forest_counter - 1][from] == 1 || color_matrix[forest_counter - 1][to] == 1) {
		color_matrix[forest_counter - 1][from] = 1;
		color_matrix[forest_counter - 1][to] = 1;
	}
	// Jeœli dwa wierzcho³ki sa szare
	else if (color_matrix[forest_counter - 1][from] == 1 && color_matrix[forest_counter - 1][to] == 1) {
		color_matrix[forest_counter - 1][from] = 1;
		color_matrix[forest_counter - 1][to] = 1;
	}

	// Kopiujemy wiersz do nastêpnego wiersza, jeœli istnieje
	if (forest_counter < rows) {
		for (int j = 0; j < columns; j++) {
			color_matrix[forest_counter][j] = color_matrix[forest_counter - 1][j];
		}
	}
	return color_matrix;
}
vector<vector<int>> InsertValIntoForest(Edge edge, vector<vector<int>> forest_matrix, vector<vector<int>> adjacency_matrix, list<Edge> &LER_edges, int& new_forest_val, int& forest_counter, int rows = 8, int columns = 8) {
	int from = edge.from - 1;
	int to = edge.to - 1;

	// Jeœli oba wierzcho³ki s¹ bia³e=> tworzymy nowy las
	if (forest_matrix[forest_counter - 1][from] == 0 && forest_matrix[forest_counter - 1][to] == 0) {
		new_forest_val++;
		forest_matrix[forest_counter - 1][from] = new_forest_val;
		forest_matrix[forest_counter - 1][to] = new_forest_val;
		//Dodanie Wartosci do LER
		LER_edges.push_back(edge);

	}
	// Jeœli jeden wierzcho³ek nale¿y do lasu, przypisujemy jego wartoœæ do drugiego
	else if (forest_matrix[forest_counter - 1][from] == 0 || forest_matrix[forest_counter - 1][to] == 0) {
		int existing_forest_val = max(forest_matrix[forest_counter - 1][from], forest_matrix[forest_counter - 1][to]);
		forest_matrix[forest_counter - 1][from] = existing_forest_val;
		forest_matrix[forest_counter - 1][to] = existing_forest_val;
		//Dodanie Wartosci do Listy
		LER_edges.push_back(edge);

	}
	// Gdy oba wierzcholki naleza do tego samego lasu=> odrzucamy
	else if (forest_matrix[forest_counter - 1][from] == forest_matrix[forest_counter - 1][to]) {
		forest_counter -= 1;
	}
	// Jeœli oba wierzcho³ki nale¿¹ do ró¿nych lasów=> ³¹czymy lasy
	else if (forest_matrix[forest_counter - 1][from] > 0 && forest_matrix[forest_counter - 1][to] > 0) {
		int forest_to_replace = forest_matrix[forest_counter - 1][to];
		int forest_to_keep = forest_matrix[forest_counter - 1][from];
		int existing_forest_val = max(forest_matrix[forest_counter - 1][from], forest_matrix[forest_counter - 1][to]) + 1;
		for (int i = 0; i < columns; i++) {
			if (forest_matrix[forest_counter - 1][i] == forest_to_replace || forest_matrix[forest_counter - 1][i] == forest_to_keep) {
				forest_matrix[forest_counter - 1][i] = existing_forest_val;
			}
		}
		//Dodanie Wartosci do Listy
		LER_edges.push_back(edge);
	}
	// Kopiowanie bie¿¹cego stanu do nastêpnego wiersza (jeœli istnieje i jeœli las nie jest kompletny)
	if (forest_counter < rows) {
		bool is_forest_complete = true;
		int first_value = forest_matrix[forest_counter - 1][0];

		for (int value : forest_matrix[forest_counter - 1]) {
			if (value != first_value) {
				is_forest_complete = false;
				break;
			}

		}

		//jesli las nie jest kompletny to kopiujemy obecny wiersz do nastepnego
		if (!is_forest_complete) {
			for (int j = 0; j < columns; j++) {
				forest_matrix[forest_counter][j] = forest_matrix[forest_counter - 1][j];
			}
		}
	}


	return forest_matrix;
}
list<Edge> MakeColorAndForestTable(vector<vector<int>> adjacency_matrix, list<Edge>LER_edges, int rows, int columns) {

	int from = 0;
	int to = 0;

	list<Edge> edges;

	//Dodanie Wartosci do Listy
	for (int i = 0; i < rows; i++) {
		for (int j = i + 1; j < rows; j++) {
			if (adjacency_matrix[i][j] != 0) {
				edges.push_back({i + 1, j + 1, adjacency_matrix[i][j] });
			}
		}
	}
	//Posortowanie Listy => Utworzenie LE(listy krawedzi)
	edges.sort([](Edge& x, Edge& y) {
		return x.weight < y.weight;
	});

	//Wydruk LE
	cout << "\nPosortowana lista krawedzi:\n";
	for (auto& edge : edges) {
		cout << "from: " << edge.from << " to: " << edge.to << " weight: " << edge.weight << endl;
	}
	vector<vector<int>> color_matrix(rows, vector<int>(columns, 0));
	vector<vector<int>> forest_matrix(rows, vector<int>(columns, 0));
	cout << "\n\n";

	//Wypelnienie lasu i koloru
	int forest_counter = 1;
	int new_forest_val = 0;

	// Algorytm wype³niania macierzy kolor
	for (auto& edge : edges) {
		if (forest_counter > rows) {
			break;
		}
		//DLA KOLORU
		color_matrix = InsertValIntoColor(edge, color_matrix, forest_counter);
		//DLA LASU
		forest_matrix = InsertValIntoForest(edge, forest_matrix, adjacency_matrix,LER_edges, new_forest_val, forest_counter);
		
		// Czy las jest skonczony
		vector<int> current_row = forest_matrix[forest_counter - 1];
		int first_value = current_row[0];
		bool is_forest_complete = false; // Zakladamy, ¿e las jest skoñczony

		for (int value : current_row) {
			if (value != first_value) {
				is_forest_complete = true; // Jeœli znajdziemy inn¹ wartoœæ, las nie jest skoñczony
				break;
			}
		}

		if (!is_forest_complete) {
			cout << "LAS SKONCZONY\n";
			break; 
		}
		else {
			cout << "las nie jest skonczony\n";
		}
		
		forest_counter++;
	}
	//Posortowanie LER
	LER_edges.sort([](Edge& x, Edge& y) {
		return x.weight < y.weight;
		});
	//Wydruk LER
	cout << "\nPosortowana LER:\n";
	for (auto& edge : LER_edges) {
		cout << "from: " << edge.from << " to: " << edge.to << " weight: " << edge.weight << endl;
	}

	//DLA KOLORU
	cout << "Kolor: \n";
	cout << "1 2 3 4 5 6 7 8\n";
	for (int i = 0; i < forest_counter; i++) {
		for (int j = 0; j < columns; j++) {
				cout << color_matrix[i][j] << " ";
		}
		cout << endl;
	}
	cout << endl; cout << endl;
	//DLA LASU
	cout << "Las: \n";
	cout << "1 2 3 4 5 6 7 8\n";
	for (int i = 0; i < forest_counter; i++) {
		for (int j = 0; j < columns; j++) {
			cout << forest_matrix[i][j] << " ";
		}
		cout << endl;
	}
	return LER_edges;
}




int main() {

	
	const int rows = 8; 
	int columns = 8;

	vector<vector<int>> adjacency_matrix = AdjencyMatrix(rows, columns);
	list<Edge> LER_edges;

	MakeColorAndForestTable(adjacency_matrix, LER_edges, rows, columns);


	return 0;
}
