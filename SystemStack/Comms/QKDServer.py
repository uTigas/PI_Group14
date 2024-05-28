from fastapi import FastAPI , Request , Depends
from contextlib import asynccontextmanager
import requests
from addressDB import *
from reconInt import *
from util import *
from msg import *

PIN = None

@asynccontextmanager
async def lifespan(app: FastAPI):
    connect_to_db()
    init_db()
    load_pin()
    load_address()
    load_user_registry()
    yield
    disconnect_from_db()

app = FastAPI(lifespan=lifespan)

async def get_request_body(request: Request):
    return await request.body()

@app.post("/keys/{tx_id}")
def get_key(tx_id: str , body: bytes = Depends(get_request_body)):
    try:
        msg = GetKeyMsg(body,tx_id)
        tx_id, rx_id = msg.loads()
    except InvalidMsg:
        return {"error": "Invalid message"}
    except NoTxId:
        return {"error": "No Tx ID found"}
        
    address = get_address(rx_id)
    if address is None:
        return {"error": "No Rx ID address found"}

    if get_key_cache(tx_id, rx_id) == "generating":
        return {"error": "Key is being generated"}
    
    if get_key_cache(tx_id, rx_id) is not None:
        return {"error": "Key already exists"}

    store_key_cache(tx_id, rx_id, "generating")

    if address == ADDRESS:
        key = generate_key(size=1024)
        store_key_cache(tx_id, rx_id, key.encode())
        store_key_cache(rx_id, tx_id, key.encode())
        return {"status": "Success"}

    tried_keys = []
    for _ in range(5):
        key_file , key_file_name = get_key_file(tried_keys)
        reconn_msg = ReconnMsg.construct(tx_id, rx_id, key_file)
        response = requests.post(address[0] + "/recon", headers={"Content-Type": "application/json"}, data=str(reconn_msg))

        if response.status_code == 200:
            break
        else:
            tried_keys.append(key_file)
            remove_key_file(key_file_name)

    if response.status_code != 200:
        return {"error": "Failed to establish consensus"}
    
    key = open_key_file(key_file_name)
    remove_key_file(key_file_name)
    store_key_cache(tx_id, rx_id, key.encode())

    return {"status": "Success"}

@app.post("/recon")
def reconn(body: bytes = Depends(get_request_body)):
    try:
        msg = ReconnMsg(body)
        tx_id, rx_id, desired_key = msg.loads()
    except InvalidMsg:
        return {"error": "Invalid message"}
    
    if rx_id not in userRegistry:
        return {"error": "No Tx ID found"}
    
    exist , key_file_name = exists_key_file(desired_key)
    if not exist:
        return {"error": "Key not found"}
    
    key = open_key_file(key_file_name)
    remove_key_file(key_file_name)
    store_key_cache(tx_id, rx_id, key.encode())

    return {"status": "Success"}

@app.get("/keys")
def get_keys(body: bytes = Depends(get_request_body)):
    try:
        msg = GetKeysMsg(body)
        tx_id = msg.loads()
    except InvalidMsg:
        return {"error": "Invalid message"}
    except NoTxId:
        return {"error": "No Tx ID found"}
    
    keys = get_keys_from_tx_id(tx_id)
    temp = ReturnKeysMsg.construct(tx_id, keys)

    return temp.encrypt()

@app.post("/users")
def register_user(body: bytes = Depends(get_request_body)):
    try:
        msg = RegisterUserMsg(body,"self")
        msg.load()
    except InvalidMsg:
        return {"error": "Invalid message"}
    
    new_id , qkd_address = create_id(ADDRESS)
    key = generate_key()

    try:
        store_user_registry(new_id)
    except ValueError:
        return {"error": "Key must be 16, 24 or 32 bytes long"}
    

    return ReturnRegisterMsg.construct(new_id, qkd_address, key).encrypt()
