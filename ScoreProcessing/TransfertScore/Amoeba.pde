import oscP5.*;

class Amoeba {
  private int id;
  private Position pos;
  private OscMessage lastMsg;
  private boolean detected = true;
  private long alive;
  
  Amoeba(int id, Position pos, OscMessage lastMsg){
    this.id = id;
    this.pos = pos;
    this.lastMsg = lastMsg;
  }
  
  int getId(){
    return id;
  }
  
  Position getPos(){
    return pos;
  }
  
  OscMessage getLastMsg(){
    return lastMsg;
  }
  
  boolean getDetected(){
    return detected;
  }
  
  long getAlive(){
    return alive;
  }
  
  //a voir si on en fait des individuels ??
  void setPosition(Position pos){
    this.pos = pos;
  }
  
  void setLastMsg(OscMessage lastMsg){
    this.lastMsg = lastMsg;
  }
  
  void toggleDetected(){
    detected = !detected;
  }
  
  void setDetected(boolean detect){
    detected = detect;
  }
  
  void setAlive(long alive){
    this.alive = alive;
  }
  
}
