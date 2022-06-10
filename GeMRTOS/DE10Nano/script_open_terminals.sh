cygstart nios2-terminal -v --flush --no-quit-on-ctrl-d --instance=0
# cygstart nios2-terminal -v --flush --no-quit-on-ctrl-d --instance=1 
# cygstart nios2-terminal -v --flush --no-quit-on-ctrl-d --instance=1 | tee output.txt
# de https://askubuntu.com/questions/420981/how-do-i-save-terminal-output-to-a-file

#cygstart terminal_err.sh
cygstart nios2-terminal -v --flush --no-quit-on-ctrl-d --instance=1 
#cygstart terminal_1.sh
cygstart nios2-terminal -v --flush --no-quit-on-ctrl-d --instance=2
#cygstart terminal_2.sh
cygstart nios2-terminal -v --flush --no-quit-on-ctrl-d --instance=3
#cygstart terminal_3.sh
cygstart nios2-terminal -v --flush --no-quit-on-ctrl-d --instance=4
cygstart nios2-terminal -v --flush --no-quit-on-ctrl-d --instance=5

nios2-download --go -r ./software/hellogrtos/hellogrtos.elf --instance=0