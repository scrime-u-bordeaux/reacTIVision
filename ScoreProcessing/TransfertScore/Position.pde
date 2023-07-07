class Position {
  private float x;
  private float y;
  private float angle;

 Position(float x, float y,float angle) {
    this.x = x;
    this.y = y;
    this.angle = angle;
  }
  Position() {
    this.x = -1;
    this.y = -1;
    this.angle = 0;
  }
  
  float getX(){
    return x;
  }
  
  float getY(){
    return y;
  }
  float getAngle(){
    return angle;
  }
  
  void setX(float x){
    this.x = x;
  }
  
  void setY(float y){
    this.y = y;
  }
   void setAngle(float angle){
    this.angle = angle;
  }
  
}
