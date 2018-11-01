// lab48.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "lab48.h"
#include "plot.h"
#include "resource.h"
#include <array>
#include <vector>
plot a;
std::array<std::vector<double>, 2> test = plot::parabola(100);

INT_PTR CALLBACK    MyWin(HWND, UINT, WPARAM, LPARAM);
double Pos[] = { 0.0, 0.0, 0.7, 0.7 },// x1,y1,x2,y2
Scale[] = { -16.0, -16, 16.0, 16.0 }; // x1,y1,x2,y2
std::vector<plot> func;
int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{

	DialogBoxParam(hInstance, MAKEINTRESOURCE(IDD_ABOUTBOX), 0, MyWin,0);
    

}


INT_PTR CALLBACK MyWin(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
		//a.Set(hDlg, test[0], test[1], Scale, Pos);
		//func.push_back(a);


		//test = plot::line(100);
		//a.Set(hDlg, test[0], test[1], Scale, Pos);
		//func.push_back(a);

		//test = plot::_line(100);
		//a.Set(hDlg, test[0], test[1], Scale, Pos);
		//func.push_back(a);

        return (INT_PTR)TRUE;
	case WM_PAINT: {
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hDlg, &ps);
		Sleep(50);
		if (func.size() != 0)func[0].clear(hdc);
		for (auto it = func.begin(); it != func.end(); ++it)
			it->Draw();

		EndPaint(hDlg, &ps);
	} break;

	case WM_SIZE: {
		for (auto it = func.begin(); it != func.end(); ++it)
		{
			it->SetPosX();
			it->SetPosY();
		}
	} break;

	case WM_KEYDOWN:
		switch (wParam)
		{
		case 189:
			Scale[0]--; Scale[1]--; Scale[2]++; Scale[3]++;
			break;
		case 187:
			Scale[0]++; Scale[1]++; Scale[2]--; Scale[3]--;
			break;
		case VK_LEFT:
			Scale[0]--; Scale[2]--;
			break;
		case VK_RIGHT:
			Scale[0]++; Scale[2]++;
			break;
		case VK_DOWN:
			Scale[3]--; Scale[1]--;
			break;
		case VK_UP:
			Scale[3]++; Scale[1]++;
			break;
		}
		for (auto it = func.begin(); it != func.end(); ++it)
		{
			it->SetScaleX(Scale);
			it->SetScaleY(Scale);
		}
		SendMessage(hDlg, WM_PAINT, wParam, lParam);

		break;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}
