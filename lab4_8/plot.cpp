#include "stdafx.h"
#include "plot.h"
#include <array>
#pragma warning (disable:4996)
#define _CRT_SECURE_NO_WARNINGS
plot::plot()
{
	hWnd = 0; hdc = 0;
	pDrawFun = &plot::Draw_Null;

	_tcscpy(XLabel, _T("X")); _tcscpy(YLabel, _T("Y"));

	// Pen
	Pen_Axes = CreatePen(PS_SOLID, 2, RGB(0, 0, 0));
	Pen_Line = CreatePen(PS_SOLID, 2, RGB(0, 0, 255));
	Brush_Bkgnd = CreateSolidBrush(RGB(255, 255, 255));

	Indent = 5;

	// Ticks
	NumTicksX = MaxTicksX;
	NumTicksY = MaxTicksX;

	SetFont();
	AxesFont = 0;
}

plot :: ~plot()
{
	DeleteObject(Pen_Axes); DeleteObject(Pen_Line);
	DeleteObject(Brush_Bkgnd);
	if (AxesFont) DeleteObject(AxesFont);
}

std::array<std::vector<double>, 2> plot::parabola(int n)
{
	std::array<std::vector<double>, 2> a;

	for (double i = -n / 2; i<n; i += 0.1)
	{
		a[0].push_back(i);
		a[1].push_back(i*i);
	}
	return a;
}

std::array<std::vector<double>, 2> plot::line(int n)
{
	std::array<std::vector<double>, 2> a;

	for (double i = -n / 2; i<n; i += 0.1)
	{
		a[0].push_back(i);
		a[1].push_back(i);
	}
	return a;
}

std::array<std::vector<double>, 2> plot::_line(int n)
{
	std::array<std::vector<double>, 2> a;

	for (double i = -n / 2; i<n; i += 0.1)
	{
		a[0].push_back(i);
		a[1].push_back(-i);
	}
	return a;
}
std::array<std::vector<double>, 2> plot::random(int n)
{
	std::array<std::vector<double>, 2> a;

	for (double i = -n / 2; i<n; i += 0.1)
	{
		a[0].push_back(rand() % (2 * n) - n);
		a[1].push_back(rand() % (2 * n) - n);
	}
	return a;
}

//
// Set functions 
void plot::Set(HWND chWnd, std::vector<double> cXData, std::vector<double> cYData, double*cScale, double *cPos)
{
	if (!hWnd || !hdc) { hWnd = chWnd; hdc = GetDC(hWnd); }

	SetFont();

	if (!cPos) { Pos[0] = 0.0;         Pos[1] = 0.0;         Pos[2] = 1.0;         Pos[3] = 1.0; }
	if (!cScale) { Scale[0] = 0.0;       Scale[1] = 0.0;       Scale[2] = 1.0;       Scale[3] = 1.0; }
	else { Scale[0] = cScale[0]; Scale[1] = cScale[1]; Scale[2] = cScale[2]; Scale[3] = cScale[3]; }
	SetPosX(cPos);
	SetPosY(cPos);
	DataLen = XData.size();
	SetData(cXData, cYData);
}
void plot::SetData(std::vector<double>cXData, std::vector<double> cYData)
{
	DataLen = YData.size(); XData = cXData; YData = cYData;
	if (cXData.size() && cYData.size())
	{


		if (XData.size() == 2)
			if (XData[1] - XData[0] >= 0)
			{
				pDrawFun = &plot::Draw_XLinear;                return;
			} // X Linera data
			else { pDrawFun = &plot::Draw_Null;                  return; } // for dX<0 not realised 
			if (DataLen == 2)
				if (YData[1] - YData[0] >= 0)
				{
					pDrawFun = &plot::Draw_YLinear; DataLen = XData.size(); return;
				} // Y Linera data
				else { pDrawFun = &plot::Draw_Null;                  return; } // for dY<0 not realised
				if (DataLen == 1)
				{
					pDrawFun = &plot::Draw_Point;                      return;
				} // one point

				  // is data sorted?


				double x_sort = XData[1] - XData[0], y_sort = YData[1] - YData[0];
				int fl_XSorted = 1, fl_YSorted = 1;
				for (ulint i = 1; i<DataLen; ++i)
				{
					if (x_sort*(XData[i] - XData[i - 1])<0) fl_XSorted = 0;
					else if (x_sort == 0) x_sort = XData[i] - XData[i - 1];
					if (y_sort*(YData[i] - YData[i - 1])<0) fl_YSorted = 0;
					else if (y_sort == 0) y_sort = YData[i] - YData[i - 1];
				}

				// only for ascending data realised
				if (fl_XSorted && (x_sort >= 0))
				{
					pDrawFun = &plot::Draw_XSorted; return;
				} // X Sorted data
				if (fl_YSorted && (y_sort >= 0))
				{
					pDrawFun = &plot::Draw_YSorted; return;
				} // Y Sorted data

				  // Other cases, suitable for all cases, except when X or Y is given as Linear
				pDrawFun = &plot::Draw_Full;
	}
	else
	{   // uncorrect data
		DataLen = 0; XData.clear(); YData.clear();
		pDrawFun = &plot::Draw_Null;
	}
	return;

}

void plot::clear(HDC)
{
	//SelectClipRgn(hdc, RGOut);
	SelectObject(hdc, Brush_Bkgnd);
	Rectangle(hdc, OutPos[0], OutPos[1], OutPos[2], OutPos[3]);
}


//
// Set axes font height
void plot::SetFont(usint FH)
{
	if (hdc)
	{
		if (FH <= 0) FH = FontHeight;
		FontHeight = FH;
		AxesFont = CreateFont(FontHeight, 0, 0, 0, 0, 0, 0, 0,
			DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
			DEFAULT_QUALITY, FIXED_PITCH | FF_DONTCARE, _T("Courier New"));

		HFONT oldFont = (HFONT)SelectObject(hdc, AxesFont);
		SIZE Size;
		GetTextExtentPoint32(hdc, _T(" "), 1, &Size);
		FontLenght = (usint)Size.cx;
	}
	else
	{
		FontHeight = 16;
		FontLenght = 8;
	}
}

// Set axes Labels
void plot::SetXLabel(TCHAR *tcLabel)
{
	_tcscpy_s(XLabel, 32, tcLabel);
}
void plot::SetYLabel(TCHAR *tcLabel)
{
	_tcscpy_s(YLabel, 32, tcLabel);
}
//
// calculates ticks position and ticks labels for X or Y axes
void SetScale(double X1, double X2, int &ScalePow,
	usint NumTicks, usint &CurNumTicks,
	double A, double B, usint *TicksPos,
	usint TicksLabelLen,
	TCHAR *TicksLabel)
{
	double dX = X2 - X1, LabelBase = 1, base = 1, cTX, *TX, ATXmin, ATXmax;
	int i, PointPow = 0, bPow, Ticks_D;
	ulint ibase = 1;
	long cITX, *ITX;
	TCHAR *buf, *Label;

	for (i = 0; i<TicksLabelLen; ++i) LabelBase *= 10;

	TX = new double[NumTicks + 2];
	buf = new TCHAR[TicksLabelLen + 2];

	// calc distance between ticks Ticks_D * 10^ScalePow
	ScalePow = 0;
	while (dX >= NumTicks * 10) { dX /= 10.0; X1 /= 10.0; X2 /= 10.0; base *= 10; ++ScalePow; }
	while (dX <  NumTicks) { dX *= 10.0; X1 *= 10.0; X2 *= 10.0; base /= 10; --ScalePow; }

	Ticks_D = (usint)(dX / NumTicks);
	if (dX >(double)(Ticks_D*(NumTicks + 1.5))) ++Ticks_D;

	// position of the first tick
	i = 0;
	if (X1 > 0) cTX = ((ulint)(X1 / Ticks_D) + 1)*Ticks_D;
	else          cTX = -(double)((ulint)(-X1 / Ticks_D)   *Ticks_D);

	// calc ticks value
	while (cTX <= X2)
	{
		TX[i] = cTX;
		TicksPos[i] = (usint)(A*cTX*base + B);
		cTX += Ticks_D;
		++i;
	}
	CurNumTicks = i;

	ATXmin = TX[0], ATXmax = TX[CurNumTicks - 1];
	if (ATXmin < 0) ATXmin = -ATXmin;
	if (ATXmax < 0) ATXmax = -ATXmax;
	if (ATXmax < ATXmin) ATXmax = ATXmin;

	// round ticks value to TicksLabelLen significant digits and save it in long
	base = 1;
	while (ATXmax >= LabelBase) { ATXmax /= 10; base *= 10; ++ScalePow; }
	ITX = new long[NumTicks + 2];
	for (i = 0; i<CurNumTicks; ++i)
		if (TX[i] >= 0) ITX[i] = (long)(TX[i] / base + 0.5);
		else              ITX[i] = (long)(TX[i] / base - 0.5);

		// Point power to represent long XXX by X,XX and PointPow=2
		while (ATXmax >= 10) { ATXmax /= 10; ++PointPow; }

		// new ScalePow is a multiple of 3 
		bPow = ScalePow; ScalePow = ((ScalePow + PointPow) / 3) * 3;
		PointPow = ScalePow - bPow;
		for (i = 0; i< PointPow; ++i) ibase *= 10;
		for (i = 0; i<-PointPow; ++i) ibase *= 10;

		// create te ticks labels
		for (i = 0; i<CurNumTicks; ++i)
		{
			Label = &TicksLabel[i*(TicksLabelLen + 3)];
			if (ITX[i] < 0)
			{
				ITX[i] = -ITX[i];
				_tcscpy(Label, _T("-"));
			}
			else
				_tcscpy(Label, _T(" "));

			if (PointPow>0)
			{
				cITX = ITX[i] / ibase;
				_itot(cITX, buf, 10);
				_tcscat(Label, buf);
				_tcscat(Label, _T("."));
				_itot((ITX[i] - (cITX - 1)*ibase), buf, 10);
				_tcscat(Label, &buf[1]);
			}
			else
			{
				_itot(ITX[i] * ibase, buf, 10);
				_tcscat(Label, buf);
			}
		}

		delete[] TX, ITX, buf;
}
// Set X scale
void plot::SetScaleX(double *cScale)
{
	if (cScale)
	{
		if (cScale[0]>cScale[2]) { Scale[0] = cScale[2]; Scale[2] = cScale[0]; }
		else { Scale[0] = cScale[0]; Scale[2] = cScale[2]; }
	}

	if ((Scale[2] - Scale[0]) == 0)
	{
		if (Scale[0] == 0)
		{
			Scale[0] = -5e-11; Scale[2] = 5e-11;
		}
		else
			Scale[2] = Scale[0] * 1.00001;
	}

	Ax = (double)(InPos[2] - InPos[0]) / (Scale[2] - Scale[0]);
	Bx = (double)InPos[0] - Ax*Scale[0];

	SetScale(Scale[0], Scale[2], ScalePowX,
		NumTicksX, CurNumTicksX,
		Ax, Bx, TicksPosX,
		TicksLabelLen,
		&TicksLabelX[0][0]);
}


// Set X scale
void plot::SetScaleY(double *cScale)
{
	if (cScale)
	{
		if (cScale[1]>cScale[3]) { Scale[1] = cScale[3]; Scale[3] = cScale[1]; }
		else { Scale[1] = cScale[1]; Scale[3] = cScale[3]; }
	}

	Ay = (double)(InPos[1] - InPos[3]) / (Scale[3] - Scale[1]);
	By = (double)InPos[3] - Ay*Scale[1];

	if ((Scale[3] - Scale[1]) == 0)
	{
		if (Scale[1] == 0)
		{
			Scale[1] = -5e-11; Scale[3] = 5e-11;
		}
		else
			Scale[3] = Scale[1] * 1.00001;
	}

	SetScale(Scale[1], Scale[3], ScalePowY,
		NumTicksY, CurNumTicksY,
		Ay, By, TicksPosY,
		TicksLabelLen,
		&TicksLabelY[0][0]);
}

// Set position
void plot::SetPPosX(int *cPPos)
{
	if (cPPos)
	{
		if (cPPos[0]>cPPos[2]) { OutPos[0] = cPPos[2]; OutPos[2] = cPPos[0]; }
		else { OutPos[0] = cPPos[0]; OutPos[2] = cPPos[2]; }
	}
	usint LabelMaxLen = FontLenght*(TicksLabelLen + 2);
	InPos[0] = OutPos[0] + 2 * Indent + LabelMaxLen;
	InPos[2] = OutPos[2] - Indent - (LabelMaxLen - FontLenght) / 2;
	if (InPos[2] <= InPos[0]) InPos[2] = InPos[0] + 1;

	usint buf = (InPos[2] - InPos[0]) / LabelMaxLen - 1;
	NumTicksX = MaxTicksX;
	if (buf<MaxTicksX) NumTicksX = buf;

	RGOut = CreateRectRgn(OutPos[0], OutPos[1], OutPos[2], OutPos[3]);
	RGIn = CreateRectRgn(InPos[0], InPos[1], InPos[2], InPos[3]);

	SetScaleX();
}

void plot::SetPosX(double *cPos)
{
	RECT rect;
	GetClientRect(hWnd, &rect);

	if (cPos)
	{
		if (cPos[0]>cPos[2]) { Pos[0] = cPos[2]; Pos[2] = cPos[0]; }
		else { Pos[0] = cPos[0]; Pos[2] = cPos[2]; }
		if (Pos[0]<0) Pos[0] = 0.0;
		if (Pos[2]>1) Pos[0] = 1.0;
	}

	int lenght = (int)(rect.right - rect.left);
	OutPos[0] = (int)(lenght*Pos[0]);
	OutPos[2] = (int)(lenght*Pos[2]);

	SetPPosX();
}


void plot::SetPPosY(int *cPPos)
{
	if (cPPos)
	{
		if (cPPos[1]>cPPos[3]) { OutPos[1] = cPPos[3]; OutPos[3] = cPPos[1]; }
		else { OutPos[1] = cPPos[1]; OutPos[3] = cPPos[3]; }
	}
	InPos[1] = OutPos[1] + 2 * Indent + FontHeight;
	InPos[3] = OutPos[3] - 2 * Indent - 2 * FontHeight;
	if (InPos[3] <= InPos[1]) InPos[3] = InPos[1] + 1;

	usint buf = (InPos[3] - InPos[1]) / (2 * FontHeight) + 1;
	NumTicksY = MaxTicksY;
	if (buf<MaxTicksY) NumTicksY = buf;

	RGOut = CreateRectRgn(OutPos[0], OutPos[1], OutPos[2], OutPos[3]);
	RGIn = CreateRectRgn(InPos[0], InPos[1], InPos[2], InPos[3]);

	SetScaleY();
}

void plot::SetPosY(double *cPos)
{
	RECT rect;
	GetClientRect(hWnd, &rect);

	if (cPos)
	{
		if (cPos[1]>cPos[3]) { Pos[1] = cPos[3]; Pos[3] = cPos[1]; }
		else { Pos[1] = cPos[1]; Pos[3] = cPos[3]; }
		if (Pos[1]<0) Pos[1] = 0.0;
		if (Pos[3]>1) Pos[3] = 1.0;
	}
	int height = (int)(rect.bottom - rect.top);
	OutPos[1] = (int)(height*Pos[1]);
	OutPos[3] = (int)(height*Pos[3]);

	SetPPosY();
}


//
// Draws plot
void plot::Draw()
{
	if (!hdc) return;

	// Background
	//SelectClipRgn(hdc, RGOut);
	//SelectObject(hdc, Brush_Bkgnd);
	//Rectangle(hdc, OutPos[0], OutPos[1], OutPos[2], OutPos[3]);

	// Line
	SelectObject(hdc, Pen_Line);
	SelectClipRgn(hdc, RGIn);

	BeginPath(hdc);

	(this->*pDrawFun)();
	EndPath(hdc);
	StrokePath(hdc);


	// Axes
	SelectClipRgn(hdc, RGOut);
	SelectObject(hdc, Pen_Axes);

	BeginPath(hdc);
	MoveToEx(hdc, InPos[0], InPos[1], NULL);
	LineTo(hdc, InPos[0], InPos[3]);
	LineTo(hdc, InPos[2], InPos[3]);
	LineTo(hdc, InPos[2], InPos[1]);
	LineTo(hdc, InPos[0], InPos[1]);

	// ticks X
	int i, TicksStr1 = InPos[1], TicksEnd1 = TicksStr1 + TicksLen, TicksStr2 = InPos[3], TicksEnd2 = TicksStr2 - TicksLen,
		HS = Indent, Len, SLen;
	for (i = 0; i<CurNumTicksX; ++i)
	{
		MoveToEx(hdc, TicksPosX[i], TicksStr1, NULL);
		LineTo(hdc, TicksPosX[i], TicksEnd1);
		MoveToEx(hdc, TicksPosX[i], TicksStr2, NULL);
		LineTo(hdc, TicksPosX[i], TicksEnd2);
	}

	// ticks Y
	TicksStr1 = InPos[0]; TicksEnd1 = TicksStr1 + TicksLen; TicksStr2 = InPos[2]; TicksEnd2 = TicksStr2 - TicksLen;
	HS = FontHeight / 2;
	for (i = 0; i<CurNumTicksY; ++i)
	{
		MoveToEx(hdc, TicksStr1, TicksPosY[i], NULL);
		LineTo(hdc, TicksEnd1, TicksPosY[i]);
		MoveToEx(hdc, TicksStr2, TicksPosY[i], NULL);
		LineTo(hdc, TicksEnd2, TicksPosY[i]);
	}
	EndPath(hdc);
	StrokePath(hdc);

	// set font
	HFONT oldFont = (HFONT)SelectObject(hdc, AxesFont);

	// X ticks label
	TicksStr2 = InPos[3]; HS = Indent;
	for (i = 0; i<CurNumTicksX; ++i) {
		Len = _tcsclen(TicksLabelX[i]); SLen = (Len + 1)*FontLenght;
		TextOut(hdc, TicksPosX[i] - SLen / 2, TicksStr2 + HS, TicksLabelX[i], Len);
	}

	// X axes label
	if (!ScalePowX) {
		Len = _tcsclen(XLabel); SLen = Len*FontLenght;
		TextOut(hdc, InPos[2] - SLen, TicksStr2 + HS + FontHeight, XLabel, Len);
	}
	else {
		TCHAR Label[48], Pow[4];
		if (ScalePowX > 0) _tcscpy(Label, _T("1E+"));
		else                 _tcscpy(Label, _T("1E"));
		_itot(ScalePowX, Pow, 10);
		_tcscat(Label, Pow); _tcscat(Label, _T(" ")); _tcscat(Label, XLabel);

		Len = _tcsclen(Label); SLen = Len*FontLenght;
		TextOut(hdc, InPos[2] - SLen, TicksStr2 + HS + FontHeight, Label, Len);
	}

	// Y ticks label
	TicksStr1 = InPos[0]; HS = FontHeight / 2;
	for (i = 0; i<CurNumTicksY; ++i) {
		Len = _tcsclen(TicksLabelY[i]); SLen = Len*FontLenght;
		TextOut(hdc, TicksStr1 - Indent - SLen, TicksPosY[i] - HS, TicksLabelY[i], Len);
	}

	// Y axes label
	if (!ScalePowY) {
		TextOut(hdc, TicksStr1, InPos[1] - Indent - FontHeight, YLabel, _tcsclen(YLabel));
	}
	else {
		TCHAR Label[40], Pow[4];
		if (ScalePowY > 0) _tcscpy(Label, _T("1E+"));
		else                 _tcscpy(Label, _T("1E"));
		_itot(ScalePowY, Pow, 10);
		_tcscat(Label, Pow); _tcscat(Label, _T(" "));
		Len = _tcsclen(Label); SLen = Len*FontLenght;
		_tcscat(Label, YLabel);

		TextOut(hdc, TicksStr1 - SLen, InPos[1] - Indent - FontHeight, Label, _tcsclen(Label));
	}

	SelectClipRgn(hdc, NULL);  // reset draw region
	SelectObject(hdc, oldFont); // reset font
}


// Draw function for linear X
int plot::Draw_XLinear()
{
	double dX = XData[1] - XData[0], Xend = XData[0] + (DataLen - 1)*dX, cX;
	ulint n, n1 = 0, n2 = DataLen;

	if (XData[0] < Scale[2] && Xend > Scale[0]) // data in scale
	{
		if (XData[0] < Scale[0]) n1 = (ulint)((Scale[0] - XData[0]) / dX);   // first point to draw
		if (Xend     > Scale[2]) n2 = (ulint)((Scale[2] - XData[0]) / dX) + 2; // last point to draw
	}
	else return 0;

	if (n2 - n1 > 30 * (ulint)(InPos[2] - InPos[0])) // draw with decimation
	{
		cX = Ax*(XData[0] + n1*dX) + Bx;
		int PX = (int)cX;

		double adX = Ax*dX, Ymax = YData[n1], Ymin = Ymax;
		cX -= 1;

		n = n1 + 1;
		while (n < n2)
		{
			cX += adX;
			while (cX<PX)
			{
				if (n == DataLen - 1) break;
				if (YData[n]>Ymax) Ymax = YData[n];
				if (YData[n]<Ymin) Ymin = YData[n];
				cX += adX;
				++n;
			}
			if ((ulint)((Ymax - Ymin)*Ay))
			{
				MoveToEx(hdc, PX, (int)(Ay*Ymin + By), NULL);
				LineTo(hdc, PX, (int)(Ay*Ymax + By));
			}
			MoveToEx(hdc, PX, (int)(Ay*YData[n - 1] + By), NULL);
			++PX;
			LineTo(hdc, PX, (int)(Ay*YData[n] + By));

			Ymax = YData[n], Ymin = Ymax;

			++n;
		}
	}
	else // draw without decimation
	{
		cX = XData[0] + n1*dX;
		MoveToEx(hdc, (int)(Ax*cX + Bx), (int)(Ay*YData[n1] + By), NULL);
		for (n = n1 + 1; n<n2; ++n) { cX += dX; LineTo(hdc, (int)(Ax*cX + Bx), (int)(Ay*YData[n] + By)); }
	}

	return 1;
}
// Draw function for linear Y
int plot::Draw_YLinear()
{
	double dY = YData[1] - YData[0], Yend = YData[0] + (DataLen - 1)*dY, cY;
	ulint n, n1 = 0, n2 = DataLen;

	if (YData[0] < Scale[3] && Yend > Scale[1]) // data in scale
	{
		if (YData[0] < Scale[1]) n1 = (ulint)((Scale[1] - YData[0]) / dY);   // first point to draw
		if (Yend     > Scale[3]) n2 = (ulint)((Scale[3] - YData[0]) / dY) + 2; // last point to draw
	}
	else return 0;

	if (n2 - n1 > 30 * (ulint)(InPos[3] - InPos[1])) // draw with decimation
	{
		cY = Ay*(YData[0] + n1*dY) + By;
		int PY = (int)cY;

		double adY = Ay*dY, Xmax = XData[n1], Xmin = Xmax;
		cY -= 1;

		n = n1 + 1;
		while (n < n2)
		{
			cY += adY;
			while (cY<PY)
			{
				if (n == DataLen - 1) break;
				if (XData[n]>Xmax) Xmax = XData[n];
				if (XData[n]<Xmin) Xmin = XData[n];
				cY += adY;
				++n;
			}
			if ((ulint)((Xmax - Xmin)*Ax))
			{
				MoveToEx(hdc, (int)(Ax*Xmin + Bx), PY, NULL);
				LineTo(hdc, (int)(Ax*Xmax + Bx), PY);
			}
			MoveToEx(hdc, (int)(Ax*XData[n - 1] + Bx), PY, NULL);
			++PY;
			LineTo(hdc, (int)(Ax*XData[n] + Bx), PY);

			Xmax = XData[n]; Xmin = Xmax;

			++n;
		}
	}
	else // draw without decimation
	{
		cY = YData[0] + n1*dY;
		MoveToEx(hdc, (int)(Ax*XData[n1] + Bx), (int)(Ay*cY + By), NULL);
		for (n = n1 + 1; n<n2; ++n) { cY += dY; LineTo(hdc, (int)(Ax*XData[n] + Bx), (int)(Ay*cY + By)); }
	}

	return 1;
}
// Binary search in sorted array
ulint BinSearch(std::vector<double> V, ulint N, double S)
{
	N = V.size();
	ulint n_mid, n_first = 0, n_last = N - 1;

	if (V[0]  >S) return 1;
	if (V[N - 1]<S) return N - 1;

	while (n_first < n_last)
	{
		n_mid = n_first + (n_last - n_first) / 2;
		if (V[n_mid] <= S) n_first = n_mid + 1;
		else               n_last = n_mid;
	}
	return n_first;
}
// Draw function for X sorted
int plot::Draw_XSorted()
{
	double cX;
	ulint n, n1 = 0, n2 = XData.size();
	DataLen = XData.size();

	if (XData[0] < Scale[2] && XData[DataLen - 1] > Scale[0]) // data in scale
	{
		n1 = BinSearch(XData, DataLen, Scale[0]) - 1; // first point to draw
		n2 = BinSearch(XData, DataLen, Scale[2]) + 1; // last point to draw
	}
	else return 0;

	if (n2 - n1 > 100 * (ulint)(InPos[2] - InPos[0])) // draw with decimation
	{
		cX = Ax*XData[n1] + Bx;
		int PX = (int)cX;
		double Ymax = YData[n1], Ymin = Ymax;

		n = n1 + 1;
		while (n < n2)
		{
			cX = Ax*XData[n] + Bx;
			while (cX<PX + 1)
			{
				if (n == DataLen - 1) break;
				if (YData[n]>Ymax) Ymax = YData[n];
				if (YData[n]<Ymin) Ymin = YData[n];
				++n;
				cX = Ax*XData[n] + Bx;
			}
			if ((ulint)((Ymax - Ymin)*Ay))
			{
				MoveToEx(hdc, PX, (int)(Ay*Ymin + By), NULL);
				LineTo(hdc, PX, (int)(Ay*Ymax + By));
			}
			MoveToEx(hdc, PX, (int)(Ay*YData[n - 1] + By), NULL);
			PX = (ulint)cX;
			LineTo(hdc, PX, (int)(Ay*YData[n] + By));

			Ymax = YData[n], Ymin = Ymax;

			++n;
		}
	}
	else // draw without decimation
	{
		MoveToEx(hdc, (int)(Ax*XData[n1] + Bx), (int)(Ay*YData[n1] + By), NULL);
		for (n = n1 + 1; n<n2; ++n) { LineTo(hdc, (int)(Ax*XData[n] + Bx), (int)(Ay*YData[n] + By)); }
	}

	return 1;
}
// Draw function for Y sorted
int plot::Draw_YSorted()
{
	double cY;
	ulint n, n1 = 0, n2 = DataLen;
	DataLen = YData.size();
	if (YData[0] < Scale[3] && YData[DataLen - 1] > Scale[1])
	{
		n1 = BinSearch(YData, DataLen, Scale[1]) - 1;
		n2 = BinSearch(YData, DataLen, Scale[3]) + 1;
	}
	else return 0;

	if (n2 - n1 > 100 * (ulint)(InPos[3] - InPos[1]))
	{
		cY = Ay*YData[n1] + By;
		int PY = (int)cY;
		double Xmax = XData[n1], Xmin = Xmax;

		n = n1 + 1;
		while (n < n2)
		{
			cY = Ay*YData[n] + By;
			while (cY<PY + 1)
			{
				if (n == DataLen - 1) break;
				if (XData[n]>Xmax) Xmax = XData[n];
				if (XData[n]<Xmin) Xmin = XData[n];
				++n;
				cY = Ay*YData[n] + By;
			}
			if ((ulint)((Xmax - Xmin)*Ax))
			{
				MoveToEx(hdc, (int)(Ax*Xmin + Bx), PY, NULL);
				LineTo(hdc, (int)(Ax*Xmax + Bx), PY);
			}
			MoveToEx(hdc, (int)(Ax*XData[n - 1] + Bx), PY, NULL);
			PY = (ulint)cY;
			LineTo(hdc, (int)(Ax*XData[n] + Bx), PY);

			Xmax = XData[n], Xmin = Xmax;

			++n;
		}
	}
	else // draw without decimation
	{
		MoveToEx(hdc, (int)(Ax*XData[n1] + Bx), (int)(Ay*YData[n1] + By), NULL);
		for (n = n1 + 1; n<n2; ++n) { LineTo(hdc, (int)(Ax*XData[n] + Bx), (int)(Ay*YData[n] + By)); }
	}

	return 1;
}
// Draw function for data len = 1
int plot::Draw_Point()
{
	MoveToEx(hdc, (int)(Ax*XData[0] + Bx), (int)(Ay*YData[0] + By), NULL);
	LineTo(hdc, (int)(Ax*XData[0] + Bx), (int)(Ay*YData[0] + By));
	return 1;
}
// Draw function all cases except in cases when X or Y is given as Linear
int plot::Draw_Full()
{
	bool flag = 0;

	for (ulint n = 1; n<DataLen; ++n)
	{
		if (flag) // previous point in scale
		{
			LineTo(hdc, (int)(Ax*XData[n] + Bx), (int)(Ay*YData[n] + By));

			if (XData[n] < Scale[0] || XData[n] > Scale[2] ||
				YData[n] < Scale[1] || YData[n] > Scale[3])
			{
				flag = 0;
			} // current point out of scale
		}
		else // previous point out of scale
		{
			if (XData[n] >= Scale[0] || XData[n] <= Scale[2] || // current point in scale
				YData[n] >= Scale[1] || YData[n] <= Scale[3])
			{
				MoveToEx(hdc, (int)(Ax*XData[n - 1] + Bx), (int)(Ay*YData[n - 1] + By), NULL);
				LineTo(hdc, (int)(Ax*XData[n] + Bx), (int)(Ay*YData[n] + By));
				flag = 1;
			}
		}
	}

	return 1;
}
