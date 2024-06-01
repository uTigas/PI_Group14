import requests
from msg import RegisterUserMsg, ReturnRegisterMsg
from util import set_user_registry

set_user_registry({"self": "1234" * 8})
qkd_address = "localhost:5000"

def register_user():
    url = f"http://{qkd_address}/users"
    msg = RegisterUserMsg.construct()
    
    # Print the message to be sent for debugging
    print(f"Sending registration message: {msg}")

    try:
        response = requests.post(url, data=msg.encrypt())
        return response
    except requests.exceptions.RequestException as e:
        print(f"An error occurred during the request: {e}")
        return None

response = register_user()
if response and response.status_code == 200:
    print("User registered successfully")
    temp = ReturnRegisterMsg(response.content, "self")
    user, qkd_address, key = temp.loads()
    print(f"User: {user}, QKD Address: {qkd_address}, Key: {key}")
else:
    print("Error registering user")

