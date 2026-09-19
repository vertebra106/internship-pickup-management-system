package campus.express;

/** 用有限的状态代替容易拼错的任意字符串。 */
public enum ExpressStatus {
    PENDING("待取件"), PICKED_UP("已取件");

    private final String label;

    ExpressStatus(String label) { this.label = label; }

    public String getLabel() { return label; }
}
