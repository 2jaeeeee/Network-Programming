// 2019년 1학기 네트워크프로그래밍 

// 플랫폼: VS2017


#pragma comment(lib, "ws2_32")
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <windows.h>
#include <time.h>
#include "resource.h"

SOCKET sock1;
SOCKET sock;

int size;
#define BUFSIZE     50
#define NAMESIZE	20
HINSTANCE Hlnstance;
char multi_address[40];
int port;
char name[NAMESIZE];
char old[NAMESIZE];
char Name[NAMESIZE + 1];
HWND HDIg;
int same;
struct ip_mreq mreq;
SOCKADDR_IN localaddr;
SOCKADDR_IN peeraddr;
SOCKADDR_IN remoteaddr;

int CALLBACK DlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);

//소켓 함수 오류 출력
void err_display(char *msg)
{
	LPVOID lpMsgBuf;

	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf, 0, NULL);

	MessageBox(NULL, (char *)lpMsgBuf, msg, MB_ICONERROR);

	LocalFree(lpMsgBuf);

	return;
}

//소켓 함수 오류 출력 후 종료
void err_quit(char *msg)
{
	LPVOID lpMsgBuf;

	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf, 0, NULL);

	MessageBox(NULL, (char *)lpMsgBuf, msg, MB_ICONERROR);

	LocalFree(lpMsgBuf);

	WSACleanup();
	exit(0);
}
DWORD WINAPI Receiver(LPVOID arg)
{
	int retval;

	// socket()
	sock1 = socket(AF_INET, SOCK_DGRAM, 0);
	if (sock1 == INVALID_SOCKET) err_quit(const_cast<char*>("socket()"));

	// SO_REUSEADDR 옵션 설정
	BOOL optval = TRUE;
	retval = setsockopt(sock1, SOL_SOCKET,
		SO_REUSEADDR, (char *)&optval, sizeof(optval));
	if (retval == SOCKET_ERROR) err_quit(const_cast<char*>("setsockopt()"));

	// bind()
	ZeroMemory(&localaddr, sizeof(localaddr));
	localaddr.sin_family = AF_INET;
	localaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	localaddr.sin_port = htons(port);
	retval = bind(sock1, (SOCKADDR *)&localaddr, sizeof(localaddr));
	if (retval == SOCKET_ERROR) err_quit(const_cast<char*>("bind()"));

	// 멀티캐스트 그룹 가입
	mreq.imr_multiaddr.s_addr = inet_addr(multi_address);
	mreq.imr_interface.s_addr = htonl(INADDR_ANY);
	retval = setsockopt(sock1, IPPROTO_IP, IP_ADD_MEMBERSHIP,
		(char *)&mreq, sizeof(mreq));
	if (retval == SOCKET_ERROR) err_quit(const_cast<char*>("setsockopt()"));

	// 데이터 통신에 사용할 변수
	int addrlen;
	char buf[BUFSIZE + 1];
	char str[2 * BUFSIZE];
	time_t timer;
	// 멀티캐스트 데이터 받기

	addrlen = sizeof(peeraddr);
	while (1) {
		// 데이터 받기
		retval = recvfrom(sock1, Name, NAMESIZE, 0,
			(SOCKADDR *)&peeraddr, &addrlen);
		if (retval == SOCKET_ERROR) {
			err_display(const_cast<char*>("recvfrom()3"));
			continue;
		}
		Name[retval] = '\0';
		retval = recvfrom(sock1, buf, BUFSIZE, 0,
			(SOCKADDR *)&peeraddr, &addrlen);
		if (retval == SOCKET_ERROR) {
			err_display(const_cast<char*>("recvfrom()4"));
			continue;
		}
			buf[retval] = '\0';

			
				timer = time(NULL);
				sprintf(str, "%s [%s %s]\n", buf, Name, ctime(&timer));

				// 받은 데이터 출력

				//	ntohs(peeraddr.sin_port), buf);
				SendDlgItemMessage(HDIg, LIST_CHAT, EM_SETSEL, 0, -1);
				SendDlgItemMessage(HDIg, LIST_CHAT, EM_SETSEL, -1, -1);
				SendDlgItemMessage(HDIg, LIST_CHAT, EM_REPLACESEL, FALSE, (LPARAM)str);

			
		
	}

	// 멀티캐스트 그룹 탈퇴
	retval = setsockopt(sock1, IPPROTO_IP, IP_DROP_MEMBERSHIP,
		(char *)&mreq, sizeof(mreq));
	if (retval == SOCKET_ERROR) err_quit(const_cast<char*>("setsockopt()"));

	// closesocket()
	closesocket(sock1);

	// 윈속 종료
	WSACleanup();
	return 0;
}
BOOL CALLBACK DlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{

	char Port[10]="", str[40];
	int addr;
	char a, b, c;
	char buf[BUFSIZE];
	int retval;
	int ttl=0;
	HANDLE hThread;
	char str1[2 * BUFSIZE];
	time_t timer;
	int addrlen = sizeof(peeraddr);

	HDIg = hDlg;
	switch (uMsg) {
	case WM_INITDIALOG: //초기화
		
		SetDlgItemText(hDlg, IP, "235.7.8.1");
		SetDlgItemText(hDlg, PORT, "9000");

		if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) == INVALID_SOCKET)
			err_quit(const_cast<char*>("socket()"));

		EnableWindow(GetDlgItem(hDlg, NAME), false);
		EnableWindow(GetDlgItem(hDlg, NAME_BUTTON), false);
		EnableWindow(GetDlgItem(hDlg, SEND_MESSAGE), false);
		EnableWindow(GetDlgItem(hDlg, SEND_BUTTON), false);
		EnableWindow(GetDlgItem(hDlg, RECONNECT_BUTTON), false);
		EnableWindow(GetDlgItem(hDlg, RENAME_BUTTON), false);
		break;

	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case CONNECT_BUTTON:
			
			GetDlgItemText(hDlg, IP, multi_address, sizeof(multi_address));
			GetDlgItemText(hDlg, PORT, Port, sizeof(Port));
			a = multi_address[0]; b = multi_address[1]; c = multi_address[2];
			addr = 100 * atoi(&a) + 10 * atoi(&b) + atoi(&c);
			port = atoi(Port);

			if (addr <= 224 || addr >= 239) {
				MessageBox(hDlg, "IP ERROR : 클래스 D 주소를 입력하세요(224~239)", "", MB_ICONERROR);
				break;
			}
			if (port < 1024 || port > 49151) {
				MessageBox(hDlg, "PORT ERROR : 유효한 포트번호를 입력하세요(1024~49151)", "", MB_ICONERROR);
				break;
			}
			
			// socket()
			sock = socket(AF_INET, SOCK_DGRAM, 0);
			if (sock == INVALID_SOCKET) err_quit(const_cast<char*>("socket()"));

			// 멀티캐스트 TTL 설정
			ttl = 2;
			retval = setsockopt(sock, IPPROTO_IP, IP_MULTICAST_TTL,
				(char *)&ttl, sizeof(ttl));
			if (retval == SOCKET_ERROR) err_quit(const_cast<char*>("setsockopt()"));

			// 소켓 주소 구조체 초기화
			ZeroMemory(&remoteaddr, sizeof(remoteaddr));
			remoteaddr.sin_family = AF_INET;
			remoteaddr.sin_addr.s_addr = inet_addr(multi_address);
			remoteaddr.sin_port = htons(port);
			

			MessageBox(hDlg, "접속 성공", "", MB_ICONINFORMATION);

			EnableWindow(GetDlgItem(hDlg, IP), false);
			EnableWindow(GetDlgItem(hDlg, PORT), false);
			EnableWindow(GetDlgItem(hDlg, CONNECT_BUTTON), false);
			EnableWindow(GetDlgItem(hDlg, NAME), true);
			EnableWindow(GetDlgItem(hDlg, NAME_BUTTON), true);
			EnableWindow(GetDlgItem(hDlg, RECONNECT_BUTTON), true);

			break;
		case NAME:
			GetDlgItemText(hDlg, NAME, name, sizeof(name));
			break;
		case NAME_BUTTON:
			
			if (strlen(name) == 0) {
				MessageBox(hDlg, "NAME ERROR : 대화명을 입력하세요.", "", MB_ICONERROR);
				break;
			}
			if (strlen(name)>10) {
				MessageBox(hDlg, "NAME ERROR : 대화명을 10자 이하로 설정하세요.", "", MB_ICONERROR);
				SetDlgItemText(hDlg, NAME, "");
				break;
			}
			
			sprintf(old, "%s", name);
			//리시버 스레드 생성
			hThread = CreateThread(NULL, 0, Receiver,
				NULL, 0, NULL);
			if (hThread == NULL) { closesocket(sock); }
			else { CloseHandle(hThread); }



			sprintf(str1, "대화명 입력 성공 : %s", name);
			MessageBox(hDlg, str1, "", MB_ICONINFORMATION);

		
			EnableWindow(GetDlgItem(hDlg, SEND_MESSAGE), true);
			EnableWindow(GetDlgItem(hDlg, SEND_BUTTON), true);

			EnableWindow(GetDlgItem(hDlg, RENAME_BUTTON), true);
			EnableWindow(GetDlgItem(hDlg, NAME_BUTTON), false);
			break;
		
		case RENAME_BUTTON:
			
			if (strlen(name) == 0) {
				MessageBox(hDlg, "NAME ERROR : 대화명을 입력하세요.", "", MB_ICONERROR);
				break;
			}
			if (strlen(name)>10) {
				MessageBox(hDlg, "NAME ERROR : 대화명을 10자 이하로 설정하세요.", "", MB_ICONERROR);
				SetDlgItemText(hDlg, NAME, "");
				break;
			}
			sprintf(str1, "대화명 변경 성공 : %s", name);
			MessageBox(hDlg, str1, "", MB_ICONINFORMATION);

			timer = time(NULL);
			sprintf(str1, "대화명 변경 : %s -> %s[%s]\n", old, name, ctime(&timer));

			SendDlgItemMessage(HDIg, LIST_CHAT, EM_SETSEL, 0, -1);
			SendDlgItemMessage(HDIg, LIST_CHAT, EM_SETSEL, -1, -1);
			SendDlgItemMessage(HDIg, LIST_CHAT, EM_REPLACESEL, FALSE, (LPARAM)str1);

			EnableWindow(GetDlgItem(hDlg, SEND_MESSAGE), true);
			EnableWindow(GetDlgItem(hDlg, SEND_BUTTON), true);

			break;
		case RECONNECT_BUTTON:
			EnableWindow(GetDlgItem(hDlg, IP), true);
			EnableWindow(GetDlgItem(hDlg, PORT), true);
			EnableWindow(GetDlgItem(hDlg, CONNECT_BUTTON), true);
			EnableWindow(GetDlgItem(hDlg, NAME), false);
			EnableWindow(GetDlgItem(hDlg, NAME_BUTTON), false);
			break;

	
		case SEND_BUTTON:

			GetDlgItemText(hDlg, SEND_MESSAGE, buf, sizeof(buf));
			// socket()
			sock = socket(AF_INET, SOCK_DGRAM, 0);
			if (sock == INVALID_SOCKET) err_quit(const_cast<char*>("socket()"));

			// 멀티캐스트 TTL 설정
			ttl = 2;
			retval = setsockopt(sock, IPPROTO_IP, IP_MULTICAST_TTL,
				(char *)&ttl, sizeof(ttl));
			if (retval == SOCKET_ERROR) err_quit(const_cast<char*>("setsockopt()"));

			// 소켓 주소 구조체 초기화
			ZeroMemory(&remoteaddr, sizeof(remoteaddr));
			remoteaddr.sin_family = AF_INET;
			remoteaddr.sin_addr.s_addr = inet_addr(multi_address);
			remoteaddr.sin_port = htons(port);
			retval = sendto(sock, name, strlen(name), 0,(SOCKADDR *)&remoteaddr, sizeof(remoteaddr));
			if (retval == SOCKET_ERROR) {
				err_display(const_cast<char*>("sendto()"));
				break;
			}
			retval = sendto(sock, buf, strlen(buf), 0,
				(SOCKADDR *)&remoteaddr, sizeof(remoteaddr));
			if (retval == SOCKET_ERROR) {
				err_display(const_cast<char*>("sendto()"));
				break;
			}
			SetDlgItemText(hDlg, SEND_MESSAGE, "");

			break;
		case IDCANCEL:
			EndDialog(hDlg, IDCANCEL);
			return TRUE;
		}
		return FALSE;
	}
	return FALSE;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
	LPSTR lpCmdLine, int nCmdShow)
{
	WSADATA wsa;

	Hlnstance = hInstance;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return 1;

	// 대화상자 생성
	DialogBox(hInstance, MAKEINTRESOURCE(IDD_DIALOG1), NULL, DlgProc);


	// 윈속 종료
	WSACleanup();
	return 0;
}