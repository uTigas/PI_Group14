var CryptoJS = require("crypto-js");

export function encrypt(data: string, key: string): string {
    return CryptoJS.AES.encrypt(data, key).toString();
}

export function decrypt(data: string, key: string): string {
    return CryptoJS.AES.decrypt(data, key).toString(CryptoJS.enc.Utf8);
}

export function saveToLocalStorage(self_id: string, qkd_address: string, qkd_key: string) {
    localStorage.setItem("SELF_ID", self_id);
    localStorage.setItem("QKD_ADDRESS", qkd_address);
    localStorage.setItem("QKD_KEY", qkd_key);
}

const SELF_ID = localStorage.getItem("SELF_ID") || "SELF_ID";
const QKD_ADDRESS = localStorage.getItem("QKD_ADDRESS") || "QKD_ADDRESS";
const QKD_KEY = localStorage.getItem("QKD_KEY") || "QKD_KEY";

export function baseEncryptedMessage(): any {
    return {
        "tx_id": SELF_ID,
        "ts": Date.now(),
    };
}

export function encryptMessage(message: any): string {
    return encrypt(JSON.stringify(message), QKD_KEY);
}

export function decryptMessage(message: string): any {
    return JSON.parse(decrypt(message, QKD_KEY));
}

// post to QKD_ADDRESS/keys/SELF_ID
export function postGetKey(rx_id: string): void {

    const msg = baseEncryptedMessage();
    msg["rx_id"] = rx_id;

    fetch(QKD_ADDRESS + "/keys/" + SELF_ID, {
        method: "POST",
        headers: {
            "Content-Type": "application/json",
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
            "Content-Type": "application/json",
        },
        body: encryptMessage(msg),
    }).then((response) => {
        return response.json();
    }).then((data) => {
        const _msg = decryptMessage(data);
        return _msg;
    });
}

