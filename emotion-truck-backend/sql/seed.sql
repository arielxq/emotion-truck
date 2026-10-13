USE emotion_truck;

-- 初始化裝置狀態資料
INSERT INTO device_state (device_id, source, current_state, bags_count, daily_count)
VALUES ('ESP32-001', 'HARDWARE', 'REST_CLOSED', 0, 0)
ON DUPLICATE KEY UPDATE
    source = VALUES(source),
    current_state = VALUES(current_state),
    bags_count = VALUES(bags_count),
    daily_count = VALUES(daily_count);

INSERT INTO device_state (device_id, source, current_state, bags_count, daily_count)
VALUES ('SIM-001', 'SIMULATION', 'REST_CLOSED', 0, 0)
ON DUPLICATE KEY UPDATE
    source = VALUES(source),
    current_state = VALUES(current_state),
    bags_count = VALUES(bags_count),
    daily_count = VALUES(daily_count);