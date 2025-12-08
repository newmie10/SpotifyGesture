const express = require("express");
const axios = require("axios");
const crypto = require("crypto");
require("dotenv").config();

const app = express();
app.use(express.json());

const {
  SPOTIFY_CLIENT_ID,
  SPOTIFY_CLIENT_SECRET,
  SPOTIFY_REDIRECT_URI = "http://127.0.0.1:5173/callback",
  PORT = 5173,
} = process.env;

const REQUIRED_SCOPES = [
  "user-read-playback-state",
  "user-modify-playback-state",
  "user-read-currently-playing",
].join(" ");

const STATE_TTL_MS = 10 * 60 * 1000;
const stateStore = new Map(); // state -> { verifier, createdAt }
let tokenBundle = null; // { accessToken, refreshToken, expiresAt }

const base64UrlEncode = (input) =>
  Buffer.from(input)
    .toString("base64")
    .replace(/\+/g, "-")
    .replace(/\//g, "_")
    .replace(/=+$/, "");

const createCodeVerifier = () => base64UrlEncode(crypto.randomBytes(64));
const createCodeChallenge = (verifier) =>
  base64UrlEncode(crypto.createHash("sha256").update(verifier).digest());
const createState = () => crypto.randomBytes(12).toString("hex");

const saveState = (state, verifier) => stateStore.set(state, { verifier, createdAt: Date.now() });
const consumeState = (state) => {
  const entry = stateStore.get(state);
  if (!entry) return null;
  stateStore.delete(state);
  if (Date.now() - entry.createdAt > STATE_TTL_MS) return null;
  return entry.verifier;
};

const setTokens = ({ access_token, refresh_token, expires_in }) => {
  const expiresAt = Date.now() + (expires_in ?? 3600) * 1000 - 30_000; // refresh 30s early
  tokenBundle = {
    accessToken: access_token,
    refreshToken: refresh_token || tokenBundle?.refreshToken,
    expiresAt,
  };
};

const ensureEnv = () => {
  if (!SPOTIFY_CLIENT_ID || !SPOTIFY_CLIENT_SECRET) {
    throw new Error("SPOTIFY_CLIENT_ID and SPOTIFY_CLIENT_SECRET must be set");
  }
};

const exchangeCodeForTokens = async (code, codeVerifier) => {
  ensureEnv();
  const params = new URLSearchParams({
    grant_type: "authorization_code",
    code,
    redirect_uri: SPOTIFY_REDIRECT_URI,
    client_id: SPOTIFY_CLIENT_ID,
    code_verifier: codeVerifier,
  });

  const response = await axios.post("https://accounts.spotify.com/api/token", params, {
    headers: { "Content-Type": "application/x-www-form-urlencoded" },
    auth: { username: SPOTIFY_CLIENT_ID, password: SPOTIFY_CLIENT_SECRET },
  });

  setTokens(response.data);
  return response.data;
};

const refreshAccessToken = async () => {
  if (!tokenBundle?.refreshToken) throw new Error("No refresh token available. Re-login.");
  ensureEnv();

  const params = new URLSearchParams({
    grant_type: "refresh_token",
    refresh_token: tokenBundle.refreshToken,
  });

  const response = await axios.post("https://accounts.spotify.com/api/token", params, {
    headers: { "Content-Type": "application/x-www-form-urlencoded" },
    auth: { username: SPOTIFY_CLIENT_ID, password: SPOTIFY_CLIENT_SECRET },
  });

  setTokens(response.data);
  return tokenBundle.accessToken;
};

const ensureAccessToken = async () => {
  if (tokenBundle?.accessToken && tokenBundle.expiresAt > Date.now()) {
    return tokenBundle.accessToken;
  }
  if (tokenBundle?.refreshToken) {
    return refreshAccessToken();
  }
  throw new Error("No Spotify tokens yet. Hit /login first.");
};

const withDevice = (baseUrl, deviceId) =>
  deviceId ? `${baseUrl}?device_id=${encodeURIComponent(deviceId)}` : baseUrl;

const withVolume = (baseUrl, volume) =>
  volume ? `${baseUrl}?volume_percent=${encodeURIComponent(volume)}` : baseUrl;

const extractDeviceId = (req) => req.query.device_id || req.body?.device_id;
const extractVolume = (req) => req.query.volume_percent || req.body?.volume_percent;

const spotifyRequest = async (method, url, data) => {
  try {
    const token = await ensureAccessToken();
    const res = await axios({
      method,
      url,
      data,
      headers: { Authorization: `Bearer ${token}` },
    });
    return res.data ?? { ok: true };
  } catch (err) {
    if (err.response?.status === 401 && tokenBundle?.refreshToken) {
      await refreshAccessToken();
      const res = await axios({
        method,
        url,
        data,
        headers: { Authorization: `Bearer ${tokenBundle.accessToken}` },
      });
      return res.data ?? { ok: true };
    }
    throw err;
  }
};

app.get("/", (_req, res) => {
  res.json({
    message: "Spotify auth sandbox",
    login: "/login",
    callback: "/callback",
    controls: ["/play", "/pause", "/next", "/me/player"],
  });
});

app.get("/login", (_req, res) => {
  try {
    ensureEnv();
  } catch (err) {
    res.status(500).json({ error: err.message });
    return;
  }

  const state = createState();
  const codeVerifier = createCodeVerifier();
  const codeChallenge = createCodeChallenge(codeVerifier);

  saveState(state, codeVerifier);

  const params = new URLSearchParams({
    response_type: "code",
    client_id: SPOTIFY_CLIENT_ID,
    redirect_uri: SPOTIFY_REDIRECT_URI,
    scope: REQUIRED_SCOPES,
    state,
    code_challenge_method: "S256",
    code_challenge: codeChallenge,
  });

  res.redirect(`https://accounts.spotify.com/authorize?${params.toString()}`);
});

app.get("/callback", async (req, res) => {
  const { code, state, error } = req.query;
  if (error) return res.status(400).json({ error });
  if (!code || !state) return res.status(400).json({ error: "Missing code or state" });

  const codeVerifier = consumeState(String(state));
  if (!codeVerifier) return res.status(400).json({ error: "Invalid or expired state" });

  try {
    await exchangeCodeForTokens(String(code), codeVerifier);
    res.send(
      `<p>Spotify auth success. You can close this tab.</p><p>Now you can POST to /play, /pause, /next from curl or a client.</p>`,
    );
  } catch (err) {
    const message = err.response?.data || err.message || "Auth failed";
    res.status(500).json({ error: message });
  }
});

app.get("/status", (_req, res) => {
  res.json({
    hasTokens: !!tokenBundle?.accessToken,
    expiresAt: tokenBundle?.expiresAt,
    hasRefresh: !!tokenBundle?.refreshToken,
  });
});

app.post("/play", async (req, res) => {
  try {
    const deviceId = extractDeviceId(req);
    const data =
      req.body && Object.keys(req.body).length
        ? (() => {
            const clone = { ...req.body };
            if ("device_id" in clone) delete clone.device_id; // prevent sending to Spotify body
            return clone;
          })()
        : undefined;
    const url = withDevice("https://api.spotify.com/v1/me/player/play", deviceId);
    const result = await spotifyRequest("put", url, data);
    res.json({ ok: true, result });
  } catch (err) {
    res.status(err.response?.status || 500).json({ error: err.response?.data || err.message });
  }
});

app.post("/pause", async (req, res) => {
  try {
    const deviceId = extractDeviceId(req);
    const data =
      req.body && Object.keys(req.body).length
        ? (() => {
            const clone = { ...req.body };
            if ("device_id" in clone) delete clone.device_id; // prevent sending to Spotify body
            return clone;
          })()
        : undefined;
    const url = withDevice("https://api.spotify.com/v1/me/player/pause", deviceId);
    const result = await spotifyRequest("put", url, data);
    res.json({ ok: true, result });
  } catch (err) {
    res.status(err.response?.status || 500).json({ error: err.response?.data || err.message });
  }
});

app.post("/next", async (req, res) => {
  try {
    const deviceId = extractDeviceId(req);
    const data =
      req.body && Object.keys(req.body).length
        ? (() => {
            const clone = { ...req.body };
            if ("device_id" in clone) delete clone.device_id; // prevent sending to Spotify body
            return clone;
          })()
        : undefined;
    const url = "https://api.spotify.com/v1/me/player/next"
    const result = await spotifyRequest("post", url);
    res.json({ ok: true, result });
  } catch (err) {
    res.status(err.response?.status || 500).json({ error: err.response?.data || err.message });
  }
});

app.post("/previous", async (req, res) => {
  try {
    const deviceId = extractDeviceId(req);
    const data =
      req.body && Object.keys(req.body).length
        ? (() => {
            const clone = { ...req.body };
            if ("device_id" in clone) delete clone.device_id; // prevent sending to Spotify body
            return clone;
          })()
        : undefined;
    const url = "https://api.spotify.com/v1/me/player/previous"
    const result = await spotifyRequest("post", url);
    res.json({ ok: true, result });
  } catch (err) {
    res.status(err.response?.status || 500).json({ error: err.response?.data || err.message });
  }
});

app.post("/volume", async (req, res) => {
  try {
    const vol = extractVolume(req)
    const url = withVolume("https://api.spotify.com/v1/me/player/volume", vol);
    console.log(url);
    const result = await spotifyRequest("put", url);
    res.json({ ok: true, result });
  } catch (err) {
    res.status(err.response?.status || 500).json({ error: err.response?.data || err.message });
  }
});

app.get("/me/player", async (_req, res) => {
  try {
    const player = await spotifyRequest("get", "https://api.spotify.com/v1/me/player");
    res.json(player);
  } catch (err) {
    res.status(err.response?.status || 500).json({ error: err.response?.data || err.message });
  }
});

app.listen(PORT, () => {
  // eslint-disable-next-line no-console
  console.log(`Spotify auth server running on http://localhost:${PORT}`);
});

