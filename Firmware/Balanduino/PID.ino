void PID(double restAngle, double offset, double turning) {
  /* Steer robot */
  if (steerForward) {
    if(wheelVelocity < 0)
      offset += (double)wheelVelocity/(velocityScaleMove*0.8); // Scale down offset at high speed - wheel velocity is negative when driving forward
    restAngle -= offset;
  } 
  else if (steerBackward) {
    if(wheelVelocity > 0)
      offset -= (double)wheelVelocity/velocityScaleMove; // Scale down offset at high speed - wheel velocity is negative when driving forward
    restAngle += offset;
  }
  /* Brake */
  else if (steerStop) {
    int32_t positionError = wheelPosition - targetPosition;
    if (abs(positionError) > zoneA) // Inside zone A
      restAngle -= (double)positionError/positionScaleA;
    else if (abs(positionError) > zoneB) // Inside zone B
      restAngle -= (double)positionError/positionScaleB;
    else // Inside zone C
      restAngle -= (double)positionError/positionScaleC;   
    restAngle -= (double)wheelVelocity/velocityScaleStop;
    if (restAngle < 175) // Limit rest Angle
      restAngle = 175;
    else if (restAngle > 185)
      restAngle = 185;
  }
  
  if(restAngle - lastRestAngle > 1) // Don't change restAngle with more than 1 degree in each loop
    restAngle = lastRestAngle+1;
  else if(restAngle - lastRestAngle < -1)
    restAngle = lastRestAngle-1;
  lastRestAngle = restAngle;  
  
  /* Update PID values */
  double error = (restAngle - pitch);
  double pTerm = Kp * error;
  iTerm += Ki * error;
  double dTerm = Kd * (error - lastError);
  lastError = error;
  double PIDValue = pTerm + iTerm + dTerm;

  /* Steer robot sideways */
  if (steerLeft) {
    turning -= abs((double)wheelVelocity/velocityScaleTurning); // Scale down at high speed
    if(turning < 0)
      turning = 0;
    PIDLeft = PIDValue-turning;
    PIDRight = PIDValue+turning;
  }
  else if (steerRight) {
    turning -= abs((double)wheelVelocity/velocityScaleTurning); // Scale down at high speed
    if(turning < 0)
      turning = 0;
    PIDLeft = PIDValue+turning;
    PIDRight = PIDValue-turning;
  }
  else {
    PIDLeft = PIDValue;
    PIDRight = PIDValue;
  }

  //PIDLeft *= 0.95; // compensate for difference in the motors

  /* Set PWM Values */
  if (PIDLeft >= 0)
    moveMotor(left, forward, PIDLeft);
  else
    moveMotor(left, backward, -PIDLeft);
  if (PIDRight >= 0)
    moveMotor(right, forward, PIDRight);
  else
    moveMotor(right, backward, -PIDRight);
}
