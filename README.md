# DSC to PPD
DSCtoPPD is a converter from Project Diva Arcade Future Tone charts (.dsc files) to Project Project Dxxx files (.ppd)

## HOW TO USE

Either drag and drop the .dsc file to "DSCtoPPD.exe" or execute in cmd and give .dsc file path as argument\
Choose which porting method you want to use, press enter, that's it ! Result in output folders\
Don't forget to put your own "movie.mp4" in chart and project folders or PPD will crash when loading the song. Also remember to complete Data.ini with the star numbers, author name and thumb timings

## FEATURES

-All note data (including chain slides, same direction slides, notes parameters) in layer file\
-BPM changes as a script (BPM.fsml) (will not modify bpm in the editor but only in real gameplay)\
-PPDproject, PPDscore, PPDlayer export\
-data.ini auto filled (end time, start time, bpm etc...)\
-chain slide scales

## DOWNLOAD

You can get the windows 64-bit version in the release tab on the right

## COMPILE

DSCtoPPD also compiles (at least) on Linux and Mac\
You'll need SDL3 devel to compile and run
```
gcc src/*.c -o DSCtoPPD -lSDL3
```
