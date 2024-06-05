import json
import time
from abc import ABC, abstractmethod
from authentication.comms.util import get_time_cache , store_time_cache , decrypt_msg , encrypt_msg


class Msg_E(ABC):

    def __init__(self, encrypted_msg: str , tx_id_key: str):
        self._encrypted_msg = encrypted_msg
        self._tx_id_key = tx_id_key # whats uncrypted

    def loads(self):
        try:
            self.decrypt(self._tx_id_key)
            temp = self._loads()
            self.valid()
            store_time_cache(self.tx_id, self.ts)
            return temp
        except Exception as e:
            raise InvalidMsg(str("[Error]:"+str(e))+".")
        
    @abstractmethod
    def _loads(self):
        pass

    @classmethod
    def construct(cls, *args, **kwargs):
        msg = cls(None, None)
        msg.ts = int(time.time())
        msg._construct( *args, **kwargs)
        return msg
        
    @abstractmethod
    def _construct(cls , *args, **kwargs):    
        pass

    def decrypt(self, key):
        msg = decrypt_msg(self._encrypted_msg, key)
        self._msg = json.loads(msg)
        self.ts = self._msg["ts"]
        self.tx_id = self._msg["tx_id"]
        return self
    
    def valid(self):
        if self.tx_id is not None and \
            get_time_cache(self.tx_id) < self.ts and \
            any( val is not None for val in self.__dict__.values() ):
                return True
        raise InvalidMsg("Invalid message")
    
    def encrypt(self, key: str):
        if self._encrypted_msg is None:
            self._encrypted_msg = encrypt_msg(str(self), key)
        return self._encrypted_msg
    
    def to_dict(self):
        return {k: v for k, v in self.__dict__.items() if not k.startswith("_")}
    
    def __str__(self) -> str:
        return json.dumps( self.to_dict() )
    
class Msg_S:

    def __init__(self, msg: str):
        self._raw_msg = msg
        
    def loads(self):
        try:
            self._msg = json.loads(self._raw_msg)
            return self._loads()
        except json.JSONDecodeError:
            raise InvalidMsg("Invalid JSON")

    @abstractmethod
    def _loads(self):
        pass

    @classmethod
    def construct(cls , *args, **kwargs):
        msg = cls(bytes(0))
        msg.ts = int(time.time())
        msg._construct( *args, **kwargs)
        return msg

    @abstractmethod
    def _construct(cls , *args, **kwargs):    
        pass

    def to_dict(self):
        return {k: v for k, v in self.__dict__.items() if not k.startswith("_")}

    def __str__(self) -> str:
        return json.dumps( self.to_dict() )
    
class InvalidMsg(Exception):
    
    def __init__(self, msg: str):
        self.msg = msg

    def __str__(self):
        return self.msg

class NoTxId(Exception):
    pass
        
class GetKeyMsg(Msg_E):
    def _loads(self) -> tuple[str, str]:
        self.rx_id = self._msg["rx_id"]        
        return self.tx_id, self.rx_id

    def _construct(self,tx_id: str, rx_id: str):
        self.tx_id = tx_id
        self.rx_id = rx_id

class ReconnMsg(Msg_S):
    def _loads(self) -> tuple[str, str, int]:
        self.tx_id = self._msg["tx_id"]
        self.rx_id = self._msg["rx_id"]
        self.desired_key = self._msg["desired_key"]
        return self.tx_id, self.rx_id, self.desired_key

    def _construct(self, tx_id: str, rx_id: str, desired_key: int):
        self.tx_id = tx_id
        self.rx_id = rx_id
        self.desired_key = desired_key

class GetKeysMsg(Msg_E):
    def _loads(self) -> str:
        return self.tx_id

    def _construct(self, tx_id: str):
        self.tx_id = tx_id

class ReturnKeysMsg(Msg_E):

    def _loads(self) -> dict[str, bytes]:
        self.keys = self._msg["keys"]
        return self.keys

    def _construct(self, tx_id: str, keys: dict[str, bytes]):
        self.tx_id = tx_id
        self.keys = keys

class RegisterUserMsg(Msg_E):

    def _loads(self) -> tuple[str, str]:
        return 

    def _construct(self):
        self.tx_id = "self"

class ReturnRegisterMsg(Msg_E):

    def _loads(self) -> str:
        self.user = self._msg["user"]
        self.qkd_address = self._msg["qkd_address"]
        self.key = self._msg["key"]
        return self.user , self.qkd_address , self.key

    def _construct(self,user: str , qkd_address: str , key: str):
        self.user = user
        self.qkd_address = qkd_address
        self.key = key  
        self.tx_id = "self"
