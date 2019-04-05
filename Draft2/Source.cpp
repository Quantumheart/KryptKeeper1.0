#include <functional>
#include <iostream>
#include <iterator>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include "sha256.h"
using namespace std;

struct Record {string id;
	string site;
	string user;
	string pass;
};

void PopulateVector(vector<Record> &vRecords);
void ResizeEntry(string &entry, int maxWidth);
void UpdateDatabase(vector<Record> &vRecords);
void DisplayRecords(vector<Record> &vRecords);
void FirstRunMenu(vector<Record> &vRecords);
void DeleteRecord(vector<Record> &vRecords);
void PromptForMasterPassword(string &hash);
void ViewRecord(vector<Record> &vRecords);
void EditRecord(vector<Record> &vRecords);
void AddRecord(vector<Record> &vRecords);
void MainMenu(vector<Record> &vRecords);
void WriteHashToKeyFile(string &key);
string CreateMasterPassword();
bool isFirstRun();
string GetInput();
string GetHash();

int main()
{
	if (isFirstRun())
	{
		cout << "Welcome to Password Manager.\nPlease begin by creating a master password.\n";
		WriteHashToKeyFile(CreateMasterPassword());

		cout << "Press ENTER to start storing your passwords securely.";
		cin.ignore();
	}

	PromptForMasterPassword(GetHash());

	vector<Record> Records;

	PopulateVector(Records);
	DisplayRecords(Records);

	return 0;
}

void EditRecord(vector<Record> &vRecords)
{
	string searchString;
	bool recordExists = false;

	cout << "=======================================================================================================================\n";
	cout << "|                                                 Edit Record                                                         |\n";
	cout << "=======================================================================================================================\n";

	while (!recordExists)
	{
		cout << "Select a record to edit: ";
		getline(cin, searchString);

		for (unsigned i = 0; i < vRecords.size(); ++i)
		{
			Record &rec = vRecords[i];

			if (searchString.compare((rec.id)) == 0)
			{
				recordExists = true;
				cout << "Record " << (rec.id) << " is Selected.\n";

				string username;
				string password;

				cout << "Username: ";
				getline(cin, username);
				cout << "Password: ";
				getline(cin, password);

				rec.user = username;
				rec.pass = password;

				break;
			}
		}
	}

	DisplayRecords(vRecords);
}

void AddRecord(vector<Record> &vRecords)
{
	//system("cls");
	cout << "=======================================================================================================================\n";
	cout << "|                                                  Add Record                                                         |\n";
	cout << "=======================================================================================================================\n";

	string title;
	string username;
	string password;

	cout << "Title: ";
	getline(cin, title);
	cout << "Username: ";
	getline(cin, username);
	cout << "Password: ";
	getline(cin, password);

	Record rec;
	rec.site = title;
	rec.user = username;
	rec.pass = password;

	vRecords.push_back(rec);
	
	for (unsigned i = 0; i < vRecords.size(); i++)
	{
		Record &r = vRecords[i];
		r.id = to_string(i + 1);
	}	

	DisplayRecords(vRecords);
}

string GetInput()
{
	string s;

	cout << ">> ";
	getline(cin, s);

	return s;
}

void MainMenu(vector<Record> &vRecords) {

	//system("cls");
	struct MenuAction {

		string description;
		function<void()> action;
	};

	const map <string, MenuAction> actionTable{

		{ "1",{ "View Record", [&]() { ViewRecord(vRecords); } } },
		{ "2",{ "Add Record", [&]() { AddRecord(vRecords); } } },
		{ "3",{ "Edit Record", [&]() { EditRecord(vRecords); } } },
		{ "4",{ "Delete Record", [&]() { DeleteRecord(vRecords); } } },
		{ "q",{ "Quit", [&]() { UpdateDatabase(vRecords); } } }
	};

	for (auto const& x : actionTable) {
		cout << x.first << ". " << (x.second).description << "\n";
	}

	string input;

	while (actionTable.count(input) == 0)
	{
		input = GetInput();
	}

	actionTable.at(input).action();
}

void PopulateVector(vector<Record> &vRecords)
{
	ifstream inFile("database.txt");
	string entry;

	while (getline(inFile, entry))
	{
		istringstream iss(entry);

		Record rec;
		getline(iss, rec.id, '\t');
		getline(iss, rec.site, '\t');
		getline(iss, rec.user, '\t');
		getline(iss, rec.pass, '\t');

		vRecords.push_back(rec);
	}
}

string GetHash()
{
	ifstream inFile("key.txt");
	string hash;

	getline(inFile, hash);

	return hash;
}

void PromptForMasterPassword(string &hash)
{
	bool isAuthenticated = false;
	string masterPassword;

	cout << "Enter Master Password to unlock database.\n";

	while (!isAuthenticated)
	{
		cout << "Master Password: ";
		getline(cin, masterPassword);

		SHA256 sha256;

		if (sha256(masterPassword) == hash)
		{
			isAuthenticated = true;
			cout << "Access Granted!\n";
		}
		else
		{
			cout << "Access Denied!\n";
		}
	}
}

void WriteHashToKeyFile(string &key)
{
	SHA256 sha256;
	ofstream outFile("key.txt");
	outFile << sha256(key);
}

string CreateMasterPassword()
{
	bool isValidPassword = false;
	string masterPassword;
	string confirmMasterPassword;

	while (!isValidPassword)
	{
		cout << "Master Password: ";
		getline(cin, masterPassword);
		cout << "Confirm Master Password: ";
		getline(cin, confirmMasterPassword);

		if (masterPassword.empty())
		{
			isValidPassword = false;
			cout << "Master Password Cannot Be Blank!\n";
		}
		else if (masterPassword == confirmMasterPassword)
		{
			isValidPassword = true;
			cout << "Master Password has been created. Store this in a safe location.\n";
		}
		else
		{
			isValidPassword = false;
			cout << "Passwords Do Not Match!\n";
		}
	}

	return masterPassword;
}

bool isFirstRun()
{
	bool result = true;
	ifstream inFile("key.txt");

	if (inFile)
	{
		result = false;
	}

	return result;
}

void ResizeEntry(string &entry, int colWidth) {

	unsigned allowedStringSize = (colWidth - 5);
	unsigned actualStringSize = (entry).size();
	unsigned adjustedStringSize = (colWidth - 8);

	if (actualStringSize > allowedStringSize) {
		(entry).resize(adjustedStringSize);
		(entry).resize(((entry).size() + 3), '.');
	}
}

void DisplayRecords(vector<Record> &vRecords)
{
	//system("cls");

	cout << "=======================================================================================================================\n";
	cout << "|                                                      Records                                                        |\n";
	cout << "=======================================================================================================================\n";

	if (vRecords.size() == 0)
	{
		cout << "\n" << string(45, ' ') << "NO RECORDS TO DISPLAY\n\n";
		FirstRunMenu(vRecords);
	}
	else
	{
		for (unsigned i = 0; i < vRecords.size(); ++i)
		{
			Record &rec = vRecords[i];
			ResizeEntry(rec.site, 25);
			ResizeEntry(rec.user, 40);
			ResizeEntry(rec.pass, 40);

			cout << right << "| " << setw(3) << rec.id << left << setw(3) << " |"
				<< setw(25) << rec.site
				<< setw(40) << rec.user
				<< setw(44) << string(20, '*') << " |"
				<< "\n-----------------------------------------------------------------------------------------------------------------------\n";
		}

		MainMenu(vRecords);
	}
}

void FirstRunMenu(vector<Record> &vRecords)
{
	struct FirstRunMenuAction {

		string description;
		function<void()> action;
	};

	const map <string, FirstRunMenuAction> FirstRunMenuTable{

		{ "1",{ "Add Record", [&]() { AddRecord(vRecords); } } },
		{ "q",{ "Quit", []() { cout << "Quit" << "\n";  } } }
	};

	for (auto const& x : FirstRunMenuTable) {
		cout << x.first << ". " << (x.second).description << "\n";
	}

	string input;

	while (FirstRunMenuTable.count(input) == 0) {
		input = GetInput();
	}

	FirstRunMenuTable.at(input).action();
}

void DeleteRecord(vector<Record> &vRecords)
{
	string searchString;
	bool recordExists = false;

	cout << "=======================================================================================================================\n";
	cout << "|                                                 Delete Record                                                       |\n";
	cout << "=======================================================================================================================\n";

	while (!recordExists)
	{
		cout << "Select a record to delete: ";
		getline(cin, searchString);

		for (unsigned i = 0; i < vRecords.size(); ++i)
		{
			Record &rec = vRecords[i];

			if (searchString.compare((rec.id)) == 0)
			{
				recordExists = true;
				cout << "Record " << (rec.id) << " is Selected.\n";
				
				vRecords.erase(vRecords.begin() + i);

				break;
			}
		}
	}

	for (unsigned i = 0; i < vRecords.size(); i++)
	{
		Record &r = vRecords[i];
		r.id = to_string(i + 1);
	}

	DisplayRecords(vRecords);
}

void ViewRecord(vector<Record> &vRecords)
{
	string searchString;
	bool recordExists = false;

	cout << "=======================================================================================================================\n";
	cout << "|                                                 View Record                                                         |\n";
	cout << "=======================================================================================================================\n";

	while (!recordExists)
	{
		cout << "Select a record to view: ";
		getline(cin, searchString);

		for (unsigned i = 0; i < vRecords.size(); ++i)
		{
			Record &rec = vRecords[i];

			if (searchString.compare((rec.id)) == 0)
			{
				recordExists = true;
				cout << "Record " << (rec.id) << " is Selected.\n";

				cout << "website: " << rec.site << "\n"
					<< "username/email: " << rec.user << "\n"
					<< "password: " << rec.pass << "\n";

				break;
			}
		}
	}

	for (unsigned i = 0; i < vRecords.size(); i++)
	{
		Record &r = vRecords[i];
		r.id = to_string(i + 1);
	}

	DisplayRecords(vRecords);
}

void UpdateDatabase(vector<Record> &vRecords)
{	
	ofstream outFile;
	outFile.open("database.txt", ios_base::app);
	

	for (unsigned i = 0; i < vRecords.size(); ++i)
	{
		Record &rec = vRecords[i];
		outFile << rec.id << "\t" << rec.site << "\t" << rec.user << "\t" << rec.pass << "\n";
	}
}

/*

*****NOTES*****

DeleteRecord() function (this will screw up record id's: need to re-number records after delete)......DONE
ViewRecord() function.................................................................................DONE
Hide passwords when records are displayed.............................................................DONE
Hide passwords when records are displayed.............................................................DONE
Write vector to file..................................................................................DONE
Generate password for user
On EditRecord() ask user what part of the record they want to edit?
Renaming files/ deleting files (research) (add to UpdateDatabase() function?)
XOR File


*/
