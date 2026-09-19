package campus.express;

import java.io.IOException;
import java.io.PrintWriter;
import java.io.PrintStream;
import java.io.StringWriter;
import java.nio.charset.StandardCharsets;
import java.nio.file.Files;
import java.nio.file.Path;
import java.util.List;
import java.util.Scanner;
import java.util.concurrent.TimeUnit;

/** 无第三方测试框架；失败时抛出 AssertionError，并以非零退出码结束。 */
public final class ExpressTests {
    private static int passed;
    private static Path root;

    @FunctionalInterface
    private interface CheckedAction { void run() throws Exception; }

    private static void check(boolean condition, String message) {
        if (!condition) throw new AssertionError(message);
    }

    private static void test(String name, CheckedAction action) throws Exception {
        action.run();
        passed++;
        System.out.println("PASS " + name);
    }

    private static void expect(Class<? extends Throwable> type, CheckedAction action) throws Exception {
        try { action.run(); }
        catch (Exception ex) {
            if (type.isInstance(ex)) return;
            throw new AssertionError("异常类型错误：" + ex, ex);
        }
        throw new AssertionError("预期异常未发生：" + type.getSimpleName());
    }

    private static ExpressService service(Path file) { return new ExpressService(new ExpressRepository(file)); }
    private static void add(ExpressService service, String number, String code) throws IOException {
        service.add(number, "张三", "01234567890", code, "校园快递");
    }
    private static String ui(ExpressService service, String input) {
        StringWriter text = new StringWriter();
        new ConsoleUI(service, new Scanner(input), new PrintWriter(text)).run();
        return text.toString();
    }

    public static void main(String[] args) throws Exception {
        System.setOut(new PrintStream(System.out, true, StandardCharsets.UTF_8));
        root = Files.createTempDirectory("campus-express-tests-");
        System.out.println("测试证据目录：" + root);
        Path file = root.resolve("business/express.tsv");
        ExpressService service = service(file);
        test("首次启动无文件", () -> check(service.findAll().isEmpty() && !service.isReadOnly(), "应为空且可写"));
        test("正常新增、默认状态、电话前导零", () -> {
            add(service, "A001", "1-1");
            Express entry = service.findByTrackingNumber("A001");
            check(entry.getStatus() == ExpressStatus.PENDING, "状态");
            check(entry.getPhone().equals("01234567890"), "前导零");
        });
        test("重复单号", () -> expect(IllegalArgumentException.class, () -> add(service, " A001 ", "1-2")));
        test("重复取件码", () -> expect(IllegalArgumentException.class, () -> add(service, "A002", "1-1")));
        test("两种查询及不存在查询", () -> {
            check(service.findByPickupCode("1-1").getTrackingNumber().equals("A001"), "取件码查询");
            check(service.findByTrackingNumber("A001") != null, "单号查询");
            check(service.findByPickupCode("missing") == null, "不存在查询");
        });
        test("修改取件码唯一性（含磁盘不变）", () -> {
            add(service, "A002", "1-2");
            String before = Files.readString(file);
            expect(IllegalArgumentException.class, () -> service.update("A001", "李四", "1234567", "1-2", "公司"));
            check(service.findByTrackingNumber("A001").getPickupCode().equals("1-1"), "内存被错误修改");
            check(Files.readString(file).equals(before), "磁盘被错误修改");
        });
        test("修改保留自己的码及更换新码", () -> {
            service.update("A001", "李四", "1234567", "1-1", "新公司");
            service.update("A001", "王五", "00012345678", "2-1", "公司,\"分部\"");
            check(service.findByPickupCode("1-1") == null, "旧码仍存在");
            check(service.findByPickupCode("2-1").getRecipient().equals("王五"), "修改失败");
        });
        test("取件及自动保存", () -> {
            service.pickUp("2-1");
            check(service(file).findByPickupCode("2-1").getStatus() == ExpressStatus.PICKED_UP, "取件未保存");
        });
        test("禁止重复取件", () -> expect(IllegalArgumentException.class, () -> service.pickUp("2-1")));
        test("已取件后修改不重置状态", () -> {
            service.update("A001", "王五", "00012345678", "2-1", "公司,\"分部\"");
            check(service.findByPickupCode("2-1").getStatus() == ExpressStatus.PICKED_UP, "状态被重置");
        });
        test("已取件记录仍占用取件码", () -> expect(IllegalArgumentException.class, () -> add(service, "A003", "2-1")));
        test("重新创建服务读取全部字段", () -> {
            Express saved = service(file).findByTrackingNumber("A001");
            check(saved.getRecipient().equals("王五") && saved.getPhone().equals("00012345678")
                    && saved.getCompany().equals("公司,\"分部\"") && saved.getPickupCode().equals("2-1"), "字段往返失败");
        });
        test("不存在记录的修改删除取件", () -> {
            expect(IllegalArgumentException.class, () -> service.update("missing", "张三", "1234567", "x", "公司"));
            expect(IllegalArgumentException.class, () -> service.delete("missing"));
            expect(IllegalArgumentException.class, () -> service.pickUp("missing"));
        });
        test("空值、空白、非法电话、过长字段", () -> {
            expect(IllegalArgumentException.class, () -> add(service, " ", "x"));
            expect(IllegalArgumentException.class, () -> add(service, null, "x"));
            expect(IllegalArgumentException.class, () -> service.add("B", "张三", "abc", "x", "公司"));
            expect(IllegalArgumentException.class, () -> service.add("B", "", "1234567", "x", "公司"));
            expect(IllegalArgumentException.class, () -> add(service, "B", ""));
            expect(IllegalArgumentException.class, () -> service.add("B", "张三", "1234567", "x", " "));
            expect(IllegalArgumentException.class, () -> add(service, "B".repeat(101), "x"));
        });
        test("拒绝制表符、换行及控制字符", () -> {
            for (String bad : List.of("a\tb", "a\nb", "a\rb", "a\u0000b", "a\u2028b")) {
                expect(IllegalArgumentException.class, () -> service.add("B", bad, "1234567", "x", "公司"));
            }
        });
        test("查询列表不能绕过业务层修改", () -> {
            service.findAll().clear();
            check(service.findAll().size() == 2, "内部列表泄露");
        });
        test("菜单、查询选项和空输入反馈", () -> {
            String output = ui(service, "abc\n\n3\n9\n3\n2\nmissing\n0\n");
            check(output.contains("无效菜单选项") && output.contains("无效查询方式") && output.contains("未找到"), "反馈缺失");
        });
        test("删除取消后数据仍在", () -> {
            check(ui(service, "5\nA002\nn\n0\n").contains("已取消删除"), "未取消");
            check(service(file).findByTrackingNumber("A002") != null, "误删");
        });
        test("确认删除并保存", () -> {
            check(ui(service, "5\nA002\nY\n0\n").contains("操作成功"), "未成功");
            check(service.findByTrackingNumber("A002") == null && service(file).findByTrackingNumber("A002") == null, "未删除");
        });
        test("输入中途结束不提交", () -> {
            String output = ui(service, "1\nB001\n张三\n");
            check(output.contains("输入已结束") && service.findByTrackingNumber("B001") == null, "部分输入被提交");
        });
        test("空文件正常读取", () -> {
            Path empty = Files.createFile(root.resolve("empty.tsv"));
            check(service(empty).findAll().isEmpty() && !service(empty).isReadOnly(), "空文件异常");
        });
        test("坏行、重复键、未知状态、空列：跳过并保护原文件", () -> {
            Path corrupt = root.resolve("corrupt.tsv");
            String good = "C001\t赵六\t1234567\tc1\t公司\tPENDING\n";
            String content = good + "broken\n" + good
                    + "C002\t赵六\t1234567\tc2\t公司\tUNKNOWN\n"
                    + "C003\t\t1234567\tc3\t公司\tPENDING\n"
                    + "C004\t赵六\t1234567\tc1\t公司\tPENDING\n"
                    + "C005\t赵六\t1234567\tc5\t公司\tPENDING\n";
            Files.writeString(corrupt, content, StandardCharsets.UTF_8);
            ExpressService loaded = service(corrupt);
            check(loaded.findAll().size() == 2 && loaded.getWarnings().size() == 5 && loaded.isReadOnly(), "坏行处理");
            expect(IllegalStateException.class, () -> add(loaded, "C006", "c6"));
            expect(IllegalStateException.class, () -> loaded.update("C001", "新姓名", "1234567", "c1", "公司"));
            expect(IllegalStateException.class, () -> loaded.delete("C001"));
            expect(IllegalStateException.class, () -> loaded.pickUp("c1"));
            check(Files.readString(corrupt).equals(content), "损坏文件被覆盖");
            check(ui(loaded, "2\n0\n").contains("C005"), "坏行后有效记录无法查看");
        });
        test("文件读取异常进入只读", () -> {
            ExpressService broken = service(Files.createDirectory(root.resolve("directory.tsv")));
            check(broken.isReadOnly() && !broken.getWarnings().isEmpty(), "读取失败未提示");
            expect(IllegalStateException.class, () -> add(broken, "D", "d"));
        });
        test("非法 UTF-8 不崩溃且禁止覆盖", () -> {
            Path invalid = root.resolve("invalid.tsv");
            Files.write(invalid, new byte[] {(byte) 0xc3, (byte) 0x28});
            check(service(invalid).isReadOnly(), "解码错误未保护");
        });
        test("保存失败：新增修改删除取件均不改变内存", () -> {
            Path parent = Files.createDirectory(root.resolve("save-failure"));
            Path target = parent.resolve("express.tsv");
            ExpressService failing = service(target);
            add(failing, "F001", "f1");
            Path backup = root.resolve("saved-before-failure.tsv");
            Files.move(target, backup);
            Files.delete(parent);
            Files.writeString(parent, "模拟父目录被普通文件占用");
            expect(IOException.class, () -> add(failing, "F002", "f2"));
            expect(IOException.class, () -> failing.update("F001", "新名字", "1234567", "new", "公司"));
            expect(IOException.class, () -> failing.delete("F001"));
            expect(IOException.class, () -> failing.pickUp("f1"));
            check(failing.findAll().size() == 1 && failing.findByPickupCode("f1").getStatus() == ExpressStatus.PENDING
                    && failing.findByPickupCode("f1").getRecipient().equals("张三"), "失败污染内存");
            String output = ui(failing, "6\nf1\n0\n");
            check(output.contains("保存失败") && !output.contains("操作成功"), "误报成功");
            Files.delete(parent);
            Files.createDirectory(parent);
            Files.move(backup, target);
            check(service(target).findByPickupCode("f1").getStatus() == ExpressStatus.PENDING, "旧文件不完整");
            failing.pickUp("f1");
            check(service(target).findByPickupCode("f1").getStatus() == ExpressStatus.PICKED_UP, "恢复后不能重试");
        });
        test("删除最后一条后重启为空", () -> {
            service.delete("A001");
            check(Files.size(file) == 0 && service(file).findAll().isEmpty(), "最后记录未删除");
        });
        test("原子替换失败保留内存且清理临时文件", () -> {
            Path folder = Files.createDirectory(root.resolve("replace-failure"));
            Path target = folder.resolve("express.tsv");
            ExpressService failing = service(target);
            add(failing, "R001", "r1");
            Path backup = folder.resolve("backup.tsv");
            Files.move(target, backup);
            Files.createDirectory(target);
            Files.writeString(target.resolve("occupied.txt"), "阻止文件替换非空目录");
            expect(IOException.class, () -> failing.pickUp("r1"));
            check(failing.findByPickupCode("r1").getStatus() == ExpressStatus.PENDING, "内存被修改");
            check(service(backup).findByPickupCode("r1").getStatus() == ExpressStatus.PENDING, "备份被修改");
            try (var files = Files.list(folder)) {
                check(files.noneMatch(path -> path.toString().endsWith(".tmp")), "临时文件未清理");
            }
        });
        test("界面校验失败后可继续正常新增", () -> {
            ExpressService fresh = service(root.resolve("ui-recovery.tsv"));
            String output = ui(fresh, "1\nU001\n张三\nabc\nu1\n公司\n1\nU001\n张三\n1234567\nu1\n公司\n2\n0\n");
            check(output.contains("操作失败") && output.contains("操作成功") && output.contains("状态：待取件"), "界面无法恢复");
            check(fresh.findAll().size() == 1, "新增数量错误");
        });
        test("真实进程：新增→退出→重启查询→修改→取件→再次重启", () -> {
            Path work = Files.createDirectory(root.resolve("process-demo"));
            String first = process(work, "1\nP001\n中文姓名\n01234567890\np1\n公司,\"分部\"\n0\n", "first");
            check(first.contains("操作成功") && first.contains("已退出"), "首次进程失败");
            String second = process(work, "3\n1\np1\n4\nP001\n新姓名\n01234567890\np2\n新公司\n6\np2\n6\np2\n0\n", "second");
            check(second.contains("中文姓名") && second.contains("待取件") && second.contains("不能重复取件"), "重启查询或取件失败");
            String third = process(work, "3\n2\nP001\n0\n", "third");
            check(third.contains("新姓名") && third.contains("p2") && third.contains("已取件"), "最终重启读取失败");
        });
        System.out.println("RESULT: " + passed + " passed, 0 failed");
    }

    /** 独立 JVM 使用临时工作目录，绝不接触项目的真实 data 文件。 */
    private static String process(Path work, String input, String label) throws Exception {
        String java = Path.of(System.getProperty("java.home"), "bin", "java.exe").toString();
        String classes = Path.of("build", "classes").toAbsolutePath().toString();
        Path stdin = work.resolve(label + "-input.txt");
        Path stdout = work.resolve(label + "-output.txt");
        Files.writeString(stdin, input, StandardCharsets.UTF_8);
        Process process = new ProcessBuilder(java, "-cp", classes, "campus.express.Main")
                .directory(work.toFile()).redirectInput(stdin.toFile()).redirectOutput(stdout.toFile())
                .redirectErrorStream(true).start();
        if (!process.waitFor(20, TimeUnit.SECONDS)) {
            process.destroyForcibly();
            throw new AssertionError("子进程超时");
        }
        String output = Files.readString(stdout, StandardCharsets.UTF_8);
        check(process.exitValue() == 0, "进程异常退出：" + output);
        return output;
    }
}
