# This is guide on how to run the Library Management System GUI
1. This GUI only works on WSL (Windows Subsystem for Linux) or Linux OS
2. Install Ubuntu and other required packages using the following command in your Ubuntu terminal: `apt-get update && apt full-upgrade -y && apt-get install gcc g++ gdb make -y`
3. Clone the repository from GitHub
4. Open the Ubuntu terminal (using command `wsl` in Powershell) and locate the directory where you cloned the repository (using the command `cd`) 
5. Run the following command to compile the code: `gcc admin_ncurses.c final_ncurses.c  member_portal_ncurses.c books_portal_ncurses.c issue_portal_ncurses.c return_portal_ncurses.c fine_portal_ncurses.c -lncurses -o final.exe`
6. Run the compiled code using the following command: `./final.exe`
7. You will be presented with the Admin portal of the Library Management System GUI. You can navigate through the different options using the arrow keys and select options. 
THANK YOU!