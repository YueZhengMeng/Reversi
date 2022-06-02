
// ReversiDlg.h: 头文件
//

#pragma once
#define SIZE 8

// CReversiDlg 对话框
class CReversiDlg : public CDialogEx
{
// 构造
public:
	CReversiDlg(CWnd* pParent = nullptr);	// 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_REVERSI_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持
	bool IsPlaying;
	bool NowColor;
	int black = 0;
	int white = 0;
	int ChessBoard[SIZE][SIZE];//棋盘，-1为空，0为白，1为黑

	bool LastColor;
	int LastChessBoard[SIZE][SIZE];

	int GetChessBoardColor(int, int);
	void SetChessBoardColor(int nx, int ny, int color);
	void DrawOneChess(int nx, int ny);
	void DrawOneChess(int nx, int ny, int color);
	void DrawALLChess();
	void CleanPrompt();
	void RenewRetract(int temp[SIZE][SIZE], int tempColor);
	bool RenewChessBoard(int x, int y);
	bool RenewNowColor();
	void RenewConut();
	void CleanChessBoard();
	void EndGame();
	CPoint GetNextSameColorChessPos(int, int, int, int);
	bool CanItPlaceChessPieces(int, int, int);

// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedQuit();
	afx_msg void OnClose();
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnBnClickedStart();
	afx_msg void OnBnClickedEndgame();
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnBnClickedPrompt();
	afx_msg void OnBnClickedRetract();
};
