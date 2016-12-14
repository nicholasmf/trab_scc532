#include <iostream>
#include <stdlib.h>
#include <fstream>
#include <string>

#define SIZE 4096
#define TOP 10

using namespace std;

class Products_list
{
public:
	Products_list();
	Products_list(int sup_thres_min, int conf_thres_min);
	Products_list(int argc, char **argv);
	~Products_list();
	int add_item(string item);
	void add_transaction(string transaction);
	int get_size();
	void print_list();
	void sort_list();

	void inc_count_trans();
	int get_count_trans();

	void print_support2();
	void print_confidence2();
	void get_confidence_rec(string items[], int size, int support, string trans_list[], int init);
	void apriori_rec();
	void print_best_rules();

private:
	string products[SIZE];
	int support[SIZE];
	string transactions[SIZE];
	int support2[SIZE][SIZE];
	string best_rules[TOP];
	float ranking[TOP];
	int size;
	int count_trans;
	int support_threshold_min;
	int support_threshold_max;
	int confidence_threshold_min;
	int confidence_threshold_max;
	float lift_threshold_min;
	float conviction_threshold_min;
	float novelty_threshold_min;
};

Products_list::Products_list()
{
	size = 0;
	count_trans = 0;
	this->support_threshold_min = 1;
	this->support_threshold_max = 1000000;
	this->confidence_threshold_min = 0;
	this->confidence_threshold_max = 100;
	this->lift_threshold_min = 0;
	this->conviction_threshold_min = 0;
	this->novelty_threshold_min = -1;

	for (int i = 0; i < this->size; i++) {
		for (int j = 0; j < this->size; j++) {
			support2[i][j] = 0;
		}
	}
}

Products_list::Products_list(int sup_thres_min, int conf_thres_min) {
	size = 0;
	count_trans = 0;
	this->support_threshold_min = sup_thres_min;
	this->support_threshold_max = 1000000;
	this->confidence_threshold_min = conf_thres_min;
	this->confidence_threshold_max = 100;
	this->novelty_threshold_min = -1;

	for (int i = 0; i < this->size; i++) {
		for (int j = 0; j < this->size; j++) {
			support2[i][j] = 0;
		}
	}
}

Products_list::Products_list(int argc, char **argv) {
	size = 0;
	count_trans = 0;
	this->support_threshold_min = 1;
	this->support_threshold_max = 1000000;
	this->confidence_threshold_min = 0;
	this->confidence_threshold_max = 100;
	this->lift_threshold_min = 0;
	this->conviction_threshold_min = 0;
	this->novelty_threshold_min = -1;

	for (int i = 1; i < argc; i += 2) {
		string op = argv[i];
		if (op == "-sup_min") this->support_threshold_min = atoi(argv[i + 1]);
		else if (op == "-sup_max") this->support_threshold_max = atoi(argv[i + 1]);
		else if (op == "-conf_min") this->confidence_threshold_min = atoi(argv[i + 1]);
		else if (op == "-conf_max") this->confidence_threshold_max = atoi(argv[i + 1]);
		else if (op == "-lift_min") this->lift_threshold_min = atoi(argv[i + 1]);
		else if (op == "-conv_min") this->conviction_threshold_min = atoi(argv[i + 1]);
		else if (op == "-nov_min") this->novelty_threshold_min = atof(argv[i + 1]);
		else {
			cout << op << " invalida\n";
		}
	}

	for (int i = 0; i < this->size; i++) {
		for (int j = 0; j < this->size; j++) {
			support2[i][j] = 0;
		}
	}

	for (int i = 0; i < TOP; i++) {
		ranking[i] = -1;
	}

	cout << "#support_min: " << this->support_threshold_min << endl << \
		"#support_max: " << this->support_threshold_max << endl << \
		"#confidence_min: " << this->confidence_threshold_min << "%" << endl << \
		"#confidence_max: " << this->confidence_threshold_max << "%" << endl << \
		"#lift_min: " << this->lift_threshold_min << endl << \
		"#conviction_min: " << this->conviction_threshold_min << endl << \
		"#novelty_min: " << this->novelty_threshold_min << endl << "#" << endl;
}

Products_list::~Products_list()
{
}

// Add or increment count on item and return it id
int Products_list::add_item(string item) {
	int i = 0;
	//cout << "this.item[0]: " << (this->products[i] == item) << '\n';
	while (i < this->size && this->products[i] != item) {
		i++;
	}
	if (i == this->size) {
		this->products[i] = item;
		this->support[i] = 1;
		this->size++;
	}
	else
	{
		this->support[i]++;
	}

	return i;
}

void Products_list::add_transaction(string transaction) {
	string item, transaction_list[SIZE];
	int size = this->get_count_trans();
	int count = 0;
	int itens_id[SIZE];

	this->transactions[size] = transaction;
	this->inc_count_trans();

	item = "";
	for (int i = 0; i < transaction.length(); i++) {
		if (transaction[i] == ' ') {
			itens_id[count] = this->add_item(item);
			transaction_list[count++] = item;
			//if (item == "NESCAU") nescau = true;
			item = "";
		}
		else {
			item += transaction[i];
		}
	}
	itens_id[count] = this->add_item(item);
	transaction_list[count++] = item;
	//if (nescau) std::cout << transaction << endl << endl;

	for (int i = 0; i < count; i++) {
		for (int j = 0; j < count; j++) {
			this->support2[itens_id[i]][itens_id[j]]++;
		}
	}
	//mylist->inc_count_trans();

}

int Products_list::get_size() {
	return this->size;
}

void Products_list::print_list() {
	for (int i = 0; i < this->size; i++) {
		std::cout << (float)support[i] / (float)this->count_trans << '\n';
	}
}

void Products_list::sort_list() {

}

void Products_list::inc_count_trans() {
	this->count_trans++;
}

int Products_list::get_count_trans() {
	return this->count_trans;
}

void Products_list::print_support2() {
	for (int i = 0; i < this->size; i++) {
		for (int j = 0; j < this->size; j++) {
			//if (support2[i][j] == 14)
				std::cout << products[i] << " -> " << products[j] << ": " << (float)support2[i][j] / (float)this->count_trans * 100.00 << '%' << endl;
		}
	}
}

void Products_list::print_confidence2() {
	float confidence, lift, conviction;

	for (int i = 0; i < this->size; i++) {
		for (int j = 0; j < this->size; j++) {
			if (i != j && this->support2[i][j] >= this->support_threshold_min) {
				confidence = (float)support2[i][j] / (float)this->support2[i][i];
				lift = ((float)support2[i][j] / (float)this->count_trans) / (((float)support2[i][i] / (float)this->count_trans) * ((float)this->support2[j][j] / (float)this->count_trans));
				conviction = ((1.0 - (float)this->support2[j][j] / (float)this->count_trans) / (1.0 - confidence / 100.00));
				std::cout << (float)support2[i][j] / (float)this->count_trans << "," << confidence << "," << lift << endl;
			}
		}
	}
}

void Products_list::get_confidence_rec(string items[], int size, int support, string trans_list[], int init) {
	int new_sup = 0;
	float confidence = 0, lift, conviction, novelty;
	int i, j, count;
	size_t result;
	bool found;

	if (support >= this->support_threshold_min && support <= this->support_threshold_max && size < 6) {
		for (int k = 0; k < this->size; k++) {
			found = false;
			for (i = 0; i < size; i++) {
				if (this->products[k] == items[i]) found = true;
			}
			if (this->support[k] >= this->support_threshold_min && this->support[k] <= this->support_threshold_max && !found) {
				string new_trans_list[SIZE];
				count = 0;
				//if (support != trans_count) cout << "trans_count: " << trans_count << " support: " << support << endl;
				for (i = 0; i < support; i++) {
					found = false;
					result = trans_list[i].find(this->products[k]);
					while (result != string::npos) {
						if (result != string::npos && (result == 0 || trans_list[i][result - 1] == ' ') && \
							(result + this->products[k].length() == trans_list[i].length() || trans_list[i][result + this->products[k].length()] == ' ')) {
							found = true;
						}
						result = trans_list[i].find(this->products[k], result + 1);
					}
					if (found) new_trans_list[count++] = trans_list[i];
					/*if (trans_list[i].find(this->products[k]) != string::npos) {
						new_trans_list[count++] = this->transactions[i];
					}*/
					/*
					for (j = 0; j < size; j++) {
						if (trans_list[i].find(items[j]) == string::npos) j = size + 1;
					}
					if (j == size) {
						new_trans_list[count++] = this->transactions[i];
					}
					*/
				}
				new_sup = count;
				confidence = (float)count / (float)support * 100.00;
				lift = ((float)count / (float)this->count_trans) / (((float)support / (float)this->count_trans) * ((float)this->support[k] / (float)this->count_trans));
				conviction = ((1.0 - (float)this->support[k] / (float)this->count_trans) / (1.0 - confidence / 100.00));
				novelty = ((float)new_sup / (float)this->count_trans) - ((float)this->support[k] / (float)this->count_trans * (float)support / (float)this->count_trans);
				if (count >= this->support_threshold_min) {
					//std::cout << this->products[k] << " <- " << items[0] << ": " << confidence << '%' << endl;
					//cout << "support: " << new_sup << " confidence: " << confidence << " lift: " << lift << " conviction: " << conviction << endl;
					if (confidence >= this->confidence_threshold_min && \
						confidence <= this->confidence_threshold_max && \
						count <= this->support_threshold_max && \
						lift >= this->lift_threshold_min && \
						conviction >= this->conviction_threshold_min && \
						novelty >= this->novelty_threshold_min) {
						string output = "";
						/*
						cout << "(" << ((float)new_sup / (float)this->count_trans) * 100.00 << "%/" << new_sup << ", " << \
							confidence << '%' << "," << \
							(float)this->support[k]/(float)this->count_trans * 100.00 << "%, " << \
							lift << ", " << conviction << ")" << endl;
						*/
						output += std::to_string((float)new_sup / (float)this->count_trans) + ",";
						output += std::to_string(confidence / 100.0) + ",";
						output += std::to_string(lift) + ",";
						output += std::to_string(conviction) + ",";
						output += std::to_string(novelty) + ",";
						output += std::to_string(size) + ",";
						//cout << (float)new_sup / (float)this->count_trans << "," << confidence / 100.0 << "," << lift << "," << conviction << "," << novelty << "," << size << ",";
						for (int i = 0; i < size; i++) {
							//cout << items[i] << " ";
							output += items[i] + " ";
						}
						output += " => " + products[k] + "\n";
						std::cout << output;
						//std::cout << " => " << this->products[k] << endl;

						// Verifica se entra no ranking de novidade
						i = 0;
						while (this->ranking[i] > novelty && i < TOP) i++;
						if (i != TOP) {
							for (int j = TOP - 1; j > i; j--) {
								ranking[j] = ranking[j - 1];
								best_rules[j] = best_rules[j - 1];
							}
							ranking[i] = novelty;
							best_rules[i] = output;
						}
					}
					items[size] = this->products[k];
					if (k > init)
						this->get_confidence_rec(items, size + 1, new_sup, new_trans_list, k);
				}
			}
		}
	}
}

void Products_list::apriori_rec() {
	string trans_list[SIZE];
	string items[SIZE];
	int count = 0;
	size_t result;
	bool found;

	for (int i = 0; i < this->size; i++) {
		count = 0;
		for (int j = 0; j < this->count_trans; j++) {
			found = false;
			result = this->transactions[j].find(this->products[i]);
			while (result != string::npos) {
				if (result != string::npos && (result== 0 || this->transactions[j][result - 1] == ' ') && \
					(result + this->products[i].length() == this->transactions[j].length() || this->transactions[j][result + this->products[i].length()] == ' ')) {
					found = true;
				}
				result = this->transactions[j].find(this->products[i], result + 1);
			}
			if (found) trans_list[count++] = this->transactions[j];
		}
		/*
		if (count != this->support[i]) {
			std::cout << count << " - " << support[i] << endl;
			for (int k = 0; k < count; k++) {
				std::cout << this->products[i] << endl << trans_list[k] << endl;
				getc(stdin);
			}
		}
		*/
		items[0] = this->products[i];
		this->get_confidence_rec(items, 1, this->support[i], trans_list, i);
	}
}

void Products_list::print_best_rules() {
	printf("#\n# Melhores regras (novidade)\n");
	for (int i = 0; i < TOP; i++) {
		if (best_rules[i] != "") {
			cout << "#" << best_rules[i];
		}
	}
}

int main(int argc, char **argv) {
	Products_list *mylist;
	string line, item;
	ifstream myfile;
	myfile.open("./list.data");
	int op = -1;
	string argv1 = argv[1];

	if (argc == 1) {
		cout << "Numero de parametros errado\n\n";
		return EXIT_FAILURE;
	}
	else if (argv1 == "--print_list") {
		op = 1;
		mylist = new Products_list();
	}
	else if (argv1 == "--print_data") {
		op = 2;
		mylist = new Products_list();
	}
	else {
		op = 0;
		mylist = new Products_list(argc, argv);
	}

	if (myfile.is_open()) {
		while (getline(myfile, line)) {
			/*
			item = "";
			for (int i = 0; i < line.length(); i++) {
				if (line[i] == ' ') {
					mylist->add_item(item);
					item = "";
				}
				else {
					item += line[i];
				}
			}
			mylist->add_item(item);
			//mylist->inc_count_trans();
			*/
			mylist->add_transaction(line);
		}
		myfile.close();
	}
	else {
		std::cout << "Unable to open file";
		return EXIT_FAILURE;
	}

	if (op == -1) {
		cout << "No operation selected\n";
		return EXIT_FAILURE;
	}
	else if (op == 1) {
		mylist->print_list();
	}
	else if (op == 0) {
		mylist->apriori_rec();
		mylist->print_best_rules();
	}
	else if (op == 2) {
		mylist->print_confidence2();
	}
	else {
		cout << "Invalid operation\n";
		return EXIT_FAILURE;
	}
	//std::cout << "Transacoes: " << mylist->get_count_trans() << endl;

	return EXIT_SUCCESS;
}
