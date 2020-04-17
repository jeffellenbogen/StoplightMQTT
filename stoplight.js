// Stoplight MQTT code


var LEDstatus = 0;
var status = "READY";
var circleRadius = 200;

function setup() {
  createCanvas(600, 600); 
  noStroke();
  textSize(30);

  uniqueID = int(10000*Math.random(10000))
  alert = uniqueID
  client = new Paho.MQTT.Client("192.168.1.6", 9001, "stoplight.js" + str(uniqueID)); //port 9001 is for websockets
  client.onMessageArrived = onMessageArrived;
  client.connect({onSuccess:onConnect});
}


function onConnect() {
  console.log("onConnect");
  client.subscribe("stopLight/status");
  
  message = new Paho.MQTT.Message(str(status));
  message.destinationName = "stopLight/status";
  client.send(message);
  

}

// called when a message arrives
function onMessageArrived(message) {
  console.log("onMessageArrived:"+message.destinationName+ " " +message.payloadString);
  if (message.destinationName == "stopLight/status")
  {
    status = message.payloadString;
    if (status == "READY")
    {
      LEDstatus = 0;
      fill(255,0,200);
    }
    else if (status == "GREEN")
    {
      LEDstatus = 1;
      fill(0,255,0);
    }
    else if (status == "YELLOW")
    {
      LEDstatus = 2;
      fill(232, 225, 30);
    }
    else if (status == "RED")
    {
      LEDstatus = 3;
      fill(255,0,0);
    }
  }
}

function draw() {
  background(255); //
  strokeWeight(4);
  circle(width/2,height/2,circleRadius)
  text("Status: " + status,185,50);

}

function mousePressed() { 
  
}

function mouseReleased() {
  if (dist(mouseX,mouseY,width/2,height/2)<circleRadius)
  {
    LEDstatus++;
    if (LEDstatus > 3)
      LEDstatus = 1;
    if (LEDstatus == 1)
      status = "GREEN";
    else if (LEDstatus == 2)      
      status = "YELLOW";
    else if (LEDstatus == 3)
      status = "RED";
    message = new Paho.MQTT.Message(str(status));
    message.destinationName = "stopLight/status";
    client.send(message);
  }


}




