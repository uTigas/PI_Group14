import psycopg2
import os

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
    with open("init_address_db.sql", "r") as f:
        cur = conn.cursor()
        cur.execute(f.read())
        cur.close()

def get_address(rx_id):
    global conn
    cur = conn.cursor()
    cur.execute("SELECT address FROM addresses WHERE id = %s", (rx_id,))
    address = cur.fetchone()
    cur.close()
    return address
