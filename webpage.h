// webpage.h
// Contains a minimal mobile-friendly UI for controlling CHESH2.

#ifndef WEBPAGE_H
#define WEBPAGE_H

const char webpage[] PROGMEM = R"=====(
<!doctype html>
<html>
<head>
<meta name="viewport" content="width=device-width,initial-scale=1"/>
<title>CHESH2 Control</title>
<style>
body{font-family:Arial,Helvetica,sans-serif;margin:8px;background:#111;color:#eee}
.row{display:flex;gap:8px;flex-wrap:wrap}
.btn{padding:10px 12px;border-radius:6px;background:#222;color:#fff;border:1px solid #444}
.slider{width:100%}
.section{margin-bottom:12px;padding:10px;background:#0f0f0f;border-radius:8px}
</style>
</head>
<body>
<h2>CHESH2 - ParaxEye</h2>
<div class="section">
  <div><strong>Eye Color</strong></div>
  <div class="row">
    <button class="btn" onclick="setColor(101,67,33)">Brown</button>
    <button class="btn" onclick="setColor(65,42,20)">Dark Brown</button>
    <button class="btn" onclick="setColor(80,120,200)">Blue</button>
    <button class="btn" onclick="setColor(160,200,240)">Light Blue</button>
    <button class="btn" onclick="setColor(80,150,90)">Green</button>
    <button class="btn" onclick="setColor(170,170,180)">Gray</button>
    <button class="btn" onclick="setColor(140,110,70)">Hazel</button>
    <button class="btn" onclick="setColor(200,140,70)">Amber</button>
  </div>
  <div style="margin-top:8px">
    Custom: R <input id="r" type=number value=80 min=0 max=255> G <input id="g" type=number value=120 min=0 max=255> B <input id="b" type=number value=140 min=0 max=255>
    <button class="btn" onclick="applyCustom()">Apply</button>
  </div>
</div>

<div class="section">
  <div><strong>Direction</strong></div>
  <div class="row">
    <button class="btn" onclick="setGaze(0,0)">Center</button>
    <button class="btn" onclick="setGaze(-1,0)">Left</button>
    <button class="btn" onclick="setGaze(1,0)">Right</button>
    <button class="btn" onclick="setGaze(0,-1)">Up</button>
    <button class="btn" onclick="setGaze(0,1)">Down</button>
    <button class="btn" onclick="setGaze(-0.7,-0.7)">UL</button>
    <button class="btn" onclick="setGaze(0.7,-0.7)">UR</button>
    <button class="btn" onclick="setGaze(-0.7,0.7)">DL</button>
    <button class="btn" onclick="setGaze(0.7,0.7)">DR</button>
  </div>
</div>

<div class="section">
  <div><strong>Animation</strong></div>
  <div class="row">
    <label>Auto Gaze: <input id="autogaze" type=checkbox checked onchange="toggleAuto()"></label>
    <label>Blinking: <input id="blink" type=checkbox checked onchange="toggleBlink()"></label>
  </div>
</div>

<script>
function setColor(r,g,b){
  fetch(`/setColor?r=${r}&g=${g}&b=${b}`);
}
function applyCustom(){
  const r=document.getElementById('r').value;
  const g=document.getElementById('g').value;
  const b=document.getElementById('b').value;
  setColor(r,g,b);
}
function setGaze(x,y){
  fetch(`/setGaze?x=${x}&y=${y}`);
}
function toggleAuto(){
  // Not implemented server-side: UI only
}
function toggleBlink(){
}
</script>
</body>
</html>
)=====";

#endif
