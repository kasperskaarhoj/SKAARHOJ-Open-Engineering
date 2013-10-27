
// This variable holds the state of whether we select AUX1 outputs or Preview bus:
bool directlyToProgram = false;

/*************************
 * Set button functions
 *************************/
void buttonFunctions()  {


  // Check if buttons has been pressed down. This is a byte where each bit represents a button-down press:
  uint8_t buttonDownPress = buttons.buttonDownAll();


  // B6: "PGM DIRECT"
  if (buttons.isButtonIn(6, buttonDownPress))  {   // Executes button command if pressed:
    directlyToProgram=!directlyToProgram;
  }
  buttons.setButtonColor(6, directlyToProgram ? 2 : 3);  // Red (2) if enabled, otherwise Green (3)
  
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


  // INPUT SELECTION:

  // B1: "SDI 5"
  if (buttons.isButtonIn(1, buttonDownPress))  {   // Executes button command if pressed:
    if (!directlyToProgram) {
      AtemSwitcher.changePreviewInput(5);  // Set input 5 to Preview
    } else {
      AtemSwitcher.changeProgramInput(5);  // Set input 5 to Program
    }
  }
  buttons.setButtonColor(1, AtemSwitcher.getProgramTally(5) ? 2 : (AtemSwitcher.getPreviewTally(5) && !directlyToProgram ? 3 : 5));  // Sets color of button to RED (2) if it has Program Tally, GREEN (3) if Preview tally, otherwise dimmed background lit.

  // B2: "SDI 6"
  if (buttons.isButtonIn(2, buttonDownPress))  {   // Executes button command if pressed:
    if (!directlyToProgram) {
      AtemSwitcher.changePreviewInput(6);  // Set input 6 to Preview
    } else {
      AtemSwitcher.changeProgramInput(6);  // Set input 6 to Program
    }
  }
  buttons.setButtonColor(2, AtemSwitcher.getProgramTally(6) ? 2 : (AtemSwitcher.getPreviewTally(6) && !directlyToProgram ? 3 : 5));  // Sets color of button to RED (2) if it has Program Tally, GREEN (3) if Preview tally, otherwise dimmed background lit.

  // B3: "SDI 7"
  if (buttons.isButtonIn(3, buttonDownPress))  {   // Executes button command if pressed:
    if (!directlyToProgram) {
      AtemSwitcher.changePreviewInput(7);  // Set input 7 to Preview
    } else {
      AtemSwitcher.changeProgramInput(7);  // Set input 7 to Program
    }
  }
  buttons.setButtonColor(3, AtemSwitcher.getProgramTally(7) ? 2 : (AtemSwitcher.getPreviewTally(7) && !directlyToProgram ? 3 : 5));  // Sets color of button to RED (2) if it has Program Tally, GREEN (3) if Preview tally, otherwise dimmed background lit.

  // B4: "SDI 8"
  if (buttons.isButtonIn(4, buttonDownPress))  {   // Executes button command if pressed:
    if (!directlyToProgram) {
      AtemSwitcher.changePreviewInput(8);  // Set input 8 to Preview
    } else {
      AtemSwitcher.changeProgramInput(8);  // Set input 8 to Program
    }
  }
  buttons.setButtonColor(4, AtemSwitcher.getProgramTally(8) ? 2 : (AtemSwitcher.getPreviewTally(8) && !directlyToProgram ? 3 : 5));  // Sets color of button to RED (2) if it has Program Tally, GREEN (3) if Preview tally, otherwise dimmed background lit.

  // B5: "HDMI 1"
  if (buttons.isButtonIn(5, buttonDownPress))  {   // Executes button command if pressed:
    if (!directlyToProgram) {
      AtemSwitcher.changePreviewInput(1);  // Set input 1 to Preview
    } else {
      AtemSwitcher.changeProgramInput(1);  // Set input 1 to Program
    }
  }
  buttons.setButtonColor(5, AtemSwitcher.getProgramTally(1) ? 2 : (AtemSwitcher.getPreviewTally(1) && !directlyToProgram ? 3 : 5));  // Sets color of button to RED (2) if it has Program Tally, GREEN (3) if Preview tally, otherwise dimmed background lit.
}




