#include "framework.h"
#include "PickupMainDialog7022.h"
#include "PickupAddDialog7022.h"
#include "PickupModifyDialog7022.h"
#include "PickupQueryDialog7022.h"
#include "resource.h"

IMPLEMENT_DYNAMIC(PickupMainDialog7022, CDialogEx)

PickupMainDialog7022::PickupMainDialog7022(CWnd* parentWindow7022)
    : CDialogEx(IDD_PICKUP_MAIN7022, parentWindow7022)
{
}

void PickupMainDialog7022::DoDataExchange(CDataExchange* dataExchange7022)
{
    CDialogEx::DoDataExchange(dataExchange7022);
    DoDataExchange7022(dataExchange7022);
}

BOOL PickupMainDialog7022::OnInitDialog()
{
    CDialogEx::OnInitDialog();
    return OnInitDialog7022();
}

void PickupMainDialog7022::DoDataExchange7022(CDataExchange* dataExchange7022)
{
    DDX_Control(dataExchange7022, IDC_RECORD_LIST7022, recordList7022);
}

BOOL PickupMainDialog7022::OnInitDialog7022()
{
    headerBrush7022.CreateSolidBrush(RGB(255, 255, 255));
    SetupRecordList7022();
    RefreshRecords7022();
    return TRUE;
}

void PickupMainDialog7022::SetupRecordList7022()
{
    recordList7022.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_DOUBLEBUFFER);
    recordList7022.InsertColumn(0, L"", LVCFMT_LEFT, 1);
    recordList7022.InsertColumn(1, L"", LVCFMT_LEFT, 1);
    recordList7022.InsertColumn(2, L"", LVCFMT_LEFT, 1);
    recordList7022.InsertColumn(3, L"", LVCFMT_LEFT, 1);
    DistributeColumns7022();
    AlignHeaderLabels7022();
}

void PickupMainDialog7022::DistributeColumns7022()
{
    CRect listClientRectangle7022;
    recordList7022.GetClientRect(&listClientRectangle7022);

    const int availableWidth7022 = listClientRectangle7022.Width();
    const int standardColumnWidth7022 = availableWidth7022 / 4;

    for (int columnIndex7022 = 0; columnIndex7022 < 3; ++columnIndex7022)
    {
        recordList7022.SetColumnWidth(columnIndex7022, standardColumnWidth7022);
    }

    recordList7022.SetColumnWidth(3, availableWidth7022 - standardColumnWidth7022 * 3);
}

void PickupMainDialog7022::AlignHeaderLabels7022()
{
    const UINT headerIds7022[] = {
        IDC_HEADER_CODE7022,
        IDC_HEADER_RECIPIENT7022,
        IDC_HEADER_PHONE7022,
        IDC_HEADER_STATUS7022
    };

    CRect listRectangle7022;
    recordList7022.GetWindowRect(&listRectangle7022);
    ScreenToClient(&listRectangle7022);

    int labelLeft7022 = listRectangle7022.left + 5;
    for (int columnIndex7022 = 0; columnIndex7022 < 4; ++columnIndex7022)
    {
        CWnd* headerLabel7022 = GetDlgItem(headerIds7022[columnIndex7022]);
        if (headerLabel7022 != nullptr)
        {
            CRect headerRectangle7022;
            headerLabel7022->GetWindowRect(&headerRectangle7022);
            ScreenToClient(&headerRectangle7022);

            const int columnWidth7022 = recordList7022.GetColumnWidth(columnIndex7022);
            headerLabel7022->MoveWindow(
                labelLeft7022,
                headerRectangle7022.top,
                columnWidth7022 - 5,
                headerRectangle7022.Height(),
                TRUE);
            labelLeft7022 += columnWidth7022;
        }
    }
}

void PickupMainDialog7022::RefreshRecords7022()
{
    recordList7022.DeleteAllItems();

    int rowIndex7022 = 0;
    for (const PickupRecord7022& record7022 : pickupBook7022.GetRecords7022())
    {
        recordList7022.InsertItem(rowIndex7022, record7022.pickupCode7022);
        recordList7022.SetItemText(rowIndex7022, 1, record7022.recipientName7022);
        recordList7022.SetItemText(rowIndex7022, 2, record7022.phoneNumber7022);
        recordList7022.SetItemText(rowIndex7022, 3, record7022.collected7022 ? L"已取件" : L"待取件");
        ++rowIndex7022;
    }
}

bool PickupMainDialog7022::GetSelectedPickupCode7022(CString& pickupCode7022) const
{
    POSITION selectedPosition7022 = recordList7022.GetFirstSelectedItemPosition();
    if (selectedPosition7022 == nullptr)
    {
        return false;
    }

    const int selectedIndex7022 = recordList7022.GetNextSelectedItem(selectedPosition7022);
    pickupCode7022 = recordList7022.GetItemText(selectedIndex7022, 0);
    return !pickupCode7022.IsEmpty();
}

void PickupMainDialog7022::SelectRecord7022(const CString& pickupCode7022)
{
    for (int rowIndex7022 = 0; rowIndex7022 < recordList7022.GetItemCount(); ++rowIndex7022)
    {
        if (recordList7022.GetItemText(rowIndex7022, 0).CompareNoCase(pickupCode7022) == 0)
        {
            recordList7022.SetItemState(rowIndex7022, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);
            recordList7022.EnsureVisible(rowIndex7022, FALSE);
            break;
        }
    }
}

void PickupMainDialog7022::OnBrowse7022()
{
    RefreshRecords7022();
}

void PickupMainDialog7022::OnAdd7022()
{
    PickupAddDialog7022 addDialog7022(this);
    if (addDialog7022.DoModal() != IDOK)
    {
        return;
    }

    CString message7022;
    if (!pickupBook7022.Add7022(addDialog7022.GetRecord7022(), message7022))
    {
        AfxMessageBox(message7022, MB_ICONWARNING);
        return;
    }

    RefreshRecords7022();
    AfxMessageBox(L"快递取件信息已添加。", MB_ICONINFORMATION);
}

void PickupMainDialog7022::OnModify7022()
{
    CString pickupCode7022;
    if (!GetSelectedPickupCode7022(pickupCode7022))
    {
        AfxMessageBox(L"请先在列表中选择一条记录。", MB_ICONINFORMATION);
        return;
    }

    PickupRecord7022 record7022;
    if (!pickupBook7022.Get7022(pickupCode7022, record7022))
    {
        AfxMessageBox(L"没有找到该取件记录。", MB_ICONWARNING);
        return;
    }

    PickupModifyDialog7022 modifyDialog7022(record7022, this);
    if (modifyDialog7022.DoModal() != IDOK)
    {
        return;
    }

    CString message7022;
    if (!pickupBook7022.Update7022(pickupCode7022, modifyDialog7022.GetRecord7022(), message7022))
    {
        AfxMessageBox(message7022, MB_ICONWARNING);
        return;
    }

    RefreshRecords7022();
    SelectRecord7022(pickupCode7022);
    AfxMessageBox(L"快递取件信息已修改。", MB_ICONINFORMATION);
}

void PickupMainDialog7022::OnDelete7022()
{
    CString pickupCode7022;
    if (!GetSelectedPickupCode7022(pickupCode7022))
    {
        AfxMessageBox(L"请先在列表中选择一条记录。", MB_ICONINFORMATION);
        return;
    }

    CString confirmation7022;
    confirmation7022.Format(L"确定删除取件码为 %s 的记录吗？", pickupCode7022.GetString());
    if (AfxMessageBox(confirmation7022, MB_YESNO | MB_ICONQUESTION) != IDYES)
    {
        return;
    }

    CString message7022;
    if (!pickupBook7022.Remove7022(pickupCode7022, message7022))
    {
        AfxMessageBox(message7022, MB_ICONWARNING);
        return;
    }

    RefreshRecords7022();
    AfxMessageBox(L"快递取件信息已删除。", MB_ICONINFORMATION);
}

void PickupMainDialog7022::OnQuery7022()
{
    PickupQueryDialog7022 queryDialog7022(this);
    if (queryDialog7022.DoModal() != IDOK)
    {
        return;
    }

    PickupRecord7022 record7022;
    if (!pickupBook7022.Get7022(queryDialog7022.GetPickupCode7022(), record7022))
    {
        AfxMessageBox(L"没有找到该取件码对应的信息。", MB_ICONINFORMATION);
        return;
    }

    RefreshRecords7022();
    SelectRecord7022(record7022.pickupCode7022);

    CString result7022;
    result7022.Format(L"取件码：%s\r\n收件人：%s\r\n联系电话：%s\r\n状态：%s",
        record7022.pickupCode7022.GetString(),
        record7022.recipientName7022.GetString(),
        record7022.phoneNumber7022.GetString(),
        record7022.collected7022 ? L"已取件" : L"待取件");
    AfxMessageBox(result7022, MB_ICONINFORMATION);
}

void PickupMainDialog7022::OnMarkCollected7022()
{
    CString pickupCode7022;
    if (!GetSelectedPickupCode7022(pickupCode7022))
    {
        AfxMessageBox(L"请先在列表中选择一条记录。", MB_ICONINFORMATION);
        return;
    }

    CString message7022;
    if (!pickupBook7022.MarkCollected7022(pickupCode7022, message7022))
    {
        AfxMessageBox(message7022, MB_ICONWARNING);
        return;
    }

    RefreshRecords7022();
    SelectRecord7022(pickupCode7022);
    AfxMessageBox(L"已标记为已取件。", MB_ICONINFORMATION);
}

HBRUSH PickupMainDialog7022::OnCtlColor(CDC* deviceContext7022, CWnd* controlWindow7022, UINT colorType7022)
{
    const HBRUSH defaultBrush7022 = CDialogEx::OnCtlColor(deviceContext7022, controlWindow7022, colorType7022);
    return OnCtlColor7022(deviceContext7022, controlWindow7022, colorType7022, defaultBrush7022);
}

HBRUSH PickupMainDialog7022::OnCtlColor7022(CDC* deviceContext7022, CWnd* controlWindow7022, UINT colorType7022, HBRUSH defaultBrush7022)
{
    if (colorType7022 != CTLCOLOR_STATIC || controlWindow7022 == nullptr)
    {
        return defaultBrush7022;
    }

    switch (controlWindow7022->GetDlgCtrlID())
    {
    case IDC_RECORD_HEADER_BACKDROP7022:
    case IDC_HEADER_CODE7022:
    case IDC_HEADER_RECIPIENT7022:
    case IDC_HEADER_PHONE7022:
    case IDC_HEADER_STATUS7022:
        deviceContext7022->SetBkColor(RGB(255, 255, 255));
        deviceContext7022->SetTextColor(RGB(38, 38, 38));
        return headerBrush7022;
    default:
        return defaultBrush7022;
    }
}

BEGIN_MESSAGE_MAP(PickupMainDialog7022, CDialogEx)
    ON_BN_CLICKED(IDC_BROWSE_BUTTON7022, &PickupMainDialog7022::OnBrowse7022)
    ON_BN_CLICKED(IDC_ADD_BUTTON7022, &PickupMainDialog7022::OnAdd7022)
    ON_BN_CLICKED(IDC_MODIFY_BUTTON7022, &PickupMainDialog7022::OnModify7022)
    ON_BN_CLICKED(IDC_DELETE_BUTTON7022, &PickupMainDialog7022::OnDelete7022)
    ON_BN_CLICKED(IDC_QUERY_BUTTON7022, &PickupMainDialog7022::OnQuery7022)
    ON_BN_CLICKED(IDC_MARK_COLLECTED_BUTTON7022, &PickupMainDialog7022::OnMarkCollected7022)
    ON_WM_CTLCOLOR()
END_MESSAGE_MAP()
