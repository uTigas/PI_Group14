import os
import base64

BASE_FOLDER = "GeneratedKeys"
BASE_EXT = "b64"

def get_key_file(tried_keys: list, ext = BASE_EXT) -> tuple[int, str]:
    key_file = None
    for file in os.listdir(BASE_FOLDER):
        if file.endswith(ext):
            key_file = int(file[6:len(file)-len(ext)-1])
            if key_file not in tried_keys:
                break
            
    return key_file , file

def exists_key_file(key_file_name: str) -> bool:
    return os.path.exists(f"{BASE_FOLDER}/{key_file_name}")

def exists_key_file(key_file: int) -> bool:
    for file in os.listdir(BASE_FOLDER):
        if file.endswith(BASE_EXT):
            if key_file == int(file[6:len(file)-len(BASE_EXT)-1]):
                return True , file
    return False , None

def open_key_file(key_file_name: str) -> str:
    with open(f"{BASE_FOLDER}/{key_file_name}", "rb") as file:
        return base64.b64decode(file.read())
            
def remove_key_file(key_file_name: str):
    try:
        os.remove(f"{BASE_FOLDER}/{key_file_name}")
    except FileNotFoundError:
        pass

get_key_file([])