#pragma once

#include <vector>

#include "PickupRecord7022.h"

class PickupBook7022
{
public:
    PickupBook7022();

    bool Load7022();
    bool Add7022(const PickupRecord7022& record7022, CString& message7022);
    bool Update7022(const CString& pickupCode7022, const PickupRecord7022& record7022, CString& message7022);
    bool Remove7022(const CString& pickupCode7022, CString& message7022);
    bool MarkCollected7022(const CString& pickupCode7022, CString& message7022);
    bool Get7022(const CString& pickupCode7022, PickupRecord7022& record7022) const;
    const std::vector<PickupRecord7022>& GetRecords7022() const;

private:
    bool Save7022() const;
    int FindIndex7022(const CString& pickupCode7022) const;

    std::vector<PickupRecord7022> records7022;
    CString storagePath7022;
};
