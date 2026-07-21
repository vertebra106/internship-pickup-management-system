#pragma once

#include "framework.h"
#include "PickupRecord7022.h"

class PickupModifyDialog7022 : public CDialogEx
{
    DECLARE_DYNAMIC(PickupModifyDialog7022)

public:
    PickupModifyDialog7022(const PickupRecord7022& record7022, CWnd* parentWindow7022 = nullptr);
    PickupRecord7022 GetRecord7022() const;

protected:
    virtual void DoDataExchange(CDataExchange* dataExchange7022) override;
    virtual BOOL OnInitDialog() override;
    virtual void OnOK() override;

    DECLARE_MESSAGE_MAP()

private:
    void DoDataExchange7022(CDataExchange* dataExchange7022);
    BOOL OnInitDialog7022();
    bool OnOK7022();

    PickupRecord7022 pickupRecord7022;
    int collectedState7022;
};
