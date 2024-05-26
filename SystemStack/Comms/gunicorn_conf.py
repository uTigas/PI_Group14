
import multiprocessing
import os

QKDServerPort = os.getenv("QKD_PORT", "8000")

bind = f"0.0.0.0:{QKDServerPort}"
workers = multiprocessing.cpu_count() + 1