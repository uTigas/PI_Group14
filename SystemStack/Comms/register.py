import requests
from msg import RegisterUserMsg, ReturnRegisterMsg
import qrcode

qkd_address = "localhost:5000"

def generate_qr_code(data: str):
    qr = qrcode.QRCode(
        version=1,
        error_correction=qrcode.constants.ERROR_CORRECT_L,
        box_size=10,
        border=4,
    )
    qr.add_data(data)
    qr.make(fit=True)
    img = qr.make_image(fill_color="black", back_color="white")
    img.save("qrcode.png")

    # Display the QR code
    img.show()

def register_user():
    url = f"http://{qkd_address}/users"
    msg = RegisterUserMsg.construct()
    
    # Print the message to be sent for debugging
    print(f"Sending registration message: {msg}")

    try:
        response = requests.post(url, data=msg.encrypt("1234" * 8))
        return response
    except requests.exceptions.RequestException as e:
        print(f"An error occurred during the request: {e}")
        return None

response = register_user()
if response and response.status_code == 200:
    print("User registered successfully")
    temp = ReturnRegisterMsg(response.content, "1234" * 8)
    user, qkd_address, key = temp.loads()
    print(f"User: {user}, QKD Address: {qkd_address}, Key: {key}")
    generate_qr_code(str({"user": user, "qkd_address": qkd_address, "key": key}))
else:
    print("Error registering user")

