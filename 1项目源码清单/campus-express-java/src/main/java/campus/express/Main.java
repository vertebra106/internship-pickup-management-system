package campus.express;

import java.io.Console;
import java.io.PrintWriter;
import java.nio.charset.StandardCharsets;
import java.nio.file.Path;
import java.util.Scanner;

/** 入口只组装依赖。相对路径以进程工作目录为基准。 */
public final class Main {
    private Main() { }

    public static void main(String[] args) {
        ExpressRepository repository = new ExpressRepository(Path.of("data", "express.tsv"));
        Console console = System.console();
        Scanner input = console == null ? new Scanner(System.in, StandardCharsets.UTF_8)
                : new Scanner(console.reader());
        PrintWriter output = console == null ? new PrintWriter(System.out, true, StandardCharsets.UTF_8)
                : console.writer();
        output.println("数据文件：" + repository.getFile());
        new ConsoleUI(new ExpressService(repository), input, output).run();
    }
}
