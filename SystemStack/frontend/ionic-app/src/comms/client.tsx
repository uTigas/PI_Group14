var CryptoJS = require("crypto-js");

export function encrypt(data: string, key: string): string {
  return CryptoJS.AES.encrypt(data, key).toString();
}

export function decrypt(data: string, key: string): string {
  return CryptoJS.AES.decrypt(data, key).toString(CryptoJS.enc.Utf8);
}





