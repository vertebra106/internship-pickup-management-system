#include "framework.h"
#include "PickupQueryDialog7022.h"
#include "resource.h"

IMPLEMENT_DYNAMIC(PickupQueryDialog7022, CDialogEx)

PickupQueryDialog7022::PickupQueryDialog7022(CWnd* parentWindow7022)
    : CDialogEx(IDD_PICKUP_QUERY7022, parentWindow7022)
{
}

CString PickupQueryDialog7022::GetPickupCode7022() const
{
    return pickupCode7022;
}

void PickupQueryDialog7022::DoDataExchange(CDataExchange* dataExchange7022)
{
    CDialogEx::DoDataExchange(dataExchange7022);
    DoDataExchange7022(dataExchange7022);
}

void PickupQueryDialog7022::OnOK()
{
    if (OnOK7022())
    {
        CDialogEx::OnOK();
    }
}

void PickupQueryDialog7022::DoDataExchange7022(CDataExchange* dataExchange7022)
{
    DDX_Text(dataExchange7022, IDC_QUERY_CODE_EDIT7022, pickupCode7022);
}

bool PickupQueryDialog7022::OnOK7022()
{
    UpdateData(TRUE);
    pickupCode7022.Trim();
    if (pickupCode7022.IsEmpty())
    {
        AfxMessageBox(L"请输入要查询的取件码。", MB_ICONWARNING);
        return false;
    }

    return true;
}

BEGIN_MESSAGE_MAP(PickupQueryDialog7022, CDialogEx)
END_MESSAGE_MAP()
