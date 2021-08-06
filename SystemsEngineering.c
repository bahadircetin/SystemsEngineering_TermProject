#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2); // Set the LCD address to 0x27 for a 16 chars and 2 line display
byte degree_symbol[8] =
    {
        0b00111,
        0b00101,
        0b00111,
        0b00000,
        0b00000,
        0b00000,
        0b00000,
        0b00000};

int gate = 11;
volatile unsigned long duration = 0;
unsigned char i[5];
unsigned int j[40];
unsigned char value = 0;
unsigned answer = 0;
int z = 0;
int b = 1;

// Water Level Sensor
int waterLevel = A1;
int redLevel = 12;
int greenLevel = 6;
int blueLevel = 10;
#define buzzer 13

// Flame Sensor
int flameSensor = 8;
int buzzer2Pin = 9;
int flameStat;

// Quake Sensor
const int vib_pin = 7;
const int led_pin = 4;

// RGB Bluetooth
int bluetoothData;
int redPin = 2;
int greenPin = 3;
int bluePin = 5;

void setup()
{
    Serial.begin(9600);
    lcd.init(); // Initialize the lCD
    lcd.init();
    lcd.backlight();
    lcd.print("Temp = ");
    lcd.setCursor(0, 1);
    lcd.print("Humidity = ");
    lcd.createChar(1, degree_symbol);
    lcd.setCursor(9, 0);
    lcd.write(1);
    lcd.print("C");
    lcd.setCursor(13, 1);
    lcd.print("%");

    //Water Level Sensor
    pinMode(redLevel, OUTPUT);
    pinMode(greenLevel, OUTPUT);
    pinMode(blueLevel, OUTPUT);

    //Flame Sensor
    pinMode(flameStat, INPUT);
    pinMode(buzzer2Pin, OUTPUT);

    //Quake Sensor
    pinMode(vib_pin, INPUT);
    pinMode(led_pin, OUTPUT);
    digitalWrite(vib_pin, HIGH);

    // RGB Bluetooth
    Serial.begin(9600);
    pinMode(redPin, OUTPUT);
    pinMode(greenPin, OUTPUT);
    pinMode(bluePin, OUTPUT);
}

void loop()
{
    long measurement = Tp_init();
    delay(50);
    Serial.println(measurement);
    if (measurement > 1000)
    {
        digitalWrite(led_pin, HIGH);
    }
    else
    {
        digitalWrite(led_pin, LOW);
    }

    pinMode(gate, OUTPUT);
    digitalWrite(gate, LOW);
    delay(20);
    digitalWrite(gate, HIGH);
    pinMode(gate, INPUT_PULLUP); // By default it will become high due to internal pull up
    // delayMicroseconds(40);

    duration = pulseIn(gate, LOW);
    if (duration <= 84 && duration >= 72)
    {
        while (1)
        {
            duration = pulseIn(gate, HIGH);

            if (duration <= 26 && duration >= 20)
            {
                value = 0;
            }

            else if (duration <= 74 && duration >= 65)
            {
                value = 1;
            }

            else if (z == 40)
            {
                break;
            }

            i[z / 8] |= value << (7 - (z % 8));
            j[z] = value;
            z++;
        }
    }
    answer = i[0] + i[1] + i[2] + i[3];

    if (answer == i[4] && answer != 0)
    {
        lcd.setCursor(7, 0);
        lcd.print(i[2]);
        lcd.setCursor(11, 1);
        lcd.print(i[0]);
    }

    z = 0;
    i[0] = i[1] = i[2] = i[3] = i[4] = 0;

    // Water Level Sensor
    waterLevel = analogRead(A1);
    pinMode(buzzer, OUTPUT);
    Serial.println(waterLevel);

    if (waterLevel <= 300)
    {
        digitalWrite(redLevel, HIGH);
        digitalWrite(greenLevel, LOW);
        digitalWrite(blueLevel, LOW);
        digitalWrite(buzzer, LOW);
    }

    if (waterLevel > 300 && waterLevel <= 500)
    {
        digitalWrite(redLevel, HIGH);
        digitalWrite(greenLevel, HIGH);
        digitalWrite(blueLevel, LOW);
        digitalWrite(buzzer, LOW);
    }

    if (waterLevel > 500)
    {
        digitalWrite(redLevel, HIGH);
        digitalWrite(greenLevel, HIGH);
        digitalWrite(blueLevel, HIGH);
        digitalWrite(buzzer, HIGH);
    }

    // Flame Sensor
    flameStat = digitalRead(flameSensor);

    if (flameStat == 0)
    {
        Serial.println("flamestat1");
        digitalWrite(buzzer2Pin, HIGH);
    }
    else
    {
        Serial.println("flamestat0");
        digitalWrite(buzzer2Pin, LOW);
    }

    // RGB Bluetooth
    if (Serial.available() > 0)
    {
        bluetoothData = Serial.read();
    }
    if (bluetoothData == 'k')
    {
        digitalWrite(redPin, LOW);
        digitalWrite(greenPin, HIGH);
        digitalWrite(bluePin, LOW);
    }
    else if (bluetoothData == 'y')
    {
        digitalWrite(redPin, HIGH);
        digitalWrite(greenPin, LOW);
        digitalWrite(bluePin, LOW);
    }
    else if (bluetoothData == 'm')
    {
        digitalWrite(redPin, LOW);
        digitalWrite(greenPin, LOW);
        digitalWrite(bluePin, HIGH);
    }
    else if (bluetoothData == 'o')
    {
        digitalWrite(redPin, HIGH);
        digitalWrite(greenPin, HIGH);
        digitalWrite(bluePin, HIGH);
    }
    else if (bluetoothData == 'c')
    {
        digitalWrite(redPin, LOW);
        digitalWrite(greenPin, LOW);
        digitalWrite(bluePin, LOW);
    }
    else
    {
        digitalWrite(redPin, HIGH);
        digitalWrite(greenPin, HIGH);
        digitalWrite(bluePin, HIGH);
    }
}

long Tp_init()
{
    delay(10);
    long measurement = pulseIn(vib_pin, LOW);
    return measurement;
}