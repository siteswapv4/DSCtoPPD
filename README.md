# DSC to PPD
DSCtoPPD is a converter from Project Diva Arcade Future Tone charts (.dsc files) to Project Project Dxxx files (.ppd) 

## HOW TO USE

Either drag and drop the .dsc file to "DSCtoPPD.exe" or execute in cmd and give .dsc file path as argument__
Choose which porting method you want to use, press enter, that's it ! Result in output folders__
Don't forget to put your own "movie.mp4" in chart and project folders or PPD will crash when loading the song. Also remember to complete Data.ini with the star numbers, author name and thumb timings__

## FEATURES

-All note data (including chain slides, same direction slides, notes parameters) in layer file__
-BPM changes as a script (BPM.fsml) (will not modify bpm in the editor but only in real gameplay)__
-PPDproject, PPDscore, PPDlayer export__
-data.ini auto filled (end time, start time, bpm etc...)__
-chain slide scales__

If you have any issue please contact me on discord : siteswap__

## DOWNLOAD

You can get the windows 64-bit version in the release tab on the right__

## COMPILE

DSCtoPPD also compiles for linux and macos although there are no releases. Just use : 
"gcc main.c -o DSCtoPPD -lm" 
If you want to compile for Windows 32-bit or other versions of windows, you need a gcc compiler recent enough have std=c11 and use : 
"gcc main.c -o DSCtoPPD.exe -D_POSIX_C_SOURCE=200809L -std=c11 -municode" 
