// myFunctions.cpp

#include "Template.h"

bool server_connected = false;
WiFiClient client;
const char* ssid = "Nicu-2.4"; // wifi name
const char* password = "test_test"; // wifi pass
const char* serverIP = "192.168.1.131";  // server ip
const int serverPort = 55004;


// send commands + verify
bool send_data_v(String data){
  if (!server_connected) return false;
    Send_to_server(data);
    while (!client.available()) {
    delay(100);
    }
    String _data = Read_from_server();
    if(_data.indexOf(data) != -1){
      return true;
    }
    return false;
}
// send string to a server
void Send_to_server(String data) {
  if (server_connected) {
    client.print(data);
    Serial.print("Send ");
    Serial.println(data);
  } else {
    Serial.println("Client not connected");
  }
}
// read string from server
String Read_from_server() {
  if (!server_connected) return "Client not connected";

  if (client.available()) {
    String data = client.readStringUntil('\n');
    data.trim();
    return data;
  }
  return "No data readed from Server";
}


// {INIT helper} trying to connect to wifi otherwise server_connected = false;
void Conn_to_wifi() {
  WiFi.begin(ssid, password);
  int ct =0;
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
    // after 60s of tying it returns
    if(ct > 10){
      Serial.println("No connection to WiFi");
      server_connected = false;
      return;
    }
    ct++;
  }
  Serial.println("Connected to WiFi");
}
// {INIT helper} trying to connect to server otherwise server_connected = false;
void Conn_to_server() {
  for(int i=0; i<10;i++){
    if (client.connect(serverIP, serverPort)) {
      server_connected = true;
      Serial.println("Connected to server");
      return;
    }
    Serial.println("Trying to connect to server");
  }
  Serial.println("Failed to connect to the server");
  server_connected = false;
}
// {INIT helper} send board name and verify reciving it
bool Server_test_ok(String board_name) {
  if (!server_connected) return false;

  Send_to_server("name " + board_name);

  int connection_tires = 0;
  while (!client.available()) {
    delay(100);
    connection_tires++;
    if (connection_tires > 10) return false;
  }

  String server_data = Read_from_server();

  String _board_name = server_data.substring(server_data.indexOf(board_name), server_data.indexOf(board_name) + board_name.length());

  if (_board_name == board_name && server_data.indexOf(board_name) != -1) {
    Serial.println("Server_test: " + _board_name + " - OK");
    return true;
  }
  Serial.println("Server_test: " + _board_name + " - Error");
  Serial.println("Actual Input: " + _board_name);
  return false;
}
// INIT
bool Board_init(String board_name,String commands_auto,String commands_manual,String status){
  Serial.println("===============================");
  if(!server_connected){
    Conn_to_wifi();
    Conn_to_server();
  }else{
    send_data_v("connected");
  }
  if(Server_test_ok(board_name)){
    if(send_data_v(commands_auto) && send_data_v(commands_manual) && send_data_v(status)){
      return true;
      Serial.println(F("ONLINE"));
    }
    return false;
    Serial.println(F("OFFLINE"));
  }
  return false;
  Serial.println(F("OFFLINE"));
}










