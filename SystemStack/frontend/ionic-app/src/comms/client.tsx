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
export function postGetKey(rx_id: string): void {

    const msg = baseEncryptedMessage();
    msg["rx_id"] = rx_id;

    fetch(QKD_ADDRESS + "/keys/" + SELF_ID, {
        method: "POST",
        headers: {
            "Content-Type": "application/octet-stream",
            "Content-Transfer-Encoding": "base64"
        },
        body: encryptMessage(msg),
    });
}

// get to QKD_ADDRESS/keys/SELF_ID
export function getGetKey(): any {
    const msg = baseEncryptedMessage();
    fetch(QKD_ADDRESS + "/keys/" + SELF_ID, {
        method: "GET",
        headers: {
            "Content-Type": "application/octet-stream",
            "Content-Transfer-Encoding": "base64"
        },
        body: encryptMessage(msg),
    }).then((response) => {
        return response.text();
    }).then((data) => {
        const _msg = decryptMessage(data);
        return _msg;
    });
}

// register User to QKD Server
export function registerUser(): void {

    const msg = {
        "tx_id": "self",
        "ts": Date.now(),
    };

    fetch(QKD_ADDRESS + "/users", {
        method: "POST",
        headers: {
            "Content-Type": "application/octet-stream",
            "Content-Transfer-Encoding": "base64"
        },
        body: encryptMessage(msg, QKD_PIN),
    }).then((response) => {
        return response.text();
    }).then((data) => {
        const _msg = decryptMessage(data, QKD_PIN);
        return _msg;
    });
}
