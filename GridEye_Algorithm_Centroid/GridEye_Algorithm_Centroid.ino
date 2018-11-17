#include <Wire.h>
#include <Adafruit_AMG88xx.h>

Adafruit_AMG88xx amg;

float pixels[AMG88xx_PIXEL_ARRAY_SIZE];


//Variable definition
int row = 8;
int col = 8;
float Current_Pixel_Measurement[8][8];
float Average_Pixel_Measurement[8][8]; 
float Sum_Pixel_Measurement[8][8];
float Previous_Pixel_Measurement[8][8];
int frame_count = 0;
int Active_Pixel[8][8];
float threshold_temperature_difference = 1;
int threshold_temperature = 25;
int current_max_row=0;
int current_max_col=0;
int previous_max_row=0;
int previous_max_col =0;
int direction_x=0;
int direction_y=0;
int x_centroid_previous =0;
int y_centroid_previous =0;
int sum_centroid_x = 0;
int sum_centroid_y = 0;
float x_centroid = 0;
float y_centroid = 0;
int count_centroid = 0;
int sum_active_pixel=0;

float current_millis;
float elapsed_millis;

void setup() {
    Serial.begin(9600);
    Serial.println(F("AMG88xx pixels"));    

    for(int i=0; i<row; i++){
      for(int j=0; j<col; j++)
      {
        Average_Pixel_Measurement[i][j] = 0;   
        Sum_Pixel_Measurement[i][j] = 0;
      }
    }

    current_millis = millis();

    bool status;
    
    // default settings
    status = amg.begin();
    if (!status) {
        Serial.println("Could not find a valid AMG88xx sensor, check wiring!");
        while (1);
    }
    
    Serial.println("-- Pixels Test --");

    Serial.println();

    delay(100); // let sensor boot up
}


void loop() { 
    int motion_detection=0;
    elapsed_millis = millis() - current_millis;
    if(elapsed_millis<=200) //Average for 200 ms This will be 15 frames average. Great for eliminating noise
    {
          frame_count++;
          int count_pixel = 0;
          amg.readPixels(pixels);
          for(int i=0; i<row; i++)
          {
            for(int j=0; j<col; j++)
            {
              Current_Pixel_Measurement[i][j] = pixels[count_pixel];
              count_pixel++;
              Sum_Pixel_Measurement[i][j] += Current_Pixel_Measurement[i][j];
            }
          }            
    }
    if(elapsed_millis>200)
    {      
      current_millis = millis();
      
      for(int i=0; i<row; i++){
        for(int j=0; j<col; j++){
          Average_Pixel_Measurement[i][j] = Sum_Pixel_Measurement[i][j]/frame_count;  
          if(Average_Pixel_Measurement[i][j]> threshold_temperature){
            motion_detection=1;                           
          }
            
        }
      }
  
//Algorithm
      float Diff_Pixel[8][8]; //Reset pixel, so defined here

      for(int i=0; i<row; i++){
        for(int j=0; j<col; j++){
          Diff_Pixel[i][j] = Current_Pixel_Measurement[i][j] - Previous_Pixel_Measurement[i][j]; //Find difference between current and previous pixel measurement to detect moiton
          if(Diff_Pixel[i][j] > threshold_temperature_difference){
            Active_Pixel[i][j] = 1;
            sum_active_pixel += Active_Pixel[i][j];
            count_centroid++;
            sum_centroid_x+= j;
            sum_centroid_y+=i;
          }
          else{
            Active_Pixel[i][j] = 0;
          }
        }
      }

      
//Conditions for Motion Detection
// Active_Pixel needs to have atleast 1 non-zero value
// Temperature during detection should be greater than a threshold value

      if(motion_detection==1 && sum_active_pixel>0){
      x_centroid = sum_centroid_x/count_centroid;
      y_centroid = sum_centroid_y/count_centroid;
        
        if(x_centroid>x_centroid_previous){          
          direction_x = 1;                
        }
        
        if(x_centroid<x_centroid_previous){
          direction_x = -1;
        }
        
        if(x_centroid == 0 || x_centroid==x_centroid_previous){
          direction_x = 0;      
        }
  
        if(y_centroid>y_centroid_previous){
          direction_y = 1;                
        }
        
        if(y_centroid<y_centroid_previous){
          direction_y = -1;
        }
        
        if(y_centroid == 0 || y_centroid==y_centroid_previous){
          direction_y = 0;      
        }
        x_centroid_previous = x_centroid;
        y_centroid_previous = y_centroid;
      }
      
      //Printing Directions for Demonstration
      if(direction_x ==1){
        Serial.println("Going Right");
      }

      if(direction_x ==-1){
        Serial.println("Going Left");
      }
      
      if(direction_y ==1){
        Serial.println("Going Up");
      }
      if(direction_y ==-1){
        Serial.println("Going Down");
      }

      
      previous_max_row = current_max_row;
      previous_max_col = current_max_col;
      frame_count = 0;
      current_max_row=0;
      current_max_col =0;
      direction_y=0;
      direction_x=0;
      sum_centroid_x = 0;
      sum_centroid_y = 0;
      x_centroid = 0;
      y_centroid = 0;
      count_centroid = 0;
      sum_active_pixel=0;
      
//Assigning current timestep parameters to previous
//Resetting current time step variables
      for(int i=0;i<row;i++)
      {
        for(int j=0; j<col;j++)
        {            
          Sum_Pixel_Measurement[i][j] = 0;
          Previous_Pixel_Measurement[i][j] = Current_Pixel_Measurement[i][j];
          Average_Pixel_Measurement[i][j] = 0;
        }
      }
      
      
//Print matrices (if necessary)
//Good for debugging

//      Serial.println("[");
//      for(int i=0;i<row;i++)
//      {
//        for(int j=0; j<col;j++)
//        {            
//          Serial.print(Active_Pixel[i][j]);       
//          Serial.print(", ");
//        }
//        Serial.println();
//      }
//      Serial.println("]");


  
            
  }
}
