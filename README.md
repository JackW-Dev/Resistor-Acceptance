# Resistor-Acceptance
A simple ANSI C90 program for resistor quality control.
This application will run in the console and will provide a text based UI for the user to navigate using.

The application will allow the user to carry out any of the following commands:
1.  Input details for a batch of resistors
2.  Input details for a batch of resistors and store the details in a log
3.  View the data log
4.  View the data log filtered by supplier
5.  Exit application

The system currently supports four key suppliers and is hard-coded to allow the user to select the source through the menu system.
The system supports resistors with 4, 5 or 6 colour bands and will promt the user to input the colours of the bands following this selection.
Once provided, the system will prompt for the actual resistance of a portion of resistors in this batch.

The system will calculate the expected tolerance range for each resistor and will compare this to the actual value to calculate an acceptance rate for the batch.
These details will be output for the user and, if selected, logged to a log file named SupplierInfo

This application was produced as a first year university assignment.
