#include "servo.h"
#include "../def.h"
#include <Arduino.h>
// Di chuyển servo đến góc mục tiêu
void moveServoTo(int target){
  // Nếu góc mục tiêu lớn hơn góc hiện tại
  if(target > headDegree){
    // Tăng góc từ góc hiện tại đến góc mục tiêu
    for(int i=headDegree;i<=target;i+=degreePerTurn){
      myServo.write(i);
      delay(0);
    }
  // Nếu góc mục tiêu nhỏ hơn góc hiện tại
  }else{
    // Giảm góc từ góc hiện tại đến góc mục tiêu
    for(int i=headDegree;i>=target;i-=degreePerTurn){
      myServo.write(i);
      delay(0);
    }
  }
  headDegree = target;
}