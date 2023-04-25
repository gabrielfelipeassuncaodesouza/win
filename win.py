#! /usr/bin/python3
#
# Win.py - Window streaming manager
#
# Site      : https://github.com/gabrielfelipeassuncaodesouza/
# Author    : Gabriel Felipe <gabrielfelipeassuncaodesouza@gmail.com
#
#
# ---------------
# This program receive as a paramter arguments and a key value
# each argument does something on the final result
#
# Exemplo:
#   $ win.py -l     <Lista todos os clientes>
#   $ win.py -c <Nome> -n <Novo nome>
#   $ [!] Name of client updated.
#
#
#   Licenca: MIT
#

# Imports

from datetime import datetime
import abc
import argparse
import os
import shelve
import sys
import textwrap

# Global variables

path = os.path.join('/data', 'data', 'com.termux', 'files', 'home', 'win')
os.chdir(path)
clients = shelve.open('test')
db.close()
colors = {
    "RED": '\033[31m',
    "GREEN": '\033[32m',
    "BLANK": '\033[m'
}

# Main classes 

class CalculaValor(abc.ABC):
    """
    Define an interface that contains the rules to calculate the Day of the payment
    """
    @abc.abstractmethod
    def calcular(self):
        pass

class CalculaTrintaEUm:
    """
    This class implements the interface CalculaValor and is called when the month has 31 days
    """
    def calcular(self, date):
        periodo = datetime.now() - date
        days = datetime.strptime("01/02/23", "%d/%m/%y") - datetime.strptime("01/01/23", "%d/%m/%y")
        return periodo > days

class CalculaTrinta:
    """
    This class implements the interface CalculaValor and is called when the month as 30 days
    """
    def calcular(self, date):
        periodo = datetime.now() - date
        days = datetime.strptime("31/01/23", "%d/%m/%y") - datetime.strptime("01/01/23", "%d/%m/%y")
        return periodo > days

# Implement the interfaces

CalculaValor.register(CalculaTrinta)
CalculaValor.register(CalculaTrintaEUm)

class Client:
    """
    The principal class of the program. Contains the information of the client
    """
    def __init__(self, name, service, date=datetime.now()):
        self._name = name
        self._service  = service
        self._date = date
        self._CalculaValor = self._metodoDeCalculo()

    def _metodoDeCalculo(self):
        """
        According to the date od payment, it sets a different way of calculation
        """
        if(self._date.month in [4, 6, 9, 11]):
            return CalculaTrinta()
        else:
            return CalculaTrintaEUm()

    def noPrazo(self):
        """
        Use the return of the method calcula of the interface
        """
        if self._CalculaValor.calcular(self._date):
            return '{}Prazo Vencido{}'.format(colors["RED"],colors["BLANK"])

        return '{}No prazo{}'.format(colors["GREEN"], colors["BLANK"])

    def setCalculo(self, CalculoValor):
        """
        Changes the method of calculation, if it's necessary
        """
        self._CalculoValor = CalculoValor

    def __str__(self):
        try:
            return '\nName of client:      ---> '+ str(self._name) + '\nService of Streaming ---> ' + str(self._service) + '\nDate of payment      ---> ' + str(self._date.strftime("%d/%m/%y")) + '\nStatus: ' + str(self.noPrazo())
        except Exception as e:
            return str(e)

    def update(self):
        """
        Updates the date of the client to current date
        """
        self._date = datetime.today()

# Functions of the program

def list_clients():
    """
    List all clients of the list
    """
    if not clients.items():
        title = 'EMPTY'.center(20, '=')
    else:
        title = 'CLIENT INFO'.center(20, '=')
    print(f'\n{title}\n')
    for client in clients.values():
        print(client)


def isThereClient(name):

    for i in clients.keys():
        if str(name) == i:
            print(f"[!] This client already exists")
            return True

    return False

def add(args):
    """
    Add a new client in the system

    param: args = the arg parser object
    """
    if not args.name or not args.service:
        print("Missing arguments".center(20, '='))
        return

    if not isThereClient(args.name):
        clients[args.name] = Client(args.name, args.service)
        if args.date:
            change(args, args.name)

        print(f"[*] Client {clients[args.name]._name} added on the system") 

def change(args):
    """
    Changes a characterisct of a client
    """
    name = args.change
    changeValue(args, name)

def changeValue(args, arg_name):
    """
    Changes the date of the payment
    """
    name = str(arg_name)
    obj = clients[name]

    if args.date:
        date = str(args.date)
        try:
            date = datetime.strptime(date, "%d/%m/%y")
        except ValueError:
            print('[!Invalid value]')
        else:
            obj._date = date

    if args.name:
        n = str(args.name)
        if not isThereClient(n):
            del_client(name)
            obj._name = n
            name = n

    clients[name] = obj

def del_client(name):
    """
    Delete a client of the system

    param: args = arg parser object
    """
    try:
        del(clients[name])
        print(f"[*] Client {args.backspace} removed of system")
    except KeyError as e:
        print(f'''[!!] No client {args.backspace} on system
Error: {e}''')

def enableTestMode():
    global clients, db
    clients.close()
    db = open('mode.txt', 'w')
    db.write('test')
    print('[*] Debug mode enable')
    db.close()

def switchNormalMode():
    global clients, db
    clients.close()
    db = open('mode.txt', 'w')
    db.write('data')
    print('[*] Swicthed to normal mode')
    db.close()

def createParser():
    """
    Creates a arg parser object
    """
    parser = argparse.ArgumentParser(description='WIN Streaming Screen Manager',formatter_class=argparse.RawDescriptionHelpFormatter, epilog=textwrap.dedent('''Example:
    win.py -a -n Maria -s Netflix -d 190804 # add client
    win.py -u Maria # update date of payment
    win.py -c Maria -s Disney+ # changes the service to Disney+
    win.py -d Maria # deletes the client Maria
    '''))
    parser.add_argument('-a', '--add', action='store_true', help='add client')
    parser.add_argument('-u', '--update', help='update the date of the payment')
    parser.add_argument('-b', '--backspace', help='delete a client')
    parser.add_argument('-n', '--name', help='sets the name of the client')
    parser.add_argument('-s', '--service', help='sets the service of streaming')
    parser.add_argument('-l', '--list', action='store_true', help='list all clients')
    parser.add_argument('-c', '--change', help='changes the value of a object')
    parser.add_argument('-d', '--date', help='sets the date of the payment')
    parser.add_argument('-t', '--test', action='store_true', help='test mode')
    parser.add_argument('-z', '--zshell', action='store_true', help='normal mode')

    return parser

def handle(args):
    """
    According to the parameters do something
    """
    if args.add: add(args)
    if args.change: change(args)
    if args.list: list_clients()
    if args.backspace: del_client(args.backspace)
    if args.update: print('I will fix it')
    if args.test: enableTestMode()
    if args.zshell: switchNormalMode()
if __name__ == '__main__':

    """
    Here it's the details of the arguments that the program accept
    """
    parser = createParser()
    args = parser.parse_args()
    handle(args)
