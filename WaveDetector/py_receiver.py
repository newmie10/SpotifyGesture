import serial
import requests
import time
import os
from dotenv import load_dotenv, set_key

load_dotenv()  # Load variables from .env
API_ID = os.getenv("SPOTIFY_ID")
API_KEY = os.getenv("SPOTIFY_SECRET")
ENV_PATH = ".env"

# 1. Set these values:
SERIAL_PORT = "/dev/ttyUSB0"  # Windows example: "COM3"
BAUD_RATE = 115200

# Optional: fix a device_id if needed, otherwise None
DEVICE_ID = None  # e.g. "1234abcd..."


def spotify_api(method=None, endpoint=None, json_body=None, params=None, token=None):
    """Helper to call Spotify Web API."""
    if endpoint is None:
        data = {
            'grant_type': 'client_credentials',
            'client_id': API_ID,
            'client_secret': API_KEY,
        }

        response = requests.post('https://accounts.spotify.com/api/token', data=data)
        print("POST https://accounts.spotify.com/api/token", "->", response.status_code, response.text[:200])
        return response
    else:
        if token is None or method is None or endpoint is None:
            print("Provide correct parameters please. ")
            return
        url = f"https://api.spotify.com/v1/me/player/{endpoint}"

        headers = {
            "Authorization": f"Bearer {token}",
            "Content-Type": "application/json"
        }

        response = requests.request(
            method=method,
            url=url,
            headers=headers,
            json=json_body,
            params=params
        )

        print(method, url, "->", response.status_code, response.json())
        return response

def update_token():
    """Updates the Spotify API token in env if expired or missing"""
    token = os.getenv("ACCESS_TOKEN", "0")
    if len(token) > 1:
        expiry = os.getenv("ACCESS_EXPIRY", "0")
        print("Expiry: ", expiry, "Time: ", time.time())
        if len(expiry) > 1 and time.time() < float(expiry):
            print("Token not changed")
            return
        
    # tokens last for 1 hour
    res = spotify_api().json()
    set_key(".env", "ACCESS_TOKEN", str(res['access_token']))
    set_key(".env", "ACCESS_EXPIRY", str(time.time() + 3600))
    load_dotenv(override=True)
    print("New token acquired")


def handle_command(cmd, token):
    """Map serial commands to Spotify actions."""
    cmd = cmd.upper()

    if cmd == "PLAY":
        params = {}
        if DEVICE_ID:
            params["device_id"] = DEVICE_ID
        spotify_api("PUT", "play", params=params, token=token)

    elif cmd == "PAUSE":
        params = {}
        if DEVICE_ID:
            params["device_id"] = DEVICE_ID
        spotify_api("PUT", "me/player/pause", params=params)

    elif cmd == "NEXT":
        params = {}
        if DEVICE_ID:
            params["device_id"] = DEVICE_ID
        spotify_api("POST", "me/player/next", params=params)

    elif cmd == "PREV":
        params = {}
        if DEVICE_ID:
            params["device_id"] = DEVICE_ID
        spotify_api("POST", "me/player/previous", params=params)

    elif cmd.startswith("VOL "):
        # Example: "VOL 50"
        try:
            level = int(cmd.split()[1])
            level = max(0, min(100, level))
        except Exception:
            print("Invalid VOL command")
            return

        params = {"volume_percent": level}
        if DEVICE_ID:
            params["device_id"] = DEVICE_ID
        spotify_api("PUT", "me/player/volume", params=params)

    else:
        print("Unknown command:", cmd)


def main():
    # Open serial port
    ser = serial.Serial(SERIAL_PORT, BAUD_RATE, timeout=1)
    print(f"Listening on {SERIAL_PORT} at {BAUD_RATE} baud")

    time.sleep(2)  # give ESP32 time to reset
    update_token()

    token = os.getenv("ACCESS_TOKEN")

    while True:
        line = ser.readline().decode(errors="ignore").strip()
        if not line:
            continue
        print("Received:", line)
        handle_command(line, token)


if __name__ == "__main__":
    main()
