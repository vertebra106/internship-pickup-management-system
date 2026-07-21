#pragma once

#include "framework.h"
#include "PickupRecord7022.h"

class PickupAddDialog7022 : public CDialogEx
{
    DECLARE_DYNAMIC(PickupAddDialog7022)

public:
    explicit PickupAddDialog7022(CWnd* parentWindow7022 = nullptr);
    PickupRecord7022 GetRecord7022() const;

protected:
    virtual void DoDataExchange(CDataExchange* dataExchange7022) override;
    virtual void OnOK() override;

    DECLARE_MESSAGE_MAP()

private:
    void DoDataExchange7022(CDataExchange* dataExchange7022);
    bool OnOK7022();

    PickupRecord7022 pickupRecord7022;
};
