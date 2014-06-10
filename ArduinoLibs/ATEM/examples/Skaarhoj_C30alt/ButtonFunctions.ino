
// This variable holds the state of whether we select AUX1 outputs or Preview bus:
bool AUX1busSelect = false;

/*************************
 * Set button functions
 *************************/
void buttonFunctions()  {


  // Check if buttons has been pressed down. This is a byte where each bit represents a button-down press:
  uint8_t buttonDownPress = buttons.buttonDownAll();

  // B1: "AUX1 BUS"
  if (buttons.isButtonIn(1, buttonDownPress))  {   // Executes button command if pressed:
    AUX1busSelect=!AUX1busSelect;
  }
  buttons.setButtonColor(1, AUX1busSelect ? 4 : 5);  // Sets color of button to AMBER (2) if in AUX1 BUS mode. Otherwise color "5" which is dimmed yellow

  // B2: "DSK1"
  if (buttons.isButtonIn(2, buttonDownPress))  {   // Executes button command if pressed:
    bool DSK1CurrentStatus = AtemSwitcher.getDownstreamKeyerStatus(1);
    AtemSwitcher.changeDownstreamKeyOn(1,!DSK1CurrentStatus);  // Toggle downstream keyer 1 by sending the opposite value of the current value.
  }
  buttons.setButtonColor(2, AtemSwitcher.getDownstreamKeyerStatus(1) ? 2 : 5);  // Sets color of button to RED (2) if DSK1 state in the switcher is active. Otherwise color "5" which is dimmed yellow

  // B3: "Lower Third"
  if (buttons.isButtonIn(3, buttonDownPress))  {   // Executes button command if pressed:
    bool KEY4CurrentStatus = AtemSwitcher.getUpstreamKeyerStatus(4);
    AtemSwitcher.changeUpstreamKeyOn(4,!KEY4CurrentStatus);  // Toggle upstream keyer 4 by sending the opposite value of the current value.
  }
  buttons.setButtonColor(3, AtemSwitcher.getUpstreamKeyerStatus(4) ? 2 : 5);  // Sets color of button to RED (2) if upstream keyer 4 is active. Otherwise color "5" which is dimmed yellow

  // B7: "Fade To Black"
  if (buttons.isButtonIn(7, buttonDownPress))  {   // Executes button command if pressed:
    AtemSwitcher.fadeToBlackActivate();
  }
  if (AtemSwitcher.getFadeToBlackState())  {  // Setting button color. This is a more complex example which includes blinking during execution:
    if (AtemSwitcher.getFadeToBlackTime()>0 && (AtemSwitcher.getFadeToBlackFrameCount()!=AtemSwitcher.getFadeToBlackTime()))  {  // It's important to test if fadeToBlack time is more than zero because it's the kind of state from the ATEM which is usually not captured during initialization. Hopefull this will be solved in the future.
        // Blinking with AMBER color if Fade To Black is executing:
      if ((unsigned long)millis() & B10000000)  {
        buttons.setButtonColor(7, 4);  
      } 
      else {
        buttons.setButtonColor(7, 5); 
      }
    } 
    else {
      buttons.setButtonColor(7, 2);  // Sets color of button to RED (2) if Fade To Black is activated
    }
  } 
  else {
    buttons.setButtonColor(7, 5);  // Dimmed background if no fade to black
  }

  // B8: "CUT"
  if (buttons.isButtonIn(8, buttonDownPress))  {   // Executes button command if pressed:
    AtemSwitcher.doCut();  // Sends CUT command
  }
  buttons.setButtonColor(8, buttons.buttonIsPressed(8) ? 4 : 5);  // Sets the CUT button color to AMBER as long as it's pushed


  // INPUT SELECTION:
  // Now, take care of the three bus input select buttons. These changes function if the "AUX1 BUS" button was pressed:
  if (AUX1busSelect)  {
    // If AUX1 selection is enabled:    

    // B4: "1"
    if (buttons.isButtonIn(4, buttonDownPress))  {   // Executes button command if pressed:
      AtemSwitcher.changeAuxState(1,1);  // Set input 1 to AUX1
    }
    buttons.setButtonColor(4, AtemSwitcher.getAuxState(1)==1 ? 2 : 5);  // Sets color of button to RED (2) if it's active on AUX1

    // B5: "2"
    if (buttons.isButtonIn(5, buttonDownPress))  {   // Executes button command if pressed:
      AtemSwitcher.changeAuxState(1,2);  // Set input 2 to AUX1
    }
    buttons.setButtonColor(5, AtemSwitcher.getAuxState(1)==2 ? 2 : 5);  // Sets color of button to RED (2) if it's active on AUX1

    // B6: "3"
    if (buttons.isButtonIn(6, buttonDownPress))  {   // Executes button command if pressed:
      AtemSwitcher.changeAuxState(1,3);  // Set input 3 to AUX1
    }
    buttons.setButtonColor(6, AtemSwitcher.getAuxState(1)==3 ? 2 : 5);  // Sets color of button to RED (2) if it's active on AUX1
  } 
  else {
    // Normal operation (Preview select, program indication):

    // B4: "1"
    if (buttons.isButtonIn(4, buttonDownPress))  {   // Executes button command if pressed:
      AtemSwitcher.changePreviewInput(1);  // Set input 1 to Preview
    }
    buttons.setButtonColor(4, AtemSwitcher.getProgramTally(1) ? 2 : (AtemSwitcher.getPreviewTally(1) ? 3 : 5));  // Sets color of button to RED (2) if it has Program Tally, GREEN (3) if Preview tally, otherwise dimmed background lit.

    // B5: "2"
    if (buttons.isButtonIn(5, buttonDownPress))  {   // Executes button command if pressed:
      AtemSwitcher.changePreviewInput(2);  // Set input 1 to Preview
    }
    buttons.setButtonColor(5, AtemSwitcher.getProgramTally(2) ? 2 : (AtemSwitcher.getPreviewTally(2) ? 3 : 5));  // Sets color of button to RED (2) if it has Program Tally, GREEN (3) if Preview tally, otherwise dimmed background lit.

    // B6: "3"
    if (buttons.isButtonIn(6, buttonDownPress))  {   // Executes button command if pressed:
      AtemSwitcher.changePreviewInput(3);  // Set input 1 to Preview
    }
    buttons.setButtonColor(6, AtemSwitcher.getProgramTally(3) ? 2 : (AtemSwitcher.getPreviewTally(3) ? 3 : 5));  // Sets color of button to RED (2) if it has Program Tally, GREEN (3) if Preview tally, otherwise dimmed background lit.
  }
}




