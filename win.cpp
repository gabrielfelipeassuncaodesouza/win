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
void initOptions();

/* Here it's defined the class Client who contains all
 * informations about the client. So, all operations with
 * dates and more else will be done here
 */


/* Interface*/

class CalcInterface {
	public:
		std::string calcStatus(struct tm* date) {
			time_t now = time(0);
			struct tm* currentData = localtime(&now);
			currentData->tm_mon++;

			int currentDay = currentData->tm_mday;
			int currentMon = currentData->tm_mon;
			int storagedDay = date->tm_mday;
			int storagedMon = date->tm_mon;

			if(currentMon - storagedMon > 0 &&
			currentDay > storagedDay)
				return "Prazo vencido";

			return "No prazo";
		}
};

class Client {
	public:
		std::string getName();
		std::string getService();
		std::string getDate();
		std::string getStatus();
		void setName(std::string name);
		void setService(std::string service);
		void setDate(std::string date);
		bool isFull();
	private:
		std::string name;
		std::string service;
		struct tm* dateOfPayment;
		CalcInterface* calc;
		void updateDate();

	public:
		Client() {
			time_t now = time(0);
			struct tm* data = localtime(&now);
			data->tm_mon++;
			this->dateOfPayment = data;
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
	int delMode;
	bool saved;
	bool isProcessing;
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

std::string Client::getStatus() {
	struct tm* aux = this->dateOfPayment;
	return this->calc->calcStatus(aux);
}

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

void Client::updateDate() {
	time_t now = time(0);
	struct tm* newData = localtime(&now);
	newData->tm_mon++;
	this->dateOfPayment = newData;
	std::cout << "[!] Date updated" << std::endl;
}

bool Client::isFull() {
	return this->service.size() != 0 && this->name.size() != 0;
}

ClientNode* getClientByName(std::string client) {
	if(list.head == NULL)
		return NULL;

	ClientNode* aux = list.head;
	ClientNode* retorno = NULL;
	while(aux != NULL) {
		if(aux->client.getName() == client) {
			retorno = aux;
			break;
		}

		aux = aux->next;
	}
	return retorno;
}

void addObject(ClientList* list, Client cliente) {
	if(list->head != NULL && !list->tail->client.isFull()) {
		std::cout << "[!] First complete this client" << std::endl;
		initOptions();
		return;
	}

	ClientNode* novo = new ClientNode();
	novo->client = cliente;
	application.client = novo;

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

	Client client;
	client.setName(name);
	addObject(&list, client);

	std::cout << "[*] Client " << name << " added!" << std::endl;
	application.addMode = OFF;
	application.isProcessing = false;
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
}

void switchClient(std::string client) {

	if(getClientByName(client) == NULL) {
		std::cout << "[!] Error" << std::endl;
		application.swMode = OFF;
		application.isProcessing = false;
		return;
	}

	application.isProcessing = true;
	application.client = getClientByName(client);
	std::cout << "[*] Client top now is " << application.client->client.getName() << std::endl;
	application.swMode = OFF;
	application.isProcessing = false;
}

void delClient(std::string client) {

	if(isEmpty()) return;
	
	if(getClientByName(client) == NULL) {
		std::cout << "[!] Client doesn't exist" << std::endl;
		application.delMode = OFF;
		application.isProcessing = false;
		return;
	}

	application.isProcessing = true;
	ClientNode* aux = list.head;
	ClientNode* prev = NULL;

	std::string name;
	while(aux != NULL) {
		if(aux->client.getName() == client) {
			name = aux->client.getName();

			//TODO: improve this verifications
			if(aux == list.head) {
				application.client = aux->next;
				list.head = aux->next;
				aux = list.head;
			}
			
			else if(aux == list.tail) {
				prev->next = NULL;
				list.tail = prev;
				aux = NULL;
			}
			else {
				prev->next = aux->next;
				aux = aux->next;
			}
			break;
		}
		prev = aux;
		aux = aux->next;
	}

	std::cout << "[*] Client " << name << " Removed " << std::endl;
	application.delMode = OFF;
	application.isProcessing = false;

}

void printClients() {
	if(isEmpty()) return;

	std::cout << "Client top: " << application.client->client.getName() << std::endl;

	ClientNode* aux = list.head;
	while(aux != NULL) {
		std::cout << "\nName: " << aux->client.getName() << std::endl;
		std::cout << "Service: " << aux->client.getService() << std::endl;
		std::cout << "Date: " << aux->client.getDate() << std::endl;
		std::cout << "Status: " << aux->client.getStatus()<< std::endl;

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

bool isStringIn(std::string s, std::string options[], int size) {
	for(int i = 0; i < size; i++) {
		if(options[i] == s)
			return true;
	}
	return false;
}

bool isValid(std::string arg) {
//	std::cout << "Comand analized: " << arg << std::endl;
//	std::cout << "Size: " << arg.size() << std::endl;

	std::string options[7] = {
		"add",
		"service",
		"exit",
		"date",
		"switch",
		"delete",
		"list"
	};

	if((!isStringIn(arg, options, 7)) && 
	(application.sMode == OFF &&
	 application.dMode == OFF &&
	 application.swMode == OFF &&
	 application.delMode == OFF &&
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
	if(application.addMode == ON) 
		addClient(arg);

	if(application.sMode == ON)
		addService(arg);

	if(application.dMode == ON)
		addDate(arg);

	if(application.swMode == ON)
		switchClient(arg);

	if(application.delMode == ON)
		delClient(arg);
}

void toggleModes(std::string arg) {
	application.isProcessing = true;

	if(!isValid(arg)) return;
	
	handle(arg);

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

	if(arg == "delete")
		application.delMode = ON;
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

void initOptions() {
	application.addMode = OFF;
	application.sMode = OFF;
	application.dMode = OFF;
	application.swMode = OFF;
	application.delMode = OFF;
	application.isProcessing = false;
}

void initList() {
	list.head = NULL;
	list.tail = NULL;
	application.client = NULL;
	initOptions();
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

void save() {
	char choice;
	while(true) {
		std::cout << "Save changes?[S/N]? ";
		std::cin >> choice;
		choice = tolower(choice);
		if(choice != 'n' && choice != 's') {
			std::cout << "[!] Invalid" << std::endl;
			continue;
		}

		break;
	}

	if(choice == 's') {
		record();
		return;
	}
}

int main(int argc, char** argv) {
	//std::cout << "argc: " << argc << "argv: "<< argv[1];
	initList();
	//printWelcomeMessage();
	std::string arg;
	do {
		readCommand(&arg);
		toggleModes(arg);
	} while(arg != "exit");

	save();
	return 0;
}
