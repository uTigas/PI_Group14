
import multiprocessing
import os

QKDServerPort = os.getenv("QKD_PORT", "8000")
bind = f"0.0.0.0:{QKDServerPort}"
workers = 1
worker_class = "uvicorn.workers.UvicornWorker"
# accesslog = "-"
# errorlog = "-" 
# loglevel = "info" 