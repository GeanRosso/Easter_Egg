import React, { useState, useEffect } from "react";
import "./App.css";

const API_BASE = "http://localhost:5258/api/arduino";

function App() {
  const [data, setData] = useState({ leds: 0, dist: 0 });
  const [password, setPassword] = useState("");
  const [message, setMessage] = useState("");
  const [failedAttempts, setFailedAttempts] = useState(0);
  const [isExploded, setIsExploded] = useState(false);
  const [countdown, setCountdown] = useState(0);

  // 1. Fetch Loop
  useEffect(() => {
    const interval = setInterval(() => {
      fetch(`${API_BASE}/status`)
        .then((res) => res.json())
        .then((json) => setData(json))
        .catch(() => console.log("System Offline"));
    }, 200);
    return () => clearInterval(interval);
  }, []);

  // 2. Handle Unlock Logic
  const handleUnlock = async () => {
    const response = await fetch(`${API_BASE}/unlock`, {
      method: "POST",
      headers: { "Content-Type": "application/json" },
      body: JSON.stringify({ password: password }),
    });

    if (response.ok) {
      setMessage("ACCESS GRANTED");
      setFailedAttempts(0);
    } else {
      const newFails = failedAttempts + 1;
      setFailedAttempts(newFails);
      setPassword("");

      if (newFails >= 3) {
        triggerSelfDestruct();
      } else {
        setMessage(`WRONG PASSWORD (${newFails}/3)`);
      }
    }
  };

  const triggerSelfDestruct = () => {
    setMessage("SECURITY BREACH: SELF-DESTRUCT INITIATED");
    setCountdown(5);
    fetch(`${API_BASE}/panic`, { method: "POST" }).catch((err) =>
      console.error("Could not initiate hardware panic"),
    );
  };

  useEffect(() => {
    if (countdown > 0) {
      const timer = setTimeout(() => setCountdown(countdown - 1), 1000);
      return () => clearTimeout(timer);
    } else if (countdown === 0 && failedAttempts >= 3) {
      setIsExploded(true);
    }
  }, [countdown, failedAttempts]);

  return (
    <div className={`white-theme ${isExploded ? "dead" : ""}`}>
      {isExploded ? (
        /* SCREEN AFTER EXPLOSION */
        <div className="explosion-screen">
          <h1 className="boom-text">💥 SYSTEM TERMINATED 💥</h1>
          <p>Unauthorized access detected. Hardware Locked.</p>
          <button
            className="reboot-btn"
            onClick={() => window.location.reload()}
          >
            REBOOT SYSTEM
          </button>
        </div>
      ) : (
        <div className="system-container">
          <h1 className="logo-text">BIO-LOCK v2</h1>

          <div className="snake-container">
            {[1, 2, 3, 4, 5, 6].map((num) => (
              <div
                key={num}
                className={`node node-${num} ${data.leds >= num ? "glowing" : ""}`}
              >
                <div className="inner-core"></div>
              </div>
            ))}
          </div>

          <div className={`input-section ${data.leds === 6 ? "active" : ""}`}>
            <input
              type="password"
              placeholder="IDENTITY PIN"
              value={password}
              onChange={(e) => setPassword(e.target.value)}
            />
            <button onClick={handleUnlock}>AUTHORIZE</button>
            {message && <p className="status-message">{message}</p>}
          </div>

          {countdown > 0 && <h2 className="timer">T-MINUS: {countdown}</h2>}

          <p className="sensor-readout">{data.dist}mm PROXIMITY</p>
        </div>
      )}
    </div>
  );
}

export default App;
