#include "framework.h"
#include "PickupModifyDialog7022.h"
#include "resource.h"

IMPLEMENT_DYNAMIC(PickupModifyDialog7022, CDialogEx)

PickupModifyDialog7022::PickupModifyDialog7022(const PickupRecord7022& record7022, CWnd* parentWindow7022)
    : CDialogEx(IDD_PICKUP_MODIFY7022, parentWindow7022)
    , pickupRecord7022(record7022)
    , collectedState7022(record7022.collected7022 ? 1 : 0)
{
}

PickupRecord7022 PickupModifyDialog7022::GetRecord7022() const
{
    return pickupRecord7022;
}

void PickupModifyDialog7022::DoDataExchange(CDataExchange* dataExchange7022)
{
    CDialogEx::DoDataExchange(dataExchange7022);
    DoDataExchange7022(dataExchange7022);
}

BOOL PickupModifyDialog7022::OnInitDialog()
{
    CDialogEx::OnInitDialog();
    return OnInitDialog7022();
}

void PickupModifyDialog7022::OnOK()
{
    if (OnOK7022())
    {
        CDialogEx::OnOK();
    }
}

void PickupModifyDialog7022::DoDataExchange7022(CDataExchange* dataExchange7022)
{
    DDX_Text(dataExchange7022, IDC_PICKUP_CODE_EDIT7022, pickupRecord7022.pickupCode7022);
    DDX_Text(dataExchange7022, IDC_RECIPIENT_NAME_EDIT7022, pickupRecord7022.recipientName7022);
    DDX_Text(dataExchange7022, IDC_PHONE_NUMBER_EDIT7022, pickupRecord7022.phoneNumber7022);
    DDX_Check(dataExchange7022, IDC_PICKUP_STATUS_CHECK7022, collectedState7022);
}

BOOL PickupModifyDialog7022::OnInitDialog7022()
{
    CWnd* codeEdit7022 = GetDlgItem(IDC_PICKUP_CODE_EDIT7022);
    if (codeEdit7022 != nullptr)
    {
        codeEdit7022->EnableWindow(FALSE);
    }

    return TRUE;
}

bool PickupModifyDialog7022::OnOK7022()
{
    UpdateData(TRUE);
    pickupRecord7022.pickupCode7022.Trim();
    pickupRecord7022.recipientName7022.Trim();
    pickupRecord7022.phoneNumber7022.Trim();
    pickupRecord7022.collected7022 = collectedState7022 != 0;

    if (pickupRecord7022.pickupCode7022.IsEmpty() ||
        pickupRecord7022.recipientName7022.IsEmpty() ||
        pickupRecord7022.phoneNumber7022.IsEmpty())
    {
        AfxMessageBox(L"收件人和联系电话不能为空。", MB_ICONWARNING);
        return false;
    }

    if (pickupRecord7022.recipientName7022.Find(L'\t') >= 0 ||
        pickupRecord7022.phoneNumber7022.Find(L'\t') >= 0)
    {
        AfxMessageBox(L"输入内容不能包含制表符。", MB_ICONWARNING);
        return false;
    }

    return true;
}

BEGIN_MESSAGE_MAP(PickupModifyDialog7022, CDialogEx)
END_MESSAGE_MAP()
