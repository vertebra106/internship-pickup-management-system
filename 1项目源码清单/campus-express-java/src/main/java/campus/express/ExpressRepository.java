package campus.express;

import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.IOException;
import java.nio.charset.StandardCharsets;
import java.nio.file.Files;
import java.nio.file.NoSuchFileException;
import java.nio.file.Path;
import java.nio.file.StandardCopyOption;
import java.util.ArrayList;
import java.util.List;

/** 只负责 UTF-8 文件与对象之间的转换，不负责菜单。 */
public final class ExpressRepository {
    private final Path file;

    public ExpressRepository(Path file) { this.file = file.toAbsolutePath().normalize(); }
    public Path getFile() { return file; }

    /** 同时返回有效记录和诊断；有诊断时，Service 会进入只读保护。 */
    public static final class LoadResult {
        private final ArrayList<Express> records = new ArrayList<>();
        private final ArrayList<String> warnings = new ArrayList<>();
        public List<Express> getRecords() { return List.copyOf(records); }
        public List<String> getWarnings() { return List.copyOf(warnings); }
    }

    /** 缺文件视为空数据；单条坏记录不影响后续记录；读取失败不伪装成正常空库。 */
    public LoadResult load() {
        LoadResult result = new LoadResult();
        try (BufferedReader reader = Files.newBufferedReader(file, StandardCharsets.UTF_8)) {
            String line;
            int lineNumber = 0;
            while ((line = reader.readLine()) != null) {
                lineNumber++;
                try {
                    String[] fields = line.split("\t", -1);
                    if (fields.length != 6) throw new IllegalArgumentException("应有 6 列");
                    Express entry = new Express(fields[0], fields[1], fields[2], fields[3],
                            fields[4], ExpressStatus.valueOf(fields[5]));
                    for (Express old : result.records) {
                        if (old.getTrackingNumber().equals(entry.getTrackingNumber())
                                || old.getPickupCode().equals(entry.getPickupCode())) {
                            throw new IllegalArgumentException("快递单号或取件码重复");
                        }
                    }
                    result.records.add(entry);
                } catch (IllegalArgumentException ex) {
                    result.warnings.add("第 " + lineNumber + " 行已跳过：" + ex.getMessage());
                }
            }
        } catch (NoSuchFileException ex) {
            // 首次运行时不要求用户先手工创建数据文件。
        } catch (IOException | SecurityException ex) {
            result.warnings.add("读取失败，已读到的记录仅供查看：" + ex.getMessage());
        }
        return result;
    }

    /** 先写同目录临时文件，再原子替换，防止写到一半破坏原文件。 */
    public void save(List<Express> entries) throws IOException {
        Files.createDirectories(file.getParent());
        Path temporary = Files.createTempFile(file.getParent(), "express-", ".tmp");
        try {
            try (BufferedWriter writer = Files.newBufferedWriter(temporary, StandardCharsets.UTF_8)) {
                for (Express entry : entries) {
                    writer.write(String.join("\t", entry.getTrackingNumber(), entry.getRecipient(),
                            entry.getPhone(), entry.getPickupCode(), entry.getCompany(), entry.getStatus().name()));
                    writer.newLine();
                }
            }
            // 不支持原子替换时明确失败，不降级为可能损坏旧文件的覆盖写入。
            Files.move(temporary, file, StandardCopyOption.ATOMIC_MOVE, StandardCopyOption.REPLACE_EXISTING);
        } finally {
            try {
                Files.deleteIfExists(temporary);
            } catch (IOException | SecurityException ignored) {
                // 清理失败不改变保存结果；可能留下临时文件，启动时不会读取它。
            }
        }
    }
}
