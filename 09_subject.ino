// Arduino pin assignment
#define PIN_LED  9
#define PIN_TRIG 12
#define PIN_ECHO 13

// configurable parameters
#define SND_VEL 346.0     // sound velocity at 24 celsius degree (unit: m/sec)
#define INTERVAL 25       // sampling interval (unit: msec)
#define PULSE_DURATION 10 // ultra-sound Pulse Duration (unit: usec)
#define _DIST_MIN 100     // minimum distance to be measured (unit: mm)
#define _DIST_MAX 300     // maximum distance to be measured (unit: mm)

#define TIMEOUT ((INTERVAL / 2) * 1000.0) // maximum echo waiting time (unit: usec)
#define SCALE (0.001 * 0.5 * SND_VEL)     // coefficent to convert duration to distance

#define _EMA_ALPHA 0.5    // EMA weight of new sample (range: 0 to 1)
                          // Setting EMA to 1 effectively disables EMA filter.

// global variables
unsigned long last_sampling_time;   // unit: msec
float dist_prev = _DIST_MAX;        // Distance last-measured
float dist_ema;                     // EMA distance
int count = 0;
int n = 30;
float q[30]; //n과 같은 수 

void setup() {
  // initialize GPIO pins
  pinMode(PIN_LED,OUTPUT);
  pinMode(PIN_TRIG,OUTPUT);
  pinMode(PIN_ECHO,INPUT);
  digitalWrite(PIN_TRIG, LOW);




  
  // initialize serial port
  Serial.begin(57600);
}

void loop() {
  float dist_raw, dist_filtered,dist_median;
  
  // wait until next sampling time. 
  // millis() returns the number of milliseconds since the program started. 
  // will overflow after 50 days.
  if (millis() < last_sampling_time + INTERVAL)
    return;

  // get a distance reading from the USS
  dist_raw = USS_measure(PIN_TRIG,PIN_ECHO);

  // Modify the below if-else statement to implement the range filter
  if ((dist_raw == 0.0) || (dist_raw > _DIST_MAX)) { // 값 초과
      sort(n);
      dist_prev = dist_raw;
      dist_median = findMedium(n);
      push(dist_median,n);
      //dist_filtered = dist_prev;
  } else if (dist_raw < _DIST_MIN) { // 값 미만 
      sort(n);
      dist_prev = dist_raw;
      dist_median = findMedium(n);
      push(dist_median,n);
      //dist_filtered = dist_prev;
  } else {    // In desired Range // 정 상 값
      push(dist_raw,n);
      dist_median = dist_raw;
      dist_prev = dist_raw;
  }

  // Modify the below line to implement the EMA equation
  dist_ema = dist_median * _EMA_ALPHA + (1 - _EMA_ALPHA) * dist_ema;

  // output the distance to the serial port
  Serial.print("Min:");   Serial.print(_DIST_MIN);
  Serial.print(",raw:"); Serial.print(min(dist_raw, _DIST_MAX + 100));
  Serial.print(",median:");  Serial.print(dist_median);
  Serial.print(",ema:");  Serial.print(min(dist_ema, _DIST_MAX + 100));
  Serial.print(",Max:");  Serial.print(_DIST_MAX);
  Serial.println("");

  // do something here
  if ((dist_prev < _DIST_MIN) || (dist_prev > _DIST_MAX))
    digitalWrite(PIN_LED, 1);       // LED OFF
  else
    digitalWrite(PIN_LED, 0);       // LED ON

  // update last sampling time
  last_sampling_time += INTERVAL;
}

// get a distance reading from USS. return value is in millimeter.
float USS_measure(int TRIG, int ECHO)
{
  digitalWrite(TRIG, HIGH);
  delayMicroseconds(PULSE_DURATION);
  digitalWrite(TRIG, LOW);
  
  return pulseIn(ECHO, HIGH, TIMEOUT) * SCALE; // unit: mm

  // Pulse duration to distance conversion example (target distance = 17.3m)
  // - pulseIn(ECHO, HIGH, timeout) returns microseconds (음파의 왕복 시간)
  // - 편도 거리 = (pulseIn() / 1,000,000) * SND_VEL / 2 (미터 단위)
  //   mm 단위로 하려면 * 1,000이 필요 ==>  SCALE = 0.001 * 0.5 * SND_VEL
  //
  // - 예, pusseIn()이 100,000 이면 (= 0.1초, 왕복 거리 34.6m)
  //        = 100,000 micro*sec * 0.001 milli/micro * 0.5 * 346 meter/sec
  //        = 100,000 * 0.001 * 0.5 * 346
  //        = 17,300 mm  ==> 17.3m
}


void push(float value,int n){
  
  if(count < n){
    q[count] = value;
    count++;
  }

  else{
    for(int i = 0; i < n - 1 ;i++){
      q[i] = q[i+1];
    }
    q[n-1] = value;
  }
  
}

float findMedium(int n){
  
  if(n%2 == 0){
    return (q[(n/2) - 1] + q[(n/2)])/2;
    
  }
  else{
    return q[(n-1)/2];
  }

}


void sort(int n){
  int score;
  while(score != n -1 ){
    score = 0;
    for(int i = 0; i < n - 1; i++){
    if(q[i] > q[i+1]){ // 지금 > 다음
      float temp;
      temp = q[i+1];
      q[i + 1] = q[i];
      q[i] = temp;
      
     }
    }
    for(int i = 0; i < n - 1; i++){
      if(q[i] <= q[i+1]){
        score++;
      }
    }
  
    
  }
  
}
