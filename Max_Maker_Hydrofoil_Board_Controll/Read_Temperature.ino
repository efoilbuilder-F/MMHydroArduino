void Read_Temperature()
{
  // take N samples in a row, with a slight delay
  for (i = 0; i < NUMSAMPLES1; i++)
  {
    samples1[i] = analogRead(THERMISTORPIN1);

    samples2[i] = analogRead(THERMISTORPIN2);

    samples3[i] = analogRead(THERMISTORPIN3);

    samples4[i] = analogRead(THERMISTORPIN4);
    delay(5);
  }

  // average all the samples out
  average1 = 0;
  average2 = 0;
  average3 = 0;
  average4 = 0;


  for (i = 0; i < NUMSAMPLES1; i++)
  {
    average1 += samples1[i];
  }
  average1 /= NUMSAMPLES1;



  for (i = 0; i < NUMSAMPLES2; i++)
  {
    average2 += samples2[i];
  }
  average2 /= NUMSAMPLES2;



  for (i = 0; i < NUMSAMPLES3; i++)
  {
    average3 += samples3[i];
  }
  average3 /= NUMSAMPLES3;



  for (i = 0; i < NUMSAMPLES4; i++)
  {
    average4 += samples4[i];
  }
  average4 /= NUMSAMPLES4;



  // convert the value to resistance
  average1 = 1023 / average1 - 1;
  average1 = SERIESRESISTOR / average1;

  float steinhart1; //I have no idea why they call it "steinhart". This is just the variable for the temperature reading like 18C°...
  steinhart1 = average1 / THERMISTORNOMINAL;     // (R/Ro)
  steinhart1 = log(steinhart1);                  // ln(R/Ro)
  steinhart1 /= BCOEFFICIENT;                   // 1/B * ln(R/Ro)
  steinhart1 += 1.0 / (TEMPERATURENOMINAL + 273.15); // + (1/To)
  steinhart1 = 1.0 / steinhart1;                 // Invert
  steinhart1 -= 273.15;                         // convert to C



  average2 = 1023 / average2 - 1;
  average2 = SERIESRESISTOR / average2;

  float steinhart2; //I have no idea why they call it "steinhart". This is just the variable for the temperature reading like 18C°...
  steinhart2 = average2 / THERMISTORNOMINAL;     // (R/Ro)
  steinhart2 = log(steinhart2);                  // ln(R/Ro)
  steinhart2 /= BCOEFFICIENT;                   // 1/B * ln(R/Ro)
  steinhart2 += 1.0 / (TEMPERATURENOMINAL + 273.15); // + (1/To)
  steinhart2 = 1.0 / steinhart2;                 // Invert
  steinhart2 -= 273.15;                         // convert to C




  average3 = 1023 / average3 - 1;
  average3 = SERIESRESISTOR / average3;

  float steinhart3; //I have no idea why they call it "steinhart". This is just the variable for the temperature reading like 18C°...
  steinhart3 = average3 / THERMISTORNOMINAL;     // (R/Ro)
  steinhart3 = log(steinhart3);                  // ln(R/Ro)
  steinhart3 /= BCOEFFICIENT;                   // 1/B * ln(R/Ro)
  steinhart3 += 1.0 / (TEMPERATURENOMINAL + 273.15); // + (1/To)
  steinhart3 = 1.0 / steinhart3;                 // Invert
  steinhart3 -= 273.15;                         // convert to C





  average4 = 1023 / average4 - 1;
  average4 = SERIESRESISTOR / average4;

  float steinhart4; //I have no idea why they call it "steinhart". This is just the variable for the temperature reading like 18C°...
  steinhart4 = average4 / THERMISTORNOMINAL;     // (R/Ro)
  steinhart4 = log(steinhart4);                  // ln(R/Ro)
  steinhart4 /= BCOEFFICIENT;                   // 1/B * ln(R/Ro)
  steinhart4 += 1.0 / (TEMPERATURENOMINAL + 273.15); // + (1/To)
  steinhart4 = 1.0 / steinhart4;                 // Invert
  steinhart4 -= 273.15;                         // convert to C


  lcd.clear();
  lcd.setCursor(0, 2);
  lcd.print("V");
  lcd.print(steinhart1, 0); //,0 stands for zero decimals, so its like rounding.
  lcd.print(" M");
  lcd.print(steinhart2, 0);
  lcd.print(" B");
  lcd.print(steinhart3, 0);
  lcd.print(" C");
  lcd.print(steinhart4, 0);
  //LastTempCheckTime = millis();
}
