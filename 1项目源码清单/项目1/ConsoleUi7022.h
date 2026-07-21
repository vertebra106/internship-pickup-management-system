#pragma once

#include <string>

#include "PickupBook7022.h"

class ConsoleUi7022
{
public:
    void Run7022();

private:
    void PrintMenu7022() const;
    void Browse7022() const;
    void AddRecord7022();
    void ModifyRecord7022();
    void DeleteRecord7022();
    void QueryRecord7022() const;
    void MarkCollected7022();
    void PrintRecord7022(const PickupRecord7022& record7022) const;
    std::wstring ReadLine7022(const std::wstring& prompt7022) const;
    int ReadChoice7022() const;

    PickupBook7022 pickupBook7022;
};
