import os
import random
from Crypto.Cipher import AES
from Crypto.Util.Padding import unpad , pad
import base64
import logging

userRegistry = {} # dict of user and key
keyCache = {} # dict of dict of key of tx_id and rx_id
timeCache = {} # dict of dict of time , last action of the user

PIN = None
ADDRESS = None

# Configure logging
logging.basicConfig(filename='requests.log', level=logging.INFO, format='%(asctime)s - %(name)s - %(levelname)s - %(message)s')

# Create a logger
logger = logging.getLogger(__name__)


def load_user_registry():
    global userRegistry
    with open("userRegistry.txt", "r") as f:
        for line in f:
            user, key = line.strip().split(" ")
            userRegistry[user] = key

def load_pin():
    global PIN
    PIN = os.getenv("QKD_PIN", "1234"*8)
    userRegistry["self"] = PIN

def load_address():
    global ADDRESS
    ADDRESS = "http://localhost:" + os.getenv("QKD_PORT", "8000")

# def get_address():
#     return "http://localhost:" + os.getenv("QKD_PORT", "8000")

def flush_user_registry():
    with open("userRegistry.txt", "a") as f:
        for user, key in userRegistry.items():
            f.write(f"{user} {key}\n")

def store_user_registry(user, key):
    if len(key) not in [16, 24, 32]:
        raise ValueError("Key must be 16, 24 or 32 bytes long")
    userRegistry[user] = key

def store_key_cache( tx_id, rx_id, key):
    if tx_id not in keyCache:
        keyCache[tx_id] = {}
    keyCache[tx_id][rx_id] = key

def store_time_cache(tx_id, time):
    timeCache[tx_id] = time

def get_key_cache(tx_id, rx_id):
    if tx_id not in keyCache:
        return None
    if rx_id not in keyCache[tx_id]:
        return None
    return keyCache[tx_id][rx_id]

def get_time_cache(tx_id):
    return timeCache.setdefault(tx_id, 0)

def get_user_registry(tx_id):
    return userRegistry[tx_id]

def get_keys_from_tx_id(tx_id):
    return {tx_id: userRegistry[tx_id]} if tx_id in userRegistry else {}

def in_user_registry(tx_id):
    return tx_id in userRegistry

def decrypt_msg(msg:str, key: str) -> str:
    cipher = AES.new(key.encode(), AES.MODE_CBC, iv='0000000000000000'.encode())
    return unpad(cipher.decrypt(base64.b64decode(msg)), 16, style = 'pkcs7').decode()

def encrypt_msg(msg: str, key: str) -> str:
    msg = pad(msg.encode(), 16, style = 'pkcs7')
    cipher = AES.new(key.encode(), AES.MODE_CBC , iv='0000000000000000'.encode())
    return base64.b64encode((cipher.encrypt(msg))).decode()

ALPHA = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789"
def generate_key(size=32):
    return "".join(random.choices(ALPHA, k=size))
