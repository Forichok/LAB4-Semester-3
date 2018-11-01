#include <vector>
typedef unsigned short int usint;
typedef unsigned long int ulint;

struct info
{
	double *x, *y;
	int xcount, ycount;
};
class plot
{

public:
	plot();
	~plot();
	static	std::array<std::vector<double>, 2> parabola(int n);
	static std::array<std::vector<double>, 2> line(int n);
	static std::array<std::vector<double>, 2> _line(int n);
	static std::array<std::vector<double>, 2> random(int n);
	//*************************
	// Set Functions
	//*************************

	// chWnd  - handle to window 
	// cPos   - relative position in the window [x0, y0, x1, y1] ( 0<= x,y <= 1 )
	// cScale - axes scale [x0, y0, x1, y1]
	// cXData, cYData - pointer to X, Y data
	// cXLen, cYLen - lenght of X, Y data, cXLen=cYLen or cXLen=2, cYlen>1 or cYLen=2, cXlen>1
	//                if cX(Y)Len=2 - X(Y) is linear, dX(Y)=cX(Y)Data[1]-cX(Y)Data[0] must be >=0,
	//                for dX(Y)<0 drawing function is not realized  
	void Set(HWND chWnd, std::vector<double> cXData, std::vector<double> cYData, double*cScale = 0, double *cPos = 0);
	// some as Set
	void SetData(std::vector<double> cXData, std::vector<double> cYData);
	void clear(HDC);
	// Set position in the window in pixels [x0, ..., x1, ...]
	void SetPPosX(int *cPPos = 0);
	// Set realtive position in the window  [x0, ..., x1, ...]
	void SetPosX(double *cPos = 0);
	void SetPPosY(int *cPPos = 0);  // Some for Y  [..., y0, ..., y1]
	void SetPosY(double *cPos = 0); // Some for Y  [..., y0, ..., y1]

	void SetScaleX(double *cScale = 0); // Set X scale [x0, ..., x1, ...]
	void SetScaleY(double *cScale = 0); // Set Y scale [..., y0, ..., y1]

	void Draw(); // Draws X, Y plot

	void SetXLabel(TCHAR *tcLabel); // Set X axes label maxlen 30
	void SetYLabel(TCHAR *tcLabel); // Set Y axes label maxlen 30

	void SetFont(usint FH = 0); // Set axes font height

								//************************
								// Get Functions
								//************************

								// get axes scale [x0, y0, x1, y1]
	void GetScale(double *Sca) { Sca[0] = Scale[0]; Sca[1] = Scale[1]; Sca[2] = Scale[2]; Sca[3] = Scale[3]; }

protected:
	// const
	static const usint MaxTicksX = 10, MaxTicksY = 10,
		TicksLen = 5,
		TicksLabelLen = 5;

	// Window
	HWND hWnd;
	HDC hdc;

	// Position
	double Pos[4];
	int OutPos[4], InPos[4];
	HRGN RGOut, RGIn;

	// Scale
	double Scale[4];

	// Data
	ulint DataLen;
	std::vector<double>XData, YData;
	double  Ax, Bx, Ay, By;
	int (plot::*pDrawFun)();

	// Pens and Axes
	HPEN Pen_Axes, Pen_Line;
	HBRUSH Brush_Bkgnd;
	HFONT AxesFont;

	usint Indent, FontHeight, FontLenght;

	// Axes Labels
	TCHAR XLabel[32], YLabel[32];

	usint NumTicksX, NumTicksY,
		CurNumTicksX, CurNumTicksY;
	int ScalePowX, ScalePowY;
	usint TicksPosX[MaxTicksX + 2], TicksPosY[MaxTicksY + 2];

	TCHAR TicksLabelX[MaxTicksX + 2][TicksLabelLen + 3],
		TicksLabelY[MaxTicksY + 2][TicksLabelLen + 3];

	// Draw functions
	int Draw_Null() { return 1; } // Error
	int Draw_Point();           // DataLen=1
	int Draw_XLinear();         // X Linear dX>=0
	int Draw_YLinear();			// Y Linear dY>=0
	int Draw_XSorted();			// X Sorted X[i]-X[i-1] >= 0
	int Draw_YSorted();         // Y Sorted Y[i]-Y[i-1] >= 0
	int Draw_Full();            // For all cases except when X or Y is given as Linear 
};