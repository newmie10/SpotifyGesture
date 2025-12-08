import serial
import requests
import time
import os
from dotenv import load_dotenv, set_key

load_dotenv()  # Load variables from .env
API_ID = os.getenv("SPOTIFY_ID")
API_KEY = os.getenv("SPOTIFY_SECRET")
URI = "http://127.0.0.1:5173"
ENV_PATH = ".env"

# 1. Set these values:
SERIAL_PORT = "/dev/ttyACM0"  # Windows example: "COM3"
BAUD_RATE = 115200

# Optional: fix a device_id if needed, otherwise None
DEVICE_ID = "0d1841b0976bae2a3a310dd74c0f3df354899bc8"  # e.g. "1234abcd..."


def js_api(method, endpoint, json_body=None):
    """Helper to call Spotify Web API."""
    print(f"Sending command {method} on {endpoint}")

    # url = f"{URI}/{endpoint}?device_id={DEVICE_ID}"
    url = f"{URI}/{endpoint}"

    headers = {
        "Content-Type": "application/json",
    }

    resp = requests.request(
        method=method,
        url=url,
        headers=headers,
        json=json_body,
    )

    print(method, url, "->", resp.status_code, resp.json())
    return resp

# def refresh_access_token():
#     """Use stored REFRESH_TOKEN to get a new ACCESS_TOKEN."""
#     refresh_token = os.getenv("REFRESH_TOKEN")
#     if not refresh_token:
#         print("No REFRESH_TOKEN in env; run spotify_auth_cli.py first.")
#         return False

#     data = {
#         "grant_type": "refresh_token",
#         "refresh_token": refresh_token,
#     }

#     resp = requests.post(
#         "https://accounts.spotify.com/api/token",
#         data=data,
#         auth=(API_ID, API_KEY),  # your SPOTIFY_ID / SPOTIFY_SECRET
#     )

#     print("POST /api/token (refresh) ->", resp.status_code, resp.text[:200])

#     if resp.status_code != 200:
#         return False

#     payload = resp.json()
#     access_token = payload["access_token"]
#     expires_in = payload.get("expires_in", 3600)
#     new_refresh = payload.get("refresh_token")

#     set_key(ENV_PATH, "ACCESS_TOKEN", access_token)
#     set_key(ENV_PATH, "ACCESS_EXPIRY", str(time.time() + expires_in - 60))
#     if new_refresh:
#         set_key(ENV_PATH, "REFRESH_TOKEN", new_refresh)

#     load_dotenv(override=True)
#     return True


# def update_token():
#     """Ensure ACCESS_TOKEN is valid; refresh if needed."""
#     token = os.getenv("ACCESS_TOKEN")
#     expiry = os.getenv("ACCESS_EXPIRY")

#     if token and expiry:
#         try:
#             if time.time() < float(expiry):
#                 return  # still valid
#         except ValueError:
#             pass

#     if not refresh_access_token():
#         print("Failed to refresh token. Run spotify_auth_cli.py again.")


def handle_command(cmd):
    cmd = cmd.upper()

    if cmd == "PLAY":
        js_api("POST", "play")

    elif cmd == "PAUSE":
        js_api("POST", "pause")

    elif cmd == "NEXT":
        js_api("POST", "next")

    elif cmd == "PREV":
        js_api("POST", "previous")

    elif cmd.startswith("VOL "):
        value = cmd[4:].strip()          # everything after "VOL "
        if value.isdigit():
            js_api("POST", f"volume?volume_percent={value}")

    else:
        print("Unknown command:", cmd)


def main():
    ser = serial.Serial(SERIAL_PORT, BAUD_RATE, timeout=1)
    print(f"Listening on {SERIAL_PORT} at {BAUD_RATE} baud")

    time.sleep(2)

    while True:
        # update_token()
        # token = os.getenv("ACCESS_TOKEN")

        line = ser.readline().decode(errors="ignore").strip()
        if not line:
            continue

        print("Received:", line)
        handle_command(line)

if __name__ == "__main__":
    main()