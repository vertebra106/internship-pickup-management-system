#include "framework.h"
#include "PickupBook7022.h"

namespace
{
CString BuildStoragePath7022()
{
    wchar_t executablePath7022[MAX_PATH] = {};
    ::GetModuleFileNameW(nullptr, executablePath7022, MAX_PATH);

    CString storagePath7022(executablePath7022);
    const int separatorPosition7022 = storagePath7022.ReverseFind(L'\\');
    if (separatorPosition7022 >= 0)
    {
        storagePath7022 = storagePath7022.Left(separatorPosition7022 + 1);
    }
    else
    {
        storagePath7022.Empty();
    }

    return storagePath7022 + L"pickup_records_gui7022.txt";
}

bool ValidateRecord7022(const PickupRecord7022& record7022, CString& message7022)
{
    if (record7022.pickupCode7022.IsEmpty())
    {
        message7022 = L"取件码不能为空。";
        return false;
    }

    if (record7022.recipientName7022.IsEmpty())
    {
        message7022 = L"收件人不能为空。";
        return false;
    }

    if (record7022.phoneNumber7022.IsEmpty())
    {
        message7022 = L"联系电话不能为空。";
        return false;
    }

    if (record7022.pickupCode7022.Find(L'\t') >= 0 ||
        record7022.recipientName7022.Find(L'\t') >= 0 ||
        record7022.phoneNumber7022.Find(L'\t') >= 0)
    {
        message7022 = L"输入内容不能包含制表符。";
        return false;
    }

    message7022.Empty();
    return true;
}
}

PickupBook7022::PickupBook7022()
    : storagePath7022(BuildStoragePath7022())
{
    Load7022();
}

bool PickupBook7022::Load7022()
{
    records7022.clear();

    CFileStatus fileStatus7022;
    if (!CFile::GetStatus(storagePath7022, fileStatus7022))
    {
        return true;
    }

    CStdioFile inputFile7022;
    try
    {
        if (!inputFile7022.Open(storagePath7022, CFile::modeRead | CFile::typeUnicode))
        {
            return false;
        }

        CString line7022;
        while (inputFile7022.ReadString(line7022))
        {
            int tokenPosition7022 = 0;
            PickupRecord7022 record7022;
            record7022.pickupCode7022 = line7022.Tokenize(L"\t", tokenPosition7022);
            record7022.recipientName7022 = line7022.Tokenize(L"\t", tokenPosition7022);
            record7022.phoneNumber7022 = line7022.Tokenize(L"\t", tokenPosition7022);
            const CString statusText7022 = line7022.Tokenize(L"\t", tokenPosition7022);

            record7022.pickupCode7022.Trim();
            record7022.recipientName7022.Trim();
            record7022.phoneNumber7022.Trim();

            if (!record7022.pickupCode7022.IsEmpty() &&
                !record7022.recipientName7022.IsEmpty() &&
                !record7022.phoneNumber7022.IsEmpty())
            {
                record7022.collected7022 = statusText7022 == L"1";
                records7022.push_back(record7022);
            }
        }

        inputFile7022.Close();
        return true;
    }
    catch (CFileException* exception7022)
    {
        exception7022->Delete();
        return false;
    }
}

bool PickupBook7022::Add7022(const PickupRecord7022& record7022, CString& message7022)
{
    PickupRecord7022 newRecord7022 = record7022;
    newRecord7022.pickupCode7022.Trim();
    newRecord7022.recipientName7022.Trim();
    newRecord7022.phoneNumber7022.Trim();

    if (!ValidateRecord7022(newRecord7022, message7022))
    {
        return false;
    }

    if (FindIndex7022(newRecord7022.pickupCode7022) >= 0)
    {
        message7022 = L"该取件码已经登记，不能重复添加。";
        return false;
    }

    records7022.push_back(newRecord7022);
    if (!Save7022())
    {
        records7022.pop_back();
        message7022 = L"保存数据失败。";
        return false;
    }

    message7022.Empty();
    return true;
}

bool PickupBook7022::Update7022(const CString& pickupCode7022, const PickupRecord7022& record7022, CString& message7022)
{
    const int recordIndex7022 = FindIndex7022(pickupCode7022);
    if (recordIndex7022 < 0)
    {
        message7022 = L"没有找到需要修改的取件记录。";
        return false;
    }

    PickupRecord7022 updatedRecord7022 = record7022;
    updatedRecord7022.pickupCode7022 = pickupCode7022;
    updatedRecord7022.pickupCode7022.Trim();
    updatedRecord7022.recipientName7022.Trim();
    updatedRecord7022.phoneNumber7022.Trim();

    if (!ValidateRecord7022(updatedRecord7022, message7022))
    {
        return false;
    }

    const PickupRecord7022 previousRecord7022 = records7022[recordIndex7022];
    records7022[recordIndex7022] = updatedRecord7022;
    if (!Save7022())
    {
        records7022[recordIndex7022] = previousRecord7022;
        message7022 = L"保存数据失败。";
        return false;
    }

    message7022.Empty();
    return true;
}

bool PickupBook7022::Remove7022(const CString& pickupCode7022, CString& message7022)
{
    const int recordIndex7022 = FindIndex7022(pickupCode7022);
    if (recordIndex7022 < 0)
    {
        message7022 = L"没有找到需要删除的取件记录。";
        return false;
    }

    const PickupRecord7022 deletedRecord7022 = records7022[recordIndex7022];
    records7022.erase(records7022.begin() + recordIndex7022);
    if (!Save7022())
    {
        records7022.insert(records7022.begin() + recordIndex7022, deletedRecord7022);
        message7022 = L"保存数据失败。";
        return false;
    }

    message7022.Empty();
    return true;
}

bool PickupBook7022::MarkCollected7022(const CString& pickupCode7022, CString& message7022)
{
    const int recordIndex7022 = FindIndex7022(pickupCode7022);
    if (recordIndex7022 < 0)
    {
        message7022 = L"没有找到需要取件的记录。";
        return false;
    }

    if (records7022[recordIndex7022].collected7022)
    {
        message7022 = L"该快递已经标记为已取件。";
        return false;
    }

    records7022[recordIndex7022].collected7022 = true;
    if (!Save7022())
    {
        records7022[recordIndex7022].collected7022 = false;
        message7022 = L"保存数据失败。";
        return false;
    }

    message7022.Empty();
    return true;
}

bool PickupBook7022::Get7022(const CString& pickupCode7022, PickupRecord7022& record7022) const
{
    const int recordIndex7022 = FindIndex7022(pickupCode7022);
    if (recordIndex7022 < 0)
    {
        return false;
    }

    record7022 = records7022[recordIndex7022];
    return true;
}

const std::vector<PickupRecord7022>& PickupBook7022::GetRecords7022() const
{
    return records7022;
}

bool PickupBook7022::Save7022() const
{
    CStdioFile outputFile7022;
    try
    {
        if (!outputFile7022.Open(storagePath7022, CFile::modeCreate | CFile::modeWrite | CFile::typeUnicode))
        {
            return false;
        }

        for (const PickupRecord7022& record7022 : records7022)
        {
            const CString statusText7022 = record7022.collected7022 ? L"1" : L"0";
            const CString line7022 = record7022.pickupCode7022 + L"\t" +
                record7022.recipientName7022 + L"\t" +
                record7022.phoneNumber7022 + L"\t" + statusText7022 + L"\n";
            outputFile7022.WriteString(line7022);
        }

        outputFile7022.Close();
        return true;
    }
    catch (CFileException* exception7022)
    {
        exception7022->Delete();
        return false;
    }
}

int PickupBook7022::FindIndex7022(const CString& pickupCode7022) const
{
    for (int recordIndex7022 = 0; recordIndex7022 < static_cast<int>(records7022.size()); ++recordIndex7022)
    {
        if (records7022[recordIndex7022].pickupCode7022.CompareNoCase(pickupCode7022) == 0)
        {
            return recordIndex7022;
        }
    }

    return -1;
}
