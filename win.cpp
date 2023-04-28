/* 
 * Win it's a program that is designed to be a pratical and 
 * quickly method to save, change and delete data of clients
 * of a determinated service
 *
 * Author: Gabriel Felipe
 * Github: https://github.com/gabrielfelipeassuncaodesouza
 * Email: gabrielfelipeassuncaodesouza@gmail.com
 *
 */

#include <iostream>
#include <stdbool.h>
#include <ctime>
#include <regex>

bool isEmpty();

/* Here it's defined the class Client who contains all
 * informations about the client. So, all operations with
 * dates and more else will be done here
 */

int counter = 0;

class Client {
	public:
		std::string getName();
		std::string getService();
		std::string getDate();
		int getIndex();
		void setName(std::string name);
		void setService(std::string service);
		void setDate(std::string date);
		bool isFull();
	private:
		std::string name;
		std::string service;
		struct tm* dateOfPayment;
		int index;

	public:
		Client() {
			time_t now = time(0);
			struct tm* data = localtime(&now);
			this->dateOfPayment = data;
			this->index = counter;
		}
};

/*
 * Here it's a configuration of the system
 */

enum modes {
	OFF = 0,
	ON
};

typedef struct ClientNode {
	Client client;
	ClientNode* next;
} ClientNode;

typedef struct ClientList {
	ClientNode* head;
	ClientNode* tail;
} ClientList;

ClientList list;

typedef struct Config {
	int addMode;
	int sMode;
	int dMode;
	int swMode;
	bool saved;
	bool isProcessing;
	bool finishedEdit;
	ClientNode* client;
} Config;

Config application;

/*
 * Here is the implementation of the methods of class Client
 *
 */

std::string Client::getName() { return this->name; }

std::string Client::getService() {
	if(this->service == "")
		return "None";

	return this->service;
}

std::string Client::getDate() {
	if(this->dateOfPayment == NULL)
		return "None";

	int day = this->dateOfPayment->tm_mday;
	int mon = this->dateOfPayment->tm_mon;
	return std::to_string(this->dateOfPayment->tm_mday) + "/" + std::to_string(this->dateOfPayment->tm_mon);
}

int Client::getIndex() { return this->index; }

void Client::setName(std::string name) {
	this->name = name;
}

void Client::setService(std::string service) {
	this->service = service;
}

void Client::setDate(std::string date) {
	struct tm* data = new tm();
	std::string deli = "/";
	auto start = 0U;
	auto end = date.find(deli);
	for(int i=0; i<2; i++) {
		if(i == 0)
			data->tm_mday = stoi(date.substr(start, end - start));
		else if(i == 1) {
			data->tm_mon = stoi(date.substr(start, end - start));
		}
		start = end + deli.size();
		end = date.find(deli, start);
	}

//	char* dateF;
//	strcpy(dateF, date.c_str());
//	char* formated = strtok(dateF, "/");
//	std::cout << formated[0] << formated[1] << std::endl;
//	data->
	this->dateOfPayment = data;
}

bool Client::isFull() {
	return this->service.size() != 0 && this->name.size() != 0;
}

ClientNode* getClientByIndex(int index) {
	if(list.head == NULL)
		return NULL;

	ClientNode* aux = list.head;
	ClientNode* retorno = NULL;
	while(aux != NULL) {
		if(aux->client.getIndex() == index) {
			retorno = aux;
			break;
		}

		aux = aux->next;
	}
	return retorno;
}

void addObject(ClientList* list, Client cliente) {

	ClientNode* novo = new ClientNode();
	novo->client = cliente;
	application.client = novo;
	counter++;

	if(list->head == NULL) {
		novo->next = NULL;
		list->head = novo;
		list->tail = novo;
		return;
	}

	list->tail->next = novo;
	list->tail = novo;
	list->tail->next = NULL;
}

void addClient(std::string name) {
	if(!application.finishedEdit) {
		std::cout << "[!] First you need to complete this client" << std::endl;
		application.addMode = OFF;
		application.isProcessing = false;
		return;
	}

	Client client;
	client.setName(name);
	addObject(&list, client);

	std::cout << "[*] Client " << name << " added!" << std::endl;
	application.addMode = OFF;
	application.isProcessing = false;
	if(!client.isFull()) {
		application.finishedEdit = false;
		return;
	}

	application.finishedEdit = true;
}

void addService(std::string service) {
	//verifications need to be written on the method
	if(isEmpty()) return;

	ClientNode* aux = application.client;
	aux->client.setService(service);
	application.client = aux;

	std::string name = aux->client.getName();
	application.isProcessing = true;
	std::cout << "[*] Service of " << name << " updated to " << service << std::endl;

	application.sMode = OFF;
	application.isProcessing = false;

	if(!aux->client.isFull()) {
		application.finishedEdit = false;
		return;
	}

	application.finishedEdit = true;
}

bool dataValida(std::string date) {
	std::regex str_date("^(0[1-9]|[12][0-9]|3[01])[/](0[1-9]|1[012])$");
	if(!regex_match(date, str_date)) {
		std::cout << "\n[!] Date " << date << " wrong" << std::endl;
		return false;
	}
	return true;
}

void addDate(std::string date) {
	if(isEmpty() || !dataValida(date)) {
		application.dMode = OFF;
		application.isProcessing = false;
		return;
	}

	application.isProcessing = true;
	ClientNode* aux = application.client;
	aux->client.setDate(date);
	application.client = aux;
	std::cout << "Date set to " << date << std::endl; 

	application.dMode = OFF;
	application.isProcessing = false;

	//TODO: this responsibility can be taken to the add Method
	if(!aux->client.isFull()) {
		application.finishedEdit = false;
		return;
	}
	application.finishedEdit = true;
}

void switchClient(std::string arg) {
	int i;
	try {
		i = stoi(arg); 
	}
	catch(std::invalid_argument e) {
		std::cout << "[!] Index error" << std::endl;
		return;
	}
	
	if(getClientByIndex(i) == NULL) {
		std::cout << "[!] Error" << std::endl;
		application.swMode = OFF;
		application.isProcessing = false;
		return;
	}

	application.isProcessing = true;
	application.client = getClientByIndex(i);
	std::cout << "[*] Client top now is " << application.client->client.getName();
	application.swMode = OFF;
	application.isProcessing = false;
}

void printClients() {
	if(isEmpty()) return;

	ClientNode* aux = list.head;
	while(aux != NULL) {
		std::cout << "\nName: " << aux->client.getName() << std::endl;
		std::cout << "Service: " << aux->client.getService() << std::endl;
		std::cout << "Date: " << aux->client.getDate() << std::endl;
		std::cout << "Index: " << aux->client.getIndex()<< std::endl;

		aux = aux->next;
	}
	application.isProcessing = false;
}

/* These are auxiliar functions created to do the tasks in a 
 * quickly way
 *

bool stringContains(std::string text, std::string word) {
	bool achou = false;
	int i, j;
	for(i = 0; i < text.size(); i++) {
		std::cout << i << std::endl;
	}
	return achou;
}
*/

bool isEmpty() {
	if(list.head == NULL) {
		std::cout << "[!] No clients" << std::endl;
		application.dMode = OFF;
		application.addMode = OFF;
		application.sMode = OFF;
		application.swMode = OFF;
		application.isProcessing = false;
		return true;
	}
	return false;
}

bool isValid(std::string arg) {
//	std::cout << "Comand analized: " << arg << std::endl;
//	std::cout << "Size: " << arg.size() << std::endl;

	if((arg != "add" && 
	arg != "service" &&
	arg != "exit" &&
	arg != "date" &&
	arg != "switch" &&
	arg != "list") && 
	(application.sMode == OFF &&
	 application.dMode == OFF &&
	 application.swMode == OFF &&
	application.addMode == OFF)) {
		std::cout << "[!] Invalid command\n";
		application.isProcessing = false;
		return false;
	}
	return true;
}

/* This functions are responsable to control de flux of the
 * program and switch to the different ways of the application
 */

void readCommand(std::string *arg) {
	if(!application.isProcessing)
		std::cout << "\033[32mwin@user#> \033[m";
	std::cin >> *arg;
}

void handle(std::string arg) {
	application.isProcessing = true;

	if(!isValid(arg)) return;

	if(application.addMode == ON) 
		addClient(arg);

	if(application.sMode == ON)
		addService(arg);

	if(application.dMode == ON)
		addDate(arg);

	if(application.swMode == ON)
		switchClient(arg);

	//std::cout << arg << std::endl;
	if(arg == "add")
		application.addMode = ON;

	if(arg == "list") 
		printClients();

	if(arg == "service")
		application.sMode = ON;

	if(arg == "date")
		application.dMode = ON;
	
	if(arg == "switch")
		application.swMode = ON;
}

/* This functions below do the initialization of the program
 *
 * The function init sets the pointers head and tail of the
 * list to NULL value
 *
 * The function printWelcomeMessage shows to the user a manual about how to use them
 *
 */

void loadContent() {
	FILE* data = fopen("data.txt", "r");
	if(data == 0) {
		std::cout << "[!] Error to load data" << std::endl;
		exit(1);
	}
	int qtde;
	fscanf(data, "%d", &qtde);
	for(int i=0; i < qtde; i++) {
		char nome[20];
        	fscanf(data, "%s", nome);
		char servico[20];
		fscanf(data, "%s", servico);
		char date[20];
		fscanf(data, "%s", date);

		//Creates a new Client
		Client novo;
		novo.setName(std::string(nome));
		novo.setService(std::string(servico));
		novo.setDate(std::string(date));
		addObject(&list, novo);

		std::cout << "N: " << nome << " S: " << servico << " D: " << date << std::endl;
	}
        fclose(data);
	application.client = list.head;
	std::cout << "Client of top: " << application.client->client.getName() << std::endl;
}

void init() {
	list.head = NULL;
	list.tail = NULL;
	application.addMode = OFF;
	application.sMode = OFF;
	application.dMode = OFF;
	application.swMode = OFF;
	application.isProcessing = false;
	application.finishedEdit = true;
	application.client = NULL;
	loadContent();
}

void printWelcomeMessage() {
	std::cout << "Welcome to Win!" << std::endl;
	std::cout << "\nOfficial repository:       https://github.com/gabrielfelipeassuncaodesouza/win/" << std::endl;
	std::cout << "\nHow it works? " << std::endl;
	std::cout << "\n - Add a new client:	win -a -n <Name> -s <Service>" << std::endl;
	std::cout << " - List all clients:	win -l" << std::endl;
	std::cout << " - Deletes a client: win -b <client>" << std::endl;
	std::cout << "\nFor more information, type win --help:" << std::endl;
}

int getListaLength() {
	ClientNode* aux = list.head;
	int qtd = 0;
	while(aux != NULL){
		qtd++;
		aux = aux->next;
	}

	return qtd;
}

void record() {
	FILE* data = fopen("data.txt", "w");
        if(data == 0) {
        	printf("Error 404 not found");
                exit(1);
        }

        int qtde;
	fscanf(data, "%d", &qtde);
	for(int i=0; i<qtde; i++) {
		for(int j=0; j<3; j++)
			fprintf(data, NULL);
	}

	fseek(data, 0, SEEK_SET);
        fprintf(data, NULL);

	fclose(data);

	FILE* d = fopen("data.txt", "a");
        if(d == 0) {
        	printf("Error 404 not found");
                exit(1);
        }

	qtde = getListaLength();
	std::cout << "Lista has " << qtde << std::endl;
	fprintf(d, "%02d", qtde);

	fseek(d, 0, SEEK_END); 
	ClientNode* aux = list.head;
	for(int i=0; i<qtde; i++) {

		std::cout << "Recording client " << aux->client.getName() << std::endl;
		fprintf(d, "\n%s\n", aux->client.getName().c_str());
		fprintf(d, "%s\n", aux->client.getService().c_str());
		fprintf(d, "%s\n", aux->client.getDate().c_str());
		aux = aux->next;
	}
	fclose(d);
}

int main(int argc, char** argv) {
	//std::cout << "argc: " << argc << "argv: "<< argv[1];
	init();
	//printWelcomeMessage();
	std::string arg;
	do {
		readCommand(&arg);
		handle(arg);
	} while(arg != "exit");

	char choice;
	std::cout << "Save changes?[S/N]? ";
	std::cin >> choice;
	if(choice == 'S' || choice == 's')
		record();

	return 0;
}
