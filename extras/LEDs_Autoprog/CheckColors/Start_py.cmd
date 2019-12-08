@Echo Off
REM Could be used for debugging
ECHO Starting MobaLedCheckColors.py...
Date /T >                       Messages.txt
Time /T >>                      Messages.txt
python -V >>                    Messages.txt
:python MobaLedCheckColors.py >> Messages.txt 2>&1
:python MobaLedCheckColors.py >> Messages.txt
python MobaLedCheckColors.py
exit
