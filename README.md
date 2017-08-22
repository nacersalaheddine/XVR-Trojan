# XVR Trojan
 - Runs on windows and it was tested on windows 10.
 - The language is C using MinGW
 - You can fix bugs and modify as much as you want, and I am learning C for quite a time and wanted to share my program.
 
# Console commands:

 Commands	 # Args # Description
#############################################################################################################
 echo		 # 0	# Echo
 indentify	 # 0	#Retrieves the computer information. Computer name, username, screen width, screen height
 system		 # 1	#Executes cmd command
 send_file	 # 2	#sends file {SLAVE_PATH}; {MASTER_PATH}
 get_file	 # 2	#receivies file {SLAVE_PATH}; {MASTER_PATH}
	if {SLAVE_PATH} equals:
		screenshot_small	#receivies screenshot with 30% of the screen
		screenshot_half		#receivies screenshot with 50% of the screen
		screenshot_full		#receivies screenshot of the screen
		keylogger_path		#receivies keylogger data and clears it
		
 msci_sound	 # 1	#Executes MSCI command with limitations; Example: msci_sound Play test.mp3 from 0
 keylog_clear # 0	#Clears keylogger data
 mbox		 # 3	#Messagebox {TITLE}; {MSG}; {TYPE}
	{TYPE} can be: ERROR, QUESTION, WARNING, INFO
	otherwise will be plain			
	
 terminate	 # 0  	#Stops the slave
 exit		 # 0	#Stops the master
