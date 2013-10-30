#define DIRERCTIONAL_PIN 10
#define MOVE_PIN 11
#define END_LEFT_PIN 6
#define END_RIGHT_PIN 7
#define HEADER 255
#define FOOTER 254
#define BREAKER 222
#define MESSAGE_LENGTH 8

int END_LEFT_VALUE;
int END_RIGHT_VALUE;
boolean stop_call_flag = true;

typedef struct BUFFER{
  byte current;
  byte cursor;
  byte buffer [MESSAGE_LENGTH];
};

BUFFER buffer = {
  0, 0,
  { 
    0, 0, 0, 0, 0, 0, 0, 0 
  }
};

typedef struct CMD{
  int type;
  int direction;
  unsigned long distanceToRun;
  unsigned long runnedDistance;
  int speed;
};

CMD cmd = {
  0, 0, 0, 0, 0
};

void setup()
{
  Serial.begin(9600);
  pinMode(DIRERCTIONAL_PIN, OUTPUT);
  pinMode(MOVE_PIN, OUTPUT);
  pinMode(END_LEFT_PIN,INPUT);
  pinMode(END_RIGHT_PIN,INPUT);
  Serial.flush();
}

void loop()
{
  if(consumeUntilHeader())
  {
    readCommand();
  }
  
  // IF TOUCHING THE END ?
  // ALERT MAXMSP
  // STOP MOVE
  
  END_RIGHT_VALUE = digitalRead(END_RIGHT_PIN);
  END_LEFT_VALUE = digitalRead(END_LEFT_PIN);
  if( HIGH == END_LEFT_VALUE || HIGH == END_RIGHT_VALUE )
  {
      Serial.write(END_RIGHT_VALUE);
      Serial.write(END_LEFT_VALUE);
      Serial.write(' ');
      cmd.type = 0;
      cmd.direction = 0;
      cmd.distanceToRun = 0;
      cmd.runnedDistance = 0;
      cmd.speed = 0;
  }

  // IF DISTANCE TO RUN IS NOT RUNNED YET 
  // OR GO TO END MODE
  // MOVE
  if((cmd.type == 1 && cmd.runnedDistance < cmd.distanceToRun) || cmd.type == 30)
  {
    digitalWrite(DIRERCTIONAL_PIN, cmd.direction ? HIGH : LOW ); // HIGH:LEFT / LOW:RIGHT
    digitalWrite(MOVE_PIN, LOW);  // This LOW to HIGH change is what creates the
    digitalWrite(MOVE_PIN, HIGH); // "Rising Edge" so the easydriver knows to when to step.
    delayMicroseconds(cmd.speed);    
    cmd.runnedDistance++;
    stop_call_flag = true;
  }
  // AT THE END OF MOVE
  // ALERT MAXMSP
  else if(stop_call_flag)
  {
    stop_call_flag = false;
    Serial.write(2);
  }
}



boolean consumeUntilHeader()
{
  while(Serial.available())
  {
    byte value = Serial.read();
    if(value == HEADER)
    {
      return true;
    }
    if(value == BREAKER)
    { 
      cmd.type = 0;
      return false;
    }
  }
  return false;
}



void readCommand()
{
  while(true)
  {
    if(Serial.available())
    {
      buffer.current = Serial.read();
      if(buffer.current == FOOTER || buffer.cursor == MESSAGE_LENGTH)
      {
        cmd.type           =         buffer.buffer[0];
        cmd.direction      =         buffer.buffer[1];
        cmd.distanceToRun  = 16384 * buffer.buffer[2];
        cmd.distanceToRun +=   128 * buffer.buffer[3];
        cmd.distanceToRun +=     1 * buffer.buffer[4];
        cmd.runnedDistance = 0;
        cmd.speed          = 16384 * buffer.buffer[5];
        cmd.speed         +=   128 * buffer.buffer[6];
        cmd.speed         +=     1 * buffer.buffer[7];
        buffer.cursor      = 0;
        break;
      }
      else if(buffer.current == BREAKER)
      { 
        cmd.type           = 0;
        buffer.cursor      = 0;
        break;
      }
      else
      {
        buffer.buffer[buffer.cursor++] = buffer.current;
      }
    }
  } 
}

