
// This variable holds the state of whether we select AUX1 outputs or Preview bus:
bool directlyToProgram = false;

/*************************
 * Set button functions
 *************************/
void buttonFunctions()  {

  // Check if buttons has been pressed down. This is a byte where each bit represents a button-down press:
  uint8_t buttonDownPress = buttons.buttonDownAll();


  // B7: "AUTO"
  if (buttons.isButtonIn(7, buttonDownPress))  {   // Executes button command if pressed:
    AtemSwitcher.doAuto();  // Sends AUTO command
  }
  buttons.setButtonColor(7, AtemSwitcher.getTransitionPosition()>0 ? 4 : 5);  // Sets the AUTO button color to AMBER if a transition is on-going

  // B8: "CUT"
  if (buttons.isButtonIn(8, buttonDownPress))  {   // Executes button command if pressed:
    AtemSwitcher.doCut();  // Sends CUT command
  }
  buttons.setButtonColor(8, buttons.buttonIsPressed(8) ? 4 : 5);  // Sets the CUT button color to AMBER as long as it's pushed


  // B1: "DSK1 ON"
  if (buttons.isButtonIn(1, buttonDownPress))  {   // Executes button command if pressed:
    AtemSwitcher.changeDownstreamKeyOn(1, !AtemSwitcher.getDownstreamKeyerStatus(1));
  }
  buttons.setButtonColor(1, AtemSwitcher.getDownstreamKeyerStatus(1) ? 4 : 5);  // Amber if on

  // B2: "KEY1 ON"
  if (buttons.isButtonIn(2, buttonDownPress))  {   // Executes button command if pressed:
    AtemSwitcher.changeUpstreamKeyOn(1, !AtemSwitcher.getUpstreamKeyerStatus(1));
  }
  buttons.setButtonColor(2, AtemSwitcher.getUpstreamKeyerStatus(1) ? 4 : 5);  // Amber if on

  // B3: "KEY2 ON"
  if (buttons.isButtonIn(3, buttonDownPress))  {   // Executes button command if pressed:
    AtemSwitcher.changeUpstreamKeyOn(2, !AtemSwitcher.getUpstreamKeyerStatus(2));
  }
  buttons.setButtonColor(3, AtemSwitcher.getUpstreamKeyerStatus(2) ? 4 : 5);  // Amber if on


  // B4 is missing here. Should select "Background"


  // B5: "KEY1 next transition ON"
  if (buttons.isButtonIn(5, buttonDownPress))  {   // Executes button command if pressed:
    AtemSwitcher.changeUpstreamKeyNextTransition(1, !AtemSwitcher.getUpstreamKeyerOnNextTransitionStatus(1));
  }
  buttons.setButtonColor(5, AtemSwitcher.getUpstreamKeyerOnNextTransitionStatus(1) ? 4 : 5);  // Amber if on

  // B6: "KEY1 next transition ON"
  if (buttons.isButtonIn(6, buttonDownPress))  {   // Executes button command if pressed:
    AtemSwitcher.changeUpstreamKeyNextTransition(2, !AtemSwitcher.getUpstreamKeyerOnNextTransitionStatus(2));
  }
  buttons.setButtonColor(6, AtemSwitcher.getUpstreamKeyerOnNextTransitionStatus(2) ? 4 : 5);  // Amber if on



  // Sending commands for PREVIEW input selection:
  uint8_t busSelection = previewSelect.buttonDownAll();
  for (uint8_t i=1;i<=8;i++)  {
    if (AtemSwitcher.getPreviewTally(i))  {
      previewSelect.setButtonColor(i, 3);
    } 
    else {
      previewSelect.setButtonColor(i, 5);   
    }
    if (previewSelect.isButtonIn(i, busSelection))  { 
      AtemSwitcher.changePreviewInput(i); 
    }  
  }

  // Sending commands for PROGRAM input selection:
  busSelection = programSelect.buttonDownAll();
  for (uint8_t i=1;i<=8;i++)  {
    if (AtemSwitcher.getProgramTally(i))  {
      programSelect.setButtonColor(i, 2);
    } 
    else {
      programSelect.setButtonColor(i, 5);   
    } 
    if (programSelect.isButtonIn(i, busSelection))  { 
      AtemSwitcher.changeProgramInput(i); 
    }
  }
}



