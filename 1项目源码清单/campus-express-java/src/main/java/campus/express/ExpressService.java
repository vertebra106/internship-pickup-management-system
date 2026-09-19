package campus.express;

import java.io.IOException;
import java.util.ArrayList;
import java.util.List;

/** 集中实现唯一性、状态转换和“保存成功才提交内存”的业务规则。 */
public final class ExpressService {
    private final ExpressRepository repository;
    private final List<String> warnings;
    private ArrayList<Express> records;

    public ExpressService(ExpressRepository repository) {
        this.repository = repository;
        ExpressRepository.LoadResult loaded = repository.load();
        records = new ArrayList<>(loaded.getRecords());
        warnings = loaded.getWarnings();
    }

    public List<String> getWarnings() { return warnings; }
    public boolean isReadOnly() { return !warnings.isEmpty(); }

    /** 返回副本，外部删除列表元素不会改变业务数据；实体本身也是不可变的。 */
    public List<Express> findAll() { return new ArrayList<>(records); }

    public Express findByTrackingNumber(String trackingNumber) {
        String key = Express.requireText(trackingNumber, "快递单号");
        for (Express entry : records) {
            if (entry.getTrackingNumber().equals(key)) return entry;
        }
        return null;
    }

    public Express findByPickupCode(String pickupCode) {
        String key = Express.requireText(pickupCode, "取件码");
        for (Express entry : records) {
            if (entry.getPickupCode().equals(key)) return entry;
        }
        return null;
    }

    public void add(String number, String recipient, String phone, String code, String company) throws IOException {
        Express entry = new Express(number, recipient, phone, code, company, ExpressStatus.PENDING);
        if (findByTrackingNumber(entry.getTrackingNumber()) != null) {
            throw new IllegalArgumentException("快递单号已存在。");
        }
        ensureCodeAvailable(entry.getPickupCode(), null);
        ArrayList<Express> next = new ArrayList<>(records);
        next.add(entry);
        commit(next);
    }

    /** 修改保留原单号和状态；同一条记录保留自己的取件码是合法的。 */
    public void update(String number, String recipient, String phone, String code, String company) throws IOException {
        Express old = requireByNumber(number);
        Express updated = new Express(old.getTrackingNumber(), recipient, phone, code, company, old.getStatus());
        ensureCodeAvailable(updated.getPickupCode(), old.getTrackingNumber());
        ArrayList<Express> next = new ArrayList<>(records);
        next.set(next.indexOf(old), updated);
        commit(next);
    }

    /** 二次确认属于交互职责，UI 确认后才调用此方法。 */
    public void delete(String number) throws IOException {
        Express old = requireByNumber(number);
        ArrayList<Express> next = new ArrayList<>(records);
        next.remove(old);
        commit(next);
    }

    public void pickUp(String code) throws IOException {
        Express old = findByPickupCode(code);
        if (old == null) throw new IllegalArgumentException("未找到该取件码对应的快递。");
        if (old.getStatus() == ExpressStatus.PICKED_UP) {
            throw new IllegalArgumentException("该快递已取件，不能重复取件。");
        }
        Express picked = new Express(old.getTrackingNumber(), old.getRecipient(), old.getPhone(),
                old.getPickupCode(), old.getCompany(), ExpressStatus.PICKED_UP);
        ArrayList<Express> next = new ArrayList<>(records);
        next.set(next.indexOf(old), picked);
        commit(next);
    }

    private Express requireByNumber(String number) {
        Express entry = findByTrackingNumber(number);
        if (entry == null) throw new IllegalArgumentException("未找到该快递单号对应的记录。");
        return entry;
    }

    private void ensureCodeAvailable(String code, String excludedNumber) {
        Express found = findByPickupCode(code);
        if (found != null && !found.getTrackingNumber().equals(excludedNumber)) {
            throw new IllegalArgumentException("取件码已存在。");
        }
    }

    /** 候选列表保存失败时直接抛出异常，旧内存不变，不会误报操作成功。 */
    private void commit(ArrayList<Express> next) throws IOException {
        if (isReadOnly()) throw new IllegalStateException("数据文件存在读取问题，当前为只读模式。请备份、修复文件后重启。");
        repository.save(next);
        records = next;
    }
}
