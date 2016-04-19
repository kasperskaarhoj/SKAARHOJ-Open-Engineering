void HWrunLoop()	{
    uint16_t bUp;
    uint16_t bDown;

#if (SK_MODEL == SK_E21TVS)
    // BI16 buttons:
    bUp = buttons.buttonUpAll();
    bDown = buttons.buttonDownAll();
    uint8_t b16Map[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16};    // These numbers refer to the drawing in the web interface
    for (uint8_t a = 0; a < 16; a++) {
      uint8_t color = actionDispatch(b16Map[a], bDown & (B1 << a), bUp & (B1 << a));
      buttons.setButtonColor(a+1, (color&15)>0?((!(color&16) || (millis()&512)>0) && ((color&15)!=5)?1:3):0);    // TODO: Set up support for mono-color blinking bit...
    }

#if SK_HWEN_SLIDER
    // Slider:
    bool hasMoved = slider.uniDirectionalSlider_hasMoved();
    actionDispatch(17, hasMoved, hasMoved && slider.uniDirectionalSlider_isAtEnd(), 0, slider.uniDirectionalSlider_position());
#endif

#if SK_HWEN_GPIO
    // GPIO Inputs
    bUp = GPIOboard.inputUpAll();
    bDown = GPIOboard.inputDownAll();
    uint8_t gpio1Map[] = {26,27,28,29,30,31,32,33};
    for (uint8_t a = 0; a < 8; a++) {
      actionDispatch(gpio1Map[a], bDown & (B1 << a), bUp & (B1 << a));
    }

    // GPIO Outputs
    uint8_t gpio2Map[] = {18,19,20,21,22,23,24,25};
    for (uint8_t a = 0; a < 8; a++) {
      uint8_t state = actionDispatch(gpio2Map[a]);
      GPIOboard.setOutput(a+1, state!=5 && state!=0);   // TODO: IMPLEMENT CACHING of writing to this relay!
    }
#endif

#endif
}