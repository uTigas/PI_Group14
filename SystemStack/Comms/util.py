import os
import random
from Crypto.Cipher import AES
from Crypto.Util.Padding import unpad , pad
import base64
import logging

timeCache = {} # dict of dict of time , last action of the user

PIN = None

# Configure logging
logging.basicConfig(filename='requests.log', level=logging.INFO, format=f'%(asctime)s - %(name)s - %(levelname)s {os.getenv("QKD_ADDRESS","localhost")}- %(message)s')

# Create a logger
logger = logging.getLogger(__name__)

def load_pin():
    global PIN
    PIN = os.getenv("QKD_PIN", "1234"*8)

def get_address_():
    return "http://" + os.getenv("QKD_ADDRESS","http://localhost:") + ":" + os.getenv("QKD_PORT", "8000")

def store_time_cache(tx_id, time):
    global timeCache
    timeCache[tx_id] = time

def get_time_cache(tx_id):
    global timeCache
    return timeCache.setdefault(tx_id, 0)

def decrypt_msg(msg:str, key: str) -> str:
    cipher = AES.new(key.encode(encoding='utf-8'), AES.MODE_CBC, iv='0000000000000000'.encode())
    return unpad(cipher.decrypt(base64.b64decode(msg)), 16, style = 'pkcs7').decode(encoding='utf-8')

def encrypt_msg(msg: str, key: str) -> str:
    msg = pad(msg.encode(), 16, style = 'pkcs7')
    cipher = AES.new(key.encode(encoding='utf-8'), AES.MODE_CBC , iv='0000000000000000'.encode())
    return base64.b64encode((cipher.encrypt(msg))).decode(encoding='utf-8')

ALPHA = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789"
def generate_key(size=32):
    return "".join(random.choices(ALPHA, k=size))
