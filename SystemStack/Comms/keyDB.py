import sqlite3
import os

create_table_key = """CREATE TABLE IF NOT EXISTS keys (
    tx_id TEXT NOT NULL,
    rx_id TEXT NOT NULL,
    key TEXT NOT NULL
);"""

insert_key = """INSERT INTO keys (tx_id, rx_id, key) VALUES (?, ?, ?);"""

select_key = """SELECT key FROM keys WHERE tx_id = ? AND rx_id = ?;"""

select_keys = """SELECT rx_id, key FROM keys WHERE tx_id = ?;"""

select_all = """SELECT * FROM keys;"""


conn = None

def init_db_connection():
    global conn
    if conn is None:
        conn = sqlite3.connect(os.getenv("KEY_DB", "keys1.db"), check_same_thread=False)
    return conn

def init_db_key():
    global conn
    cur = conn.cursor()
    cur.execute(create_table_key)
    conn.commit()
    cur.close()

def store_key_cache( tx_id: str, rx_id: str, key: str) -> None:
    global conn
    cur = conn.cursor()
    cur.execute(insert_key, (tx_id, rx_id, key))
    conn.commit()
    cur.close()
    
def get_key_cache(tx_id: str, rx_id: str) -> str:
    global conn
    cur = conn.cursor()
    cur.execute(select_key, (tx_id, rx_id))
    key = cur.fetchone()
    cur.close()
    return key[0] if key else None

def get_key_cache_() -> dict: 
    global conn
    cur = conn.cursor()
    cur.execute(select_all)
    keys = cur.fetchall()
    cur.close()
    return {(tx_id, rx_id): key for tx_id, rx_id, key in keys}

def get_keys_from_tx_id(tx_id: str) -> dict:
    global conn
    cur = conn.cursor()
    cur.execute(select_keys, (tx_id,))
    keys = cur.fetchall()
    cur.close()
    return {rx_id: key for rx_id, key in keys}

create_table_user = """CREATE TABLE IF NOT EXISTS users (
    user TEXT NOT NULL,
    key TEXT NOT NULL
);"""

insert_user = """INSERT INTO users (user, key) VALUES (?, ?);"""

select_user = """SELECT key FROM users WHERE user = ?;"""

select_all_user = """SELECT * FROM users;"""

def init_db_user():
    global conn
    cur = conn.cursor()
    cur.execute(create_table_user)
    cur.execute(insert_user, ("self", os.getenv("QKD_PIN", "1234"*8)))
    conn.commit()
    cur.close()

def get_user_registry_() -> dict:
    global conn
    cur = conn.cursor()
    cur.execute(select_all_user)
    users = cur.fetchall()
    cur.close()
    return {user: key for user, key in users}

def get_user_registry(tx_id: str) -> str:
    global conn
    cur = conn.cursor()
    cur.execute(select_user, (tx_id,))
    key = cur.fetchone()
    cur.close()
    return key[0] if key else None

def in_user_registry(tx_id: str) -> bool:
    global conn
    cur = conn.cursor()
    cur.execute(select_user, (tx_id,))
    key = cur.fetchone()
    cur.close()
    return key is not None

def store_user_registry(user: str, key: str) -> None:
    global conn
    cur = conn.cursor()
    cur.execute(insert_user, (user, key))
    conn.commit()
    cur.close()

