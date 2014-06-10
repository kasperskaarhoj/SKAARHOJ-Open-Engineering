
void extraButtonsCommands2()  {
  
  // Check if buttons has been pressed down. This is a byte where each bit represents a button-down press:
  uint8_t buttonDownPress = extraButtons.buttonDownAll();

  // B1: "BARS"
  if (extraButtons.isButtonIn(1, buttonDownPress))  {   // Executes button command if pressed:
    AtemSwitcher.changePreviewInput(9);
  }
  extraButtons.setButtonColor(1, AtemSwitcher.getPreviewInput()==9 ? 3 : 5);  // Sets color of button to AMBER (2) if in AUX1 BUS mode. Otherwise color "5" which is dimmed yellow
 
  // B2: "COL1"
  if (extraButtons.isButtonIn(2, buttonDownPress))  {   // Executes button command if pressed:
    AtemSwitcher.changePreviewInput(10);
  }
  extraButtons.setButtonColor(2, AtemSwitcher.getPreviewInput()==10 ? 3 : 5);  // Sets color of button to AMBER (2) if in AUX1 BUS mode. Otherwise color "5" which is dimmed yellow
 
  // B3: "Key1"
  if (extraButtons.isButtonIn(3, buttonDownPress))  {   // Executes button command if pressed:
    AtemSwitcher.changeUpstreamKeyOn(1,!AtemSwitcher.getUpstreamKeyerStatus(1));  // Toggle upstream keyer 1 by sending the opposite value of the current value.
  }
  extraButtons.setButtonColor(3, AtemSwitcher.getUpstreamKeyerStatus(1) ? 2 : 5);  // Sets color of button to RED (2) if upstream keyer 4 is active. Otherwise color "5" which is dimmed yellow

  // B4: "Key2"
  if (extraButtons.isButtonIn(4, buttonDownPress))  {   // Executes button command if pressed:
    AtemSwitcher.changeUpstreamKeyOn(2,!AtemSwitcher.getUpstreamKeyerStatus(2));  // Toggle upstream keyer 1 by sending the opposite value of the current value.
  }
  extraButtons.setButtonColor(4, AtemSwitcher.getUpstreamKeyerStatus(2) ? 2 : 5);  // Sets color of button to RED (2) if upstream keyer 4 is active. Otherwise color "5" which is dimmed yellow

  // B5: "PIP"
  if (extraButtons.isButtonIn(5, buttonDownPress))  {   // Executes button command if pressed:
    cmd_pipToggle();
  }
  extraButtons.setButtonColor(5, AtemSwitcher.getUpstreamKeyerStatus(1) ? 2 : 5);  // Sets color of button to RED (2) if upstream keyer 4 is active. Otherwise color "5" which is dimmed yellow

  // B6: "Fade To Black"
  if (extraButtons.isButtonIn(6, buttonDownPress))  {   // Executes button command if pressed:
    AtemSwitcher.fadeToBlackActivate();
  }
  if (AtemSwitcher.getFadeToBlackState())  {  // Setting button color. This is a more complex example which includes blinking during execution:
    if (AtemSwitcher.getFadeToBlackTime()>0 && (AtemSwitcher.getFadeToBlackFrameCount()!=AtemSwitcher.getFadeToBlackTime()))  {  // It's important to test if fadeToBlack time is more than zero because it's the kind of state from the ATEM which is usually not captured during initialization. Hopefull this will be solved in the future.
        // Blinking with AMBER color if Fade To Black is executing:
      if ((unsigned long)millis() & B10000000)  {
        extraButtons.setButtonColor(6, 4);  
      } 
      else {
        extraButtons.setButtonColor(6, 5); 
      }
    } 
    else {
      extraButtons.setButtonColor(6, 2);  // Sets color of button to RED (2) if Fade To Black is activated
    }
  } 
  else {
    extraButtons.setButtonColor(6, 5);  // Dimmed background if no fade to black
  }

  // B7: "MEDIA1 BUS"
  if (extraButtons.isButtonIn(7, buttonDownPress))  {   // Executes button command if pressed:
    BUSselect = BUSselect==10 ? 0 : 10;
  }
  extraButtons.setButtonColor(7, BUSselect==10 ? 2 : 5);  // Sets color of button to AMBER (2) if in AUX1 BUS mode. Otherwise color "5" which is dimmed yellow

  // B8: "MEDIA2 BUS"
  if (extraButtons.isButtonIn(8, buttonDownPress))  {   // Executes button command if pressed:
    BUSselect = BUSselect==11 ? 0 : 11;
  }
  extraButtons.setButtonColor(8, BUSselect==11 ? 2 : 5);  // Sets color of button to AMBER (2) if in AUX1 BUS mode. Otherwise color "5" which is dimmed yellow

}
