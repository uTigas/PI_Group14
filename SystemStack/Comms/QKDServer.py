from time import sleep
from fastapi import FastAPI , Request , Depends, Response
from fastapi.middleware.cors import CORSMiddleware
from contextlib import asynccontextmanager
import requests
from addressDB import *
from reconInt import *
from util import *
from msg import *

PIN = None

@asynccontextmanager
async def lifespan(app: FastAPI):
    logger.info("Starting QKD Server")
    logger.info("QKD Server started.")
    connect_to_db()
    logger.info("Connected to database.")
    init_db()
    logger.info("Database initialized.")
    load_pin()
    logger.info("PIN loaded.")
    load_address()
    logger.info("Address loaded.")
    load_user_registry()
    logger.info("User registry loaded.")
    yield
    disconnect_from_db()

app = FastAPI(lifespan=lifespan)

origins = [
    "http://localhost:8100",
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
    return {"status": "Success"}

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

    add = "http://localhost:" + os.getenv("QKD_PORT", "8000")
    logger.info(f"Requesting key from {address} {add}")
    if address == add:
        key = generate_key(size=1024)
        store_key_cache(tx_id, rx_id, key)
        store_key_cache(rx_id, tx_id, key)
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
    # remove_key_file(key_file_name)
    store_key_cache(tx_id, rx_id, key)

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
    # remove_key_file(key_file_name)
    store_key_cache(tx_id, rx_id, key)

    return {"status": "Success"}

@app.post("/keys/get/{tx_id}")
def get_keys(tx_id: str ,body: bytes = Depends(get_request_body)):
    try:
        logger.info(f'Get keys {tx_id} {body}')
        msg = GetKeysMsg(body,tx_id)
        tx_id = msg.loads()
    except InvalidMsg as e:
        return {"error": "Invalid message" , "msg": str(e) }
    except NoTxId:
        return {"error": "No Tx ID found"}
    
    keys = get_keys_from_tx_id(tx_id)
    temp = ReturnKeysMsg.construct(tx_id, keys)

    return temp.encrypt()

@app.post("/users")
def register_user(body: bytes = Depends(get_request_body)):
    try:
        msg = RegisterUserMsg(body,"self")
        msg.loads()
    except InvalidMsg:
        return {"error": "Invalid message"}
    add = "http://localhost:" + os.getenv("QKD_PORT", "8000")
    logger.info(f"Registering new user registered with ID. Address: {add} , {ADDRESS}")

    new_id , qkd_address = create_id(add)
    key = generate_key()

    logger.info(f"New user registered with ID: {new_id}")

    try:
        store_user_registry(new_id, key)
    except ValueError:
        return {"error": "Key must be 16, 24 or 32 bytes long"}
    

    return Response(content= ReturnRegisterMsg.construct(new_id, qkd_address, key).encrypt())
