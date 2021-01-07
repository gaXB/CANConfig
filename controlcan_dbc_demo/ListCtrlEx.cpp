// ListCtrlEx.cpp : implementation file
//

#include "stdafx.h"
#include "dbc_demo.h"
#include "ListCtrlEx.h"


// CListCtrlEx

IMPLEMENT_DYNAMIC(CListCtrlEx, CListCtrl)

CListCtrlEx::CListCtrlEx()
{

}

CListCtrlEx::~CListCtrlEx()
{
}


BEGIN_MESSAGE_MAP(CListCtrlEx, CListCtrl)
	ON_NOTIFY_REFLECT(NM_CUSTOMDRAW, &CListCtrlEx::OnNMCustomdraw)
END_MESSAGE_MAP()



// CListCtrlEx message handlers



void CListCtrlEx::OnNMCustomdraw(NMHDR *pNMHDR, LRESULT *pResult)
{
	NMLVCUSTOMDRAW* pLVCD = reinterpret_cast<NMLVCUSTOMDRAW*>( pNMHDR );  
	*pResult = CDRF_DODEFAULT;  
	// First thing - check the draw stage. If it's the control's prepaint  
	// stage, then tell Windows we want messages for every item.  
	if ( CDDS_PREPAINT == pLVCD->nmcd.dwDrawStage )  
	{  
		*pResult = CDRF_NOTIFYITEMDRAW;  
	}  
	else if ( CDDS_ITEMPREPAINT == pLVCD->nmcd.dwDrawStage )  
	{  
		// This is the notification message for an item. We'll request  
		// notifications before each subitem's prepaint stage.  
		*pResult = CDRF_NOTIFYSUBITEMDRAW;  
	}  
	else if ( (CDDS_ITEMPREPAINT | CDDS_SUBITEM) == pLVCD->nmcd.dwDrawStage )  
	{  
		//在这里不但可以处理选中行的背景颜色,  
		//也可以设置字体颜色,   
		COLORREF clrNewTextColor, clrNewBkColor;            
		int nItem = static_cast<int>(pLVCD->nmcd.dwItemSpec);  
		if (GetItemState(nItem, LVIS_SELECTED) & LVIS_SELECTED )  
		{  
			clrNewTextColor = RGB(255,255,255);         //Set the text to white  
			clrNewBkColor = RGB(51,153,255);            //Set the background color to blue  
			pLVCD->clrText = clrNewTextColor;  
			pLVCD->clrTextBk = clrNewBkColor;      
		}    
		*pResult = CDRF_DODEFAULT;  
	}  
}
