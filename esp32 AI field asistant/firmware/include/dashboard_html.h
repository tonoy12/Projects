#ifndef DASHBOARD_HTML_H
#define DASHBOARD_HTML_H

const char DASHBOARD_HTML[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
    <title>Device Dashboard</title>
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <style>
        body { background-color: #f4f7f6; color: #333333; font-family: -apple-system, BlinkMacSystemFont, "Segoe UI", Roboto, Helvetica, Arial, sans-serif; text-align: center; margin: 0; padding: 20px; }
        h2 { color: #2c3e50; font-size: 1.2rem; margin-bottom: 15px; margin-top: 30px; text-transform: uppercase; letter-spacing: 1px;}
        
        /* Clean Status Bar */
        .status-bar { background: #ffffff; padding: 10px; border-radius: 8px; box-shadow: 0 2px 4px rgba(0,0,0,0.05); margin-bottom: 20px; font-size: 0.9rem; color: #555; display: flex; justify-content: space-around;}
        .status-bar span { font-weight: bold; color: #007bff; }

        /* Clean Grid */
        .grid { display: grid; grid-template-columns: repeat(auto-fit, minmax(140px, 1fr)); gap: 15px; margin-bottom: 20px; }
        .card { background: #ffffff; padding: 20px; border-radius: 8px; box-shadow: 0 2px 4px rgba(0,0,0,0.05); border: 1px solid #e9ecef; }
        .card h3 { margin: 0 0 5px; font-size: 0.85rem; color: #6c757d; text-transform: uppercase; }
        .card p { margin: 0; font-size: 1.8rem; font-weight: 600; color: #212529;}

        /* Modern Activity Log */
        .terminal { background: #ffffff; border: 1px solid #e9ecef; border-left: 4px solid #007bff; border-radius: 8px; padding: 15px; text-align: left; margin: 0 auto; max-width: 500px; overflow-wrap: break-word; box-shadow: 0 2px 4px rgba(0,0,0,0.05);}
        .t-label { color: #6c757d; font-size: 0.75rem; font-weight: bold; margin-bottom: 3px; display: block; text-transform: uppercase;}
        .t-text { color: #212529; font-size: 1rem; margin-bottom: 15px; display: block; }
        .ai-text { color: #0056b3; font-weight: 500; }
        .stat-badge { background: #e9ecef; padding: 4px 8px; border-radius: 12px; font-size: 0.75rem; color: #495057; float: right; font-weight: bold;}

        /* Clean Buttons */
        .controls { display: flex; flex-direction: column; gap: 10px; align-items: center; margin-bottom: 20px; }
        .btn { background: #ffffff; color: #333; border: 1px solid #ced4da; padding: 12px; border-radius: 6px; cursor: pointer; font-size: 1rem; width: 100%; max-width: 300px; font-weight: 600; box-shadow: 0 1px 2px rgba(0,0,0,0.05); transition: 0.1s; }
        .btn:active { background: #e2e6ea; transform: scale(0.98); }
        
        .rgb-row { display: flex; gap: 10px; justify-content: center; max-width: 300px; margin: 0 auto; }
        .btn-r { border-left: 4px solid #dc3545; } 
        .btn-g { border-left: 4px solid #28a745; } 
        .btn-b { border-left: 4px solid #007bff; } 
        .btn-off { border-left: 4px solid #6c757d; flex-grow: 1; }
    </style>
</head>
<body>
    <div class="status-bar">
        <div>Wi-Fi Signal: <span id="wifi">--</span> dBm</div>
        <div>Free RAM: <span id="ram">--</span> KB</div>
    </div>

    <h2>Environment Telemetry</h2>
    <div class="grid">
        <div class="card"><h3>Ambient</h3><p><span id="tA">--</span>&deg;C</p></div>
        <div class="card"><h3>Object</h3><p><span id="tO">--</span>&deg;C</p></div>
        <div class="card"><h3>Light</h3><p><span id="ldr">--</span></p></div>
        <div class="card"><h3>Gyro Z</h3><p><span id="gz">--</span>&deg;</p></div>
    </div>
    
    <h2>AI Activity Log</h2>
    <div class="terminal">
        <span class="stat-badge">Queries: <span id="qCount">0</span></span>
        <br><br>
        <span class="t-label">User Input</span>
        <span class="t-text" id="lastQ">Awaiting input...</span>
        
        <span class="t-label">Assistant Response</span>
        <span class="t-text ai-text" id="lastR">System ready.</span>
    </div>

    <h2>Hardware Override</h2>
    <div class="controls">
        <button class="btn" onclick="fetch('/api/uv')">Toggle UV LED</button>
        <button class="btn" onclick="fetch('/api/motor')">Pulse Motor</button>
    </div>
    <div class="rgb-row">
        <button class="btn btn-r" onclick="fetch('/api/rgb?c=r')">R</button>
        <button class="btn btn-g" onclick="fetch('/api/rgb?c=g')">G</button>
        <button class="btn btn-b" onclick="fetch('/api/rgb?c=b')">B</button>
        <button class="btn btn-off" onclick="fetch('/api/rgb?c=o')">OFF</button>
    </div>

    <script>
        setInterval(() => {
            fetch('/api/data')
            .then(r => r.json())
            .then(d => {
                document.getElementById('tA').innerText = d.tA.toFixed(1);
                document.getElementById('tO').innerText = d.tO.toFixed(1);
                document.getElementById('ldr').innerText = d.ldr;
                document.getElementById('gz').innerText = d.gz.toFixed(1);
                
                document.getElementById('qCount').innerText = d.qCount;
                document.getElementById('lastQ').innerText = d.lastQ;
                document.getElementById('lastR').innerText = d.lastR;

                document.getElementById('wifi').innerText = d.wifi;
                document.getElementById('ram').innerText = Math.round(d.ram / 1024);
            });
        }, 1000);
    </script>
</body>
</html>
)rawliteral";

#endif