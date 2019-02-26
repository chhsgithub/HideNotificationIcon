#include <windows.h>
#include <Commctrl.h>
#include <atlstr.h>

#pragma comment( linker, "/subsystem:\"windows\" /entry:\"mainCRTStartup\"" )

using namespace std;

void deleteIcon() {
	struct TRAYDATA
	{
		HWND hWnd;
		UINT uID;
		UINT uCallbackMessage;
		DWORD Reserved1[2];
		HICON hIcon;
		DWORD Reserved2[3];
		TCHAR szExePath[MAX_PATH];
		TCHAR szTip[128];
	};

	//get handler
	HWND hWnd = NULL;
	hWnd = ::FindWindow(_T("Shell_TrayWnd"), NULL);
	hWnd = ::FindWindowEx(hWnd, NULL, _T("TrayNotifyWnd"), NULL);
	hWnd = ::FindWindowEx(hWnd, NULL, _T("SysPager"), NULL);
	hWnd = ::FindWindowEx(hWnd, NULL, _T("ToolbarWindow32"), NULL);

	//get process ID
	DWORD dwPID = 0;
	::GetWindowThreadProcessId(hWnd, &dwPID);
	DWORD dwCount = ::SendMessage(hWnd, TB_BUTTONCOUNT, NULL, NULL);

	//open process
	HANDLE hProc = ::OpenProcess(PROCESS_VM_OPERATION | PROCESS_VM_READ | PROCESS_VM_WRITE, FALSE, dwPID);

	//buffer
	LPVOID pTB = ::VirtualAllocEx(hProc, NULL, sizeof(TBBUTTON), MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);

	TBBUTTON tb;
	TRAYDATA td;
	NOTIFYICONDATA nid;
	for (DWORD i = 0; i < dwCount; i++)
	{
		::SendMessage(hWnd, TB_GETBUTTON, i, (LPARAM)pTB);
		::ReadProcessMemory(hProc, pTB, &tb, sizeof(TBBUTTON), NULL);

		//get TRAYDATA information
		::ReadProcessMemory(hProc, (LPVOID)tb.dwData, &td, sizeof(TRAYDATA), NULL);

		nid.cbSize = sizeof(NOTIFYICONDATA);
		nid.hWnd = td.hWnd;
		nid.uID = td.uID;
		nid.uCallbackMessage = td.uCallbackMessage;
		nid.hIcon = td.hIcon;
		memcpy(nid.szTip, td.szTip, sizeof(nid.szTip));
		nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
		//delete icon
		::Shell_NotifyIcon(NIM_DELETE, &nid); 
	}

	//release memory
	::VirtualFreeEx(hProc, pTB, sizeof(TBBUTTON), MEM_FREE);
	::CloseHandle(hProc);
}

void main()
{
	deleteIcon();
}

