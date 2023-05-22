//Nicholas Hardy, Riya Deokar, Marybel Boujaoude, Hassan Hijazi
const http = require("http");
const dgram = require("dgram");
const parse = require("csv-parse/lib/sync");
const createCsvWriter = require("csv-writer").createObjectCsvWriter;

const PORT = 8081;
const HOST = "192.168.1.36";

const server = dgram.createSocket("udp4");
let messageFromEsp = "";
let buttonClicked = false;

const udp = dgram.createSocket("udp4");

const message1 = "toggle_led";

http
  .createServer(function (req, res) {
    res.writeHead(200, { "Content-Type": "text/html" });
    if (messageFromEsp) {
      var messages = messageFromEsp.split("\n");
      console.log("messages:", messages);
      var data = parse(messages.join("\n"), { rows: true });
      console.log("Data:", data);

      var cat = [];
      cat.push(messages);
      console.log("cat", cat);
      var sizeOfCat = cat.length;

      const times = [];
      const temperatures = [];
      data.forEach((row) => {
        times.push(Date.now() / 1000); // divide by 1000 to get seconds
        for (var i = 0; i < cat[0].length; i++) {
          temperatures.push(parseFloat(cat[0][i])); // add the temperature value as a float
        }
        console.log(temperatures);
      });

      res.write("<html>");
      res.write("<head>");
      res.write("<title>Cat Cam</title>");
      res.write("<style>");
      res.write("body { background-color: #ccc; }");
      res.write("h1 { color: blue; }");
      res.write("</style>");
      res.write("</head>");
      res.write("<body>");
      // res.write("<h1>Hello World</h1>");
      res.write("</body>");
      res.write("</html>");
      res.write(
        '<html><body><h2 style="text-align:center;">Group_1 Cat Monitor</h2>'
      );
      res.write("<h2>Cat Camera Stream</h2>");
      res.write(
        '<img src="http://192.168.1.36:8082" width="640" height="480" alt="Cat Camera Stream"/>'
      );
      res.write("<br><br>");
      res.write(
        '<html><body><h2 style="text-align:center;">Bed Temperature</h2>'
      );
      res.write('<canvas id="chart"></canvas>');
      res.write("<br><br>");
      res.write(
        '<button id="sendButton" onclick="sendButtonClicked()" style="font-size:24px;position:absolute;top:10px;right:10px;">Light On/Off</button>'
      );
      res.write(
        '<script src="https://cdn.jsdelivr.net/npm/chart.js"></script>'
      );
      res.write("<script>");
      res.write('var ctx = document.getElementById("chart").getContext("2d");');
      res.write(`var times = ${JSON.stringify(times)};`);
      res.write(`var temperatures = ${JSON.stringify(temperatures)};`);
      res.write(
        'var chartData = { labels: times, datasets: [{ label: "Temperature (°C)", data: temperatures }] };'
      );
      res.write(
        'var chart = new Chart(ctx, { type: "line", data: chartData, options: { scales: { yAxes: [{ ticks: { beginAtZero: true } }] } } });'
      );
      res.write("var buttonClicked = false;");
      res.write("function sendButtonClicked() {");
      res.write("buttonClicked = !buttonClicked;");
      res.write('var button = document.getElementById("sendButton");');
      res.write("if (buttonClicked) {");
      res.write('button.innerHTML = "Light Off";');
      // console.log('Button Off');
      res.write("} else {");
      res.write('button.innerHTML = "Light On";');
      // console.log('Button On');
      res.write("}");
      res.write("var xhr = new XMLHttpRequest();");
      res.write('xhr.open("GET", "http://192.168.1.37:8080", true);'); // replace with the IP address of your external ESP32
      res.write("xhr.onreadystatechange = function() {");
      res.write("if (xhr.readyState == 4 && xhr.status == 200) {");
      res.write('console.log("Light Switch Toggled");');
      res.write("}");
      res.write("};");
      res.write("xhr.send();");
      udp.send(message1, 8081, "192.168.1.37", (err) => {
        if (err) {
          console.error(err);
        } else {
          console.log("UDP message sent to ESP");
        }
      });
      res.write("}");
      res.write("</script>");
      res.write("</body></html>");

      // write data to CSV file
      const csvWriter = createCsvWriter({
        path: "thermist.csv",
        header: [...Object.keys(data[0]), "time"].map(
          (header) => ({ id: header, title: header }),
          { id: "time", title: "time" }
        ), // add 'time' column to CSV header
        append: true,
        fieldDelimiter: ",",
        headerIdDelimiter: "_",
      });
      const records = data.map((row) => ({
        ...row,
        time: Date.now() / 1000,
        temperature: parseFloat(row.temperature),
      }));
      csvWriter
        .writeRecords(records)
        .then(() => console.log("Data appended to CSV file"))
        .catch((error) => console.error(error));
    } else {
      res.end("<html><body><h2>No message from ESP-32</h2></body></html>");
    }
  })
  .listen(PORT);
server.on("listening", function () {
  const address = server.address();
  console.log(
    "UDP Server listening on " + address.address + ":" + address.port
  );
});
server.on("message", function (message, remote) {
  console.log("" + message);
  messageFromEsp += message.toString() + "\n";
});
udp.send(message1, 8081, "192.168.1.37", (err) => {
  if (err) {
    console.error(err);
  } else {
    console.log("UDP message sent to ESP");
  }
});
server.bind(PORT, HOST);
