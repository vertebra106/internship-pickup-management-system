#pragma once

#include "framework.h"
#include "PickupBook7022.h"

class PickupMainDialog7022 : public CDialogEx
{
    DECLARE_DYNAMIC(PickupMainDialog7022)

public:
    explicit PickupMainDialog7022(CWnd* parentWindow7022 = nullptr);

protected:
    virtual void DoDataExchange(CDataExchange* dataExchange7022) override;
    virtual BOOL OnInitDialog() override;

    afx_msg void OnBrowse7022();
    afx_msg void OnAdd7022();
    afx_msg void OnModify7022();
    afx_msg void OnDelete7022();
    afx_msg void OnQuery7022();
    afx_msg void OnMarkCollected7022();
    afx_msg HBRUSH OnCtlColor(CDC* deviceContext7022, CWnd* controlWindow7022, UINT colorType7022);

    DECLARE_MESSAGE_MAP()

private:
    void DoDataExchange7022(CDataExchange* dataExchange7022);
    BOOL OnInitDialog7022();
    void SetupRecordList7022();
    void DistributeColumns7022();
    void AlignHeaderLabels7022();
    void RefreshRecords7022();
    bool GetSelectedPickupCode7022(CString& pickupCode7022) const;
    void SelectRecord7022(const CString& pickupCode7022);
    HBRUSH OnCtlColor7022(CDC* deviceContext7022, CWnd* controlWindow7022, UINT colorType7022, HBRUSH defaultBrush7022);

    CListCtrl recordList7022;
    PickupBook7022 pickupBook7022;
    CBrush headerBrush7022;
};
