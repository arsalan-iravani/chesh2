// webpage.h (compact control UI)
#ifndef WEBPAGE_H
#define WEBPAGE_H
const char webpage[] PROGMEM = R"=====(
<!doctype html>
<html>
<head><meta name="viewport" content="width=device-width,initial-scale=1" /><title>RealisticEye</title>
<style>
body{font-family:Arial,Helvetica,sans-serif;background:#0b0d10;color:#e8eef6;margin:8px}
.section{background:#0f1720;padding:10px;border-radius:8px;margin-bottom:10px}
.row{display:flex;flex-wrap:wrap;gap:8px}
.btn{background:#16202a;color:#fff;padding:8px;border-radius:6px;border:1px solid #26333d}
label{display:block;margin:6px 0}
input,select{background:#071018;color:#eaf;border:1px solid #22303a;padding:6px;border-radius:6px}
.slider{width:100%}
.small{font-size:0.9em;color:#bcd}
</style>
</head>
<body>
<h2>RealisticEye Control</h2>
<div class="section">
 <strong>Eye Color</strong>
 <div class="row">
  <button class="btn" onclick="setPreset('blue')">Blue</button>
  <button class="btn" onclick="setPreset('brown')">Brown</button>
  <button class="btn" onclick="setPreset('green')">Green</button>
  <button class="btn" onclick="setPreset('hazel')">Hazel</button>
  <button class="btn" onclick="setPreset('gray')">Gray</button>
  <button class="btn" onclick="setPreset('dark')">Dark</button>
 </div>
 <label>HEX: <input id="hex" value="#50A0D0" /></label>
 <button class="btn" onclick="applyHex()">Apply</button>
</div>

<div class="section">
 <strong>Movement</strong>
 <label>Gaze X: <input id="gx" type="range" min="-100" max="100" value="0" oninput="setGazeFromInputs()"></label>
 <label>Gaze Y: <input id="gy" type="range" min="-100" max="100" value="0" oninput="setGazeFromInputs()"></label>
 <label><input id="autogaze" type="checkbox" checked onchange="toggleAuto()"> Auto Gaze</label>
</div>

<div class="section">
 <strong>Animation</strong>
 <label><input id="autoblink" type="checkbox" checked onchange="toggleBlink()"> Auto Blink</label>
 <label><input id="micro" type="checkbox" checked onchange="toggleMicro()"> Micro-saccades</label>
</div>

<div class="section">
 <strong>Expression / Pupil</strong>
 <label>Expression:
  <select id="expr" onchange="setExpression()">
    <option value="0">Normal</option><option value="1">Happy</option><option value="2">Sad</option>
    <option value="3">Angry</option><option value="4">Surprised</option><option value="5">Sleepy</option>
    <option value="6">Confused</option><option value="7">Crying</option>
  </select>
 </label>
 <label>Pupil: <input id="pupil" type="range" min="0" max="100" value="35" oninput="setPupil(this.value)"></label>
</div>

<div class="section">
 <button class="btn" onclick="resetDefaults()">Reset defaults</button>
 <div id="status" class="small">Status: ready</div>
</div>

<script>
function post(url) { return fetch(url).then(r=>r.json()).catch(e=>({ok:false})); }
function setPreset(n){
  const p={blue:'#50A0D0', brown:'#7A4B2E', green:'#6FA06F', hazel:'#9A7B49', gray:'#A0A0A8', dark:'#5A2D1A'};
  const h=p[n]||'#50A0D0'; document.getElementById('hex').value=h; applyHex();
}
function hexToRgb(hex){
  hex=hex.replace('#',''); if(hex.length===3) hex=hex.split('').map(h=>h+h).join('');
  const v=parseInt(hex,16); return {r:(v>>16)&255,g:(v>>8)&255,b:v&255};
}
function applyHex(){ const hex=document.getElementById('hex').value; const rgb=hexToRgb(hex); post(`/setColor?r=${rgb.r}&g=${rgb.g}&b=${rgb.b}`).then(()=>status('Color set')); }
function setGazeFromInputs(){ const gx=document.getElementById('gx').value/100; const gy=document.getElementById('gy').value/100; post(`/setGaze?x=${gx}&y=${gy}`).then(()=>status('Gaze set')); }
function setExpression(){ const e=document.getElementById('expr').value; post(`/setExpression?e=${e}`).then(()=>status('Expression set')); }
function setPupil(v){ post(`/setPupil?t=${v}`).then(()=>status('Pupil set')); }
function toggleAuto(){ const v=document.getElementById('autogaze').checked?1:0; post(`/toggle?auto=${v}`).then(()=>status('Auto gaze toggled')); }
function toggleBlink(){ const v=document.getElementById('autoblink').checked?1:0; post(`/toggle?blink=${v}`).then(()=>status('Blink toggled')); }
function toggleMicro(){ const v=document.getElementById('micro').checked?1:0; post(`/toggle?micro=${v}`).then(()=>status('Micro toggled')); }
function resetDefaults(){ location.reload(); }
function status(msg){ const s=document.getElementById('status'); s.textContent='Status: '+msg; setTimeout(()=>s.textContent='Status: Ready',900); }
</script>
</body>
</html>
)=====";
#endif
