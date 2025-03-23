/*pinouts:
CS--------53
SCK-------52
MOSI------51
MISO------50
*/
#include <formatfloat.h>
#include <protoVECTOR.h>
#include <ctype.h>
#include <SPI.h>
#include <SD.h>
class protoSD{
private:
  File data;
  int pin;
  protoVECTOR vector;
  formatfloat formatter; 
public:
  protoSD(int card_pin){
    pin=card_pin;
  }
  int vector_size(){
    return vector.vector_size();
  }
  float empty_float(){
    return vector.empty_float();
  }
  String empty_string(){
    return vector.empty_string();
  }
  char emtpy_char(){
    return vector.emtpy_char();
  }
  int empty_int(){
    return vector.empty_int();
  }
  void begin(){
    while(!SD.begin(pin)){
      Serial.println("mode [begin] ~ card: unavailable");
      delay(100);
    }
    Serial.println("mode [begin] ~ card: available");
  }
  bool find(const char* path){
    return SD.exists(path);
  }
  void create_directory(String path){
    String directory="/"+path+"/";
    directory.toLowerCase();
    const char* directory_path=directory.c_str();
    if(!find(directory_path)){
      while(!SD.mkdir(directory_path)){
        Serial.println("mode [create_dir] ~ creating directory : "+directory);
      }
      Serial.println("mode [creat_dir] ~ "+directory+" : creation succeed!");
    }
    else{
      Serial.println("mode [create_dir] ~ "+directory+" : already exists");
      return;
    }
  }
  void listfiles(String path){
    String directory=(path=="")? "/" : "/"+path+"/";
    directory.toLowerCase();
    const char* directory_path=directory.c_str();
    data=SD.open(directory_path);
    if(!data){
      Serial.println("mode [list] ~ "+directory+": inaccessible");
      return;
    }
    else{
      Serial.println("mode [list] ~ listing files in : "+directory);
      while (true){
        File entry=data.openNextFile();
        if(!entry){
          break;
        }
        else{
          if(entry.isDirectory()){
            Serial.println("mode [list] ~ dir : "+String(entry.name()));
          }
          else{
            Serial.println("mode [list] ~ file : "+String(entry.name()));
          }
        }
        entry.close();
      }
    }
    data.close();
  }
  void remove_file(String path, String filename){
    String directory=(path=="")? "/"+filename+".txt" : "/"+path+"/"+filename+".txt";
    directory.toLowerCase();
    const char* directory_path=directory.c_str();
    while(find(directory_path)){
      while(!SD.remove(directory_path)){
        SD.remove(directory_path);
      }
    }
    Serial.println("mode [remove_file] ~ removing : "+directory+" done!");
  }
  void clearfile(String path, String filename){
    String directory=(path=="")? "/"+filename+".txt" : "/"+path+"/"+filename+".txt";
    directory.toLowerCase();
    const char* directory_path=directory.c_str();
    if(!find(directory_path)){
      Serial.println("mode [clear] ~ "+directory+" : inaccessible");
      return;
    }
    else{
      remove_file(path, filename);
      while(!data){
        data=SD.open(directory_path, FILE_WRITE);
      }
      data.close();
      Serial.println("mode [clear] ~ "+directory+" :  cleared!");
      return;
    }
  }
  void write_string(String path, String filename, String content){
    String filepath=(path=="")? "/"+filename+".txt" : "/"+path+"/"+filename+".txt";
    filepath.toLowerCase();
    content.toLowerCase();
    const char* write_path=filepath.c_str();
    const char* input=content.c_str();
    if(path!=""){
      while(!find(write_path)){
        Serial.println("mode [write_str] ~ creating : "+filepath);
        create_directory(path);
        data=SD.open(write_path, FILE_WRITE);
      }
      while(!data){
        data=SD.open(write_path, FILE_WRITE);
      }
      Serial.println("mode [write_str] ~ writting to : "+filepath);
      data.println(input);
      data.flush();
      data.close();
      Serial.println("mode [write_str] ~ writting done!");
      return;
    }
    else{
      while(!find(write_path)){
        Serial.println("mode [write_str] ~ creating : "+filepath);
        data=SD.open(write_path, FILE_WRITE);
      }
      while(!data){
        data=SD.open(write_path, FILE_WRITE);
      }
      Serial.println("mode [write_str] ~ writting to : "+filepath);
      data.println(input);
      data.flush();
      data.close();
      Serial.println("mode [write_float] ~ writting done!");
      return;
    }
  }
  String dump_string(String path, String filename, int index){
    String directory=(path=="")? "/"+filename+".txt" : "/"+path+"/"+filename+".txt";
    directory.toLowerCase();
    const char* directory_path=directory.c_str();
    while(true){
      if(vector.front_string()!=vector.empty_string()){
        break;
      }
      else{
        if(!find(directory_path)){
          Serial.println("mode [dump_str] ~ "+directory+" : inaccessible");
          break;
        }
        else{
          while(!data){
            data=SD.open(directory_path, FILE_READ);
          }
          Serial.println("mode [dump_str] ~ source: "+directory);
          Serial.print("mode [dump_str] ~ reading file");
          while(data.available()){
            String output_string=data.readStringUntil('\n');
            output_string.trim();
            vector.push_string(output_string);
            Serial.print(".");
          }
          Serial.print("\n");
          Serial.println("mode [dump_str] ~ reading file done!");
          data.close();
        }
        Serial.println("mode [dump_str] ~ refreshing the file");
        clearfile(path, filename);
        for(int i=0; i<vector.vector_size(); i++){
          if(vector.get_string(i)!=vector.empty_string()){
            write_string(path, filename, vector.get_string(i));
          }
          else{
            break;
          }
        }
        Serial.println("mode [dump_str] ~ refreshing the file done!");
        break;
      }
    }
    return vector.get_string(index);
  }
  void write_float(String path, String filename, float input){
    String filepath=(path=="")? "/"+filename+".txt" : "/"+path+"/"+filename+".txt";
    filepath.toLowerCase();
    const char* write_path=filepath.c_str();
    float formatted_float=formatter.format(input);
    if(path!=""){
      while(!find(write_path)){
        Serial.println("mode [write_float] ~ creating "+filepath);
        create_directory(path);
        data=SD.open(write_path, FILE_WRITE);
      }
      while(!data){
        data=SD.open(write_path, FILE_WRITE);
      }
      Serial.println("mode [write_float] ~ writting to : "+filepath);
      data.println(formatted_float, 4);
      data.flush();
      data.close();
      Serial.println("mode [write_float] ~ writting done!");
      return;
    }
    else{
      while(!find(write_path)){
        Serial.println("mode [write_float] ~ creating "+filepath);
        data=SD.open(write_path, FILE_WRITE);
      }
      while(!data){
        data=SD.open(write_path, FILE_WRITE);
      }
      Serial.println("mode [write_float] ~ writting to : "+filepath);
      data.println(formatted_float, 4);
      data.flush();
      data.close();
      Serial.println("mode [write_float] ~ writting done!");
      return;
    }
  }
  float dump_float(String path, String filename, int index){
    String directory=(path=="")? "/"+filename+".txt" : "/"+path+"/"+filename+".txt";
    directory.toLowerCase();
    const char* read_path=directory.c_str();
    while(true){
      if(vector.front_float()!=vector.empty_float()){
        break;
      }
      else{
        if(!find(read_path)){
          Serial.println("mode [dump_float] ~ "+directory+" inaccessble");
          break;
        }
        else{
          while(!data){
            data=SD.open(read_path, FILE_READ);
          }
          Serial.println("mode [dump_float] ~ source: "+directory);
          Serial.print("mode [dump_float] ~ reading file");
          while(data.available()){
            String output_float=data.readStringUntil('\n');
            float result_float=output_float.toFloat();
            vector.push_float(result_float);
            Serial.print(".");
          }
          data.close();
          Serial.print("\n");
          Serial.println("mode [dump_float] ~ reading done!");
        }
        Serial.println("mode [dump_float] ~ refreshing the file");
        clearfile(path, filename);
        for(int i=0; i<vector.vector_size(); i++){
          if(vector.get_float(i)!=vector.empty_float()){
            write_float(path, filename, vector.get_float(i));
          }
          else{
            break;
          }
        }
        Serial.println("mode [dump_int] ~ refreshing the file done!");
        break;
      }
    }
    return vector.get_float(index);
  }
void write_char(String path, String filename, char input){
  String filepath=(path=="")? "/"+filename+".txt" : "/"+path+"/"+filename+".txt";
  filepath.toLowerCase();
  const char* write_path=filepath.c_str();
  if(path!=""){
      while(!find(write_path)){
        Serial.println("mode [write_char] ~ creating "+filepath);
        create_directory(path);
        data=SD.open(write_path, FILE_WRITE);
      }
      while(!data){
        data=SD.open(write_path, FILE_WRITE);
      }
      Serial.println("mode [write_char] ~ writting to : "+filepath);
      data.println(input);
      data.flush();
      data.close();
      Serial.println("mode [write_char] ~ writting done!");
      return;
  }
  else{
      while(!find(write_path)){
        Serial.println("mode [write_char] ~ creating "+filepath);
        data=SD.open(write_path, FILE_WRITE);
      }
      while(!data){
        data=SD.open(write_path, FILE_WRITE);
      }
      Serial.println("mode [write_char] ~ writting to : "+filepath);
      data.println(input);
      data.flush();
      data.close();
      Serial.println("mode [write_char] ~ writting done!");
      return;
    }
}
char dump_char(String path, String filename, int index){
    String directory=(path=="")? "/"+filename+".txt" : "/"+path+"/"+filename+".txt";
    directory.toLowerCase();
    const char* read_path=directory.c_str();
    while(true){
      if(vector.front_char()!=vector.emtpy_char()){
        break;
      }
      else{
        if(!find(read_path)){
          Serial.println("mode [dump_char] ~ "+directory+" inaccessble");
          break;
        }
        else{
          while(!data){
            data=SD.open(read_path, FILE_READ);
          }
          Serial.println("mode [dump_char] ~ source: "+directory);
          Serial.print("mode [dump_char] ~ reading file");
          while(data.available()){
            char buffer[2];
            data.readBytesUntil('\n', buffer, sizeof(buffer));
            vector.push_char(buffer[0]);
            Serial.print(".");
          }
          data.close();
          Serial.print("\n");
          Serial.println("mode [dump_char] ~ reading done!");
        }
        Serial.println("mode [dump_char] ~ refreshing the file");
        clearfile(path, filename);
        for(int i=0; i<vector.vector_size(); i++){
          if(vector.get_char(i)!=vector.emtpy_char()){
            write_char(path, filename, vector.get_char(i));
          }
          else{
            break;
          }
        }
        Serial.println("mode [dump_char] ~ refreshing the file done!");
        break;
      }
    }
    return vector.get_char(index);
  }
void write_int(String path, String filename, int input){
  String filepath=(path=="")? "/"+filename+".txt" : "/"+path+"/"+filename+".txt";
  filepath.toLowerCase();
  const char* write_path=filepath.c_str();
  if(path!=""){
      while(!find(write_path)){
        Serial.println("mode [write_int] ~ creating "+filepath);
        create_directory(path);
        data=SD.open(write_path, FILE_WRITE);
      }
      while(!data){
        data=SD.open(write_path, FILE_WRITE);
      }
      Serial.println("mode [write_int] ~ writting to : "+filepath);
      data.println(input);
      data.flush();
      data.close();
      Serial.println("mode [write_int] ~ writting done!");
      return;
  }
  else{
      while(!find(write_path)){
        Serial.println("mode [write_int] ~ creating "+filepath);
        data=SD.open(write_path, FILE_WRITE);
      }
      while(!data){
        data=SD.open(write_path, FILE_WRITE);
      }
      Serial.println("mode [write_int] ~ writting to : "+filepath);
      data.println(input);
      data.flush();
      data.close();
      Serial.println("mode [write_int] ~ writting done!");
      return;
    }
}
int dump_int(String path, String filename, int index){
    String directory=(path=="")? "/"+filename+".txt" : "/"+path+"/"+filename+".txt";
    directory.toLowerCase();
    const char* read_path=directory.c_str();
    while(true){
      if(vector.front_int()!=vector.empty_int()){
        break;
      }
      else{
        if(!find(read_path)){
          Serial.println("mode [dump_int] ~ "+directory+" inaccessble");
          break;
        }
        else{
          while(!data){
            data=SD.open(read_path, FILE_READ);
          }
          Serial.println("mode [dump_int] ~ source: "+directory);
          Serial.print("mode [dump_int] ~ reading file");
          while(data.available()){
            String output=data.readStringUntil('\n');
            int result_integer=output.toInt();
            vector.push_int(result_integer);
            Serial.print(".");
          }
          data.close();
          Serial.print("\n");
          Serial.println("mode [dump_int] ~ reading done!");
        }
        Serial.println("mode [dump_int] ~ refreshing the file");
        clearfile(path, filename);
        for(int i=0; i<vector.vector_size(); i++){
          if(vector.get_int(i)!=vector.empty_int()){
            write_int(path, filename, vector.get_int(i));
          }
          else{
            break;
          }
        }
        Serial.println("mode [dump_int] ~ refreshing the file done!");
        break;
      }
    }
  return vector.get_int(index);
}
};
protoSD card(53);
void setup(){
  Serial.begin(2000000);
  card.begin();
  card.write_string("", "password", "playersapugmobile2021");
  for(int i=0; i<card.vector_size(); i++){
    if(card.dump_string("", "password", i)!=card.empty_string()){
      String password="playersapugmobile2021";
      if(card.dump_string("", "password", i)!=password){
        continue;
      }
      else{
        Serial.println("password : correct");
      }
    }
    else{
      break;
    }
  }
}
void loop(){

}