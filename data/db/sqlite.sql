CREATE TABLE IF NOT EXISTS Highscore (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    player TEXT NOT NULL,
    move TINYINT NOT NULL,
    turn TINYINT NOT NULL,
    score TINYINT NOT NULL,
    at DATETIME DEFAULT current_timestamp
);

CREATE TABLE IF NOT EXISTS Event (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    player TEXT NOT NULL,
    turn TINYINT NOT NULL,
    action TEXT NOT NULL,
    end BOOLEAN NOT NULL,
    at DATETIME DEFAULT current_timestamp
);