from time import sleep
from fastapi import FastAPI , Request , Depends, Response
from fastapi.middleware.cors import CORSMiddleware
from contextlib import asynccontextmanager
import requests
from addressDB import *
from reconInt import *
from util import *
from msg import *
from keyDB import *

@asynccontextmanager
async def lifespan(app: FastAPI):
    logger.info("Starting QKD Server")
    connect_to_db()
    init_db_connection()
    logger.info("Connected to database.")
    init_db()
    init_db_key()
    init_db_user()
    logger.info("Database initialized.")
    yield
    disconnect_from_db()

app = FastAPI(lifespan=lifespan)

origins = [
    "*"
]

# Add the CORS middleware
app.add_middleware(
    CORSMiddleware,
    allow_origins=origins, 
    allow_credentials=True,
    allow_methods=["*"],  # Allows all methods (GET, POST, etc.)
    allow_headers=["*"],  # Allows all headers
)

async def get_request_body(request: Request):
    return await request.body()

@app.get("/")
def root():
    logger.info("Root")
    return {"status": "Success"}

@app.post("/keys/{tx_id}")
def get_key(tx_id: str , body: bytes = Depends(get_request_body)):
    logger.info("Requesting keys %s", tx_id)
    try:
        msg = GetKeyMsg(body, get_user_registry(tx_id))
        tx_id, rx_id = msg.loads()
    except InvalidMsg:
        return {"error": "Invalid message"}
    except NoTxId:
        return {"error": "No Tx ID found"}
        
    address = get_address(rx_id)
    if address is None:
        return {"error": "No Rx ID address found"}
    logger.info("Address found %s", address)
    
    # if  get_key_cache(tx_id, rx_id) is not None:
    #     return {"error": "Key already exists"}

    store_key_cache(tx_id, rx_id, "generating")

    self_address = get_address_()
    if address == self_address:
        key = generate_key(size=32)
        store_key_cache(tx_id, rx_id, key)
        store_key_cache(rx_id, tx_id, key)
        return {"status": "Success"}

    tried_keys = []
    for _ in range(5):
        key_file , key_file_name = get_key_file(tried_keys)
        logger.info(f"Trying key file {key_file_name} {address}")
        reconn_msg = ReconnMsg.construct(tx_id, rx_id, key_file)
        response = requests.post("http://" + address + "/recon", headers={"Content-Type": "application/json"}, data=str(reconn_msg))
        logger.info(f"Response from {address}: {response.status_code} {response.text}")
        if response.status_code == 200:
            break
        else:
            tried_keys.append(key_file)
            # remove_key_file(key_file_name)

    if response.status_code != 200:
        return {"error": "Failed to establish consensus"}
    
    logger.info("Key established")

    key = open_key_file(key_file_name)
    # remove_key_file(key_file_name)
    store_key_cache(tx_id, rx_id, key)
    logger.info(f"Key stored {tx_id} {rx_id} {key}")

    return {"status": "Success"}

@app.post("/recon")
def reconn(body: bytes = Depends(get_request_body)):
    logger.info("Recon message received")
    try:
        msg = ReconnMsg(body)
        tx_id, rx_id, desired_key = msg.loads()
        logger.info("Recon message received %s %s %s", tx_id, rx_id, desired_key)
    except InvalidMsg:
        return {"error": "Invalid message"} , 400
    
    if not in_user_registry(rx_id):
        return {"error": "No Rx ID found"} , 400
    
    exist , key_file_name = exists_key_file(int(desired_key))
    logger.info(f"Key file exists {exist} {key_file_name}")
    if not exist:
        return {"error": "Key not found"} , 400
    
    key = open_key_file(key_file_name)
    # remove_key_file(key_file_name)
    store_key_cache(rx_id, tx_id, key)
    logger.info(f"Key stored {rx_id} {tx_id} {key}")

    return {"status": "Success"}

@app.post("/keys/get/{tx_id}")
def get_keys(tx_id: str ,body: bytes = Depends(get_request_body)):
    logger.info("Getting keys from tx_id %s", tx_id)
    try:
        msg = GetKeysMsg(body, get_user_registry(tx_id))
        tx_id = msg.loads()
    except InvalidMsg as e:
        logger.error("Invalid message %s", str(e))
        return {"error": "Invalid message" , "msg": str(e) }
    except NoTxId:
        logger.error("No Tx ID found")
        return {"error": "No Tx ID found"}
    
    keys =  get_keys_from_tx_id(tx_id)
    temp = ReturnKeysMsg.construct(tx_id, keys)
    logger.info("Returning keys %s", keys)

    return temp.encrypt(  get_user_registry(tx_id) )

@app.post("/keys/get/{tx_id}/{rx_id}")
def get_key_from_tx_id(tx_id: str , rx_id: str , body: bytes = Depends(get_request_body)):
    logger.info("Getting key from tx_id %s %s", tx_id, rx_id)
    try:
        msg = GetKeysMsg(body, get_user_registry(tx_id))
        tx_id = msg.loads()
    except InvalidMsg:
        return {"error": "Invalid message"}
    except NoTxId:
        return {"error": "No Tx ID found"}
    
    key =  get_key_cache(tx_id, rx_id)
    if rx_id is None:
        return {"error": "Key not found"}
    
    temp = ReturnKeysMsg.construct(tx_id, {rx_id: key})
    return temp.encrypt(  get_user_registry(tx_id) )

@app.post("/users")
def register_user(body: bytes = Depends(get_request_body)):
    logger.info("Registering new user")
    try:
        msg = RegisterUserMsg(body, get_user_registry("self"))
        msg.loads()
    except InvalidMsg as e:
        return {"error": "Invalid message" , "msg": str(e)}
    
    add = get_address_()
    new_id , qkd_address = create_id(add)
    key = generate_key()

    try:
        store_user_registry(new_id, key)
    except ValueError:
        return {"error": "Key must be 16, 24 or 32 bytes long"}
    logger.info("New user registered %s %s %s", new_id, qkd_address, key)
    return Response(content= ReturnRegisterMsg.construct(new_id, qkd_address, key).encrypt( get_user_registry("self")), media_type="application/json")