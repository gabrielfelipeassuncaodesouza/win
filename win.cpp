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
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

bool isEmpty();

/*
 * Here it's a configuration of the system
 */

enum modes {
	OFF = 0,
	ON
};

typedef struct Config {
	int addMode;
	int sMode;
	int dMode;
	bool isProcessing;
	bool finishedEdit;
} Config;

Config application;

/* Here it's defined the class Client who contains all
 * informations about the client. So, all operations with
 * dates and more else will be done here
 */

class Client {
	public:
		char* getName();
		char* getService();
		void setName(char* name);
		void setService(char* service);
		bool isFull();
	private:
		char* name;
		char* service;
		char* dateOfPayment;
};

char* Client::getName() {
	return this->name;
}
	
char* Client::getService() {
	return this->service;
}

void Client::setName(char* name) {
	//need realloc the memory
	this->name = name;
}

void Client::setService(char* service) {
	this->service = service;
}

bool Client::isFull() {
	return this->service != 0 && this->name != 0;
}

typedef struct ClientNode {
	Client client;
	ClientNode* next;
} ClientNode;

typedef struct ClientList {
	ClientNode* head;
	ClientNode* tail;
} ClientList;

ClientList list;

void addClient(char* name) {
	if(!application.finishedEdit) {
		std::cout << "[!] First you need to complete this client" << std::endl;
		application.addMode = OFF;
		application.isProcessing = false;
		return;
	}

	Client client;
	client.setName(name);
	ClientNode* novo = new ClientNode();
	novo->client = client;

	if(list.head == NULL) {
		novo->next = NULL;
		list.head = novo;
		list.tail = novo;
	}

	else
	{
		list.tail->next = novo;
		list.tail = novo;
		list.tail->next = NULL;
	}

	application.isProcessing = true;
	std::cout << "[*] Client " << name << " added!" << std::endl;
	application.addMode = OFF;
	application.isProcessing = false;
	if(!client.isFull()) {
		application.finishedEdit = false;
		return;
	}

	application.finishedEdit = true;
}

void addService(char* service) {
	if(isEmpty()) return;

	list.tail->client.setService(service);
	char* name = list.tail->client.getName();
	std::cout << "[*] Service of " << name << "updated to " << service << std::endl;

	application.sMode = OFF;
	application.isProcessing = false;
	if(!list.tail->client.isFull()) {
		application.finishedEdit = false;
		return;
	}
	application.finishedEdit = true;

}

//bool dataValida(std::string date) {
//	return false;
//}

//void addDate(std::string date) {
//	if(!dataValida(date)) return;
//}

void printClients() {
	if(isEmpty()) return;

	ClientNode* aux = list.head;
	while(aux != NULL) {
		std::cout << "\nName: " << aux->client.getName() << std::endl;
		std::cout << "Service: " << aux->client.getService() << std::endl;

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
		application.isProcessing = false;
		return true;
	}
	return false;
}

bool equals(char *s1, char* s2) {
	return strcmp(s1, s2) == 0;
}

bool isValid(char* arg) {

	char *add = strcpy(add, "add");
	char *list = strcpy(list, "list");
	char *service = strcpy(service, "service");
	char *date = strcpy(date, "date");
	char *exit = strcpy(exit, "exit");

	std::cout << "Comand analized: " << arg << std::endl;

	if((!equals(arg, add) && 
	!equals(arg, list) &&
	!equals(arg, service) && 
	!equals(arg, exit) && 
	!equals(arg, date)) && 
	(application.sMode == OFF && 
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

void readCommand(char *arg) {
	if(!application.isProcessing)
		std::cout << "\033[32mwin@user#> \033[m";
	scanf("%s", arg);
}

void handle(char* arg) {
	application.isProcessing = true;

	if(!isValid(arg)) return;

	if(application.addMode == ON)
		addClient(arg);

	if(application.sMode == ON)
		addService(arg);

	//if(application.dMode == ON)
	//	addDate(arg);

	char *add = strcpy(add, "add");
	char *list = strcpy(list, "list");
	char *service = strcpy(service, "service");
	char *date = strcpy(date, "date");
	char *exit = strcpy(exit, "exit");

	if(equals(arg, add))
		application.addMode = ON;

	if(equals(arg, list)) 
		printClients();

	if(equals(arg, service))
		application.sMode = ON;

	//if(equals(arg, date))
	//	application.dMode = ON;
}

/* This functions below do the initialization of the program
 *
 * The function init sets the pointers head and tail of the
 * list to NULL value
 *
 * The function printWelcomeMessage shows to the user a manual about how to use them
 *
 */

void init() {
	list.head = NULL;
	list.tail = NULL;
	application.addMode = OFF;
	application.sMode = OFF;
	application.dMode = OFF;
	application.isProcessing = false;
	application.finishedEdit = true;
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

int main(int argc, char** argv) {
	//std::cout << "argc: " << argc << "argv: "<< argv[1];
	init();
	printWelcomeMessage();
	char* arg;
	char* flag = strcpy(flag, "exit");
	do {
		readCommand(arg);
		handle(arg);
	} while(!equals(arg, flag));
	return 0;
}
