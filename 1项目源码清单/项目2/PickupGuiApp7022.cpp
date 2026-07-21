#include "framework.h"
#include "PickupGuiApp7022.h"
#include "PickupMainDialog7022.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

PickupGuiApp7022 theApp7022;

BOOL PickupGuiApp7022::InitInstance()
{
    return InitInstance7022();
}

BOOL PickupGuiApp7022::InitInstance7022()
{
    CWinApp::InitInstance();
    AfxEnableControlContainer();

    PickupMainDialog7022 mainDialog7022;
    m_pMainWnd = &mainDialog7022;
    mainDialog7022.DoModal();
    return FALSE;
}
