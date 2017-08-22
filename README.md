# XVR Trojan
 - Runs on windows and it was tested on windows 10.
 - The language is C using MinGW
 - You can fix bugs and modify as much as you want, and I am learning C for quite a time and wanted to share my program.
 
 Commands | Arguments | Description | Example
 -------- | --------- | ----------- | -------
 echo | {MSG} | Echo | echo Hello
 indentify |  | Retrieves the computer information. |
 system | {CMD} | Executes cmd command | system ipconfig
 send_file | {SLAVE_PATH}; {MASTER_PATH} | Sends file | send_file C:\fs; D:\fm
 get_file | {SLAVE_PATH}; {MASTER_PATH} | Receivies file | get_file C:\fs; D:\fm
 msci_sound | {CMD} | MSCI command | msci_sound play f.mp3 from 0
 keylog_clear |  | Clears keylogger data | 
 mbox | {TITLE}; {MSG}; {TYPE} | Message box | mbox TITLE; Msg; ERROR
 terminate |  | Stops the slave |
 exit |  | Stops the master |

# Subcommands
 - get_file
 
 Commands | Description
 -------- | -----------
 screenshot_small | Screenshot 30%
 screenshot_half | Screenshot 50%
 screenshot_full | Screenshot
 keylogger_path | Keylogger path
  
