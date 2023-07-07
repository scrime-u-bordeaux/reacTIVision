import oscP5.*;
import netP5.*;
import java.util.ArrayList;

OscP5 oscP5;
NetAddress oscAddress;
NetAddress myRemoteLocation;
ArrayList<Amoeba> amoebas;

void setup() {
  size(200,200);
  frameRate(25);
  
  
  // Port d'écoute
  oscP5 = new OscP5(this,3333);
  
  // Port local pour l'envoi des messages OSC
  oscAddress = new NetAddress("127.0.0.1", 8080);  
  
  //liste des fiducials
  amoebas = new ArrayList<Amoeba>();
  
  myRemoteLocation = new NetAddress("192.168.1.28",3333);
}

void draw() {
  background(0); 
  timer();
}

// Méthode de réception des messages OSC
void oscEvent(OscMessage msg) { 
    if (msg.addrPattern().equals("/tuio/2Dobj")) {
      // Récupération du message initial
      Object[] in = msg.arguments();
      Amoeba curentAmoeba;
      OscMessage outMsg;   
      if (in[0].toString().contains("set")){
        int id = parseInt(in[2].toString());
        curentAmoeba = findAmoeba(id);
        outMsg = new OscMessage("/2Dobj/" + id);

        float x = parseFloat(in[3].toString());
        float y = parseFloat(in[4].toString());
        float rotation = parseFloat(in[5].toString());
        Position newPos = new Position(x,y,rotation);
          if(curentAmoeba != null){
              //println("millis avant actualisation " + millis());
              actualiseAmoeba(curentAmoeba, newPos, outMsg,id, millis());
          }
          else{
             createAmoeba(id,newPos,outMsg);
          }             
        outMsg.add(itemToTransfer(in));
        //println("change " + findAmoeba(id).getAlive());
        sendMessage(outMsg);
      }
    }
}

void actualiseAmoeba(Amoeba curentAmoeba, Position pos, OscMessage outMsg,int id, long time){
  curentAmoeba.setPosition(pos);
  curentAmoeba.setLastMsg(outMsg);
  if(!curentAmoeba.getDetected()){
     curentAmoeba.setDetected(true);
     OscMessage aliveMsg = new OscMessage("/2Dobj/" + id);
     Object[] alive = new Object[2] ;
     alive[0] = "alive";
     alive[1] = id;
     aliveMsg.add(alive);
     sendMessage(aliveMsg);
  }
  //println("avantchange " + curentAmoeba.getAlive());
  curentAmoeba.setAlive(time);
  //println("aprèschange" + curentAmoeba.getAlive());
  int index = amoebas.indexOf(curentAmoeba);
  amoebas.set(index,curentAmoeba); 
  //println("dans la liste " + amoebas.get(index).getAlive());
}

void createAmoeba(int id, Position pos, OscMessage outMsg){
  Amoeba amib = new Amoeba(id,pos,outMsg);
  amib.setAlive(millis());
  amoebas.add(amib);
  OscMessage aliveMsg = new OscMessage("/2Dobj/" + id);
  Object[] alive = new Object[2] ;
  alive[0] = "alive";
  alive[1] = id;
  aliveMsg.add(alive);
  sendMessage(aliveMsg);
}

Object [] itemToTransfer(Object[] in){
  Object[] transfer = new Object[5] ;
  transfer[0] = in[0];
  transfer[1] = in[2];
  transfer[2] = in[3];
  transfer[3] = in[4];
  transfer[4] = in[5];
  return transfer;
}

Amoeba findAmoeba (int id){
      for (Amoeba a : amoebas) {
        if(a.getId() == id){
          return a;
        }
      }
      
  return null;
}


void timer(){
  for (Amoeba a : amoebas) {
    if(a.getAlive() !=0 && a.getDetected()){
       //println("birht" + a.getAlive());
       //println("millis" + millis());
       long time = millis() - a.getAlive();
       if(time >= 500){
         a.setDetected(false);
         //envoyer un messege de mort a score
         OscMessage deathMsg = new OscMessage("/2Dobj/" + a.getId());
         Object[] died = new Object[2] ;
         died[0] = "death";
         println("----- mort");
         //println(time);
         died[1] = a.getId();
         deathMsg.add(died);
         sendMessage(deathMsg);
       }
    }
  }
}

void sendMessage(OscMessage outMsg){  
   //OscP5.flush(outMsg,oscAddress);
   oscP5.send(outMsg,oscAddress);
}

// Méthode appelée lorsque la fenêtre de l'application est fermée
void stop() {
  oscP5.stop();
  super.stop();
}
