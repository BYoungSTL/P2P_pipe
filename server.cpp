#include<windows.h>
#include<iostream>
#include<Vector>
#include <string>
#include<list>
#include<iterator>

using namespace std;
CRITICAL_SECTION mute;
SECURITY_ATTRIBUTES sa;
SECURITY_DESCRIPTOR sd;
HANDLE *hPipe = new HANDLE[10];
vector<char*> ClientList;
vector<char*>:: iterator it;
int i = 0;
bool CrNew = false;
DWORD WINAPI PipeCreation(LPVOID arg) {
	for (i = 0; i < 100; i++) {
		EnterCriticalSection(&mute);
		hPipe[i] = CreateNamedPipe("\\\\.\\pipe\\TestPipe"
			, PIPE_ACCESS_DUPLEX
			, PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT
			, PIPE_UNLIMITED_INSTANCES
			, 200000
			, 200000
			, INFINITE
			, &sa);
		if (hPipe[i] == INVALID_HANDLE_VALUE) {
			cout << "creation of pipe failed";
			system("pause");
			return -1;
		}
		ConnectNamedPipe(hPipe[i], (LPOVERLAPPED)NULL);
		LeaveCriticalSection(&mute);
		CrNew = true;
		Sleep(100);
	}
	return 0;
}

char str[30];
int main() {
	char name[30];
	DWORD BiRd;
	
	InitializeCriticalSection(&mute);
	sa.nLength = sizeof(sa);                                             // Security Attr made
	sa.bInheritHandle = FALSE;                                           // Need to give all users opportunity to operate
	InitializeSecurityDescriptor(&sd, SECURITY_DESCRIPTOR_REVISION);     // with pipe
	SetSecurityDescriptorDacl(&sd, TRUE, NULL, FALSE);                   //
	sa.lpSecurityDescriptor = &sd;                                       //
	HANDLE PCr = CreateThread(NULL, 0, PipeCreation, NULL, 0, NULL);
	while (true) {
		Sleep(100);
		while (CrNew) {
			WriteFile(hPipe[i], &i, sizeof(i), &BiRd, NULL);
			it = ClientList.begin();
			for (int j = 0; j < i; j++) {
				for(int k = 0; k <31; k++)
				str[k] = ClientList.at(j)[k];
				WriteFile(hPipe[i], str, sizeof(str), &BiRd, NULL);
			}
			CrNew = false;
			EnterCriticalSection(&mute);
			ReadFile(hPipe[i], name, sizeof(name), &BiRd, NULL);
			ClientList.push_back(name);
			it = ClientList.end();
			it--;
			cout << *it << endl;
			for (int j = 0; j < i; j++) {
				WriteFile(hPipe[j], name, sizeof(name), &BiRd, NULL);
			}
			//system("pause");
			LeaveCriticalSection(&mute);
		}
	}
	return 0;
}
