#pragma once

#include <filesystem>
#include <string>
#include <vector>

#include "PickupRecord7022.h"

class PickupBook7022
{
public:
    bool Load7022();
    bool Add7022(const PickupRecord7022& record7022);
    bool Update7022(int index7022, const PickupRecord7022& record7022);
    bool Remove7022(int index7022);
    bool MarkCollected7022(int index7022);

    int FindByCode7022(const std::wstring& pickupCode7022) const;
    int GetCount7022() const;
    const PickupRecord7022& GetAt7022(int index7022) const;

private:
    bool Save7022() const;
    bool IsValidIndex7022(int index7022) const;
    std::filesystem::path GetDataPath7022() const;

    std::vector<PickupRecord7022> records7022;
};
