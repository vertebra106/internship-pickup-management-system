#pragma once

#include "framework.h"

class PickupQueryDialog7022 : public CDialogEx
{
    DECLARE_DYNAMIC(PickupQueryDialog7022)

public:
    explicit PickupQueryDialog7022(CWnd* parentWindow7022 = nullptr);
    CString GetPickupCode7022() const;

protected:
    virtual void DoDataExchange(CDataExchange* dataExchange7022) override;
    virtual void OnOK() override;

    DECLARE_MESSAGE_MAP()

private:
    void DoDataExchange7022(CDataExchange* dataExchange7022);
    bool OnOK7022();

    CString pickupCode7022;
};
