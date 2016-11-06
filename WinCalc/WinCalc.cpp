// WinCalc.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "WinCalc.h"

#include "CalcParser.h"

#define MAX_LOADSTRING 100

using calc_type_t = std::map<std::wstring, std::map<std::string, std::string>>;

static calc_type_t g_file_2_formula;

calc_type_t LoadCalcFile(const std::wstring& path = L"");
INT_PTR CALLBACK DialogProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

	g_file_2_formula = LoadCalcFile();
	if (g_file_2_formula.empty()) {
		// TODO
		return false;
	}

	HWND hDlg;
	hDlg = CreateDialogParam(hInstance, MAKEINTRESOURCE(IDD_MAIN_WIN), 0, DialogProc, 0);
	ShowWindow(hDlg, nCmdShow);

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

calc_type_t LoadCalcFile(const std::wstring& path) {
	return calc_type_t();
}

std::wstring PerformWinCalc(HWND hDlg) {
	TCHAR buff[1024];

	GetDlgItemText(hDlg, IDC_WIDTH_ET, buff, 1024);
	int w = _wtoi(buff);

	GetDlgItemText(hDlg, IDC_HEIGHT_ET, buff, 1024);
	int h = _wtoi(buff);

	GetDlgItemText(hDlg, IDC_TYPE_CBB, buff, 1024);

	std::map<std::string, CalcParser::OperandType> var;

	var["w"] = w;
	var["h"] = h;

	std::string err;
	CalcParser::CalculatorParser parser;
	const auto& form_set = g_file_2_formula[buff];

	for (auto it = form_set.begin(); it != form_set.end(); ++it) {
		err = "";
		auto v = parser.GenValue(it->second, var, err);
	}
	
	return L"";
}

void UpdateResult(const std::wstring& res) {
}

INT_PTR CALLBACK DialogProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case ID_RUN_CALC:
		    UpdateResult(PerformWinCalc(hDlg));
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
