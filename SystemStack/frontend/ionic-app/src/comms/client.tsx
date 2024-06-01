import forge from 'node-forge';

let SELF_ID = localStorage.getItem("SELF_ID") || "SELF_ID";
let QKD_ADDRESS = localStorage.getItem("QKD_ADDRESS") || "http://localhost:5000";
let QKD_KEY = localStorage.getItem("QKD_KEY") || "QKD_KEY";
let QKD_PIN = "12341234123412341234123412341234"


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
    console.log("CLIENT decryptMessgage:Decypted Message -> ", decrypt(message, key), " key: ", QKD_KEY, " Message: ", message)
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
        const encryptedMsg = encryptMessage(msg,QKD_KEY);
        console.log("GET Key Request: ", encryptedMsg)
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
            throw new Error("No key found");
        }
        const decryptedMsg = decryptMessage(data);
        console.log("Decrepted MSG @ GET getKey: ", decryptedMsg)

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
