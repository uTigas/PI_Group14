import os

userRegistry = {}

keyCache = {} # dict of dict of key

PIN = None

def load_user_registry():
    global userRegistry
    with open("userRegistry.txt", "r") as f:
        for line in f:
            user, key = line.strip().split(" ")
            userRegistry[user] = key

def load_pin():
    global PIN
    PIN = os.getenv("QKD_PIN", "1234")

def store_user_registry():
    with open("userRegistry.txt", "a") as f:
        for user, key in userRegistry.items():
            f.write(f"{user} {key}\n")

def store_key_cache( tx_id, rx_id, key):
    keyCache[tx_id][rx_id] = key

def get_key_cache(tx_id, rx_id):
    # if the key is not in the cache, return None
    if tx_id not in keyCache:
        return None
    if rx_id not in keyCache[tx_id]:
        return None
    return keyCache[tx_id][rx_id]