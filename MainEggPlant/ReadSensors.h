/*
Date:26/4/2024
Created By Mohamed Iraqi
This file defines Core functions for hardware side of the code
*/

typedef enum {
  hour_Enum = 92,
  minute_Enum,
  second_Enum,
  day_Enum,
  weekday_Enum,
  month_Enum,
  year_Enum,
  now_Enum,
  Power_Enum,
  EnergyNow_Enum,
  EnergyHourly_Enum,
  TotalEnergySincePowerUp_Enum,
  BreakOut_Enum,
  IsConnected_Enum,
  Message_Ended_Enum
} CommEnum_t;

/**
     * Init for the lcd and required inputs for measurment
     *
     * @param . void.
     * @return void
     *
     */
void ReadSensors_Init();

/**
     * Measure Current and Voltage Value
     *
     * @param CtArray The Array of current sensors (Returns readings of cts)
     * @param CtArraySize The size of the Array of current sensors 
     * @param PtArray The Array of Voltage sensors (Returns readings of Pts)
     * @param PtArraySize The size of the Array of Voltage sensors
     * @param NumOfCyclesToAverage The size of the Array of Voltage sensors
     * @return void
     *
     **/
void ReadSensors_Measure(float* CtArray, float* PtArray, int NumOfCyclesToAverage = 1, int CtArraySize = 3, int PtArraySize = 1);


/**
     * Display Ct and Pt values on lcd
     *
     * @param CtArray The Array of current sensors (Returns readings of cts)
     * @param CtArraySize The size of the Array of current sensors 
     * @param PtArray The Array of Voltage sensors (Returns readings of Pts)
     * @param PtArraySize The size of the Array of Voltage sensors
     * @return void
     *
     **/
void ReadSensors_LCDDisplayMeasurements(float* CtArray, float* PtArray, int CtArraySize = 3, int PtArraySize = 1);

/**
     * Send Power Reading to Esp
     *
     * @param PowerIdentifier The power array Type
     * @param power The power array Readings
     * @return True if transaction completed, false if transaction failed
     *
     **/
bool ReadSensors_SendPowerReadToEsp(CommEnum_t PowerIdentifier, double* power ,double Energyhourlypowertobe[][3]=0);
/**
     * Send Request to Esp
     *
     * @param Request The power array Readings
     * @return The requested String
     *
     **/
String ReadSensors_SendRequest(CommEnum_t Request);

/**
     * Send Request to Esp
     *
     * @param ToDisp The String to be displayed
     * @param clear Ture to clear display false to leave it as it was
     * @param cursorx cursor x position
     * @param cursory cursor y position
     * @return null
     *
     **/
void ReadSensors_LcdDisplay(String toDisp, bool clear = true, int cursorx = 0, int cursory = 0);

/**
     * Get Initial Hour correctly
     *
     * @param null
     * @return null
     *
     **/
void ReadSensors_GetInitHour();

/**
     * Get Initial Day correctly
     *
     * @param null
     * @return null
     *
     **/
void ReadSensors_GetInitDay();

/**
     * CalculatePower
     *
     * @param null
     * @return null
     *
     **/
void Main_CalculatePower();

/**
     * Send Power arrays to Esp
     *
     * @param null
     * @return null
     *
     **/
void Main_SendPower();

/**
     * Calculate Energy arrays 
     *
     * @param null
     * @return null
     *
     **/
void Main_CalculateEnergyArrays();

/**
     * Resets daily arrays after each new day
     *
     * @param null
     * @return null
     *
     **/
void Main_ResetArrays();

/**(NOT USED)
     * checks if Esp is Online(connected to wifi)
     *
     * @param null
     * @return ture if online or false if offline
     *
     **/
bool ReadSensors_EspOnlineStatus();