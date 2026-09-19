package campus.express;

import java.io.IOException;
import java.io.PrintWriter;
import java.util.NoSuchElementException;
import java.util.Scanner;

/** 只处理菜单、输入输出和删除确认，将业务交给 Service。 */
public final class ConsoleUI {
    private final ExpressService service;
    private final Scanner input;
    private final PrintWriter output;

    public ConsoleUI(ExpressService service, Scanner input, PrintWriter output) {
        this.service = service;
        this.input = input;
        this.output = output;
    }

    /** 全部使用 nextLine，避免 nextInt 遗留换行造成跳过输入。 */
    public void run() {
        output.println("校园快递取件信息管理系统");
        for (String warning : service.getWarnings()) output.println("警告：" + warning);
        if (service.isReadOnly()) output.println("已进入只读模式：请先备份并修复数据文件，再重新启动。");
        try {
            while (true) {
                output.println("\n1 新增  2 查看全部  3 查询  4 修改  5 删除  6 取件  0 退出");
                String option = ask("请选择：").strip();
                try {
                    switch (option) {
                        case "1": add(); break;
                        case "2": showAll(); break;
                        case "3": query(); break;
                        case "4": update(); break;
                        case "5": delete(); break;
                        case "6": service.pickUp(ask("取件码：")); success(); break;
                        case "0": output.println("已退出。"); return;
                        default: output.println("无效菜单选项，请输入 0～6。");
                    }
                } catch (IllegalArgumentException | IllegalStateException ex) {
                    output.println("操作失败：" + ex.getMessage());
                } catch (IOException | SecurityException ex) {
                    output.println("保存失败，本次操作未生效，内存未改变：" + ex.getMessage());
                }
            }
        } catch (NoSuchElementException ex) {
            output.println("\n输入已结束，未提交的输入已取消，程序退出。");
        } finally {
            output.flush();
        }
    }

    private String ask(String prompt) {
        output.print(prompt);
        output.flush();
        return input.nextLine();
    }

    private void add() throws IOException {
        service.add(ask("快递单号："), ask("收件人："), ask("联系电话（7～15 位数字）："),
                ask("取件码："), ask("快递公司："));
        success();
    }

    private void showAll() {
        if (service.findAll().isEmpty()) output.println("暂无快递记录。");
        for (Express entry : service.findAll()) show(entry);
    }

    private void show(Express entry) {
        if (entry == null) {
            output.println("未找到对应记录。");
            return;
        }
        output.println("单号：" + entry.getTrackingNumber() + " | 收件人：" + entry.getRecipient()
                + " | 电话：" + entry.getPhone() + " | 取件码：" + entry.getPickupCode()
                + " | 公司：" + entry.getCompany() + " | 状态：" + entry.getStatus().getLabel());
    }

    private void query() {
        String mode = ask("查询方式（1 取件码 / 2 快递单号）：").strip();
        switch (mode) {
            case "1": show(service.findByPickupCode(ask("取件码："))); break;
            case "2": show(service.findByTrackingNumber(ask("快递单号："))); break;
            default: output.println("无效查询方式，请选择 1 或 2。");
        }
    }

    private void update() throws IOException {
        String number = ask("要修改的快递单号：");
        Express old = service.findByTrackingNumber(number);
        if (old == null) { output.println("未找到对应记录。"); return; }
        show(old);
        output.println("请重新输入以下四项（不能为空，单号和状态不变）：");
        service.update(number, ask("收件人："), ask("联系电话："), ask("取件码："), ask("快递公司："));
        success();
    }

    private void delete() throws IOException {
        String number = ask("要删除的快递单号：");
        Express old = service.findByTrackingNumber(number);
        if (old == null) { output.println("未找到对应记录。"); return; }
        show(old);
        if (!ask("确认删除？输入 Y 确认，其他输入取消：").strip().equalsIgnoreCase("Y")) {
            output.println("已取消删除。");
            return;
        }
        service.delete(number);
        success();
    }

    private void success() { output.println("操作成功，已保存到本地文件。"); }
}
