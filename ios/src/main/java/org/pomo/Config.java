package org.pomo;
public final class Config {
    private Config() {}
    public static final int PREP_SEC = 180, WORK_SEC = 1500, SHORT_REST_SEC = 300, LONG_REST_SEC = 1200;
    public static final int LONG_REST_EVERY = 4;  // 每 4 個番茄鐘後長休息
    public static final int MAX_LEAVES = 3;       // 專注守護：離開超過 3 次即作廢
}
