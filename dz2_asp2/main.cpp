#include <iostream>
#include <string>
#include <fstream>
using namespace std;

#include "tree.h"

#define RESET   "\033[0m"
#define REDD    "\033[31m"

bool StringIsNumber(string str){
	for (int i = 0; i < str.length(); i++) if (!isdigit(str[i])) return false;
	return true;
}

int main(int argc, char** argv) {
	Tree* t = new Tree();
	ifstream fin;
	string action;
	int priority;
	int x,y,old,p;
	cout << "\n\tIMPLEMENTACIJA PRIORITETNOG REDA KORISCENJEM 2-3-4 STABALA\n\n";
	cout <<"  Izaberite stavku sa menija:\n";
	cout << "  1. Unos podataka iz datoteke i stvaranje 2-3-4 stabla \n";
	cout << "  2. Ispis crveno-crnog stabla u obliku 2-3-4 stabla\n";
	cout << "  3. Ispis crveno-crnog stabla u obliku binarnog stabla\n";
	cout << "  4. Unistavanje stabla\n";
	cout << "  5. Pretraga stabla po prioritetu akcije\n";
	cout << "  6. Pronalazenje ukupnog broja akcija sa zadatim prioritetom\n";
	cout << "  7. Umetanje nove akcije u stablo\n";
	cout << "  8. Brisanje proizvoljne akcije po prioritetu\n";
	cout << "  9. Brisanje akcije najviseg prioriteta (veca vrednost oznacava visi prioritet)\n";
	cout << "  10. Izmena prioriteta akcije\n";
	cout << "  0. Kraj programa\n";
	cout << "________________________________________________________________________________________________________________________\n\n";

	while (true) {
		cout << "Vas izbor: ";
		cin >> x;
		cout << endl;
		
		switch (x) {
		case 0: delete t; exit(0);
		case 1: 
			delete t;
			t = new Tree();
			fin.open(argv[1]);
			if (!fin.is_open()) { cout << "Datoteka nije uspesno otvorena." << endl; break; }
			while (fin) {
				fin >> action >> priority;
				/*if (StringIsNumber(action)) {
					priority = stoi(action);
					action.append(" ");
				}*/
				t->Insert(action, priority);
			}
			fin.close();
			break;
		case 2: 
			cout << *t;
			break;
		case 3: 
			t->PrintAsBinary();
			break;
		case 4: 
			delete t; 
			t = new Tree();
			break;
		case 5: 
			cout << "Unesite prioritet akcije za pretragu: ";
			cin >> y;
			cout << endl;
			t->PrioritySearch(y);
			break;
		case 6: 
			cout << "Unesite prioritet:";
			cin >> y;
			cout << endl;
			cout << "Ukupan broj akcija sa prioritetom " << y << " iznosi: " << t->CountPriorities(y) << endl;
			break;
		case 7: 
			cout << "Unesite naziv akcije za umetanje i njen prioritet odvojene razmakom: ";
			cin >> action >> priority;
			t->Insert(action, priority);
			break;
		case 8: 
			cout << "Unesite naziv akcije za brisanje i njen prioritet odvojene razmakom: ";
			cin >> action >> priority;
			t->Delete(action, priority);
			break;
		case 9: 
			t->Pop();
			break;
		case 10: 
			cout << "Unesite naziv akcije ciji prioritet zelite da promenite kao i njen trenutni prioritet u stablu: ";
			cin >> action >> old;
			cout << "Unesite novu vrednost prioriteta date akcije:";
			cin >> p;
			cout << endl;
			t->ChangeActionPriority(action, old, p);
			break;
		default:
			cout << "Nekorektan unos. Pokusajte ponovo." << endl;
		}
	}

	return 0;
}