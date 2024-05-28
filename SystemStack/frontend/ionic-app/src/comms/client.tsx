import CryptoJS from 'crypto-js';
import { pin } from 'ionicons/icons';

let SELF_ID = localStorage.getItem("SELF_ID") || "SELF_ID";
let QKD_ADDRESS = localStorage.getItem("QKD_ADDRESS") || "http://localhost:5000";
let QKD_KEY = localStorage.getItem("QKD_KEY") || "QKD_KEY";
let QKD_PIN = "12341234123412341234123412341234"
export function encrypt(msg: string, key: string): string {
    const keyBytes = CryptoJS.enc.Utf8.parse(key);
    const iv = CryptoJS.enc.Utf8.parse('0000000000000000'); // 16 bytes IV
    const encrypted = CryptoJS.AES.encrypt(msg, keyBytes, {
        iv: iv,
        mode: CryptoJS.mode.CBC,
        padding: CryptoJS.pad.Pkcs7
    });
    return encrypted.toString();
}

// Decryption function
export function decrypt(encryptedMsg: string, key: string): string {
    const keyBytes = CryptoJS.enc.Utf8.parse(key);
    const iv = CryptoJS.enc.Utf8.parse('0000000000000000'); // 16 bytes IV
    const decrypted = CryptoJS.AES.decrypt(encryptedMsg, keyBytes, {
        iv: iv,
        mode: CryptoJS.mode.CBC,
        padding: CryptoJS.pad.Pkcs7
    });
    return decrypted.toString(CryptoJS.enc.Utf8);
}

export function saveToLocalStorage(self_id: string, qkd_address: string, qkd_key: string) {
    SELF_ID = self_id
    QKD_ADDRESS = qkd_address
    QKD_KEY = qkd_key
    localStorage.setItem("SELF_ID", self_id);
    localStorage.setItem("QKD_ADDRESS", qkd_address);
    localStorage.setItem("QKD_KEY", qkd_key);
}


export function baseEncryptedMessage(): any {
    return {
        "tx_id": SELF_ID,
        "ts": Date.now(),
    };
}

export function encryptMessage(message: any, key = QKD_KEY): string {
    return encrypt(JSON.stringify(message), key);
}

export function decryptMessage(message: string, key = QKD_KEY): any {
    return JSON.parse(decrypt(message, key));
}

// post to QKD_ADDRESS/keys/SELF_ID
export async function postGetKey(rx_id: string): Promise<any> {
    const msg = baseEncryptedMessage();
    msg["rx_id"] = rx_id;

    try {
        const encryptedMsg = encryptMessage(msg);
        const response = await fetch(QKD_ADDRESS + "/keys/" + SELF_ID, {
            method: "POST",
            headers: {
                "Content-Type": "application/octet-stream",
                "Content-Transfer-Encoding": "base64"
            },
            body: encryptedMsg,
        });

        if (!response.ok) {
            throw new Error(`HTTP error! Status: ${response.status}`);
        }

        const data = await response.text();
        return data;
    } catch (error) {
        console.error('Error fetching key:', error);
        throw error;
    }
}

export async function getGetKey() {
    const msg = baseEncryptedMessage();

    try {
        const encryptedMsg = msg;
        console.log(encryptMessage(encryptedMsg,QKD_KEY))
        console.log(msg)
        const response = await fetch(QKD_ADDRESS + "/keys/get/" + SELF_ID, {
            method: "POST",
            headers: {
                "Content-Type": "application/octet-stream",
                "Content-Transfer-Encoding": "base64",
                body: encryptedMsg,
            }
        });
        

        if (!response.ok) {
            throw new Error(`HTTP error! Status: ${response.status}`);
        }
        const data = await response.text();
        console.log(data)
        if (data.startsWith("{")) {
            throw new Error("No key found");
        }
        const decryptedMsg = decryptMessage(data);
        return decryptedMsg;
    } catch (error) {
        console.error('Error fetching key:', error);
        throw error;
    }
}


// register User to QKD Server
export async function registerUser() {
    const msg = {
        "tx_id": "self",
        "ts": Date.now(),
    };

    try {
        const encryptedMsg = encryptMessage(msg, QKD_PIN);
        const response = await fetch(QKD_ADDRESS + "/users", {
            method: "POST",
            headers: {
                "Content-Type": "application/octet-stream",
                "Content-Transfer-Encoding": "base64"
            },
            body: encryptedMsg,
        });

        if (!response.ok) {
            throw new Error(`HTTP error! Status: ${response.status}`);
        }

        const data = await response.text();
        const decryptedMsg = decryptMessage(data, QKD_PIN);
        return decryptedMsg;
    } catch (error) {
        console.error('Error registering user:', error);
        throw error;  // Re-throw the error to allow further handling if needed
    }
}
