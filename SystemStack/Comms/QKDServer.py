from fastapi import FastAPI
from Crypto.Cipher import AES
from contextlib import asynccontextmanager
import requests
from addressDB import *
from reconInt import *
from util import *

PIN = None

@asynccontextmanager
async def lifespan(app: FastAPI):
    connect_to_db()
    init_db()
    load_pin()
    load_user_registry()
    yield
    disconnect_from_db()


app = FastAPI(lifespan=lifespan)
@app.get("/key/{tx_id}/{rx_id}/{tx_challenge}")
def get_key(tx_id: str, rx_id: str, tx_challenge: str):

    # check if the user exists in the registry
    if tx_id not in userRegistry:
        return {"error": "User not found"}
    
    # encrypt rx_id with tx_id's key and check if it matches tx_challenge
    key = userRegistry[tx_id]
    cipher = AES.new(key.encode(), AES.MODE_ECB)
    encrypted_rx_id = str(cipher.encrypt(rx_id.encode()).hex())
    if encrypted_rx_id != tx_challenge:
        return {"error": "Invalid challenge"}
    
    address = get_address(rx_id)
    if address is None:
        return {"error": "Address not found"}

    if get_key_cache(tx_id, rx_id) == "generating":
        return {"error": "Key is being generated"}
    
    if get_key_cache(tx_id, rx_id) is not None:
        return {"error": "Key already exists"}

    store_key_cache(tx_id, rx_id, "generating")

    tried_keys = []
    for i in range(5):
        key_file , key_file_name = get_key_file()
        response = requests.post(address[0] + "/reconn", data={"tx_id": tx_id, "rx_id": rx_id, "desired_key": key_file})

        if response.status_code == 200:
            break
        else:
            tried_keys.append(key_file)
            remove_key_file(key_file_name)

    if response.status_code != 200:
        return {"error": "Failed to establish connection"}
    
    # open the key file and return the key encrypted with tx_id's key
    key = open_key_file(key_file_name)
    cipher = AES.new(userRegistry[tx_id].encode(), AES.MODE_ECB)
    encrypted_key = cipher.encrypt(key)

    remove_key_file(key_file_name)

    store_key_cache(tx_id, rx_id, encrypted_key.hex())

    return {"status": "Success"}

@app.post("/reconn")
def reconn(tx_id: str, rx_id: str, desired_key: int):
    if rx_id not in userRegistry:
        return {"error": "User not found"}
    
    exist , file_key = exists_key_file(desired_key)
    if not exist:
        return {"error": "Key not found"}
    
    key = open_key_file(file_key)
    cipher = AES.new(userRegistry[rx_id].encode(), AES.MODE_ECB)
    encrypted_key = cipher.encrypt(key)

    remove_key_file(file_key)

    store_key_cache(tx_id, rx_id, encrypted_key.hex())

    return {"status": "Success"}

   
    
    



