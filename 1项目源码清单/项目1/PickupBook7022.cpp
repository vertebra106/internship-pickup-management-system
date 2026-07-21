#include "PickupBook7022.h"

#include <cstdint>
#include <fstream>

namespace
{
constexpr std::uint32_t kPickupFileMagic7022 = 0x37303232;
constexpr std::uint32_t kMaxTextLength7022 = 1024;
constexpr std::size_t kMaxRecords7022 = 200;

template <typename ValueType7022>
bool ReadValue7022(std::ifstream& input7022, ValueType7022& value7022)
{
    input7022.read(reinterpret_cast<char*>(&value7022), sizeof(ValueType7022));
    return static_cast<bool>(input7022);
}

template <typename ValueType7022>
void WriteValue7022(std::ofstream& output7022, const ValueType7022& value7022)
{
    output7022.write(reinterpret_cast<const char*>(&value7022), sizeof(ValueType7022));
}

bool ReadText7022(std::ifstream& input7022, std::wstring& text7022)
{
    std::uint32_t length7022 = 0;
    if (!ReadValue7022(input7022, length7022) || length7022 > kMaxTextLength7022)
    {
        return false;
    }

    text7022.resize(length7022);
    if (length7022 > 0)
    {
        input7022.read(
            reinterpret_cast<char*>(text7022.data()),
            static_cast<std::streamsize>(length7022 * sizeof(wchar_t)));
    }

    return static_cast<bool>(input7022);
}

void WriteText7022(std::ofstream& output7022, const std::wstring& text7022)
{
    const auto length7022 = static_cast<std::uint32_t>(text7022.size());
    WriteValue7022(output7022, length7022);

    if (length7022 > 0)
    {
        output7022.write(
            reinterpret_cast<const char*>(text7022.data()),
            static_cast<std::streamsize>(length7022 * sizeof(wchar_t)));
    }
}
}

bool PickupBook7022::Load7022()
{
    records7022.clear();

    const std::filesystem::path dataPath7022 = GetDataPath7022();
    if (!std::filesystem::exists(dataPath7022))
    {
        return true;
    }

    std::ifstream input7022(dataPath7022, std::ios::binary);
    if (!input7022)
    {
        return false;
    }

    std::uint32_t magic7022 = 0;
    std::uint32_t count7022 = 0;
    if (!ReadValue7022(input7022, magic7022) ||
        !ReadValue7022(input7022, count7022) ||
        magic7022 != kPickupFileMagic7022 ||
        count7022 > kMaxRecords7022)
    {
        return false;
    }

    std::vector<PickupRecord7022> loadedRecords7022;
    loadedRecords7022.reserve(count7022);

    for (std::uint32_t index7022 = 0; index7022 < count7022; ++index7022)
    {
        PickupRecord7022 record7022;
        if (!ReadText7022(input7022, record7022.pickupCode7022) ||
            !ReadText7022(input7022, record7022.recipientName7022) ||
            !ReadText7022(input7022, record7022.phoneNumber7022) ||
            !ReadText7022(input7022, record7022.collectionStatus7022))
        {
            return false;
        }

        loadedRecords7022.push_back(record7022);
    }

    records7022 = std::move(loadedRecords7022);
    return true;
}

bool PickupBook7022::Add7022(const PickupRecord7022& record7022)
{
    if (records7022.size() >= kMaxRecords7022 ||
        FindByCode7022(record7022.pickupCode7022) >= 0)
    {
        return false;
    }

    records7022.push_back(record7022);
    if (Save7022())
    {
        return true;
    }

    records7022.pop_back();
    return false;
}

bool PickupBook7022::Update7022(int index7022, const PickupRecord7022& record7022)
{
    if (!IsValidIndex7022(index7022))
    {
        return false;
    }

    const int duplicateIndex7022 = FindByCode7022(record7022.pickupCode7022);
    if (duplicateIndex7022 >= 0 && duplicateIndex7022 != index7022)
    {
        return false;
    }

    const PickupRecord7022 previousRecord7022 = records7022[static_cast<std::size_t>(index7022)];
    records7022[static_cast<std::size_t>(index7022)] = record7022;

    if (Save7022())
    {
        return true;
    }

    records7022[static_cast<std::size_t>(index7022)] = previousRecord7022;
    return false;
}

bool PickupBook7022::Remove7022(int index7022)
{
    if (!IsValidIndex7022(index7022))
    {
        return false;
    }

    const PickupRecord7022 removedRecord7022 = records7022[static_cast<std::size_t>(index7022)];
    records7022.erase(records7022.begin() + index7022);

    if (Save7022())
    {
        return true;
    }

    records7022.insert(records7022.begin() + index7022, removedRecord7022);
    return false;
}

bool PickupBook7022::MarkCollected7022(int index7022)
{
    if (!IsValidIndex7022(index7022))
    {
        return false;
    }

    PickupRecord7022& record7022 = records7022[static_cast<std::size_t>(index7022)];
    const std::wstring previousStatus7022 = record7022.collectionStatus7022;
    record7022.collectionStatus7022 = L"已取件";

    if (Save7022())
    {
        return true;
    }

    record7022.collectionStatus7022 = previousStatus7022;
    return false;
}

int PickupBook7022::FindByCode7022(const std::wstring& pickupCode7022) const
{
    for (int index7022 = 0; index7022 < GetCount7022(); ++index7022)
    {
        if (records7022[static_cast<std::size_t>(index7022)].pickupCode7022 == pickupCode7022)
        {
            return index7022;
        }
    }

    return -1;
}

int PickupBook7022::GetCount7022() const
{
    return static_cast<int>(records7022.size());
}

const PickupRecord7022& PickupBook7022::GetAt7022(int index7022) const
{
    return records7022.at(static_cast<std::size_t>(index7022));
}

bool PickupBook7022::Save7022() const
{
    std::ofstream output7022(GetDataPath7022(), std::ios::binary | std::ios::trunc);
    if (!output7022)
    {
        return false;
    }

    WriteValue7022(output7022, kPickupFileMagic7022);
    const auto count7022 = static_cast<std::uint32_t>(records7022.size());
    WriteValue7022(output7022, count7022);

    for (const PickupRecord7022& record7022 : records7022)
    {
        WriteText7022(output7022, record7022.pickupCode7022);
        WriteText7022(output7022, record7022.recipientName7022);
        WriteText7022(output7022, record7022.phoneNumber7022);
        WriteText7022(output7022, record7022.collectionStatus7022);
    }

    return static_cast<bool>(output7022);
}

bool PickupBook7022::IsValidIndex7022(int index7022) const
{
    return index7022 >= 0 && index7022 < GetCount7022();
}

std::filesystem::path PickupBook7022::GetDataPath7022() const
{
    return std::filesystem::current_path() / L"pickup_records_console7022.dat";
}
