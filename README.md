# DSC to PPD
DSCtoPPD is a converter from Project Diva Arcade Future Tone charts (.dsc files) to Project Project Dxxx files (.ppd)<br />

## HOW TO USE

Either drag and drop the .dsc file to "DSCtoPPD.exe" or execute in cmd and give .dsc file path as argument<br />
Choose which porting method you want to use, press enter, that's it ! Result in output folders<br />
Don't forget to put your own "movie.mp4" in chart and project folders or PPD will crash when loading the song. Also remember to complete Data.ini with the star numbers, author name and thumb timings<br />

## FEATURES

-All note data (including chain slides, same direction slides, notes parameters) in layer file<br />
-BPM changes as a script (BPM.fsml) (will not modify bpm in the editor but only in real gameplay)<br />
-PPDproject, PPDscore, PPDlayer export<br />
-data.ini auto filled (end time, start time, bpm etc...)<br />
-chain slide scales__

If you have any issue please contact me on discord : siteswap<br />

## DOWNLOAD

You can get the windows 64-bit version in the release tab on the right<br />

## COMPILE

DSCtoPPD also compiles for linux and macos although there are no releases. Just use :<br />
"gcc main.c -o DSCtoPPD -lm"<br />
If you want to compile for Windows 32-bit or other versions of windows, you need a gcc compiler recent enough have std=c11 and use :<br />
"gcc main.c -o DSCtoPPD.exe -D_POSIX_C_SOURCE=200809L -std=c11 -municode"<br />
