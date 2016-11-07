// WinCalc.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "WinCalc.h"

#include "CalcParser.h"

#define MAX_LOADSTRING 100

using calc_type_t = std::map<std::wstring, std::map<std::string, std::string>>;

static calc_type_t g_file_2_formula;
static TCHAR g_confgDir[MAX_PATH];

calc_type_t LoadCalcFile(const TCHAR* path);
INT_PTR CALLBACK DialogProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

	GetCurrentDirectory(sizeof g_confgDir, g_confgDir);

	g_file_2_formula = LoadCalcFile(g_confgDir);
	if (g_file_2_formula.empty()) {
		// TODO
	}

	HWND hDlg;
	hDlg = CreateDialogParam(hInstance, MAKEINTRESOURCE(IDD_MAIN_WIN), 0, DialogProc, 0);
	ShowWindow(hDlg, nCmdShow);

	if (!g_file_2_formula.empty()) {
		HWND hcb = GetDlgItem(hDlg, IDC_TYPE_CBB);
		for (auto i = g_file_2_formula.begin(); i != g_file_2_formula.end(); ++i) {
			SendMessage(hcb, CB_ADDSTRING, 0, (LPARAM)i->first.c_str());
		}

		SendMessage(hcb, CB_SETCURSEL, 0, 0);
	}

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_WINCALC));

    MSG msg;
	BOOL ret;

    // Main message loop:
	while ((ret = GetMessage(&msg, 0, 0, 0)) != 0) {
		if (ret == -1) return -1;

		if (!IsDialogMessage(hDlg, &msg)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

    return (int) msg.wParam;
}

calc_type_t LoadCalcFile(const TCHAR* path) {
	WIN32_FIND_DATA ffd;
	TCHAR szDir[MAX_PATH] = { 0 };

	calc_type_t ret;

	swprintf(szDir, sizeof szDir, L"%s\\wcs_*.txt", path);

	HANDLE hFind = FindFirstFile(szDir, &ffd);

	if (INVALID_HANDLE_VALUE == hFind) {
		return ret;
	}

	// List all the files in the directory with some info about them.

	std::string line;

	do {
		std::ifstream wif(ffd.cFileName);
		std::map<std::string, std::string> tmp;

		while (std::getline(wif, line)) {
			size_t sz = line.find(':');
			if (sz == std::string::npos) continue;

			tmp[line.substr(0, sz)] = line.substr(sz + 1);
		}

		TCHAR name[MAX_PATH];
		_wsplitpath(ffd.cFileName, nullptr, nullptr, name, nullptr);
		ret[name] = tmp;
	} while (FindNextFile(hFind, &ffd) != 0);

	return ret;
}

std::string PerformWinCalc(HWND hDlg) {
	TCHAR buff[1024];

	GetDlgItemText(hDlg, IDC_WIDTH_ET, buff, 1024);
	int w = _wtoi(buff);

	GetDlgItemText(hDlg, IDC_HEIGHT_ET, buff, 1024);
	int h = _wtoi(buff);

	GetDlgItemText(hDlg, IDC_TYPE_CBB, buff, 1024);

	std::map<std::string, CalcParser::OperandType> var;

	var["c"] = w;
	var["k"] = h;

	std::string err;
	CalcParser::CalculatorParser parser;
	const auto& form_set = g_file_2_formula[buff];

	std::string ret;
	ret.reserve(256);

	char val[128] = { 0 };
	for (auto it = form_set.begin(); it != form_set.end(); ++it) {
		err = "";
		auto v = parser.GenValue(it->second, var, err);

		try {
			snprintf(val, sizeof buff, "%g", boost::get<double>(v));
			ret += it->first + ": " + val + "\n";
		}
		catch (...) {
			ret += it->first + ": " + "¼ÆËã³ö´í\n";
		}
	}
	
	return ret;
}

void UpdateResult(HWND hDlg, const std::string& res) {
	SetDlgItemTextA(hDlg, IDC_RES_LB, res.c_str());
}

INT_PTR CALLBACK DialogProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case ID_RUN_CALC:
		    UpdateResult(hDlg, PerformWinCalc(hDlg));
			return TRUE;
		}
		break;

	case WM_CLOSE:
		DestroyWindow(hDlg);
		return TRUE;

	case WM_DESTROY:
		PostQuitMessage(0);
		return TRUE;
	}

	return FALSE;
}
