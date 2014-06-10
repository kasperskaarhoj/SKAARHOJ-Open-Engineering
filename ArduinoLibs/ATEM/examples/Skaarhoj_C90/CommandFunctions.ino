void commandCut()  {
  cmdSelect.setButtonColor(1, 4);    // Highlight CUT button

  // If VGA is the one source, make Auto instead!
  if (AtemSwitcher.getProgramInput()==8 || AtemSwitcher.getPreviewInput()==8)  {
    AtemSwitcher.doAuto(); 
  } 
  else {
    AtemSwitcher.doCut(); 
  }
  preVGA_active = false;
}
void commandAuto()  {
  AtemSwitcher.doAuto(); 
  preVGA_active = false;
}
void commandPIP()  {
  // For Picture-in-picture, do an "auto" transition:
  unsigned long timeoutTime = millis()+5000;

  // First, store original preview input:
  uint8_t tempPreviewInput = AtemSwitcher.getPreviewInput();  

  // Then, set preview=program (so auto doesn't change input)
  AtemSwitcher.changePreviewInput(AtemSwitcher.getProgramInput());  
  while(AtemSwitcher.getProgramInput()!=AtemSwitcher.getPreviewInput())  {
    AtemSwitcher.runLoop();
    if (timeoutTime<millis()) {
      break;
    }
  }

  // Then set transition status:
  bool tempOnNextTransitionStatus = AtemSwitcher.getUpstreamKeyerOnNextTransitionStatus(1);
  AtemSwitcher.changeUpstreamKeyNextTransition(1, true);  // Set upstream key next transition
  while(!AtemSwitcher.getUpstreamKeyerOnNextTransitionStatus(1))  {  
    AtemSwitcher.runLoop();
    if (timeoutTime<millis()) {
      break;
    }
  }

  // Make Auto Transition:      
  AtemSwitcher.doAuto();
  while(AtemSwitcher.getTransitionPosition()==0)  {
    AtemSwitcher.runLoop();
    if (timeoutTime<millis()) {
      break;
    }
  }
  while(AtemSwitcher.getTransitionPosition()>0)  {
    AtemSwitcher.runLoop();
    if (timeoutTime<millis()) {
      break;
    }
  }

  // Then reset transition status:
  AtemSwitcher.changeUpstreamKeyNextTransition(1, tempOnNextTransitionStatus);
  while(tempOnNextTransitionStatus!=AtemSwitcher.getUpstreamKeyerOnNextTransitionStatus(1))  {  
    AtemSwitcher.runLoop();
    if (timeoutTime<millis()) {
      break;
    }
  }
  // Reset preview bus:
  AtemSwitcher.changePreviewInput(tempPreviewInput);  
  while(tempPreviewInput!=AtemSwitcher.getPreviewInput())  {
    AtemSwitcher.runLoop();
    if (timeoutTime<millis()) {
      break;
    }
  }
  // Finally, tell us how we did:
  if (timeoutTime<millis()) {
    Serial.println("Timed out during operation!");
  } 
  else {
    Serial.println("DONE!");
  }
}
void commandPIPVGA()  {
  if (!preVGA_active)  {
    preVGA_active = true;
    preVGA_UpstreamkeyerStatus = AtemSwitcher.getUpstreamKeyerStatus(1);
    preVGA_programInput = AtemSwitcher.getProgramInput();

    AtemSwitcher.changeProgramInput(8);
    AtemSwitcher.changeUpstreamKeyOn(1, true); 
  } 
  else {
    preVGA_active = false;
    AtemSwitcher.changeProgramInput(preVGA_programInput);
    AtemSwitcher.changeUpstreamKeyOn(1, preVGA_UpstreamkeyerStatus); 
  }
}
void commandDSK1()  {
  AtemSwitcher.changeDownstreamKeyOn(1, !AtemSwitcher.getDownstreamKeyerStatus(1));
}


