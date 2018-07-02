void Anti_Spark_Routine ()
{
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Connecting Bat.");
  Serial.println("Performing anti spark routine.");
  digitalWrite(SparkRelay, LOW); //Activates the Anti Spark Relay
  delay (500); // Waits for 0.5 seconda to let capacitors charge up.
  digitalWrite(BatRelay, LOW); //closes the battery relay relay which in turn closes the big battery relay.
  delay (500); //waits another 0.5 second to let the giant battery relay physcially close.
  digitalWrite(SparkRelay, HIGH); //deactivates the Anti Spark Relay
  ChargeState = HIGH;

  delay (1000);
  lcd.clear();
  lcd.print("Bat. Connected");
  delay (1000);
  lcd.clear();

}
