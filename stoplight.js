// Stoplight MQTT code


var LEDstatus = 0;
var status = "RED";
var circleRadius = 100;

function setup() {
  createCanvas(400, 400); 
  noStroke();
  background(255);
  fill(0,0,0);

  uniqueID = int(10000*Math.random(10000))
  alert = uniqueID
  client = new Paho.MQTT.Client("192.168.1.24", 9001, "stoplight.js" + str(uniqueID)); //port 9001 is for websockets
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
    if (status == "Stoplight READY!")
    {
      LEDstatus = 0;
      fill(255,0,200);
    }
    else if (status == "RED")
    {
      LEDstatus = 1;
      fill(255,0,0);
    }
    else if (status == "YELLOW")
    {
      LEDstatus = 2;
      fill(200,170,0);
    }
    else if (status == "GREEN")
    {
      LEDstatus = 3;
      fill(0,255,0);
    }
  }
}

function draw() {
  background(0); //
  strokeWeight(4);
  circle(width/2,height/2,circleRadius)
  text(LEDstatus,30,30);

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
      status = "RED";
    else if (LEDstatus == 2)      
      status = "YELLOW";
    else if (LEDstatus == 3)
      status = "GREEN";
    message = new Paho.MQTT.Message(str(status));
    message.destinationName = "stopLight/status";
    client.send(message);
  }


}




