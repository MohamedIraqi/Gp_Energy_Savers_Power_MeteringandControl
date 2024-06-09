/*
Date:5/5/2024
Created By Mohamed Iraqi
@author Mohamed Iraqi - M.A.Iraqi.Amen@gmail.com
This file defines Core functions for Communication with The arduino Code side of the code
*/

/**
     * Init for the Wifi and getting tokens for the firebase
     *
     * @param . void.
     * @return void
     *
     */
void ArdCom_Init();

/**
     * Manages communication with the arduino
     *
     * @param . void.
     * @return void
     *
     */
void ArdCom_Com_Handler();

/**
     * Checks wifi connection and sends "." in serial if offline
     *
     * @param . void.
     * @return ture if online or false if offline
     *
     */
bool ArdCom_StatusConnectedWifi();

/**
     * Uploads Data to firestore
     *
     * @param . void.
     * @return void
     *
     */
void ArdCom_UploadData();
