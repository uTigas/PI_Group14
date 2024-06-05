import psycopg2
import os
from util import logger

HOSTNAME = "addressdb"
addressDBUrl = None
conn = None

def connect_to_db():
    global conn
    # Retrieve environment variables
    postgres_password = os.getenv("POSTGRES_PASSWORD_ADDRESS", "postgres")
    postgres_user = os.getenv("POSTGRES_USER_ADDRESS", "postgres")
    postgres_db = os.getenv("POSTGRES_DB_ADDRESS", "addressdb")
    postgres_host = HOSTNAME
    postgres_port = os.getenv("POSTGRES_PORT", "5432")  # Assuming default port
    # Form the PostgreSQL URL
    addressDBUrl = f"postgresql://{postgres_user}:{postgres_password}@{postgres_host}:{postgres_port}/{postgres_db}"

    conn = psycopg2.connect(addressDBUrl)

    return conn

def disconnect_from_db():
    global conn
    conn.close()
    return

def init_db():
    global conn
    cur = conn.cursor()
    try:
        SERVER_ADDRESS = os.getenv("QKD_ADDRESS", "localhost:5000")
        SERVER_ID = os.getenv("SERVER_ID", 1)
        with open("init_address_db.sql", "r") as f:
            c = str(f.read())
            cur.execute(c)
            cur.execute("SELECT * FROM address")
            if SERVER_ID and SERVER_ADDRESS:
                cur.execute("INSERT INTO address VALUES (%s, %s) ON CONFLICT (id) DO UPDATE SET address = EXCLUDED.address;", (SERVER_ID, SERVER_ADDRESS))
            conn.commit()  # Commit the transaction after initialization
    except Exception as e:
        logger.error("Error initializing database: " + str(e))

    cur.close()

def get_address(rx_id):
    global conn
    try:
        cur = conn.cursor()
        cur.execute("SELECT address FROM address WHERE id = %s", (int(rx_id),))
        address = cur.fetchone()
        cur.close()
        return address[0]
    except:
        return None
    
def create_id(qkd_address):
    global conn
    cur = conn.cursor()
    cur.execute("INSERT INTO address (address) VALUES (%s) RETURNING id", (qkd_address,))
    new_id = cur.fetchone()[0]
    cur.close()
    conn.commit()  # Commit the transaction after inserting data
    return str(new_id), qkd_address
