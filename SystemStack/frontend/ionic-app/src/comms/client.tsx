import forge from 'node-forge';

let SELF_ID = localStorage.getItem("SELF_ID") || "default";
let QKD_ADDRESS = localStorage.getItem("QKD_ADDRESS") || "default";
let QKD_KEY = localStorage.getItem("QKD_KEY") || "12341234123412341234123412341234";

export function decrypt(msg: string, key: string): string {
    const decipher = forge.cipher.createDecipher('AES-CBC', forge.util.createBuffer(key));
    decipher.start({ iv: forge.util.createBuffer('0000000000000000') });
    decipher.update(forge.util.createBuffer(forge.util.decode64(msg)));
    decipher.finish();
    return decipher.output.toString();
}

export function encrypt(msg: string, key: string): string {
    const cipher = forge.cipher.createCipher('AES-CBC', forge.util.createBuffer(key));
    cipher.start({ iv: forge.util.createBuffer('0000000000000000') });
    cipher.update(forge.util.createBuffer(msg));
    cipher.finish();
    return forge.util.encode64(cipher.output.getBytes());
}

export async function checkIfRegistered(): Promise<boolean> {
    try {
        let ret = await getKeys();
    }
    catch (error) {
        console.error("Not registered", error);
        return false;
    }
    return true;
}

export async function checkIfRegisteredAndRegister() {
    while (! await checkIfRegistered()) {
        let self_id = prompt("Enter your ID","self") || "self";
        let qkd_address = prompt("Enter QKD Address", "http://localhost:5000") || "http://localhost:5000";
        let qkd_key = prompt("Enter QKD Key", "12341234123412341234123412341234") || "12341234123412341234123412341234";
        saveToLocalStorage(self_id, qkd_address, qkd_key);
    }
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

export function encryptMessage(message: any, key: any = QKD_KEY): string {
    return encrypt(JSON.stringify(message), key);
}

export function decryptMessage(message: string, key: any = QKD_KEY): any {
    return JSON.parse(decrypt(message, key));
}

// post to QKD_ADDRESS/keys/SELF_ID
export async function requestKey(rx_id: string): Promise<any> {
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

export async function getKeys() {
    const msg = baseEncryptedMessage();

    try {
        const encryptedMsg = encryptMessage(msg, QKD_KEY);
        const response = await fetch(QKD_ADDRESS + "/keys/get/" + SELF_ID, {
            method: "POST",
            headers: {
                "Content-Type": "application/octet-stream",
                "Content-Transfer-Encoding": "base64",
            },
            body: encryptedMsg,
        });

        if (!response.ok) {
            throw new Error(`HTTP error! Status: ${response.status}`);
        }
        const data = await response.text();
        if (data.startsWith("{")) {
            throw new Error("No key found" + data);
        }
        const decryptedMsg = decryptMessage(data);
        return decryptedMsg;
    } catch (error) {
        console.error('Error fetching key: ', error);
        throw error;
    }
}

