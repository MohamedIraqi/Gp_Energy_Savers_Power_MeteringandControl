/*
Date:26/4/2024
Created By Mohamed Iraqi
This file defines Core functions for hardware side of the code
*/

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
     * @param power The power array Readings
     * @return True if transaction completed, false if transaction failed
     *
     **/
bool ReadSensors_SendPowerReadToEsp(float* power, int size = 3);
/**
     * Send Request to Esp
     *
     * @param Request The power array Readings
     * @return The requested String
     *
     **/
String ReadSensors_SendRequest(String Request);

/**
     * Send Request to Esp
     *
     * @param ToDisp The String to be displayed
     * @return null
     *
     **/
void ReadSensors_LcdDisplay(String toDisp);