package campus.express;

/** 一条快递记录。字段不可变，修改时创建新对象，避免绕过业务层直接改数据。 */
public final class Express {
    private final String trackingNumber;
    private final String recipient;
    private final String phone;
    private final String pickupCode;
    private final String company;
    private final ExpressStatus status;

    /** 构造时检查基本格式；跨记录的唯一性由 Service 检查。 */
    public Express(String trackingNumber, String recipient, String phone,
                   String pickupCode, String company, ExpressStatus status) {
        this.trackingNumber = requireText(trackingNumber, "快递单号");
        this.recipient = requireText(recipient, "收件人");
        this.phone = requireText(phone, "联系电话");
        this.pickupCode = requireText(pickupCode, "取件码");
        this.company = requireText(company, "快递公司");
        if (!this.phone.matches("[0-9]{7,15}")) {
            throw new IllegalArgumentException("联系电话必须为 7～15 位数字（保留前导零）。");
        }
        if (status == null) throw new IllegalArgumentException("取件状态不能为空。");
        this.status = status;
    }

    /** TSV 不支持字段内的换行或制表符；在所有入口统一拒绝控制字符。 */
    static String requireText(String value, String label) {
        if (value == null) throw new IllegalArgumentException(label + "不能为空。");
        for (int i = 0; i < value.length(); i++) {
            char ch = value.charAt(i);
            if (Character.isISOControl(ch) || ch == '\u2028' || ch == '\u2029') {
                throw new IllegalArgumentException(label + "不能含制表符、换行或其他控制字符。");
            }
        }
        String cleaned = value.strip();
        if (cleaned.isBlank()) throw new IllegalArgumentException(label + "不能为空。");
        if (cleaned.length() > 100) throw new IllegalArgumentException(label + "不能超过 100 个 UTF-16 字符单位。");
        return cleaned;
    }

    public String getTrackingNumber() { return trackingNumber; }
    public String getRecipient() { return recipient; }
    public String getPhone() { return phone; }
    public String getPickupCode() { return pickupCode; }
    public String getCompany() { return company; }
    public ExpressStatus getStatus() { return status; }
}
