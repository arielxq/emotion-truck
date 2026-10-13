-- 建立資料庫（若不存在）
CREATE DATABASE IF NOT EXISTS emotion_truck
  CHARACTER SET utf8mb4
  COLLATE utf8mb4_unicode_ci;

USE emotion_truck;

-- 表 1：emotion_log（情緒記錄）
CREATE TABLE IF NOT EXISTS emotion_log (
    id            INT AUTO_INCREMENT PRIMARY KEY,
    device_id     VARCHAR(32) NOT NULL,
    source        VARCHAR(16) DEFAULT 'HARDWARE',  -- HARDWARE / SIMULATION
    direction     VARCHAR(16),
    emotion_type  VARCHAR(32),
    emotion_text  VARCHAR(16),
    created_at    TIMESTAMP DEFAULT CURRENT_TIMESTAMP
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;

-- 表 2：collection_log（清運記錄）
CREATE TABLE IF NOT EXISTS collection_log (
    id             INT AUTO_INCREMENT PRIMARY KEY,
    device_id      VARCHAR(32) NOT NULL,
    source         VARCHAR(16) DEFAULT 'HARDWARE',
    worker_prompt  VARCHAR(32),
    bags_before    INT,
    bags_after     INT,
    returned_bags  INT DEFAULT 0,
    collected_at   TIMESTAMP DEFAULT CURRENT_TIMESTAMP
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;

-- 表 3：truck_schedule（每日排程）
CREATE TABLE IF NOT EXISTS truck_schedule (
    id                INT AUTO_INCREMENT PRIMARY KEY,
    schedule_date     DATE NOT NULL UNIQUE,
    play_time_1       TIME,
    play_time_2       TIME,
    is_holiday        TINYINT DEFAULT 0,
    collection_count  INT DEFAULT 0
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;

-- 表 4：device_state（裝置狀態）
CREATE TABLE IF NOT EXISTS device_state (
    id              INT AUTO_INCREMENT PRIMARY KEY,
    device_id       VARCHAR(32) NOT NULL UNIQUE,
    source          VARCHAR(16) DEFAULT 'HARDWARE',
    current_state   VARCHAR(32),
    bags_count      INT DEFAULT 0,
    daily_count     INT DEFAULT 0,
    last_heartbeat  TIMESTAMP DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;