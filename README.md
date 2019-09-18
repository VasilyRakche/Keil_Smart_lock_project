# Keil_Smart_lock_project
Knock to unlock, remember and lock back 

Project is done as a my preparation for the short summer school for STM microcontrollers, where I was a teacher. The detailed information is published in Montenegrin language in [this article](http://www.tehnopolis.me/online/mne/polaznici-obuke-mehatronika-za-dva-dana-stekli-nova-znanja-napravili-sistemu-pametne-brave/) Please use this article to check the video of the device at the end.

In short:

Whole project is build using only CMSIS library for STM programming. The project works based on the touch sensor, which is used as the input sensor. Touching the sensors triggers interrupt, which is responsible to mesure time between touches. That way it was possible to create basic interface for the smart lock-human interaction. Program is various bug tested, it can remember, safe reset all data, lock and unlock using a simple combination of smart lock touches. This project was completely designed by myself, all together with the electronics, 3d Modeling, algorithm and programming, from scratch. It was done in order to show capabilities of STM microcotrolers.
