let ws = new WebSocket('ws://192.168.4.1/ws');

const roll = document.getElementById("roll");
const pitch = document.getElementById("pitch");
const yaw = document.getElementById("yaw");
const altitude = document.getElementById("altitude");
const temp = document.getElementById("temp");
const ax = document.getElementById("ax");
const ay = document.getElementById("ay");
const az = document.getElementById("az");
const posX = document.getElementById("posX");
const posY = document.getElementById("posY");

const altitudeCanvas = document.getElementById("altitudeGraph");
const accelCanvas = document.getElementById("accelGraph");
const altCtx = altitudeCanvas.getContext("2d");
const accelCtx = accelCanvas.getContext("2d");

let altitudeData = [];
let accelX = [];
let accelY = [];
let accelZ = [];
let x = 0;
let y = 0;
const maxPoints = 100;

ws.onopen = () => {
  document.getElementById('status').textContent = 'Conectado ao WebSocket';
};

ws.onmessage = (event) => {
  const data = JSON.parse(event.data);

  roll.textContent = data.roll.toFixed(2);
  yaw.textContent = data.yaw.toFixed(2);
  pitch.textContent = data.pitch.toFixed(2);
  altitude.textContent = data.altitude.toFixed(2);
  temp.textContent = data.temperature?.toFixed(1) || "--";
  ax.textContent = data.ax.toFixed(2);
  ay.textContent = data.ay.toFixed(2);
  az.textContent = data.az.toFixed(2);
  x = data.posX.toFixed(2);
  y = data.posY.toFixed(2);
  posX.textContent = x;
  posY.textContent = y;

  altitudeData.push(data.altitude);
  accelX.push(data.ax);
  accelY.push(data.ay);
  accelZ.push(data.az);

  if (altitudeData.length > maxPoints) altitudeData.shift();
  if (accelX.length > maxPoints) accelX.shift();
  if (accelY.length > maxPoints) accelY.shift();
  if (accelZ.length > maxPoints) accelZ.shift();
};

function drawGraph(ctx, datasets, colors) {
  const w = ctx.canvas.width;
  const h = ctx.canvas.height;
  ctx.clearRect(0, 0, w, h);

  ctx.strokeStyle = "rgba(0, 255, 255, 0.2)";
  ctx.lineWidth = 1;
  for (let i = 1; i < 10; i++) {
    ctx.beginPath();
    ctx.moveTo((w / 10) * i, 0);
    ctx.lineTo((w / 10) * i, h);
    ctx.stroke();
  }

  let allValues = datasets.flat();
  if (allValues.length === 0) return;
  const min = Math.min(...allValues);
  const max = Math.max(...allValues);
  const range = max - min || 1;

  datasets.forEach((data, i) => {
    ctx.strokeStyle = colors[i];
    ctx.beginPath();
    data.forEach((v, idx) => {
      const x = (idx / (maxPoints - 1)) * w;
      const y = h - ((v - min) / range) * h;
      idx === 0 ? ctx.moveTo(x, y) : ctx.lineTo(x, y);
    });
    ctx.stroke();
  });
}

function drawCoordinateSystem(ctx, x, y) {
  const w = ctx.canvas.width;
  const h = ctx.canvas.height;
  ctx.clearRect(0, 0, w, h);


}

function resizeCanvas() {
  [altitudeCanvas, accelCanvas].forEach(c => {
    c.width = c.clientWidth * window.devicePixelRatio;
    c.height = c.clientHeight * window.devicePixelRatio;
  });
}
resizeCanvas();
window.addEventListener('resize', resizeCanvas);

function loop() {
  drawGraph(altCtx, [altitudeData], ["#00ffff"]);
  drawGraph(accelCtx, [accelX, accelY, accelZ], ["#00ffea", "#3bb7ff", "#005eff"]);
  requestAnimationFrame(loop);
}
loop();
