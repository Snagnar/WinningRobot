// author: jotaro shigeyama,2020
let sensorNoise = 10;
let robot, battleField;
let currentRingColor;
let step = 1;
let mode = 0,
    justSet = false,
    rotationsQueue = [],
    currRotation = 0; //if mode=1 robot is moving in correct direction

function setup() {
    createCanvas(710, 400);
    // Create robot and battlefield.
    battleField = new BattleField(random(width), random(height));
    robot = new Robot(random(width), random(height));
    setSensors();
    alert("start");
}

function mod(n, m) {
    return ((n % m) + m) % m;
}

function setSensors() {
    let dist = 25;
    for(let x = 0; x < 360; x+=36)
        robot.set_sensor(Math.cos(x*Math.PI / 180.0) * dist, Math.sin(x*Math.PI / 180.0) * dist)
}

function draw() {
    background(255);
    battleField.draw();

    //TODO2: YOUR ROBOT LOGIC HERE. USE moveForward() and rotateRight() or rotateLeft()
    {
        //TODO3: below function returns 0, 1, 2 as color at the field.
        //if you change the function as get_sensor_values_at_BF(battleField) then it will return sensor values with noise.
        //the sensor value now returns gaussian noise with mean - (100,200,300) and std sensorNoise.
        //you can increase / decrease sensorNoise with up,down arrow key.
        let sensors = robot.get_color_values_at_BF(battleField);

        let diffNum = numberOfDifferences(sensors);
        if(mode == 0) { // this is when we never crossed a line before
            if(diffNum <4) {
                step+=0.01;
                robot.rotate(0.04);
                robot.moveForward();
            }
            else {
                mode = 1;
            }
        }
        else if(mode == 1) {
            robot.moveForward();
            if(diffNum < 4)
                justSet = false;
            else if(!justSet || diffNum > 4) {
                let newRotation = getRotationFromLineCrossing(sensors, diffNum);
                if(newRotation < (Math.PI/8) || newRotation > (Math.PI * 15 / 8)) {
                    rotationsQueue.unshift(mod(currRotation+newRotation, 2 * Math.PI));
                    if(rotationsQueue.length > 20) 
                        rotationsQueue.pop();
                    let avg = 0, count = 0;
                    for(let x = 0; x<rotationsQueue.length; x++) {
                        avg += Math.pow(2, 10-x)*rotationsQueue[x];
                        count += Math.pow(2, 10-x);
                    }
                    newRotation = mod((avg / count) - currRotation, 2 * Math.PI);
                }
                else
                    rotationsQueue = [];
                
                currRotation += newRotation;
                robot.rotate(newRotation);
                justSet = true;
            }
        }
      
        fill(255, 0, 0);

        text("Sensor Differences: "+diffNum, 10, 20);
        text("Sensor noise is set to " + sensorNoise.toString(), 10, height - 20);
    }
    robot.draw();
}

function keyPressed() {
    if (keyCode === UP_ARROW) {
        sensorNoise += 10;
    } else if (keyCode === DOWN_ARROW) {
        sensorNoise -= 10;
        sensorNoise = max(0, sensorNoise);
    }
}

function mouseClicked() {
    robot = new Robot(mouseX, mouseY);
    setSensors();
}

///////////////////////////////////////////////////////////////
/////// YOU CAN HELP JOTARO MAKE BELOW CODE BETTER :)
///////////////////////////////////////////////////////////////

function getRingColor(sensors){
  let counts = [0,0,0];
  for(let s of sensors){
    counts[s]++;
  }
  
  if(Math.max(...counts) >= sensors.length/2){
    return counts.indexOf(Math.max(...counts));
  }
  
  return null;
}

// TODO: make this independent of the actual sensor number
function getRotationFromLineCrossing(sensors, numberOfSensorDifferences) {
    let angle, color1, color2;

    if(numberOfSensorDifferences != 4 && numberOfSensorDifferences != 5)
        return null;

    for(let x = 0; x<10; x++) {
        let prevPrev = sensors[mod(x - 2, 10)],
            prev = sensors[mod(x - 1, 10)],
            own = sensors[x],
            next = sensors[(x + 1) % 10],
            nextNext = sensors[(x + 2) % 10],
            nextNextNext = sensors[(x + 3) % 10];
        
        if( numberOfSensorDifferences == 4 &&
            prev == own && next == nextNext && own == next && 
            prevPrev != own && nextNextNext != own) {
                
                angle =  (x * Math.PI / 5) + (Math.PI / 10); // actually the angle is x * 36 degrees -> to radians: x * 36 * pi / 180
                color1 = own;
                color2 = sensors[(x + 5) % 10];
                break;
        }
        if( numberOfSensorDifferences == 5 &&
            prevPrev == prev && own == next && own == nextNext && own == prev) {

                angle = x * Math.PI / 5; // actually the angle is x * 36 degrees -> to radians: x * 36 * pi / 180
                color1 = own;
                color2 = sensors[(x + 5) % 10];
                break;
        }
    }
    if(((color2 + 1) % 3) == color1)
        return angle % (2 * Math.PI);
    return (Math.PI+angle) % (2 * Math.PI);
}

function numberOfDifferences(sensors) {
    let count = 0;
    for(let s = 0; s < sensors.length / 2; s++)
        if(sensors[s] != sensors[(s+sensors.length/2)%sensors.length])
            count++;
    return count;
}

function checkIfSuccessor(colorCode){
  if(colorCode != null){
    if(currentRingColor != 2){
      return currentRingColor+1 == colorCode;
    }
    return colorCode == 0;
    }
  
  return false;
  
}

function rotate_(cx, cy, x, y, angle) {
    var radians = -angle,
        cos = Math.cos(radians),
        sin = Math.sin(radians),
        nx = (cos * (x - cx)) + (sin * (y - cy)) + cx,
        ny = (cos * (y - cy)) - (sin * (x - cx)) + cy;
    return [nx, ny];
}
// Robot class
class Robot {
    constructor(x, y, d) {
        this.x = x
        this.y = y
        this.diameter = width / 10;
        this.rotation = 0;
        this.sensors = [];
    }

    moveForward() {
        this.x += step * cos(this.rotation);
        this.y += step * sin(this.rotation);
        for (let s of this.sensors) {
            s.x += step * cos(this.rotation);
            s.y += step * sin(this.rotation);
        }
    }
    rotateRight(r) {
        this.rotate(r);
    }
    rotateLeft(r) {
        this.rotate(-r);
    }
    rotate(rotation) {
        this.rotation += rotation;
        for (let s of this.sensors) {
            let new_pos = rotate_(this.x, this.y, s.x, s.y, rotation);
            s.update_pos(new_pos[0], new_pos[1]);
        }
    }

    draw() {
        fill(44, 169, 173);
        ellipse(this.x, this.y, this.diameter, this.diameter);
        for (let s of this.sensors) {
            s.draw();
        }
        strokeWeight(5);
        stroke(2, 87, 114);
        line(this.x + this.diameter / 2 * cos(this.rotation),
            this.y + this.diameter / 2 * sin(this.rotation),
            this.x + (10 + this.diameter / 2) * cos(this.rotation),
            this.y + (10 + this.diameter / 2) * sin(this.rotation));

    }
    set_sensor(x, y) {
        let s = new Sensor(this.x + x, this.y + y);
        this.sensors.push(s);
    }
    get_color_values_at_BF(battleField) {
        let res = [];
        for (let s of this.sensors) {
            res.push(battleField.get_color_at_point(s.x, s.y));
        }
        return res;
    }
    get_sensor_values_at_BF(battleField) {
        let val = get_color_values_at_BF(battleField);
        res = max(0, randomGaussian((val + 1) * 100, sensorNoise));
    }
}

class Sensor {
    constructor(x, y) {
        this.x = x;
        this.y = y;
        this.diameter = width / 50;
    }
    draw() {
        fill(255, 51, 0);
        ellipse(this.x, this.y, this.diameter, this.diameter);
    }
    update_pos(x, y) {
        this.x = x;
        this.y = y;
    }
}

class BattleField {
    constructor(x, y) {
        this.cx = x;
        this.cy = y;
        this.num_circle = 19;
        this.diameter = max(width, height) * 2;
        this.color = 1;
    }
    draw() {
        this.color = 1;
        for (let i = 0; i < this.num_circle; i++) {
            let d = this.diameter / this.num_circle;
            this.switch_color();
            strokeWeight(0);
            ellipse(this.cx, this.cy,
                this.diameter - d * i, this.diameter - d * i)
        }
    }
    switch_color() {
        switch (this.color) {
            case 0:
                fill(172);
                this.color = 1;
                break;
            case 1:
                fill(255);
                this.color = 2;
                break;
            case 2:
                fill(0);
                this.color = 0;
                break;
            default:
                console.error("something went wrong");
                break;
        }
    }
    get_color_at_point(px, py) {
        let d = sqrt((px - this.cx) * (px - this.cx) +
            (py - this.cy) * (py - this.cy));
        let t = this.diameter / this.num_circle / 2;
        let r = d / t;
        return parseInt(3 - r % 3);
    }


}