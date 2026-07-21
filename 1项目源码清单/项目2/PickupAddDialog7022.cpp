#include "framework.h"
#include "PickupAddDialog7022.h"
#include "resource.h"

IMPLEMENT_DYNAMIC(PickupAddDialog7022, CDialogEx)

PickupAddDialog7022::PickupAddDialog7022(CWnd* parentWindow7022)
    : CDialogEx(IDD_PICKUP_ADD7022, parentWindow7022)
{
    pickupRecord7022.collected7022 = false;
}

PickupRecord7022 PickupAddDialog7022::GetRecord7022() const
{
    return pickupRecord7022;
}

void PickupAddDialog7022::DoDataExchange(CDataExchange* dataExchange7022)
{
    CDialogEx::DoDataExchange(dataExchange7022);
    DoDataExchange7022(dataExchange7022);
}

void PickupAddDialog7022::OnOK()
{
    if (OnOK7022())
    {
        CDialogEx::OnOK();
    }
}

void PickupAddDialog7022::DoDataExchange7022(CDataExchange* dataExchange7022)
{
    DDX_Text(dataExchange7022, IDC_PICKUP_CODE_EDIT7022, pickupRecord7022.pickupCode7022);
    DDX_Text(dataExchange7022, IDC_RECIPIENT_NAME_EDIT7022, pickupRecord7022.recipientName7022);
    DDX_Text(dataExchange7022, IDC_PHONE_NUMBER_EDIT7022, pickupRecord7022.phoneNumber7022);
}

bool PickupAddDialog7022::OnOK7022()
{
    UpdateData(TRUE);
    pickupRecord7022.pickupCode7022.Trim();
    pickupRecord7022.recipientName7022.Trim();
    pickupRecord7022.phoneNumber7022.Trim();

    if (pickupRecord7022.pickupCode7022.IsEmpty() ||
        pickupRecord7022.recipientName7022.IsEmpty() ||
        pickupRecord7022.phoneNumber7022.IsEmpty())
    {
        AfxMessageBox(L"请完整填写取件码、收件人和联系电话。", MB_ICONWARNING);
        return false;
    }

    if (pickupRecord7022.pickupCode7022.Find(L'\t') >= 0 ||
        pickupRecord7022.recipientName7022.Find(L'\t') >= 0 ||
        pickupRecord7022.phoneNumber7022.Find(L'\t') >= 0)
    {
        AfxMessageBox(L"输入内容不能包含制表符。", MB_ICONWARNING);
        return false;
    }

    return true;
}

BEGIN_MESSAGE_MAP(PickupAddDialog7022, CDialogEx)
END_MESSAGE_MAP()
