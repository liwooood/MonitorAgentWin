
// TradeGatewayMonitor.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CTradeGatewayMonitorApp:
// �йش����ʵ�֣������ TradeGatewayMonitor.cpp
//

class CTradeGatewayMonitorApp : public CWinAppEx
{
public:
	CTradeGatewayMonitorApp();

// ��д
	public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CTradeGatewayMonitorApp theApp;