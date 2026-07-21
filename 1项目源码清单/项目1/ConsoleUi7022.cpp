#include "ConsoleUi7022.h"

#include <Windows.h>

#include <iomanip>
#include <iostream>
#include <sstream>

void ConsoleUi7022::Run7022()
{
    if (!pickupBook7022.Load7022())
    {
        std::wcout << L"提示：历史数据读取失败，当前从空记录开始。\n";
    }

    while (true)
    {
        PrintMenu7022();
        const int choice7022 = ReadChoice7022();

        switch (choice7022)
        {
        case 1:
            Browse7022();
            break;
        case 2:
            AddRecord7022();
            break;
        case 3:
            ModifyRecord7022();
            break;
        case 4:
            DeleteRecord7022();
            break;
        case 5:
            QueryRecord7022();
            break;
        case 6:
            MarkCollected7022();
            break;
        case 0:
            std::wcout << L"已退出快递取件信息登记系统。\n";
            return;
        default:
            std::wcout << L"输入无效，请重新选择。\n";
            break;
        }
    }
}

void ConsoleUi7022::PrintMenu7022() const
{
    std::wcout << L"\n========================================\n";
    std::wcout << L"         快递取件信息登记系统\n";
    std::wcout << L"========================================\n";
    std::wcout << L"1. 浏览全部记录\n";
    std::wcout << L"2. 增加取件记录\n";
    std::wcout << L"3. 修改取件记录\n";
    std::wcout << L"4. 删除取件记录\n";
    std::wcout << L"5. 查询取件记录\n";
    std::wcout << L"6. 标记已取件\n";
    std::wcout << L"0. 退出系统\n";
}

void ConsoleUi7022::Browse7022() const
{
    if (pickupBook7022.GetCount7022() == 0)
    {
        std::wcout << L"当前没有取件记录。\n";
        return;
    }

    std::wcout << L"\n序号  取件码          收件人        联系电话        状态\n";
    std::wcout << L"---------------------------------------------------------\n";

    for (int index7022 = 0; index7022 < pickupBook7022.GetCount7022(); ++index7022)
    {
        const PickupRecord7022& record7022 = pickupBook7022.GetAt7022(index7022);
        std::wcout << std::left
            << std::setw(6) << index7022 + 1
            << std::setw(16) << record7022.pickupCode7022
            << std::setw(14) << record7022.recipientName7022
            << std::setw(18) << record7022.phoneNumber7022
            << record7022.collectionStatus7022 << L"\n";
    }
}

void ConsoleUi7022::AddRecord7022()
{
    PickupRecord7022 record7022;
    record7022.pickupCode7022 = ReadLine7022(L"请输入取件码：");
    record7022.recipientName7022 = ReadLine7022(L"请输入收件人姓名：");
    record7022.phoneNumber7022 = ReadLine7022(L"请输入联系电话：");
    record7022.collectionStatus7022 = L"待取件";

    if (record7022.pickupCode7022.empty() || record7022.recipientName7022.empty())
    {
        std::wcout << L"取件码和收件人不能为空。\n";
        return;
    }

    if (!pickupBook7022.Add7022(record7022))
    {
        std::wcout << L"增加失败：取件码重复、记录已满或文件无法保存。\n";
        return;
    }

    std::wcout << L"增加成功。\n";
}

void ConsoleUi7022::ModifyRecord7022()
{
    const std::wstring pickupCode7022 = ReadLine7022(L"请输入要修改的取件码：");
    const int index7022 = pickupBook7022.FindByCode7022(pickupCode7022);
    if (index7022 < 0)
    {
        std::wcout << L"未找到该取件记录。\n";
        return;
    }

    PickupRecord7022 record7022 = pickupBook7022.GetAt7022(index7022);
    std::wcout << L"直接回车可保留原来的内容。\n";

    const std::wstring newCode7022 = ReadLine7022(L"新的取件码（当前：" + record7022.pickupCode7022 + L"）：");
    const std::wstring newName7022 = ReadLine7022(L"新的收件人（当前：" + record7022.recipientName7022 + L"）：");
    const std::wstring newPhone7022 = ReadLine7022(L"新的电话（当前：" + record7022.phoneNumber7022 + L"）：");
    const std::wstring newStatus7022 = ReadLine7022(L"新的状态（当前：" + record7022.collectionStatus7022 + L"）：");

    if (!newCode7022.empty())
    {
        record7022.pickupCode7022 = newCode7022;
    }
    if (!newName7022.empty())
    {
        record7022.recipientName7022 = newName7022;
    }
    if (!newPhone7022.empty())
    {
        record7022.phoneNumber7022 = newPhone7022;
    }
    if (!newStatus7022.empty())
    {
        record7022.collectionStatus7022 = newStatus7022;
    }

    if (!pickupBook7022.Update7022(index7022, record7022))
    {
        std::wcout << L"修改失败：新取件码可能与其他记录重复，或文件无法保存。\n";
        return;
    }

    std::wcout << L"修改成功。\n";
}

void ConsoleUi7022::DeleteRecord7022()
{
    const std::wstring pickupCode7022 = ReadLine7022(L"请输入要删除的取件码：");
    const int index7022 = pickupBook7022.FindByCode7022(pickupCode7022);
    if (index7022 < 0)
    {
        std::wcout << L"未找到该取件记录。\n";
        return;
    }

    const std::wstring confirm7022 = ReadLine7022(L"确定删除吗？输入 Y 确认：");
    if (confirm7022 != L"Y" && confirm7022 != L"y")
    {
        std::wcout << L"已取消删除。\n";
        return;
    }

    if (!pickupBook7022.Remove7022(index7022))
    {
        std::wcout << L"删除失败。\n";
        return;
    }

    std::wcout << L"删除成功。\n";
}

void ConsoleUi7022::QueryRecord7022() const
{
    const std::wstring pickupCode7022 = ReadLine7022(L"请输入要查询的取件码：");
    const int index7022 = pickupBook7022.FindByCode7022(pickupCode7022);
    if (index7022 < 0)
    {
        std::wcout << L"未找到该取件记录。\n";
        return;
    }

    std::wcout << L"\n查询结果：\n";
    PrintRecord7022(pickupBook7022.GetAt7022(index7022));
}

void ConsoleUi7022::MarkCollected7022()
{
    const std::wstring pickupCode7022 = ReadLine7022(L"请输入已取件的取件码：");
    const int index7022 = pickupBook7022.FindByCode7022(pickupCode7022);
    if (index7022 < 0)
    {
        std::wcout << L"未找到该取件记录。\n";
        return;
    }

    if (!pickupBook7022.MarkCollected7022(index7022))
    {
        std::wcout << L"状态修改失败。\n";
        return;
    }

    std::wcout << L"已标记为“已取件”。\n";
}

void ConsoleUi7022::PrintRecord7022(const PickupRecord7022& record7022) const
{
    std::wcout << L"取件码：" << record7022.pickupCode7022 << L"\n";
    std::wcout << L"收件人：" << record7022.recipientName7022 << L"\n";
    std::wcout << L"联系电话：" << record7022.phoneNumber7022 << L"\n";
    std::wcout << L"取件状态：" << record7022.collectionStatus7022 << L"\n";
}

std::wstring ConsoleUi7022::ReadLine7022(const std::wstring& prompt7022) const
{
    std::wcout << prompt7022 << std::flush;

    // ReadConsoleW returns UTF-16 text directly and avoids terminal code-page conversion.
    const HANDLE inputHandle7022 = ::GetStdHandle(STD_INPUT_HANDLE);
    DWORD inputMode7022 = 0;
    if (inputHandle7022 != nullptr &&
        inputHandle7022 != INVALID_HANDLE_VALUE &&
        ::GetConsoleMode(inputHandle7022, &inputMode7022))
    {
        wchar_t inputBuffer7022[512] = {};
        DWORD characterCount7022 = 0;
        if (::ReadConsoleW(inputHandle7022, inputBuffer7022, 511, &characterCount7022, nullptr))
        {
            std::wstring line7022(inputBuffer7022, characterCount7022);
            while (!line7022.empty() && (line7022.back() == L'\r' || line7022.back() == L'\n'))
            {
                line7022.pop_back();
            }

            return line7022;
        }
    }

    // Keep redirected input usable for automated testing and ordinary file redirection.
    std::wstring line7022;
    std::getline(std::wcin, line7022);
    return line7022;
}

int ConsoleUi7022::ReadChoice7022() const
{
    const std::wstring choiceText7022 = ReadLine7022(L"请选择功能：");
    std::wistringstream input7022(choiceText7022);
    int choice7022 = -1;
    input7022 >> choice7022;
    return input7022.fail() ? -1 : choice7022;
}
