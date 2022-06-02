
// ReversiDlg.cpp: 实现文件
//

#include "pch.h"
#include "framework.h"
#include "Reversi.h"
#include "ReversiDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()



// CReversiDlg 对话框

CReversiDlg::CReversiDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_REVERSI_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CReversiDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

CReversiDlg::ChessColor CReversiDlg::GetChessBoardColor(int nx, int ny)
{
	//计算机中数组下标是先行后列（先y轴后x轴）
	//与我们在平面直角坐标系表达位置的习惯先列后行（先x轴后y轴）不符
	//所以先创建一个函数转换这个差异。
	return ChessBoard[ny][nx];
}

void CReversiDlg::SetChessBoardColor(int nx, int ny, ChessColor color)
{
	//计算机中数组下标是先行后列（先y轴后x轴）
	//与我们在平面直角坐标系表达位置的习惯先列后行（先x轴后y轴）不符
	//所以先创建一个函数转换这个差异。
	ChessBoard[ny][nx] = color;
}

void CReversiDlg::DrawOneChess(int nx, int ny)
{
	ChessColor color = GetChessBoardColor(nx, ny);
	CDC* dc = this->GetDC();
	if (color == White)
	{
		CBrush brush_w(RGB(255, 255, 255));
		const CPoint o(100 * nx + 100, 100 * ny + 100);//圆心
		dc->SelectObject(brush_w);
		dc->Ellipse(o.x - 40, o.y - 40, o.x + 40, o.y + 40);
	}
	else if (color == Black)
	{
		CBrush brush_b(RGB(0, 0, 0));
		const CPoint o(100 * nx + 100, 100 * ny + 100);//圆心
		dc->SelectObject(brush_b);
		dc->Ellipse(o.x - 40, o.y - 40, o.x + 40, o.y + 40);
	}
}

void CReversiDlg::DrawAllChess()
{
	CDC* dc = this->GetDC();
	for (int nx = 0; nx < SIZE; nx++)
	{
		for (int ny = 0; ny < SIZE; ny++)
		{
			DrawOneChess(nx, ny);
		}
	}
}

bool CReversiDlg::RefreshChessBoard(int x,int y)
{
	if (GetChessBoardColor(x, y) != None)//如果已有棋子
		return false;
	if (!ChessPlaceableAt(x, y, CurrentColor))
		return false;
	SetChessBoardColor(x, y, CurrentColor);
	for (int direction = 0; direction < 8; direction++)
	{
		CPoint pt = GetNextSameColorChessPos(x, y, direction,CurrentColor);
		if (pt.x == SIZE || pt.y == SIZE)
			continue;
		switch (direction)//注意：Windows系统和数组坐标以左上角为原点，所以上下要相反
		{
		case 0://左
			for (int nx = pt.x + 1; nx < x; nx++)//注意符号
				SetChessBoardColor(nx, y, CurrentColor);
			break;
		case 1://左上
			for (int nx = pt.x + 1, ny = pt.y + 1; nx < x; nx++, ny++)//注意符号
				SetChessBoardColor(nx, ny, CurrentColor);
			break;
		case 2://上
			for (int ny = pt.y + 1; ny < y; ny++)//注意符号
				SetChessBoardColor(x, ny, CurrentColor);
			break;
		case 3://右上
			for (int nx = pt.x - 1, ny = pt.y + 1; nx > x; nx--, ny++)//注意符号
				SetChessBoardColor(nx, ny, CurrentColor);
			break;
		case 4://右
			for (int nx = pt.x - 1; nx > x; nx--)//注意符号
				SetChessBoardColor(nx, y, CurrentColor);
			break;
		case 5://右下
			for (int nx = pt.x - 1, ny = pt.y - 1; nx > x; nx--, ny--)//注意符号
				SetChessBoardColor(nx, ny, CurrentColor);
			break;
		case 6://下
			for (int ny = pt.y - 1; ny > y; ny--)//注意符号
				SetChessBoardColor(x, ny, CurrentColor);
			break;
		case 7://左下
			for (int nx = pt.x + 1, ny = pt.y - 1; nx < x; nx++, ny--)//注意符号
				SetChessBoardColor(nx, ny, CurrentColor);
			break;
		}
	}
	return true;
}

bool CReversiDlg::RefreshCurrentColor()
{
	//黑白棋不一定是轮流下，如果一方无棋可下则另一方一直下，直到那一方可以下
	bool b1 = false, b2 = false;
	for (int i = 0; i < SIZE; i++)
	{
		for (int j = 0; j < SIZE; j++)
		{
			if ((GetChessBoardColor(i, j) == None) && (ChessPlaceableAt(i, j, -CurrentColor)))
			{
				//对手可落子
				CurrentColor = (ChessColor)(-CurrentColor);
				b1 = true;
				break;
			}
		}
		if (b1)
			break;
	}
	if (!b1)//对手无法落子
	{
		for (int i = 0; i < SIZE; i++)
		{
			for (int j = 0; j < SIZE; j++)
			{
				if ((GetChessBoardColor(i, j) == None) && (ChessPlaceableAt(i, j, CurrentColor)))
				{
					//自己可继续落子
					b2 = true;
					break;
				}
			}
			if (b2)
				break;
		}
	}
	return (b1 || b2);
}

void CReversiDlg::RefreshChessConut()
{
	black = 0;
	white = 0;
	for (int i = 0; i < SIZE; i++)
	{
		for (int j = 0; j < SIZE; j++)
		{
			int color = GetChessBoardColor(i, j);
			if (color == White)
				white++;
			else if (color == Black)
				black++;
		}
	}
	CString str;
	str.Format(L"黑棋:%d个\t白棋:%d个", black, white);
	GetDlgItem(IDC_CHESSCOUNT)->SetWindowTextW(str);
}

void CReversiDlg::CleanChessBoard()
{
	for (int i = 0; i < SIZE; i++)
	{
		for (int j = 0; j < SIZE; j++)
		{
			ChessBoard[i][j] = None;
		}
	}
	Invalidate();
}

void CReversiDlg::EndGame()
{
	CleanChessBoard();
	IsPlaying = false;
	GetDlgItem(IDC_START)->SetWindowTextW(L"开始游戏");
	GetDlgItem(IDC_ENDGAME)->EnableWindow(FALSE);
	GetDlgItem(IDC_PROMPT)->EnableWindow(FALSE);
	GetDlgItem(IDC_CHESSCOUNT)->SetWindowTextW(L"双人黑白棋");
}

CPoint CReversiDlg::GetNextSameColorChessPos(int nx, int ny, int direction, int color)
{
	int x = nx, y = ny;
	switch (direction)//注意：Windows系统和数组坐标以左上角为原点，所以上下要相反
	{
	case 0://左
		while (1)
		{
			x--;
			if (x < 0)//未找到
				return CPoint(SIZE, SIZE);
			if (GetChessBoardColor(x, y) == None)//中间有空格
				return CPoint(SIZE, SIZE);
			if (GetChessBoardColor(x, y) == color)
				return CPoint(x, y);
		}
		//函数肯定会返回，无需break
	case 1://左上
		while (1)
		{
			x--;
			y--;
			if (x < 0 || y < 0)//未找到
				return CPoint(SIZE, SIZE);
			if (GetChessBoardColor(x, y) == None)//中间有空格
				return CPoint(SIZE, SIZE);
			if (GetChessBoardColor(x, y) == color)
				return CPoint(x, y);
		}
	case 2://上
		while (1)
		{
			y--;
			if (y < 0)//未找到
				return CPoint(SIZE, SIZE);
			if (GetChessBoardColor(x, y) == None)//中间有空格
				return CPoint(SIZE, SIZE);
			if (GetChessBoardColor(x, y) == color)
				return CPoint(x, y);
		}
	case 3://右上
		while (1)
		{
			x++;
			y--;
			if (x >= SIZE || y < 0)//未找到
				return CPoint(SIZE, SIZE);
			if (GetChessBoardColor(x, y) == None)//中间有空格
				return CPoint(SIZE, SIZE);
			if (GetChessBoardColor(x, y) == color)
				return CPoint(x, y);
		}
	case 4://右
		while (1)
		{
			x++;
			if (x >= SIZE)//未找到
				return CPoint(SIZE, SIZE);
			if (GetChessBoardColor(x, y) == None)//中间有空格
				return CPoint(SIZE, SIZE);
			if (GetChessBoardColor(x, y) == color)
				return CPoint(x, y);
		}
	case 5://右下
		while (1)
		{
			x++;
			y++;
			if (x >= SIZE || y >= SIZE)//未找到
				return CPoint(SIZE, SIZE);
			if (GetChessBoardColor(x, y) == None)//中间有空格
				return CPoint(SIZE, SIZE);
			if (GetChessBoardColor(x, y) == color)
				return CPoint(x, y);
		}
	case 6://下
		while (1)
		{
			y++;
			if (y >= SIZE)//未找到
				return CPoint(SIZE, SIZE);
			if (GetChessBoardColor(x, y) == None)//中间有空格
				return CPoint(SIZE, SIZE);
			if (GetChessBoardColor(x, y) == color)
				return CPoint(x, y);
		}
	case 7://左下
		while (1)
		{
			x--;
			y++;
			if (x < 0 || y >= SIZE)//未找到
				return CPoint(SIZE, SIZE);
			if (GetChessBoardColor(x, y) == None)//中间有空格
				return CPoint(SIZE, SIZE);
			if (GetChessBoardColor(x, y) == color)
				return CPoint(x, y);
		}
	default:
		return CPoint(SIZE, SIZE);
	}
}

bool CReversiDlg::ChessPlaceableAt(int x, int y, int color)
{
	int sum = 0;
	for (int direction = 0; direction < 8; direction++)
	{
		int count;
		switch (direction)
		{
		case 2://上
		case 6://下
		{
			int pos = GetNextSameColorChessPos(x, y, direction, color).y;
			count = abs(pos - y) - 1;
			//计算y轴坐标的差
			if (pos == SIZE)//没找到
				count = 0;
		}
		break;
		default://其它方向，计算x轴坐标的差
		{
			int pos = GetNextSameColorChessPos(x, y, direction, color).x;
			count = abs(pos - x) - 1;
			//计算y轴坐标的差
			if (pos == SIZE)//没找到
				count = 0;
		}
		}
		sum += count;
	}
	return sum;//return (sum > 0);
}

BEGIN_MESSAGE_MAP(CReversiDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_QUIT, &CReversiDlg::OnBnClickedQuit)
	ON_WM_CLOSE()
	ON_WM_SETCURSOR()
	ON_BN_CLICKED(IDC_START, &CReversiDlg::OnBnClickedStart)
	ON_BN_CLICKED(IDC_ENDGAME, &CReversiDlg::OnBnClickedEndgame)
	ON_WM_LBUTTONUP()
	ON_BN_CLICKED(IDC_PROMPT, &CReversiDlg::OnBnClickedPrompt)
END_MESSAGE_MAP()


// CReversiDlg 消息处理程序

BOOL CReversiDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	//初始化棋盘
	IsPlaying = false;
	for (int i = 0; i < SIZE; i++)
	{
		for (int j = 0; j < SIZE; j++)
		{
			ChessBoard[i][j] = None;
		}
	}

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CReversiDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CReversiDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CPaintDC dc(this);
		CPen pen(PS_SOLID, 2, RGB(0, 0, 0));
		dc.SelectObject(pen);
		for (int i = 0; i < (SIZE + 1)/*黑白棋下在格子里，要多画一条线*/; i++)
		{
			dc.MoveTo(50, 50 + i * 100);
			dc.LineTo(850, 50 + i * 100);
		}//绘制棋盘横线
		for (int i = 0; i < (SIZE + 1); i++)
		{
			dc.MoveTo(50 + i * 100, 50);
			dc.LineTo(50 + i * 100, 850);
		}//绘制棋盘竖线
		DrawAllChess();
	}
}

void CReversiDlg::CleanPrompt()
{
	for (int i = 0; i < SIZE; i++)
	{
		for (int j = 0; j < SIZE; j++)
		{
			if ((GetChessBoardColor(i, j) == None))
			{
				CDC* dc = this->GetDC();
				CBrush brush_w(RGB(240, 240, 240));
				CPen pen(PS_SOLID, 2, RGB(240, 240, 240));
				dc->SelectObject(pen);
				const CPoint o(100 * i + 100, 100 * j + 100);//圆心
				dc->SelectObject(brush_w);
				dc->Ellipse(o.x - 11, o.y - 11, o.x + 11, o.y + 11);
			}
		}
	}
}


//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CReversiDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CReversiDlg::OnBnClickedQuit()
{
	if (!IsPlaying || MessageBoxW(L"正在游戏中，确定要退出吗？", L"双人黑白棋", MB_YESNO | MB_ICONQUESTION) == IDYES)
		SendMessage(WM_CLOSE);
}


void CReversiDlg::OnClose()
{
	if (!IsPlaying || MessageBoxW(L"正在游戏中，确定要退出吗？", L"双人黑白棋", MB_YESNO | MB_ICONQUESTION) == IDYES)
		CDialogEx::OnClose();
}


BOOL CReversiDlg::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	POINT point;
	GetCursorPos(&point);
	ScreenToClient(&point);
	if (!IsPlaying || point.x < 50 || point.x>850 || point.y < 50 || point.y>850)
		return CDialogEx::OnSetCursor(pWnd, nHitTest, message);
	if (CurrentColor == Black)
		SetCursor(LoadCursorW(AfxGetApp()->m_hInstance, MAKEINTRESOURCE(IDC_BLACK)));
	else
		SetCursor(LoadCursorW(AfxGetApp()->m_hInstance, MAKEINTRESOURCE(IDC_WHITE)));
	return TRUE;
}


void CReversiDlg::OnBnClickedStart()
{
	if (IsPlaying && MessageBoxW(L"确定要重玩吗？", L"双人黑白棋", MB_YESNO | MB_ICONQUESTION) == IDNO)
		return;
	int side = MessageBoxW(L"选边：是 - 黑，否 - 白", L"双人黑白棋", MB_YESNO | MB_ICONQUESTION);
	side = (side == IDYES) ? 1 : -1;
	CString str;
	str.Format(L"正在连接服务器...");
	GetDlgItem(IDC_CHESSCOUNT)->SetWindowTextW(str);
	int ret = new_game(side, msg, message_limit);
	if (ret != 200) {
		if (ret == -1) {
			str.Format(L"无法连接到服务器");
			GetDlgItem(IDC_CHESSCOUNT)->SetWindowTextW(str);
		}
		MessageBoxW(CA2W(msg), L"双人黑白棋", MB_OK | MB_ICONERROR);
		str.Format(CA2W(msg));
		GetDlgItem(IDC_CHESSCOUNT)->SetWindowTextW(str);
		return;
	}
	GetDlgItem(IDC_START)->SetWindowTextW(L"重玩");
	CleanChessBoard();
	IsPlaying = true;
	//从服务器获取初始棋盘
	//黑白棋初始有两黑两白四个棋子
	ret = get_board((int*)ChessBoard, (int*)&CurrentColor, &IsPlaying, nullptr, msg, message_limit);
	IsPlaying = !IsPlaying;
	if (ret != 200) {
		str.Format(L"无法获取棋盘！");
		MessageBoxW(CA2W(msg), L"双人黑白棋", MB_OK | MB_ICONERROR);
		GetDlgItem(IDC_CHESSCOUNT)->SetWindowTextW(str);
	}
	GetDlgItem(IDC_ENDGAME)->EnableWindow(TRUE);
	GetDlgItem(IDC_PROMPT)->EnableWindow(TRUE);
	DrawAllChess();
	RefreshChessConut();
}


void CReversiDlg::OnBnClickedEndgame()
{
	if (MessageBoxW(L"确定要结束本局吗？", L"双人黑白棋", MB_YESNO | MB_ICONQUESTION) == IDYES)
		EndGame();
}


void CReversiDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
	//将鼠标坐标转为数组下标
	if (!IsPlaying || point.x < 50 || point.x>850 || point.y < 50 || point.y>850)
		return;
	int x = int(round((point.x - 50 - 50) / 100.0));
	int y = int(round((point.y - 50 - 50) / 100.0));
	//如果落子成功
	if (!RefreshChessBoard(x, y))
	{
		return;
	}
	IsPlaying = RefreshCurrentColor();
	CleanPrompt();
	DrawAllChess();
	RefreshChessConut();
	SendMessage(WM_SETCURSOR);
	//以上为放置棋子
	CString str;
	int ret = place_chess(y * 8 + x, msg, message_limit);
	if (ret != 200) {
		if (ret == -1) {
			str.Format(L"下棋失败！");
			GetDlgItem(IDC_CHESSCOUNT)->SetWindowTextW(str);
		}
		MessageBoxW(CA2W(msg), L"双人黑白棋", MB_OK | MB_ICONERROR);
		str.Format(CA2W(msg));
		GetDlgItem(IDC_CHESSCOUNT)->SetWindowTextW(str);
	}
	ret = get_board((int*)ChessBoard, (int*)&CurrentColor, &IsPlaying, nullptr, msg, message_limit);
	IsPlaying = !IsPlaying;
	if (ret != 200) {
		str.Format(L"无法获取棋盘！");
		MessageBoxW(CA2W(msg), L"双人黑白棋", MB_OK | MB_ICONERROR);
		GetDlgItem(IDC_CHESSCOUNT)->SetWindowTextW(str);
	}
	DrawAllChess();
	RefreshChessConut();
	SendMessage(WM_SETCURSOR);

	if (!IsPlaying)//双方都无棋可下
	{
		if (white > black)
			MessageBoxW(L"白棋胜利！\n" + str, L"双人黑白棋", MB_OK | MB_ICONINFORMATION);
		else if (black > white)
			MessageBoxW(L"黑棋胜利！\n" + str, L"双人黑白棋", MB_OK | MB_ICONINFORMATION);
		else
			MessageBoxW(L"平局！\n" + str, L"双人黑白棋", MB_OK | MB_ICONINFORMATION);
		EndGame();
	}
}



void CReversiDlg::OnBnClickedPrompt()
{
	// TODO: 在此添加控件通知处理程序代码
	for (int i = 0; i < SIZE; i++)
	{
		for (int j = 0; j < SIZE; j++)
		{
			if ((GetChessBoardColor(i, j) == None) && (ChessPlaceableAt(i, j, CurrentColor)))
			{
				CDC* dc = this->GetDC();
				CBrush brush_w(RGB(255, 0, 0));
				const CPoint o(100 * i + 100, 100 * j + 100);//圆心
				dc->SelectObject(brush_w);
				dc->Ellipse(o.x - 10, o.y - 10, o.x + 10, o.y + 10);
			}
		}
	}
}
