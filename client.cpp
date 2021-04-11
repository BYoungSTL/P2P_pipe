#include<windows.h>
#include<iostream>
#include<string>
#include<iterator>
#include<list>
#include<Vector>
#include <direct.h>

using namespace std;
SECURITY_ATTRIBUTES sa;
SECURITY_DESCRIPTOR sd;
HANDLE *hPipe = new HANDLE[100];
vector<char*> ClientList;
vector<char*>::iterator it;
int n = 0, j = 0,DiCh = 0;
bool FNewCon = true;
char str[30];
HANDLE hNamedPipe;
DWORD WrittenCount;
string DirList;
int testin = 0;

DWORD WINAPI P2PRead(LPVOID arg) {
	while (true) {
		int CtWord = 1;
		if (DiCh) {
			Sleep(100);
			WriteFile(hPipe[DiCh - 1], &CtWord, sizeof(CtWord), &WrittenCount, NULL); ///////////////
			Sleep(100);
			WriteFile(hPipe[DiCh - 1], &CtWord, sizeof(CtWord), &WrittenCount, NULL); ///////////////
			Sleep(100);
			ReadFile(hPipe[DiCh - 1], &testin, sizeof(testin), &WrittenCount, NULL); ////////////////
			Sleep(100);
			ReadFile(hPipe[DiCh - 1], &testin, sizeof(testin), &WrittenCount, NULL); ////////////////
			DiCh = 0;
			cout << testin;
		}
	}
	return 0;
}
DWORD WINAPI P2PDialog(LPVOID arg) {
	while (true) {
		for (int k = 0; k < n; k++) {
			cout << k + 1 << ". " << ClientList.at(k) << endl;
		}
		cout << "Enter your choice(0 to refresh list): ";
		cin >> DiCh;
		//DiCh = 1;
		//system("pause");
		//Sleep(100);
	}
	return 0;
}
DWORD WINAPI P2PCon(LPVOID arg) {
	string comp;
	int CtWords = 0, CurrInd = 0;
	ReadFile(hNamedPipe, str, sizeof(str), &WrittenCount, NULL);
	Sleep(100);
	comp = "\\\\";
	comp += str;
	comp += "\\pipe\\P2PCon";
	WaitNamedPipe(comp.c_str(), NMPWAIT_WAIT_FOREVER);
	hPipe[n] = CreateFile(comp.c_str()
		, GENERIC_READ | GENERIC_WRITE
		, FILE_SHARE_READ | FILE_SHARE_WRITE
		, NULL
		, OPEN_EXISTING
		, 0
		, NULL);
	if (hPipe[n] == INVALID_HANDLE_VALUE) {
		cout << "Pipe P2P connection failed!";
		system("pause");
		return -1;
	}
	ClientList.push_back(str);
	CurrInd = n;
	n++;
	Sleep(100);
	FNewCon = true;
	while (true) {
		while ((DiCh-1)!= CurrInd) {
			WIN32_FIND_DATA FiInf;
			HANDLE hFile;
			string DiName = "C:\\4Kurs\\*.*", list;
			int tester = 2;
			ReadFile(hPipe[CurrInd], &CtWords, sizeof(CtWords), &WrittenCount, NULL);
			hFile = FindFirstFile(DiName.c_str(), &FiInf);
			FindNextFile(hFile, &FiInf);
			while (hFile != INVALID_HANDLE_VALUE && FindNextFile(hFile, &FiInf)) {
				list += FiInf.cFileName;
			}
			WriteFile(hPipe[CurrInd], &tester, sizeof(tester), &WrittenCount, NULL);
		}
	}
	return 0;
}
DWORD WINAPI P2PCr(LPVOID arg) {
	int CtWords = 0, CurrInd = 0;
	CurrInd = j;
	string NCom;
	NCom = "\\\\";
	NCom += (char)arg;
	NCom += "\\pipe\\P2PCon";
	hPipe[j] = CreateNamedPipe(NCom.c_str()
		, PIPE_ACCESS_DUPLEX
		, PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT
		, PIPE_UNLIMITED_INSTANCES
		, 200000
		, 200000
		, INFINITE
		, &sa);
	if (hPipe[j] == INVALID_HANDLE_VALUE) {
		cout << "creation of pipe failed";
		system("pause");
		return -1;
	}
	ConnectNamedPipe(hPipe[j], (LPOVERLAPPED)NULL);
	Sleep(100);
	while (true) {
		while ((DiCh - 1) != CurrInd) {
			WIN32_FIND_DATA FiInf;
			HANDLE hFile;
			string DiName = "C:\\4Kurs\\*.*", list;
			int tester = 2;
			ReadFile(hPipe[CurrInd], &CtWords, sizeof(CtWords), &WrittenCount, NULL);
			hFile = FindFirstFile(DiName.c_str(), &FiInf);
			FindNextFile(hFile, &FiInf);
			while (hFile != INVALID_HANDLE_VALUE && FindNextFile(hFile, &FiInf)) {
				list += FiInf.cFileName;
			}
			WriteFile(hPipe[CurrInd], &tester, sizeof(tester), &WrittenCount, NULL);
		}
	}
	return 0;
}


int main() {
	sa.nLength = sizeof(sa);                                             // Security Attr made
	sa.bInheritHandle = FALSE;                                           // Need to give all users opportunity to operate
	InitializeSecurityDescriptor(&sd, SECURITY_DESCRIPTOR_REVISION);     // with pipe
	SetSecurityDescriptorDacl(&sd, TRUE, NULL, FALSE);                   //
	sa.lpSecurityDescriptor = &sd;                                       //
	WaitNamedPipe("\\\\DESKTOP-H5JP7NT\\pipe\\TestPipe", NMPWAIT_WAIT_FOREVER);
	hNamedPipe = CreateFile("\\\\DESKTOP-H5JP7NT\\pipe\\TestPipe"
							, GENERIC_READ|GENERIC_WRITE
							, FILE_SHARE_READ|FILE_SHARE_WRITE
							, NULL
							, OPEN_EXISTING
							, 0
							, NULL);
	if (hNamedPipe == INVALID_HANDLE_VALUE) {
		cout << "Pipe connection failed!";
		system("pause");
		return -1;
	}

	char name[30];
	DWORD size = 256;
	ReadFile(hNamedPipe, &n, sizeof(n), &WrittenCount, NULL);
	for (j = 0; j < n; j++) {
		ReadFile(hNamedPipe, str, sizeof(str), &WrittenCount, NULL);
		ClientList.push_back(str);
	}
	//delete &str;
	GetComputerName(name, &size);
	cout << name << endl;
	//system("pause");
	if (!WriteFile(hNamedPipe, name, sizeof(name), &WrittenCount, NULL)) {
		cout << "Failed to detect name of Client" << endl;
		system("pause");
		return -1;
	}
	cout << "Info about name was translated" << endl;
	for (j = 0; j < n; j++) {
		char a = '.';
		CreateThread(NULL, 0, P2PCr, (LPVOID)a, 0, NULL);
		Sleep(100);
	}
	CreateThread(NULL, 0, P2PRead, NULL, 0, NULL);
	Sleep(100);
	CreateThread(NULL, 0, P2PDialog, NULL, 0, NULL);
	Sleep(100);
	while (true) {
		if (FNewCon == true) {
			FNewCon = false;
			CreateThread(NULL, 0, P2PCon, NULL, 0, NULL);
		}
	}
	system("pause");
	return 0;
}
