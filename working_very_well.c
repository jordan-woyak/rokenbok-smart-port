
// inputs
const int accio2 = 2;

// outputs
const int input_enable = 5; // active low
const int output_load = 4; // active low
const int accio = 3;

const int data_bits[8] =
{
  13, 12, 11, 10, 9, 8, 7, 6
};

void setup()
{
  Serial.begin(115200);
  
  pinMode(accio2, INPUT);
  digitalWrite(accio2, LOW);
  
  pinMode(input_enable, OUTPUT);
  digitalWrite(input_enable, HIGH);  
  
  pinMode(output_load, OUTPUT);
  digitalWrite(output_load, HIGH);
  
  // testing
  pinMode(accio, OUTPUT);
  digitalWrite(accio, LOW);
  
  // todo: skip first frame end
  attachInterrupt(0, framed, FALLING);
}

volatile boolean have_data = false;

// todo: kill
//boolean editing_selection = false;

void loop()
{
  // wait for frame end
  while (!have_data) {}
  have_data = false;
  
  // waits a bit for input shift-reg's storage
  // (need 350)
  delayMicroseconds(500);
  
  // read byte
  digitalWrite(input_enable, LOW);
  //delayMicroseconds(5);
  int read_byte = 0;
  for (int i = 0; i != 8; ++i)
  {
    pinMode(data_bits[i], INPUT);
    read_byte |= digitalRead(data_bits[i]) << i;
  }
  digitalWrite(input_enable, HIGH);
  //delayMicroseconds(5);
  
  // process input
  
  // must start with 0x81 to sync properly?
  int write_byte = 0x00;
  
  /*
  if (0xc0 == read_byte)
  {
    //Serial.println("bcast_tpads");
  }
  else if (0xc1 == read_byte)
  {
    //Serial.println("bcast_select");
  }
  else if (0xc2 == read_byte)
  {
    // bcast_end 
  }
  else if (0xc3 == read_byte)
  {
    //Serial.println("edit_tpads");
    //write_byte = 0x80; // vfyedit
  }
  else if (0xc4 == read_byte)
  {
    //Serial.println("edit_select");
    //editing_selection = true;
    //write_byte = 0x80; // vfyedit
  }
  else if (0xc5 == read_byte)
  {
    //Serial.println("edit_end");
    //editing_selection = false;
    // hax
    //write_byte = read_byte;
  }
  else*/ if (0xc6 == read_byte)
  {
    Serial.println("presync");
    write_byte = 0x81; // sync
  }
  else if (0xc7 == read_byte)
  {
    Serial.println("sync");
    write_byte = (1 << 0); // <atrib value>
  }
  else if (0xc8 == read_byte)
  {
    Serial.println("readattrib");
    write_byte = 0x01; // <no sel timeout value>
  }
  else if (0xcc == read_byte)
  {
    Serial.println("readnoseltimeout");
    write_byte = 0x01;
  }
  else
  {
    //Serial.println(read_byte, HEX); 
  }
  
  // ugly
  //if (editing_selection && read_byte != 0xc5)
    //write_byte = 1; // hax
  
  // prepare next byte
  for (int i = 0; i != 8; ++i)
  {
    pinMode(data_bits[i], OUTPUT);
    digitalWrite(data_bits[i], write_byte & (1 << i));
  }
  
  digitalWrite(output_load, LOW);
  //delayMicroseconds(1);
  digitalWrite(output_load, HIGH);

  //delay(3); // seems to be fine up to 6
  digitalWrite(accio, LOW);
}

void framed()
{
  //delayMicroseconds(400); // seems to be fine up to 500
  digitalWrite(accio, HIGH);
    
  have_data = true;
}