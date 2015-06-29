// MainDlg.h : interface of the CMainDlg class
//
/////////////////////////////////////////////////////////////////////////////

#pragma once
#include "Obj.cpp"
#include "ReadFile.cpp"

class CMainDlg : public CDialogImpl<CMainDlg>, public CDialogResize<CMainDlg>, public CIdleHandler
{
public:
	enum { IDD = IDD_MAINDLG };

  CListViewCtrl m_list_files;

	virtual BOOL PreTranslateMessage(MSG* pMsg)
	{
		return CWindow::IsDialogMessage(pMsg);
	}

  void SetStatusText(int idx, LPCTSTR status)
  {
    m_list_files.SetItemText(idx, 1, status);
    m_list_files.UpdateWindow();
  }
  void SetRunning(int idx)
  {
    SetStatusText(idx, _T("-"));
  }
  void SetSuccess(int idx)
  {
    SetStatusText(idx, _T("OK"));
  }
  void SetFail(int idx)
  {
    SetStatusText(idx, _T("!"));
  }

	virtual BOOL OnIdle()
	{
		return FALSE;
	}

  BEGIN_DLGRESIZE_MAP(CMainDlg)
    DLGRESIZE_CONTROL(IDL_FILES, DLSZ_SIZE_X | DLSZ_SIZE_Y)
    DLGRESIZE_CONTROL(IDB_CLEAR, DLSZ_MOVE_X)
  END_DLGRESIZE_MAP()

	BEGIN_MSG_MAP(CMainDlg)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
    MSG_WM_SIZE(OnSize)
    MSG_WM_CLOSE(OnClose)
    MSG_WM_DESTROY(OnDestroy)
    MSG_WM_DROPFILES(OnDropFiles)
		//COMMAND_ID_HANDLER(ID_APP_ABOUT, OnAppAbout)
		COMMAND_ID_HANDLER(IDOK, OnOK)
		//COMMAND_ID_HANDLER(IDCANCEL, OnCancel)
    CHAIN_MSG_MAP(CDialogResize<CMainDlg>)
	END_MSG_MAP()

  void OnDestroy()
  {
    CMessageLoop* pLoop = _Module.GetMessageLoop();
    pLoop->RemoveIdleHandler(this);
    SetMsgHandled(FALSE);
  }

// Handler prototypes (uncomment arguments if needed):
//	LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
//	LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
//	LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)

	LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		// center the dialog on the screen
		CenterWindow();

		// set icons
		HICON hIcon = AtlLoadIconImage(IDR_MAINFRAME, LR_DEFAULTCOLOR, ::GetSystemMetrics(SM_CXICON), ::GetSystemMetrics(SM_CYICON));
		SetIcon(hIcon, TRUE);
		HICON hIconSmall = AtlLoadIconImage(IDR_MAINFRAME, LR_DEFAULTCOLOR, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON));
		SetIcon(hIconSmall, FALSE);

    m_list_files = GetDlgItem(IDL_FILES);
    m_list_files.SetExtendedListViewStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
    m_list_files.InsertColumn(0, _T("Path"), LVCFMT_LEFT, 250);
    m_list_files.InsertColumn(1, _T("Result"), LVCFMT_LEFT, 50);

    DragAcceptFiles();

    DlgResize_Init(true, true, WS_THICKFRAME | WS_MAXIMIZEBOX | WS_CLIPCHILDREN);

		// register object for message filtering and idle updates
		CMessageLoop* pLoop = _Module.GetMessageLoop();
		ATLASSERT(pLoop != NULL);
		pLoop->AddIdleHandler(this);

    
    int numArgs = 0;
    LPWSTR *lpArgv = CommandLineToArgvW(GetCommandLineW(), &numArgs);
    for(int i=1; i<numArgs; i++)AddPath(lpArgv[i]);

		return TRUE;
	}

  void OnSize(UINT nType, CSize size)
  {
    int newWidth = size.cx - 80;
    m_list_files.SetColumnWidth(0, newWidth);
    SetMsgHandled(FALSE);
  }

  void AddDir(LPCTSTR _dirpath)
  {
    CFindFile finder;

    CString dirpath(_dirpath);
    PathAddBackslash(dirpath.GetBuffer(dirpath.GetLength()+2));
    dirpath.ReleaseBuffer();
    dirpath += _T("*");

    BOOL bWorking = finder.FindFile(dirpath);

    while (bWorking)
    {
      bWorking = finder.FindNextFile();
      CString file = finder.GetFilePath();
      if (finder.IsDots())continue;
      else if (finder.IsDirectory())AddDir(file);
      else if(IsObjExt(file))AddMqo(file);
    }
   finder.Close();
  }
  

  FILE* OpenOutputFile(LPCTSTR srcpath)
  {
    FILE *fp = NULL;
    CPath p(srcpath);
    p.RenameExtension(_T(".tfx"));
    return _wfopen(p, _T("w"));
  }

  HRESULT LoadFromFile(LPCTSTR path)
  {
    DWORD len=0, lenPlusSpace=0;
    unsigned char *pFile = (unsigned char *)ReadFile(path, &len, &lenPlusSpace, 30, TRUE, FALSE);
    if(pFile==NULL)return E_FAIL;
    FILE *fpWrite = OpenOutputFile(path);
    if(fpWrite==NULL)return E_FAIL;

    void *pParser = ParseAlloc(malloc);
    unsigned int type = 0;
    Scanner s = {0};
    s.cur = pFile;
    s.lim = pFile+lenPlusSpace;
    s.eof = pFile+len;
    s.bLineFirst = true;

#ifdef _DEBUG
    //  FILE *fpErr = fopen("err.txt", "w");
    //  ParseTrace(fpErr, (char*)"LP: ");
#endif

    CMYWriter writer(fpWrite);

    while(type = scan(&s))
    {
      //ATLTRACE("type = %d\n", type);
      Parse(pParser, type, s.val, &writer);
    }
    ATLTRACE("ParseTerminate\n", type);
    Parse(pParser, 0, s.val, &writer);
    ParseFree(pParser, free);
    delete pFile;

    writer.Write();
    fclose(fpWrite);

    return S_OK;
  }

  void AddMqo(LPCTSTR path)
  {
    m_list_files.InsertItem(0, path);
    if(SUCCEEDED(LoadFromFile(path)))SetSuccess(0);
    else SetFail(0);
  }

  BOOL IsObjExt(LPCTSTR path)
  {
    CString ext = PathFindExtension(path);
    if(ext.CompareNoCase(_T(".obj"))==0)return TRUE;
    return FALSE;
  }
  
  void AddPath(LPCTSTR path)
  {
    if(PathIsDirectory(path))AddDir(path);
    else if(PathFileExists(path))
    {
      if(IsObjExt(path))AddMqo(path);
    }
  }

  void OnDropFiles(HDROP hDropInfo)
  {
    unsigned int numFile = ::DragQueryFile(hDropInfo, -1, NULL, 0);
    for(unsigned int i=0; i<numFile; i++)
    {
      CString path;
      ::DragQueryFile(hDropInfo, i, path.GetBuffer(1024+2), 1024);
      path.ReleaseBuffer();
      AddPath(path);
    }
    ::DragFinish(hDropInfo);
  }

  
	//LRESULT OnAppAbout(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	//{
//		CSimpleDialog<IDD_ABOUTBOX, FALSE> dlg;
//		dlg.DoModal();
//		return 0;
//	}
//
  void OnClose()
  {
    DestroyWindow();
    ::PostQuitMessage(0);
  }
	LRESULT OnOK(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
    DestroyWindow();
    ::PostQuitMessage(wID);
		return 0;
	}
};
