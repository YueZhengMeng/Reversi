
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

	enum ChessColor : int {
		White = -1,
		None,
		Black
	}; //棋子颜色，- 1为白，0为空，1为黑
	bool IsPlaying;//是否正在进行游戏
	ChessColor CurrentColor;//当前执棋颜色
	int black = 0;//黑子数量
	int white = 0;//白子数量
	ChessColor ChessBoard[SIZE][SIZE];//棋盘

	char msg[512];
	static const size_t message_limit = 512;
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持

	ChessColor GetChessBoardColor(int, int);
	void SetChessBoardColor(int nx, int ny, ChessColor color);
	void DrawOneChess(int nx, int ny);
	void DrawAllChess();
	bool RefreshChessBoard(int x, int y);
	bool RefreshCurrentColor();
	void RefreshChessConut();
	void CleanPrompt();
	void CleanChessBoard();
	void EndGame();
	CPoint GetNextSameColorChessPos(int, int, int, int);
	bool ChessPlaceableAt(int, int, int);

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
};
