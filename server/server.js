const http = require('http');
const WebSocketServer = require('websocket').server;
const express = require("express");

let valueMessage = null
let jsonData = null

const app = express()

const server = http.createServer(app);

const wsServer = new WebSocketServer({
  httpServer: server,
});

app.use(express.json())

app.get("/", (req, res) => {
  res.sendFile(__dirname + "/views/index.html") 
})

app.post("/data", (req, res) => {
  const data = req.body.data
  
  wsServer.connections.forEach((connection) => {
    connection.send(data, (err) => {
      if(err) {
        console.log("error :", err)
        res.status(500).json({ error: "write data error!" })
      }
      console.log("data terkirim ->", data)
      res.end()
    });
  });
});

app.get("/api/valueMessage", (req, res) => {
  if (valueMessage) {
    res.json({ 
      temperature: jsonData.suhu,
      humidity: jsonData.kelembaban,
      status: jsonData.status
    });
  } else {
    res.status(404).json({ error: "Data tidak tersedia" });
  }
})

wsServer.on('request', function (request) {
  const connection = request.accept(null, request.origin);
  console.log('Client terhubung');

  connection.on('message', function (message) {
    if (message.type === 'utf8') {
      console.log(`Menerima pesan: ${message.utf8Data}`);
      const data = message.utf8Data;
      valueMessage = data;

      jsonData = JSON.parse(valueMessage);
    }
  });

  connection.on('close', function (reasonCode, description) {
    console.log('Client terputus');
  });
});

server.listen(3000, function () {
  console.log('Server berjalan pada port 3000');
});
