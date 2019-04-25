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
#include <cctype>
#include <windows.h>

using namespace std;

// Acceptable characters to be used in the password generation
static const char alphnum[] = "0123456789" "!@#$%^&*" "ABCDEFGHIJKLMNOPQRSTUVWXYZ" "abcdefghijklmnopqrstuvwxyz";

struct Record {
	string id;
	string description;
	string user;
	string pass;
};

void Title();
void DeleteRecordBanner();
void AddRecordBanner();
void ViewRecordBanner();
void EditRecordsBanner();
void RecordsBanner();
bool databaseExists();

bool isFirstRun();
void PopulateVector(vector<Record> &vRecords);
void ResizeEntry(string &entry, int maxWidth);
void MainMenu(vector<Record>& vRecords);
void UpdateDatabase(vector<Record> &vRecords);
void DisplayRecords(vector<Record> &vRecords);
void FirstRunMenu(vector<Record> &vRecords);
void PromptForMasterPassword(string &hash);

/* CRUD */
void ViewRecord(vector<Record> &vRecords);
void AddRecord(vector<Record>& vRecords);
void EditRecord(vector<Record> &vRecords);
void DeleteRecord(vector<Record>& vRecords);

/*Cryptography*/
void Krypt();
void WriteHashToKeyFile(string &key);
string CreateMasterPassword();
string GetInput();
string GetHash();
char GenRand();
string GenPassword();

int main()
{
	if (isFirstRun())
	{
		Title();
		cout << "Welcome to KryptKeeper.\nPlease begin by creating a master password.\n";
		WriteHashToKeyFile(CreateMasterPassword());

		cout << "Press ENTER to start storing your passwords securely.";
		cin.ignore();
	}

	PromptForMasterPassword(GetHash());
	vector<Record> Records;

	if (!databaseExists())
	{
		FirstRunMenu(Records);
	}

	else
	{
		Krypt();
		PopulateVector(Records);
		Krypt();
	}

	MainMenu(Records);

	return 0;
}

/* CRUD */
void ViewRecord(vector<Record>& vRecords)
{
	system("cls");
	Title();
	ViewRecordBanner();
	DisplayRecords(vRecords);

	string searchString;
	bool recordExists = false;

	while (!recordExists)
	{
		cout << "Select a record to view: ";
		getline(cin, searchString);

		if (searchString == "c")
		{
			MainMenu(vRecords);
		}

		for (unsigned i = 0; i < vRecords.size(); ++i)
		{
			Record& rec = vRecords[i];

			if (searchString.compare((rec.id)) == 0)
			{
				recordExists = true;
				cout << "Record " << (rec.id) << " is Selected.\n\n";
				cout << "Description: " << rec.description << "\n"
					<< "Username: " << rec.user << "\n"
					<< "Password: " << rec.pass << "\n";

				break;
			}
		}
	}

	for (unsigned i = 0; i < vRecords.size(); i++)
	{
		Record& r = vRecords[i];
		r.id = to_string(i + 1);
	}

	cout << "\nPress ENTER to continue\n";
	cin.ignore();

	MainMenu(vRecords);
}

void EditRecord(vector<Record> &vRecords)
{
	system("cls");
	Title();
	EditRecordsBanner();
	DisplayRecords(vRecords);

	string searchString;
	bool recordExists = false;

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
				cout << "Record " << (rec.id) << " is Selected.\n\n";
				cout << "Description: " << rec.description << "\n"
					<< "Username: " << rec.user << "\n"
					<< "Password: " << rec.pass << "\n";

				bool isEditFinished = false;

				while (!isEditFinished)
				{
					bool isValidOption = false;

					string newDescription;
					string newUserName;
					string newPassword;
					string option;
					string finished;

					cout << "\nWhat would you like to edit?\n";
					cout << "1. Description\n";
					cout << "2. Username\n";
					cout << "3. Password\n";
					cout << "c. Cancel\n";

					while (!isValidOption)
					{
						cout << ">> ";
						getline(cin, option);

						if (option == "1")
						{
							isValidOption = true;
							cout << "Description: ";
							getline(cin, newDescription);

							if (newDescription == "c")
							{
								MainMenu(vRecords);
							}

							rec.description = newDescription;
							cout << "Are you finished editing this record? [y/n]\n";
							cout << ">> ";
							getline(cin, finished);

							if (finished == "y")
							{
								isEditFinished = true;
								break;
							}
						}

						if (option == "2")
						{
							isValidOption = true;
							cout << "Username: ";
							getline(cin, newUserName);

							if (newUserName == "c")
							{
								break;
							}

							rec.user = newUserName;
							cout << "Are you finished editing this record? [y/n]\n";
							cout << ">> ";
							getline(cin, finished);

							if (finished == "y")
							{
								isEditFinished = true;
								break;
							}
						}

						if (option == "3")
						{
							isValidOption = true;
							cout << "Password: ";
							getline(cin, newPassword);

							if (newPassword == "c")
							{
								break;
							}

							rec.pass = newPassword;
							cout << "Are you finished editing this record? [y/n]\n";
							cout << ">> ";
							getline(cin, finished);

							if (finished == "y")
							{
								isEditFinished = true;
								break;
							}
						}

						if (option == "c")
						{
							isValidOption = true;
							MainMenu(vRecords);
						}
					}
				}
	
				break;
			}

			if (searchString == "c")
			{
				MainMenu(vRecords);
			}
		}
	}

	MainMenu(vRecords);
}

void AddRecord(vector<Record> &vRecords)
{
	system("cls");
	Title();
	AddRecordBanner();

	string description, username, password, confirmPassword, choice;
	
	cout << "Description: ";
	getline(cin, description);
	cout << "Username: ";
	getline(cin, username);
	cout << "Would you like to generate a password? Type 'YES' if so otherwise press enter.";
	getline(cin, choice);
	
	if (choice == "YES") 
	{
		cout << "Your Password is: " << GenPassword() << endl;
	}

	bool isValidPassword = false;

	while (!isValidPassword)
	{
		cout << "Password: ";
		getline(cin, password);
		cout << "Confirm Password: ";
		getline(cin, confirmPassword);

		// only if the confirm pass word and password match will the record be added.
		if (password == confirmPassword)
		{
			isValidPassword = true;

			Record rec;
			rec.description = description;
			rec.user = username;
			rec.pass = password;

			vRecords.push_back(rec);

			for (unsigned i = 0; i < vRecords.size(); i++)
			{
				Record& r = vRecords[i];
				r.id = to_string(i + 1);
			}
		}
		else
		{
			isValidPassword = false;
			cout << "Passwords Do Not Match!\n";
		}
	}
	MainMenu(vRecords);
	//DisplayRecords(vRecords);
}

void DeleteRecord(vector<Record>& vRecords)
{
	system("cls");
	Title();
	DeleteRecordBanner();
	DisplayRecords(vRecords);

	string searchString;
	string check;
	bool recordExists = false;

	while (!recordExists)
	{
		cout << "Select a record to delete: ";
		getline(cin, searchString);

		if (searchString == "c")
		{
			MainMenu(vRecords);
		}

		for (unsigned i = 0; i < vRecords.size(); ++i)
		{
			Record& rec = vRecords[i];

			if (searchString.compare((rec.id)) == 0)
			{
				recordExists = true;
				cout << "Please confirm with yes to delete. Record: " << rec.id << "\n";
				getline(cin, check);
				if (check == "yes")
				{
					cout << "Record " << (rec.id) << " is deleted.\n";
					vRecords.erase(vRecords.begin() + i);

					break;
				}
			}
		}
	}

	for (unsigned i = 0; i < vRecords.size(); i++)
	{
		Record& r = vRecords[i];
		r.id = to_string(i + 1);
	}

	MainMenu(vRecords);
}

string GetInput()
{
	string s;

	cout << ">> ";
	getline(cin, s);

	return s;
}

void MainMenu(vector<Record> &vRecords) {

	system("cls");
	Title();
	RecordsBanner();
	DisplayRecords(vRecords);

	struct MenuAction {

		string description;
		function<void()> action;
	};

	const map <string, MenuAction> actionTable {

		{ "1",{ "View Record", [&]() { ViewRecord(vRecords); } } },
		{ "2",{ "Add Record", [&]() { AddRecord(vRecords); } } },
		{ "3",{ "Edit Record", [&]() { EditRecord(vRecords); } } },
		{ "4",{ "Delete Record", [&]() { DeleteRecord(vRecords); } } },
		{ "s",{ "Save and Exit", [&]() { UpdateDatabase(vRecords); Krypt(); exit(0); } } },
		{ "q",{ "Quit", []() { exit(0); } } }
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
		getline(iss, rec.description, '\t');
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
	system("cls");
	Title();
	bool isAuthenticated = false;
	string masterPassword;

	//cout << "Enter Master Password to unlock database.\n";

	while (!isAuthenticated)
	{
		cout << "Master Password: ";
		getline(cin, masterPassword);

		SHA256 sha256;

		if (sha256(masterPassword) == hash)
		{
			isAuthenticated = true;
		}
	}
}

void WriteHashToKeyFile(string &key)
{
	SHA256 sha256;
	ofstream outFile("key.txt");
	wchar_t* fileLPCWSTR = L"key.txt";

	int attr = GetFileAttributes(fileLPCWSTR);
	if ((attr & FILE_ATTRIBUTE_HIDDEN) == 0) {
		SetFileAttributes(fileLPCWSTR, attr | FILE_ATTRIBUTE_HIDDEN);
	}
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
			cout << "\nMaster Password has been created. Store this in a safe location.\n";
		}
		else
		{
			isValidPassword = false;
			cout << "Passwords Do Not Match!\n";
		}
	}

	return masterPassword;
}

void Krypt()
{
	ifstream inFile;
	ofstream outFile;

	char fileName[] = "database.txt";
	char renamedFile[] = "database.old";
	string key = "KryptKeeper5000!";

	inFile.open(fileName, ios::in | ios::binary);
	string str((istreambuf_iterator<char>(inFile)), istreambuf_iterator<char>());
	inFile.close();

	for (unsigned i = 0; i < str.size(); i++)
	{
		str[i] ^= key[i % key.size()];
	}

	if (rename(fileName, renamedFile) == 0)
	{
		outFile.open(fileName, ios::out | ios::binary);
		outFile.write(str.c_str(), str.size());
		remove(renamedFile);
	}
}

// if key.txt exists return false
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

// simple formatting
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
	cout << right << "| " << setw(3) << "#" << left << setw(3) << " |"
		<< setw(25) << "Description"
		<< setw(40) << "UserName"
		<< setw(44) << "Password" << " |"
		<< "\n" << string(119, '-') << "\n";

	if (vRecords.size() == 0)
	{
		cout << "\n" << string(48, ' ') << "NO RECORDS TO DISPLAY\n\n";

		FirstRunMenu(vRecords);
	}

	else
	{
		for (unsigned i = 0; i < vRecords.size(); ++i)
		{
			Record &rec = vRecords[i];
			ResizeEntry(rec.description, 25);
			ResizeEntry(rec.user, 40);
			ResizeEntry(rec.pass, 40);

			cout << right << "| " << setw(3) << rec.id << left << setw(3) << " |"
				<< setw(25) << rec.description
				<< setw(40) << rec.user
				<< setw(44) << string(20, '*') << " |"
				<< "\n" << string(119, '-') << "\n";
		}
	}
}

void FirstRunMenu(vector<Record> &vRecords)
{
	system("cls");
	Title();

	struct FirstRunMenuAction {

		string description;
		function<void()> action;
	};

	const map <string, FirstRunMenuAction> FirstRunMenuTable{

		{ "1",{ "Add Record", [&]() { AddRecord(vRecords); } } },
		{ "q",{ "Quit", []() { exit(0); } } }
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

void UpdateDatabase(vector<Record> &vRecords)
{	
	char fileName[] = "database.txt";

	remove(fileName);

	ofstream outFile;
	outFile.open("database.txt", ios_base::app);
	
	wchar_t* fileLPCWSTR = L"database.txt";

	int attr = GetFileAttributes(fileLPCWSTR);
	if ((attr & FILE_ATTRIBUTE_HIDDEN) == 0) {
		SetFileAttributes(fileLPCWSTR, attr | FILE_ATTRIBUTE_HIDDEN);
	}

	for (unsigned i = 0; i < vRecords.size(); ++i)
	{
		Record &rec = vRecords[i];
		
		outFile << rec.id << "\t" << rec.description << "\t"<< rec.user << "\t" << rec.pass << "\n";
	}
}

char GenRand()
{
	int strLen = sizeof(alphnum) - 1;
	return alphnum[rand() % strLen];
}

string GenPassword() 
{
	int n, c = 0, s = 0;
	string password;
	srand(time(0));
	cout << "Enter the length of the password required:";
	cin >> n;
	cin.ignore();

	char C;
	
	for (int z = 0; z < n; z++)
	{
		C = GenRand();
		password += C;
		if (isdigit(C))
		{
			c++;
		}
		if (C == '!' || C == '@' || C == '$' || C == '%' || C == '^' || C == '&' || C == '*' || C == '#')
		{
			s++;
		}
	}

	return password;
}

void DeleteRecordBanner()
{
	cout << string(119, '=') << "\n";
	cout << "|" << string(52, ' ') << "Delete Record" << string(52, ' ') << "|" << "\n";
	cout << string(119, '=') << "\n";
}

void AddRecordBanner()
{
	cout << string(119, '=') << "\n";
	cout << "|" << string(53, ' ') << "Add Record" << string(54, ' ') << "|" << "\n";
	cout << string(119, '=') << "\n";
}

void ViewRecordBanner()
{
	cout << string(119, '=') << "\n";
	cout << "|" << string(53, ' ') << "View Record" << string(53, ' ') << "|" << "\n";
	cout << string(119, '=') << "\n";
}

void EditRecordsBanner()
{
	cout << string(119, '=') << "\n";
	cout << "|" << string(53, ' ') << "Edit Record" << string(53, ' ') << "|" << "\n";
	cout << string(119, '=') << "\n";
}

void RecordsBanner()
{
	cout << string(119, '=') << "\n";
	cout << "|" << string(55, ' ') << "Records" << string(55, ' ') << "|" << "\n";
	cout << string(119, '=') << "\n";
}

bool databaseExists()
{
	bool result = false;
	ifstream inFile("database.txt");

	if (inFile)
	{
		result = true;
	}

	return result;
}

void Title()
{
	cout << "\n" << string(54, ' ') << "KryptKeeper" << string(54, ' ') << "\n\n";
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
