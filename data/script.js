let ws = new WebSocket('ws://192.168.4.1/ws');

const roll = document.getElementById("roll");
const pitch = document.getElementById("pitch");
const yaw = document.getElementById("yaw");
const altitude = document.getElementById("altitude");

ws.onopen = () => {
    document.getElementById('status').textContent = 'Conectado ao WebSocket';
};

ws.onmessage = (event) => {
    const data = JSON.parse(event.data);
    console.log('JSON recebido via websocket:', data);

    roll.textContent = data.roll.toFixed(2);
    yaw.textContent = data.yaw.toFixed(2);
    pitch.textContent = data.pitch.toFixed(2);
    altitude.textContent = data.altitude.toFixed(2);
};
